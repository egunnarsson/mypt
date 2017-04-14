
#ifndef DEFINES_H
#define DEFINES_H

#include <algorithm>
#include <memory>
#include <limits>

#include "Tvector2.h"
#include "Tvector3.h"
#include "Tmatrix4.h"

typedef double number;
typedef vector2<number> vec2;
typedef vector2<int> point2D;
typedef vector4<number> vec4;
typedef matrix4<number> mat4;

typedef vector3<number, POSITION_TYPE> Position;
typedef vector3<number, DIRECTION_TYPE> Direction;
typedef vector3<number, COLOR_TYPE> Color;
typedef vector3<float, COLOR_TYPE> fColor;
typedef vector3<number, SCALE_TYPE> Scale;

template<class T>
class sp : public std::shared_ptr<T>
{
public:
	sp(T* ptr) : std::shared_ptr<T>(ptr) {}
};

//#define EPSILON 1e-5
//std::numeric_limits<number>::epsilon()

static_assert(std::numeric_limits<number>::has_infinity, "std::numeric_limits<number>::has_infinity failed");
#define NUM_INF std::numeric_limits<number>::infinity()
#define NUM_MAX std::numeric_limits<number>::max()
#define NUM_MIN std::numeric_limits<number>::lowest()
#define NUM_EPS 1e-5
//std::numeric_limits<number>::epsilon()

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

inline number toRadians(number x)
{
	return x * (number)(M_PI / 180.0);
}

inline number toDegrees(number x)
{
	return x * (number)(180.0 / M_PI);
}

inline number clamp(number x)
{
	return x < 0 ? 0 : x > 1 ? 1 : x;
}

template<class T>
T tmin(const T &a, const T &b)
{
	return a < b ? a : b;
}

template<class T>
T tmax(const T &a, const T &b)
{
	return a > b ? a : b;
}

inline Position vmin(const Position &v1, const Position &v2)
{
	return Position(tmin(v1.x, v2.x), tmin(v1.y, v2.y), tmin(v1.z, v2.z));
}

inline Position vmax(const Position &v1, const Position &v2)
{
	return Position(tmax(v1.x, v2.x), tmax(v1.y, v2.y), tmax(v1.z, v2.z));
}

struct Resolution {
	Resolution() : width(0), height(0) {}
	Resolution(unsigned int _w, unsigned int _h) : width(_w), height(_h) {}
	bool operator==(const Resolution &other) const { return width == other.width && height == other.height; }
	unsigned int count() const { return width * height; }
	unsigned int width, height;
};

struct AxisRotation {
	AxisRotation(Direction _axis, number _angle) : axis(_axis), angle(_angle) {}
	Direction axis;
	number    angle;
};

#endif
