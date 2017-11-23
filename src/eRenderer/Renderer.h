
#ifndef RENDERER_H
#define RENDERER_H

#include "Defines.h"

#include <thread>
#include <vector>
#include <atomic>

#include <tbb\concurrent_queue.h>

class Scene;
class Camera;
class Renderer;

#include <iostream>

class Frame
{
public:
	//Frame(const Frame &f)         : m_buffer(f.m_buffer), m_id(f.m_id) {}
	Frame(/*fColor* buffer*/Resolution resolution, int id, Renderer *renderer) :
		m_resolution(resolution),
		m_buffer(new fColor[resolution.count()]),
		m_id(id),
		m_renderer(renderer)
	
	{
		std::cout << "new frame (" << m_id << std::endl;
	}

	~Frame() { std::cout << "delete frame " << m_id << std::endl; delete[] m_buffer; }

	//Frame&        operator=(const Frame& f)          { m_id = f.m_id; m_buffer = f.m_buffer; return *this; }

	const fColor* buffer() const                     { return m_buffer; }
	int           id() const                         { return m_id; }

	//bool          isValid() const                    { return m_buffer != nullptr && m_id >= 0; }

	bool          operator==(const Frame& rhs) const { return (m_id == rhs.m_id && m_buffer == rhs.m_buffer); /*|| (!isValid() && !rhs.isValid());*/ }
	bool          operator!=(const Frame& rhs) const { return !(*this==rhs); }

private:

	const Resolution m_resolution; // probably need this...
	fColor *const m_buffer;
	const int m_id;
	Renderer *m_renderer; // change to shared ptr... but then app cant have renderer outside shared_ptr

	static void recycle(Frame*);

	friend Renderer;
};

/** Wraps a frame,  */
/*class FramePacket
{
public:

	FramePacket(Frame, Renderer*);
	~FramePacket();

	const Frame& operator*() const { return m_frame; }
	const Frame* operator->() const { return &m_frame; }

private:

	Frame    m_frame;
	Renderer *m_renderer;
};*/

typedef std::shared_ptr<Frame> FramePtr;

class Renderer
{
public:

	Renderer();
	~Renderer();

	void setScene(Scene *scene);
	void setCamera(Camera *camera);

    /*!
    Returns the last completed frame or nullptr if no new frame has been completed since the last call. 
    */
	FramePtr getNextFrame();

	void start();
	void stop();
    bool step();
	// void resume(); // same as start but does not clear buffer/iteration, unless resolution change
	bool isRunning() const;

	int  lastFrameId() const;

private:

	void loop();
	void deleteBuffers();

	bool         m_running;
	std::thread *m_thread;
	bool         m_stop;

	int m_iteration;

	Resolution m_resolution;
	//int m_width;
	//int m_height;

	/** the current frame that is being written to by workers */
	Frame *m_currentFrame;
	/** the last finished frame, to be retrived by main program, it is then cleared */
	std::atomic<Frame*> m_lastFrame;
	/** keeps a list of old frames that are not used, they are inserted when a FramePtr is destroyed */
	tbb::concurrent_queue<Frame*> m_oldFrames;
	/** accumulation buffer */
	Color *m_buffer;

	Scene  *m_scene;
	Camera *m_camera;

	std::atomic<unsigned int> m_seed;

	friend Frame;
};

#endif
