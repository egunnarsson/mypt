
#ifndef LMATH_MATRIX3_H
#define LMATH_MATRIX3_H

#include "Tvector3.h"
#include <cmath>
#include <cassert>

template <typename T>
class matrix3
{
public:
	vector3<T> c1;
	vector3<T> c2;
	vector3<T> c3;

	matrix3() {}

	matrix3(vector3<T> _c1, vector3<T> _c2, vector3<T> _c3) : c1(_c1), c2(_c2), c3(_c3) {}

	matrix3(T m11, T m12, T m13, T m21, T m22, T m23, T m31, T m32, T m33)
	{
		c1 = vector3<T>(m11, m21, m31);
		c2 = vector3<T>(m12, m22, m32);
		c3 = vector3<T>(m13, m23, m33);
	}
	
	vector3<T>& operator [] (const size_t i)
	{
		assert(i < 3);
		return *(&c1 + i);
	}

	const vector3<T> row(size_t i) const
	{
		assert(i < 3);
		switch(i) {
		case 0: return vector3<T>(c1.x, c2.x, c3.x);
		case 1: return vector3<T>(c1.y, c2.y, c3.y);
		case 2: return vector3<T>(c1.z, c2.z, c3.z);
		}
		// throw....
		return vector3<T>(0,0,0);
	}

	const bool operator == (const matrix3 &m) const
	{
		return c1 == m.c1 && c2 == m.c2 && c3 == m.c3;
	}

	const bool operator != (const matrix3 &m) const
	{
		return !(m == *this);
	}


	const matrix3 operator - (const matrix3 &m) const
	{
		return matrix3(c1 - m.c1, c2 - m.c2, c3 - m.c3);
	}

	const matrix3 operator - (const T f) const
	{
		return matrix3(c1 - f, c2 - f, c3 - f);
	}

	const matrix3 operator + (const matrix3 &m) const
	{
		return matrix3(c1 + m.c1, c2 + m.c2, c3 + m.c3);
	}

	const matrix3 operator + (const T f) const
	{
		return matrix3(c1 + f, c2 + f, c3 + f);
	}

	const vector3<T> operator * (const vector3<T>& v) const
	{
		return vector3<T>(
			c1.x * v.x + c2.x * v.y + c3.x * v.z,  
			c1.y * v.x + c2.y * v.y + c3.y * v.z, 
			c1.z * v.x + c2.z * v.y + c3.z * v.z);
	}

	const matrix3 operator * (const T f) const
	{
		return matrix3(c1 * f, c2 * f, c3 * f); 
	}

	const matrix3 operator * (const matrix3 &m) const
	{
		vector3<T> r0 = row(0);
		vector3<T> r1 = row(1);
		vector3<T> r2 = row(2);
		return matrix3(
			r0.dot(c1), r0.dot(c2), r0.dot(c3), 
			r1.dot(c1), r1.dot(c2), r1.dot(c3), 
			r2.dot(c1), r2.dot(c2), r2.dot(c3));
	}

	const matrix3 operator / (const matrix3 &m) const
	{
		return matrix3(c1 / m.c1, c2 / m.c2, c3 / m.c3);
	}

	const matrix3 operator / (const T f) const
	{
		return (*this)*(1/f);
	}
	
	const matrix3 transpose() const
	{
		return matrix3(
			c1.x, c1.y, c1.z,
			c2.x, c2.y, c2.z, 
			c3.x, c3.y, c3.z);
	}

	const T determinant() const
	{
		return
			c1.x * c2.y * c3.z + 
			c2.x * c3.y * c1.z + 
			c3.x * c1.y * c2.z - 
			c1.x * c3.y * c2.z -
			c2.x * c1.y * c3.z - 
			c3.x * c2.y * c1.z;
	}
	
	matrix3 inverse() const
	{
		matrix3 m;
		assert(0);
		return m;
	}
	
	static const matrix3 make_identity()
	{
		return matrix3(
			1, 0, 0,
			0, 1, 0,
			0, 0, 1);
	}

	static const matrix3 make_rotation(vector3<T> axis, T angle)
	{
		matrix3 res;
		axis.normalize();

		T sinA = sinf(angle);
		T cosA = cosf(angle);
		T cosI = 1.0f - cosA;

		res[0][0] = (cosI * axis.x * axis.x) + (cosA);
		res[1][0] = (cosI * axis.x * axis.y) - (sinA * axis.z);
		res[2][0] = (cosI * axis.x * axis.z) + (sinA * axis.y);
		    
		res[0][1] = (cosI * axis.y * axis.x) + (sinA * axis.z);
		res[1][1] = (cosI * axis.y * axis.y) + (cosA);
		res[2][1] = (cosI * axis.y * axis.z) - (sinA * axis.x);
		    
		res[0][2] = (cosI * axis.z * axis.x) - (sinA * axis.y);
		res[1][2] = (cosI * axis.z * axis.y) + (sinA * axis.x);
		res[2][2] = (cosI * axis.z * axis.z) + (cosA);

		return res;
	}

	static const matrix3 make_scale(vector3<T> scale)
	{
		return matrix3(
			scale.x, 0, 0,
			0, scale.y, 0,
			0, 0, scale.z);
	}

	static const matrix3 make_scale(T scale)
	{
		return make_scale(vector3<T>(scale, scale, scale));
	}


};

#endif