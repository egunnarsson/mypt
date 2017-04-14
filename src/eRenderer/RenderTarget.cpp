
#include "RenderTarget.h"

RenderTarget::RenderTarget() :
	m_front(nullptr),
	m_back(nullptr)
{
}

RenderTarget::~RenderTarget()
{
}

LockedBuffer RenderTarget::getFront()
{
	return LockedBuffer(m_front, m_frontMutex);
}

LockedBuffer RenderTarget::getBack()
{
	return LockedBuffer(m_back, m_backMutex);
}

void RenderTarget::swap()
{
	// can this dead lock?...
	std::lock_guard<std::recursive_mutex> frontLock(m_frontMutex);
	std::lock_guard<std::recursive_mutex> backLock(m_backMutex);

	std::swap(m_front, m_back);
}

void RenderTarget::setSize(Resolution size)
{
	// can this dead lock?...
	// what if the same thread is holding a LockedBuffer... then we delete the buffer it is working on
	std::lock_guard<std::recursive_mutex> frontLock(m_frontMutex);
	std::lock_guard<std::recursive_mutex> backLock(m_backMutex);

	m_size = size;

	delete[] m_front;
	delete[] m_back;

	m_front = new fColor[m_size.width * m_size.height];
	m_back  = new fColor[m_size.width * m_size.height];
}

Resolution RenderTarget::getSize() const
{
	return m_size;
}
