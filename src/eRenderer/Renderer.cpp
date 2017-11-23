
#include <tbb\tbb.h>

#include <iostream>
#include <atomic>

#include "Log.h"

#include "Ray.h"
#include "Scene.h"
#include "AABVH.h"
#include "Mesh.h"
#include "Camera.h"
#include "Material.h"
#include "Renderer.h"

namespace
{
	struct WorkerData
	{
		const Scene  *m_scene;
		const Camera *m_camera;
		Color        *m_buffer;
		fColor       *m_destination;
		number        m_preMult;
		number        m_newMult;
	};

	class Worker
	{
	public:
		std::atomic<unsigned int> &m_seed;
		const WorkerData &m_data;

		Worker(const Worker &other) : m_data(other.m_data), m_seed(other.m_seed) {}
		Worker(const WorkerData &data, std::atomic<unsigned int> &seed) : m_data(data), m_seed(seed) {}

		inline Color radiance(Ray ray, rng<number> &rand) const
		{
			Color total(0);
			Color contribution(1, 1, 1);

			AABVH::HitInfo hit;
			
			while (true)
            {
				ray.calculateInvDir();

				if (!m_data.m_scene->intersect(ray, hit))
                {
					return total;
				}

                const std::shared_ptr<const IMaterial> &mat = hit.mesh->chunks()[hit.chunkIndex].m_material;
				const Direction &normal = hit.mesh->chunks()[hit.chunkIndex].m_normals[hit.triangleIndex * 3];

                IMaterial::SampleResult sample = mat->sample(ray, hit.t, normal, rand);

				Color color = sample.reflectance;
				const number p = tmax(tmax(color.x, color.y), color.z);

				if (rand() < p)
					color *= (1 / p);
				else
					return total + (contribution * sample.emission);

				//number importance;
				//ray = mat->sample(ray, hit.t, normal, sample.importance, rand).nudge();
                ray = sample.out;

				total = total + (contribution * sample.emission);
                contribution = contribution * color;// *sample.importance;
			}
			return total;
		}

		void operator()(const tbb::blocked_range<unsigned int>& range) const
		{
			rng<number> rand(m_seed++);

			const unsigned int width = m_data.m_camera->getWidth();
			const unsigned int height = m_data.m_camera->getHeight();

			for (unsigned int y = range.begin(); y != range.end(); ++y) // Loop over image rows
			{
				const unsigned int yi = (height - y - 1) * width;
				for (unsigned int x = 0; x < width; ++x) // Loop cols
				{
					const unsigned int i = yi + x;

					Ray ray = m_data.m_camera->getRay(vec2(static_cast<number>(x), static_cast<number>(y)), rand);
					Color color = radiance(ray, rand); // m_data.m_integrator(ray, rand);

					m_data.m_buffer[i] = (m_data.m_buffer[i] * m_data.m_preMult) + (color * m_data.m_newMult);
					m_data.m_destination[i] = fColor(
						float(sqrt(clamp(m_data.m_buffer[i].x))),
						float(sqrt(clamp(m_data.m_buffer[i].y))),
						float(sqrt(clamp(m_data.m_buffer[i].z))));
				}
			}
		}
	};

	class Pause
	{
	public:
		Renderer  &m_r;
		const bool m_wasRunning;

		Pause(Renderer &r) :
			m_r(r),
			m_wasRunning(r.isRunning())
		{
			m_r.stop();
		}

		~Pause()
		{
			if (m_wasRunning)
				m_r.start();
		}
	};
}

void Frame::recycle(Frame *f)
{
	if (f != nullptr)
	{
		if (f->m_resolution == f->m_renderer->m_resolution)
		{
			f->m_renderer->m_oldFrames.push(f);
		}
		else
		{
			delete f;
		}
	}
}

/*FramePacket::FramePacket(Frame frame, Renderer *renderer) :
	m_frame(frame),
	m_renderer(renderer)
{
}

FramePacket::~FramePacket()
{
	std::cout << "append to old frames\n";
	if (m_frame != Frame())
		m_renderer->m_oldFrames.push(m_frame);
}*/

Renderer::Renderer() :
	m_running(false),
	m_thread(nullptr),
	m_stop(true),
	m_iteration(0),
	m_buffer(nullptr),
	//m_destination(nullptr),
	m_currentFrame(nullptr),
	m_lastFrame(nullptr),
	m_scene(nullptr),
	m_camera(nullptr)
{
}

Renderer::~Renderer()
{
	deleteBuffers();
}

void Renderer::deleteBuffers()
{
	stop();
	delete[] m_buffer;
	delete m_currentFrame;
	delete m_lastFrame.exchange(nullptr);
	for (Frame *f; m_oldFrames.try_pop(f);)
		delete f;

	m_currentFrame = nullptr;
}

void Renderer::setScene(Scene *scene)
{
	if (scene == nullptr)
	{
		LOG_ERROR("scene was null");
		return;
	}

	Pause(*this);

	m_scene = scene;
}

void Renderer::setCamera(Camera *camera)
{
	if (camera == nullptr)
	{
		LOG_ERROR("camera was null");
		return;
	}

	Pause(*this);

	m_camera = camera;

	deleteBuffers();

	m_resolution.width  = m_camera->getWidth();
	m_resolution.height = m_camera->getHeight();
	m_buffer = new Color[m_resolution.count()];
}

void Renderer::start()
{
	if (!m_running)
	{
		if (m_camera == nullptr)
		{
			LOG_ERROR("trying to start with no camera");
			return;
		}
		if (m_scene == nullptr)
		{
			LOG_ERROR("trying to start with no scene");
			return;
		}
		
		for (unsigned int i = 0; i < m_resolution.count(); ++i)
			m_buffer[i] = Color(0);

		m_scene->calculateAccelerationStructure();

		m_iteration = 0;

		m_stop    = false;
		m_thread  = new std::thread(&Renderer::loop, this);
		m_running = true;
	}
}

void Renderer::stop()
{
	if (m_running)
	{
		m_stop = true;
		m_thread->join();
		delete m_thread;
		m_thread = nullptr;
		m_running = false;
	}
}

// very hacky atm
bool Renderer::step()
{
    if (!m_running)
    {
        if (m_camera == nullptr)
        {
            LOG_ERROR("trying to start with no camera");
            return false;
        }
        if (m_scene == nullptr)
        {
            LOG_ERROR("trying to start with no scene");
            return false;
        }

        m_scene->calculateAccelerationStructure();

        m_stop = false;
        m_thread = new std::thread(&Renderer::loop, this);
        m_running = true;

        stop();

        return true;
    }
    else
    {
        return false;
    }
}

bool Renderer::isRunning() const
{
	return m_running;
}

int Renderer::lastFrameId() const
{
	return m_iteration;
}

void Renderer::loop()
{
	WorkerData data;
	data.m_scene  = m_scene;
	data.m_camera = m_camera;
	data.m_buffer = m_buffer;

	std::atomic<unsigned int> seed(0);

	while (!m_stop)
	{
		++m_iteration;
		data.m_newMult = 1.0 / m_iteration;
		data.m_preMult = 1.0 - data.m_newMult;

		// make sure we have something to draw to, check recycled frames first
		if (m_currentFrame == nullptr) {
			if (!m_oldFrames.try_pop(m_currentFrame)) {
				m_currentFrame = new Frame(m_resolution, m_iteration, this);
			}
		}
		data.m_destination = m_currentFrame->m_buffer;

		// render
#if 1
		tbb::parallel_for(tbb::blocked_range<unsigned int>(0, m_resolution.height), Worker(data, seed));
#else
		Worker w(data, seed);
		w(tbb::blocked_range<unsigned int>(0, m_resolution.height));
#endif

		// swap buffers
		m_currentFrame = m_lastFrame.exchange(m_currentFrame);
	}
}

FramePtr Renderer::getNextFrame()
{
	return FramePtr(m_lastFrame.exchange(nullptr), Frame::recycle);
}
