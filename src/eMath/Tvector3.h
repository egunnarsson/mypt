
#ifndef LMATH_VECTOR3_H
#define LMATH_VECTOR3_H

#include <cmath>
#include <cassert>

#define INL __forceinline

enum VectorType {
	DIRECTION_TYPE,
	POSITION_TYPE,
	COLOR_TYPE,
	SCALE_TYPE,
	SIZE_TYPE
};

template <class T, VectorType type>
class vector3
{
public:
	T x;
	T y;
	T z;

	template<VectorType type2>
	INL static vector3 cast(vector3<T, type2> v)
	{
		return vector3(v.x, v.y, v.z);
	}

	template<class U>
	INL static vector3 castT(const vector3<U, type> &other)
	{
		return vector3(static_cast<T>(other.x), static_cast<T>(other.y), static_cast<T>(other.z));
	}

	INL vector3() {}
	INL vector3(T v) : x(v), y(v), z(v) {}
	INL vector3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}

	INL const T& operator [] (const size_t i) const
	{
		assert(i < 3);
		return *(&x + i);
	}

	INL const T& operator [] (const unsigned int i) const
	{
		assert(i < 3);
		return *(&x + i);
	}

	INL const T& operator [] (const int i) const
	{
		assert(i < 3 && i >= 0);
		return *(&x + i);
	}

	INL T& operator [] (const size_t i)
	{
		assert(i < 3);
		return *(&x + i); 
	}

	INL T& operator [] (const unsigned int i)
	{
		assert(i < 3);
		return *(&x + i);
	}
	
	INL T& operator [] (const int i)
	{
		assert(i < 3 && i >= 0);
		return *(&x + i);
	}

	INL void put(T *v)
	{
		v[0] = x;
		v[1] = y;
		v[2] = z;
	}
	
	INL bool operator == (const vector3& v) const
	{
		return (v.x == x) && (v.y == y) && (v.z == z);
	}

	INL bool operator != (const vector3& v) const
	{
		return !(v == *this);
	}
  
	INL vector3 operator - () const
	{
		return vector3(-x, -y, -z);
	}

	INL const vector3& operator = (const vector3& v)
	{
		x = v.x; 
		y = v.y;
		z = v.z;
		return *this;
	}

	INL const vector3& operator += (const vector3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	INL const vector3& operator += (const T& s)
	{
		x += s;
		y += s;
		z += s;
		return *this;
	}

	INL const vector3& operator -= (const vector3& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	INL const vector3& operator -= (const T& s)
	{
		x -= s;
		y -= s;
		z -= s;
		return *this;
	}

	INL const vector3& operator *= (const vector3& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	INL const vector3& operator *= (const T& s)
	{
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}

	INL const vector3& operator /= (const vector3& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	INL const vector3& operator /= (const T& s)
	{
		x /= s;
		y /= s;
		z /= s;
		return *this;
	}
	
	INL vector3 operator + (const T &s) const
	{
		return vector3(x + s, y + s, z + s);
	}

	INL vector3 operator + (const vector3& v) const
	{
		return vector3(x + v.x, y + v.y, z + v.z);
	}


	INL vector3 operator - (const T &s) const
	{
		return vector3(x - s, y - s, z - s);
	}

	INL vector3 operator - (const vector3 &v) const
	{
		return vector3(x - v.x, y - v.y, z - v.z);
	}

  
	INL vector3 operator * (const T &s) const
	{
		return vector3(x * s, y * s, z * s);
	}

	INL vector3 operator * (const vector3 &v) const
	{
		return vector3(x * v.x, y * v.y, z * v.z);
	}


	INL vector3 operator / (const T &s) const
	{
		T t = 1 / s;
		return vector3(x * t, y * t, z * t);
	}

	INL vector3 operator / (const vector3 &v) const
	{
		return vector3(x / v.x, y / v.y, z / v.z);
	}


	INL T length() const
	{
		return sqrt(dot(*this));
	}

	INL T lengthSquared() const
	{
		return dot(*this);
	}

	INL vector3& normalize()
	{
		T s = lengthSquared();
		if (s > 0) {
			s = 1 / sqrt(s);
			x *= s;
			y *= s;
			z *= s;
		}
		return *this;
	}

	/*void normalize()
	{

	}

	vector3 normalized()
	{

	}*/

	INL T dot(const vector3 &v) const
	{
		return x * v.x + y * v.y + z * v.z;
	}

	INL vector3 cross(const vector3 &v) const
	{
		return vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};

/*
template <class T>
class tDirection : public vector3<T, DIRECTION_TYPE>
{
public:
	tDirection() {}
	tDirection(const vector3<T, DIRECTION_TYPE> &other) : vector3<T, DIRECTION_TYPE>(other) {}
	tDirection(T x, T y, T z) : vector3<T, DIRECTION_TYPE>(x, y, z) {}
};*/

#endif