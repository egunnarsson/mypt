
#ifndef LMATH_VECTOR2_H
#define LMATH_VECTOR2_H

#include <cmath>
#include <cassert>

template <typename T>
class vector2
{
public:
	T x;
	T y;

	template<class U>
	static vector2 castT(const vector2<U> &other)
	{
		return vector2(static_cast<T>(other.x), static_cast<T>(other.y));
	}

	vector2() : x(0), y(0) {}
	vector2(T _v) : x(_v), y(_v) {}
	vector2(T _x, T _y) : x(_x), y(_y) {}
	
	T& operator [] (const size_t i)
	{
		assert(i < 2);
		return *(&x + i); 
	}

	void put(T *v) {
		v[0] = x;
		v[1] = y;
	}

	bool operator == (const vector2& v) const
	{
		return (v.x == x) && (v.y == y);
	}

	bool operator != (const vector2& v) const
	{
		return !(v == *this);
	}
  
	const vector2 operator - () const
	{
		return vector2(-x, -y);
	}

	const vector2& operator = (const vector2& v)
	{
		x = v.x; 
		y = v.y;
		return *this;
	}

	const vector2& operator += (const vector2& v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	const vector2& operator += (const T& s)
	{
		x += s;
		y += s;
		return *this;
	}

	const vector2& operator -= (const vector2& v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	const vector2& operator -= (const T& s)
	{
		x -= s;
		y -= s;
		return *this;
	}

	const vector2& operator *= (const vector2& v)
	{
		x *= v.x;
		y *= v.y;
		return *this;
	}

	const vector2& operator *= (const T& s)
	{
		x *= s;
		y *= s;
		return *this;
	}

	const vector2& operator /= (const vector2& v)
	{
		x /= v.x;
		y /= v.y;
		return *this;
	}

	const vector2& operator /= (const T& s)
	{
		x /= s;
		y /= s;
		return *this;
	}
	
	const vector2 operator + (const T &s) const
	{
		return vector2(x + s, y + s);
	}

	const vector2 operator + (const vector2& v) const
	{
		return vector2(x + v.x, y + v.y);
	}


	const vector2 operator - (const T &s) const
	{
		return vector2(x - s, y - s);
	}

	const vector2 operator - (const vector2 &v) const
	{
		return vector2(x - v.x, y - v.y);
	}

  
	const vector2 operator * (const T &s) const
	{
		return vector2(x * s, y * s);
	}

	const vector2 operator * (const vector2 &v) const
	{
		return vector2(x * v.x, y * v.y);
	}


	const vector2 operator / (const T &s) const
	{
		T t = 1 / s;
		return vector2(x * t, y * t);
	}

	const vector2 operator / (const vector2 &v) const
	{
		return vector2(x / v.x, y / v.y);
	}


	T length() const
	{
		return sqrt(dot(*this));
	}

	T lengthSquared() const
	{
		return dot(*this);
	}

	void normalize()
	{
		T s = lengthSquared();
		if (s > 0) {
			s = 1 / sqrt(s);
			x *= s;
			y *= s;
		}
	}

	vector2 normalized() const
	{
		vector2 v = *this;
		v.normalize();
		return v;
	}

	T dot(const vector2 &v) const
	{
		return x * v.x + y * v.y;
	}

};

#endif