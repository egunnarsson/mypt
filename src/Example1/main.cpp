
#include <sstream>

#include <SDL.h>
#include <GL/glew.h>
#define NO_SDL_GLEXT
#include <SDL_opengl.h>
#undef main

#include "Material.h"
#include "Camera.h"
#include "Log.h"
#include "Renderer.h"
#include "Scene.h"
#include "Mesh.h"
#include "Loader.h"

#define GLASS_M  Material(GLASS,   Color(0), Color(1,1,1) * 0.999)
#define	MIRROR_M Material(MIRROR,  Color(0), Color(1,1,1) * 0.999)
#define RED_M    Material(DIFFUSE, Color(0), Color(0.75, 0.25, 0.25))
#define BLUE_M   Material(DIFFUSE, Color(0), Color(0.25, 0.25, 0.75))
#define GRAY_M   Material(DIFFUSE, Color(0), Color(0.75, 0.75, 0.75))
#define LIGHT_M  Material(DIFFUSE, Color(12.0, 12.0, 12.0), Color(0))
#define BLACK_M  Material(DIFFUSE, Color(0), Color(0))

namespace
{
	const int W = 640;
	const int H = 480;
	const float DIV_W = 1.0f / W;
	const float DIV_H = 1.0f / H;

	const SDL_VideoInfo* info = nullptr;
	GLuint tex = 0;
};

bool init_sdl()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		LOG_ERROR("Could not initialize SDL: " + SDL_GetError());
		return false;
	}

	info = SDL_GetVideoInfo();

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_Surface *screen = nullptr;
	if ((screen = SDL_SetVideoMode(W, H, info->vfmt->BitsPerPixel, SDL_OPENGL)) == 0) {
		return false;
	}

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		return false;
	}

	glViewport(0, 0, info->current_w, info->current_h);

	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_FLOAT, nullptr);

	return true;
}

int main(int argc, char *argv[])
{
	SDL_Event event;

	Renderer renderer;
	Scene scene;
	Camera camera;

	camera.setPosition(Position(0, 56, 130));
	camera.setDirection(Direction(0, -0.042612f, -1));
	camera.setResolution(Resolution(W, H));

	renderer.setCamera(&camera);

	scene.addMesh(Mesh::makePlane(Position(0, 50, 0), Scale(100, 100, 100), AxisRotation(Direction(1, 0, 0), 90), &GRAY_M)); // Backs
	scene.addMesh(Mesh::makePlane(Position(-100, 50, 0), Scale(100, 100, 100), AxisRotation(Direction(0, 0, 1), 90), &RED_M)); // Left
	scene.addMesh(Mesh::makePlane(Position(0, 50, 0), Scale(100, 100, 100), AxisRotation(Direction(1, 0, 0), 0), &GRAY_M));// bottom 
	scene.addMesh(Mesh::makePlane(Position(0, 50, 0), Scale(100, 100, 100), AxisRotation(Direction(0, 0, 1), 90), &BLUE_M)); // Right
	scene.addMesh(Mesh::makePlane(Position(0, 150, 0), Scale(100, 100, 100), AxisRotation(Direction(1, 0, 0), 0), &GRAY_M)); // Top
	scene.addMesh(Mesh::makePlane(Position(0, 119, 0), Scale(39, 39, 39), AxisRotation(Direction(0, 0, 1), 0), &LIGHT_M));
	
	{
		eObjLoader loader("gourd.obj");
		if (loader.load())
		{
			if (loader.hasMissingNormal())
			{
				loader.calculateMissingNormals();
			}

			if (loader.hasMissingTexCoord())
			{
				loader.fillMissingTexCoords(eObjLoader::vec2(0, 0));
			}

			Mesh *mesh = Mesh::makeMesh(loader.indices(), loader.positions(), loader.normals(), loader.texCoords());

			//Mesh *mesh = new Mesh();
			//load_obj("camera.obj", *mesh);

			mat4 scale = mat4::make_scale(12.0);
			mat4 rotation = mat4::make_rotation(Direction(0, 1, 0), toRadians(25));
			mat4 translate = mat4::make_translation(Position(20, 50, 0));

			mesh->transform(translate * rotation * scale);
			mesh->setMaterial(GRAY_M);

			scene.addMesh(mesh);
		}
	}

	{
		eObjLoader loader("camera.obj");
		if (loader.load())
		{
			if (loader.hasMissingNormal())
			{
				loader.calculateMissingNormals();
			}

			if (loader.hasMissingTexCoord())
			{
				loader.fillMissingTexCoords(eObjLoader::vec2(0, 0));
			}

			Mesh *mesh = Mesh::makeMesh(loader.indices(), loader.positions(), loader.normals(), loader.texCoords());

			//Mesh *mesh = new Mesh();
			//load_obj("camera.obj", *mesh);

			mat4 scale = mat4::make_scale(12.0);
			mat4 rotation = mat4::make_rotation(Direction(0, 1, 0), toRadians(25));
			mat4 translate = mat4::make_translation(Position(0, 10, 0));

			mesh->transform(translate * rotation * scale);
			mesh->setMaterial(GRAY_M);

			scene.addMesh(mesh);
		}
	}
	
	renderer.setScene(&scene);
	//renderer.setDestBuffer(W, H, colorArray);

	if (!init_sdl())
		return -1;

	clock_t start = clock();
	clock_t frameStart = clock();

	renderer.start();

	bool run = true;
	while (run) {

		FramePtr frame = renderer.getNextFrame();

		if (frame != nullptr)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_FLOAT, frame->buffer());

			glBegin(GL_QUADS);
			glTexCoord2i(0, 1); glVertex2i(-1, -1);
			glTexCoord2i(1, 1); glVertex2i(1, -1);
			glTexCoord2i(1, 0); glVertex2i(1, 1);
			glTexCoord2i(0, 0); glVertex2i(-1, 1);
			glEnd();

			SDL_GL_SwapBuffers();

			clock_t frameEnd = clock();
			double frameTime = frameEnd - frameStart;

			int lastId = renderer.lastFrameId();
			std::ostringstream oss;
			oss << "Iterations " << lastId <<
				" Msamps per S:" << (lastId*(W*H) / (((frameEnd - start) / CLOCKS_PER_SEC) + 1.0)) / 1000000 <<
				" time:" << frameTime / CLOCKS_PER_SEC;

			frameStart = frameEnd;

			printf("%f\n", frameTime);

			SDL_WM_SetCaption(oss.str().c_str(), 0);
		}

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				run = false;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_s) {
					//sprintf_s(tmp,"smallpt_sample_%d.bmp", iterations);
					//sprintf_s(tmp,"smallpt_sample_%d.bmp",iterations);
					//SDL_SaveBMP(screen, tmp);
				}
				break;
			default: break;
			}
		}

		// otherwise the event loop hogs one cpu core doing shit with it
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	renderer.stop(); 

	uint64_t boxC = Scene::s_traceStats.boxCount;
	uint64_t triC = Scene::s_traceStats.triCount;
	uint64_t rayC = Scene::s_traceStats.rayCount;

	// 83764917 315623640(3.77) 1237294818(14.77)
	printf("%llu %llu %llu\n",rayC, triC, boxC);
	
	SDL_Quit();

	int i = 0;
	std::cin >> i;

	return 0;
}
