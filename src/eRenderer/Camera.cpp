
#include "Log.h"

#include "Camera.h"

Camera::Camera() :
	m_fieldOfView(45.0),
	m_position(0,0,1),
	m_direction(0,0,-1),
	m_worldUp(0,1,0),
	m_up(0,1,0),
	m_right(1,0,0)
{
}

void Camera::setResolution(Resolution resolution)
{
	if (resolution.height == 0 || resolution.width == 0)
	{
		LOG_ERROR("resolution was 0");
		return;
	}

	m_resolution = resolution;
	calculateViewPlane();
}

void Camera::setFieldOfView(number fieldOfView)
{
	m_fieldOfView = toRadians(fieldOfView);
	calculateViewPlane();
}

void Camera::setPosition(Position position)
{
	m_position = position;
}

void Camera::setDirection(Direction direction)
{
	m_direction = direction.normalize();
	calculateViewPlane();
}

void Camera::lookAt(Position position, Position from, Direction up /*= Direction(0, 1, 0)*/)
{
	m_position  = from;
	m_direction = Direction::cast(position - from);
	m_worldUp   = up;
	calculateViewPlane();
}

void Camera::calculateViewPlane()
{
	number aspectRatio = (number)m_resolution.width / m_resolution.height;
	number planeSize   = 2 * tan(m_fieldOfView / 2.0);
	number width       = sqrt(planeSize * aspectRatio);
	number height      = sqrt(planeSize / aspectRatio);
	number stepX       = width / m_resolution.width;
	number stepY       = height / m_resolution.height;
	Direction right    = (m_direction.cross(m_worldUp)).normalize();
	Direction up       = (m_direction.cross(right)).normalize();

	// ViewPlane(origin &+viewDirection &-(right &* (stepX * fw / 2)) &-(up &* (stepY * fh / 2))) (right &* stepX) (up &* stepY)
	m_right = right * -stepX;
	m_up    = up * -stepY;
}

unsigned int Camera::getWidth() const
{
	return m_resolution.width;
}

unsigned int Camera::getHeight() const
{
	return m_resolution.height;
}

Ray Camera::getRay(vec2 position, rng<number> &rand) const
{
	assert(position.x >= 0 && position.x < m_resolution.width && position.y >= 0 && position.y < m_resolution.height);

	// position(0,0) would be a ray along the view direction
	position.x += rand() - m_resolution.width * 0.5;
	position.y += rand() - m_resolution.height * 0.5;

	Position to = m_position + Position::cast(m_right * position.x + m_up * position.y + m_direction);
	// ? Direction dir = (m_right * position.x + m_up * position.y + m_direction).normalize();
	// ? return Ray(m_position, dir);
	return Ray::fromTo(m_position, to);

}
