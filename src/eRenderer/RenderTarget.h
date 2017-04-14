
#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#include "Defines.h"

#include <mutex>


class LockedBuffer
{
public:
	LockedBuffer(fColor *buffer, std::recursive_mutex &mutex) :
		m_buffer(buffer),
		m_mutex(mutex),
		m_lock(mutex) {}
	LockedBuffer(LockedBuffer const& ptr) :
		m_buffer(ptr.m_buffer),
		m_mutex(ptr.m_mutex),
		m_lock(ptr.m_mutex)	{}

	/*LockedBuffer& operator = (LockedBuffer const& ptr)
	{
		m_buffer = ptr.m_buffer;
		m_mutex(ptr.m_mutex);
		m_lock(std::lock_guard<std::recursive_mutex>(m_mutex));
		return *this;
	}*/

	fColor *buffer() { return m_buffer; }

private:
	
	fColor *m_buffer;
	std::recursive_mutex &m_mutex;
	std::lock_guard<std::recursive_mutex> m_lock;

};


class  RenderTarget
{
public:

	RenderTarget();
	~RenderTarget();

	LockedBuffer getFront();
	LockedBuffer getBack();

	void         swap();

	void         setSize(Resolution size);
	Resolution   getSize() const;

private:

	Resolution m_size;

	fColor *m_front;
	fColor *m_back;

	std::recursive_mutex m_frontMutex;
	std::recursive_mutex m_backMutex;
};

#endif
