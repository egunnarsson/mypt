
#ifndef CAMERA_H
#define CAMERA_H

#include "Defines.h"
#include "Ray.h"
#include "rng.h"


class Camera
{
public:
	Camera();

	void setResolution(Resolution resolution);
	void setFieldOfView(number fieldOfView);
	void setPosition(Position position);
	void setDirection(Direction direction);
	void setUp(Direction up);

	void lookAt(Position position, Position from, Direction up = Direction(0, 1, 0));

	Ray getRay(vec2 position, rng<number> &rand) const;

	unsigned int getWidth() const;
	unsigned int getHeight() const;

private:

	void calculateViewPlane();

	Resolution m_resolution;
	number     m_fieldOfView;
	Position   m_position;
	Direction  m_direction;
	Direction  m_worldUp;

	// view plane
	Direction  m_up;
	Direction  m_right;

};

#endif