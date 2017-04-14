
#ifndef LMATH_VECTOR4_H
#define LMATH_VECTOR4_H

#include <cmath>
#include <cassert>

#include "Tvector3.h"

template <typename T>
class vector4
{
public:
	T x;
	T y;
	T z;
	T w;

	vector4() : x(0), y(0), z(0), w(0) {}
	vector4(T _v) : x(_v), y(_v), z(_v), w(_v) {}

	vector4(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}
	template<class vector>
	vector4(vector v, T _w = 0) : x(v.x), y(v.y), z(v.z), w(_w) {}

	T& operator [] (const size_t i)
	{
		assert(i < 4);
		return *(&x + i); 
	}

	void put(T *v) {
		v[0] = x;
		v[1] = y;
		v[2] = z;
		v[3] = w;
	}

	bool operator == (const vector4& v) const
	{
		return (v.x == x) && (v.y == y) && (v.z == z) && (v.w == w);
	}

	bool operator != (const vector4& v) const
	{
		return !(v == *this);
	}
  
	const vector4 operator - () const
	{
		return vector4(-x, -y, -z, -w);
	}

	const vector4& operator = (const vector4& v)
	{
		x = v.x; 
		y = v.y;
		z = v.z;
		w = v.w;
		return *this;
	}

	const vector4& operator += (const vector4& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}

	const vector4& operator += (const T& s)
	{
		x += s;
		y += s;
		z += s;
		w += s;
		return *this;
	}

	const vector4& operator -= (const vector4& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}

	const vector4& operator -= (const T& s)
	{
		x -= s;
		y -= s;
		z -= s;
		w -= s;
		return *this;
	}

	const vector4& operator *= (const vector4& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= v.w;
		return *this;
	}

	const vector4& operator *= (const T& s)
	{
		x *= s;
		y *= s;
		z *= s;
		w *= s;
		return *this;
	}

	const vector4& operator /= (const vector4& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		w /= v.w;
		return *this;
	}

	const vector4& operator /= (const T& s)
	{
		x /= s;
		y /= s;
		z /= s;
		w /= s;
		return *this;
	}
	
	const vector4 operator + (const T &s) const
	{
		return vector4(x + s, y + s, z + s, w + s);
	}

	const vector4 operator + (const vector4& v) const
	{
		return vector4(x + v.x, y + v.y, z + v.z, w + v.w);
	}


	const vector4 operator - (const T &s) const
	{
		return vector4(x - s, y - s, z - s, w - s);
	}

	const vector4 operator - (const vector4 &v) const
	{
		return vector4(x - v.x, y - v.y, z - v.z, w - v.w);
	}

  
	const vector4 operator * (const T &s) const
	{
		return vector4(x * s, y * s, z * s, w * s);
	}

	const vector4 operator * (const vector4 &v) const
	{
		return vector4(x * v.x, y * v.y, z * v.z, w * v.w);
	}


	const vector4 operator / (const T &s) const
	{
		T t = 1 / s;
		return vector4(x * t, y * t, z * t, w * t);
	}

	const vector4 operator / (const vector4 &v) const
	{
		return vector4(x / v.x, y / v.y, z / v.z, w / v.w);
	}


	T length() const
	{
		return sqrt(dot(*this));
	}

	T lengthSquared() const
	{
		return dot(*this);
	}

	/*void normalize()
	{
		T s = lengthSquared();
		if (s > 0) {
			s = 1 / sqrt(s);
			x *= s;
			y *= s;
			z *= s;
			w *= s;
		}
	}

	vector4 normalized() const
	{
		vector4 v = *this;
		v.normalize();
		return v;
	}*/

	T dot(const vector4 &v) const
	{
		return x * v.x + y * v.y + z * v.z + w * v.w;
	}


};

#endif