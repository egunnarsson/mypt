
#ifndef LMATH_matrix2_H
#define LMATH_matrix2_H

#include "Tvector2.h"
#include <cmath>
#include <cassert>

template <typename T>
class matrix2
{
public:
	vector2<T> c1;
	vector2<T> c2;

	matrix2() {}

	matrix2(vector2<T> _c1, vector2<T> _c2) : c1(_c1), c2(_c2) {}

	matrix2(T m11, T m12, T m21, T m22)
	{
		c1 = vector2<T>(m11, m21);
		c2 = vector2<T>(m12, m22);
	}
	
	vector2<T>& operator [] (const size_t i)
	{
		assert(i < 2);
		return *(&c1 + i);
	}

	const vector2<T> row(size_t i) const
	{
		assert(i < 2);
		switch(i) {
		case 0: return vector2<T>(c1.x, c2.x);
		case 1: return vector2<T>(c1.y, c2.y);
		}
		return vector2<T>(0,0);
	}

	const bool operator == (const matrix2 &m) const
	{
		return c1 == m.c1 && c2 == m.c2;
	}

	const bool operator != (const matrix2 &m) const
	{
		return !(m == *this);
	}


	const matrix2 operator - (const matrix2 &m) const
	{
		return matrix2(c1 - m.c1, c2 - m.c2);
	}

	const matrix2 operator - (const T f) const
	{
		return matrix2(c1 - f, c2 - f);
	}

	const matrix2 operator + (const matrix2 &m) const
	{
		return matrix2(c1 + m.c1, c2 + m.c2);
	}

	const matrix2 operator + (const T f) const
	{
		return matrix2(c1 + f, c2 + f);
	}

	const vector2<T> operator * (const vector2<T>& v) const
	{
		return vector2<T>(
			c1.x * v.x + c2.x * v.y,
			c1.y * v.x + c2.y * v.y);
	}

	const matrix2 operator * (const T f) const
	{
		return matrix2(c1 * f, c2 * f); 
	}

	const matrix2 operator * (const matrix2 &m) const
	{
		//TODO
		return matrix2();
	}

	const matrix2 operator / (const matrix2 &m) const
	{
		return matrix2(c1 / m.c1, c2 / m.c2);
	}

	const matrix2 operator / (const T f) const
	{
		return (*this)*(1/f);
	}
	
	const matrix2 transpose() const
	{
		return matrix2(
			c1.x, c1.y,
			c2.x, c2.y);
	}

	const T determinant() const
	{
		assert(0);
		return 0;
	}
	
	
	static const matrix2 make_identity()
	{
		return matrix2(1, 0, 0, 1);
	}

	static const matrix2 make_rotation(T angle)
	{
		matrix2 res;

		assert(0);

		return res;
	}

	static const matrix2 make_scale(vector2<T> scale)
	{
		return matrix2(scale.x, 0, 0, scale.y);
	}

	static const matrix2 make_scale(T scale)
	{
		return make_scale(vector2<T>(scale, scale));
	}


};

#endif