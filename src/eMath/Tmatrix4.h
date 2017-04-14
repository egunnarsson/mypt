
#ifndef LMATH_MATRIX4_H
#define LMATH_matrix44_H

#include "Tvector4.h"
#include "Tvector3.h"
#include <cmath>
#include <cassert>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

template <typename T>
class matrix4
{
public:
	vector4<T> c1;
	vector4<T> c2;
	vector4<T> c3;
	vector4<T> c4;

	matrix4() {};

	matrix4(vector4<T> _c1, vector4<T> _c2, vector4<T> _c3, vector4<T> _c4) : c1(_c1), c2(_c2), c3(_c3), c4(_c4) {}

	matrix4(
		T m11, T m12, T m13, T m14,
		T m21, T m22, T m23, T m24,
		T m31, T m32, T m33, T m34,
		T m41, T m42, T m43, T m44)
	{
		c1 = vector4<T>(m11, m21, m31, m41);
		c2 = vector4<T>(m12, m22, m32, m42);
		c3 = vector4<T>(m13, m23, m33, m43);
		c4 = vector4<T>(m14, m24, m34, m44);
	}
	
	vector4<T>& operator [] (const size_t i)
	{
		assert(i < 4);
		return *(&c1 + i);
	}

	const vector4<T> row(size_t i) const
	{
		assert(i < 4);
		switch(i) {
		case 0: return vector4<T>(c1.x, c2.x, c3.x, c4.x);
		case 1: return vector4<T>(c1.y, c2.y, c3.y, c4.y);
		case 2: return vector4<T>(c1.z, c2.z, c3.z, c4.z);
		case 3: return vector4<T>(c1.w, c2.w, c3.w, c4.w);
		}
		// throw....
		assert(false);
		return vector4<T>(0,0,0,0);
	}

	const bool operator == (const matrix4 &m) const
	{
		return c1 == m.c1 && c2 == m.c2 && c3 == m.c3 && c4 == m.c4;
	}

	const bool operator != (const matrix4 &m) const
	{
		return !(m == *this);
	}


	const matrix4 operator - (const matrix4 &m) const
	{
		return matrix4(c1 - m.c1, c2 - m.c2, c3 - m.c3, c4 - m.c4);
	}

	const matrix4 operator - (const T f) const
	{
		return matrix4(c1 - f, c2 - f, c3 - f, c4 - f);
	}

	const matrix4 operator + (const matrix4 &m) const
	{
		return matrix4(c1 + m.c1, c2 + m.c2, c3 + m.c3, c4 + m.c4);
	}

	const matrix4 operator + (const T f) const
	{
		return matrix4(c1 + f, c2 + f, c3 + f, c4 + f);
	}

	const vector4<T> operator * (const vector4<T>& v) const
	{
		return vector4<T>(
			c1.x * v.x + c2.x * v.y + c3.x * v.z + c4.x * v.w,
			c1.y * v.x + c2.y * v.y + c3.y * v.z + c4.y * v.w,
			c1.z * v.x + c2.z * v.y + c3.z * v.z + c4.z * v.w,
			c1.w * v.x + c2.w * v.y + c3.w * v.z + c4.w * v.w);
	}

	const matrix4 operator * (const T f) const
	{
		return matrix4(c1 * f, c2 * f, c3 * f, c4 * f); 
	}

	const matrix4 operator * (const matrix4 &m) const
	{
		vector4<T> r0 = row(0);
		vector4<T> r1 = row(1);
		vector4<T> r2 = row(2);
		vector4<T> r3 = row(3);
		return matrix4(
			r0.dot(m.c1), r0.dot(m.c2), r0.dot(m.c3), r0.dot(m.c4),
			r1.dot(m.c1), r1.dot(m.c2), r1.dot(m.c3), r1.dot(m.c4),
			r2.dot(m.c1), r2.dot(m.c2), r2.dot(m.c3), r2.dot(m.c4),
			r3.dot(m.c1), r3.dot(m.c2), r3.dot(m.c3), r3.dot(m.c4));
	}

	const matrix4 operator / (const matrix4 &m) const
	{
		return matrix4(c1 / m.c1, c2 / m.c2, c3 / m.c3, c4 / m.c4);
	}

	const matrix4 operator / (const T f) const
	{
		return (*this)*(1/f);
	}
	
	const matrix4 transpose() const
	{
		return matrix4(
			c1.x, c1.y, c1.z, c1.w,
			c2.x, c2.y, c2.z, c2.w,
			c3.x, c3.y, c3.z, c3.w,
			c4.x, c4.y, c4.z, c4.w);
	}

	T determinant() const
	{
		matrix3<T> s1 = matrix3<T>(
			c2.y, c3.y, c4.y,
			c2.z, c3.z, c4.z,
			c2.w, c3.w, c4.w);

		matrix3<T> s2 = matrix3<T>(
			c3.y, c4.y, c1.y,
			c3.z, c4.z, c1.z,
			c3.w, c4.w, c1.w);

		matrix3<T> s3 = matrix3<T>(
			c4.y, c1.y, c2.y,
			c4.z, c1.z, c2.z,
			c4.w, c1.w, c2.w);

		matrix3<T> s4 = matrix3<T>(
			c1.y, c2.y, c3.y,
			c1.z, c2.z, c3.z,
			c1.w, c2.w, c3.w);
  
		T det1 = s1.determinant();
		T det2 = s2.determinant();
		T det3 = s3.determinant();
		T det4 = s4.determinant();

		return c1.x * det1 - c2.x * det2 + c3.x * det3 - c4.x * det4;
	}
	
	matrix4 inverse() const 
	{
		T v0 = c1.z * c2.w - c2.z * c1.w;
		T v1 = c1.z * c3.w - c3.z * c1.w;
		T v2 = c1.z * c4.w - c4.z * c1.w;
		T v3 = c2.z * c3.w - c3.z * c2.w;
		T v4 = c2.z * c4.w - c4.z * c2.w;
		T v5 = c3.z * c4.w - c4.z * c3.w;

		T t00 = + (v5 * c2.y - v4 * c3.y + v3 * c4.y);
		T t10 = - (v5 * c1.y - v2 * c3.y + v1 * c4.y);
		T t20 = + (v4 * c1.y - v2 * c2.y + v0 * c4.y);
		T t30 = - (v3 * c1.y - v1 * c2.y + v0 * c3.y);

		T invDet = 1.0f / (t00 * c1.x + t10 * c2.x + t20 * c3.x + t30 * c4.x);

		T d00 = t00 * invDet;
		T d10 = t10 * invDet;
		T d20 = t20 * invDet;
		T d30 = t30 * invDet;

		T d01 = - (v5 * c2.x - v4 * c3.x + v3 * c4.x) * invDet;
		T d11 = + (v5 * c1.x - v2 * c3.x + v1 * c4.x) * invDet;
		T d21 = - (v4 * c1.x - v2 * c2.x + v0 * c4.x) * invDet;
		T d31 = + (v3 * c1.x - v1 * c2.x + v0 * c3.x) * invDet;

		v0 = c1.y * c2.w - c2.y * c1.w;
		v1 = c1.y * c3.w - c3.y * c1.w;
		v2 = c1.y * c4.w - c4.y * c1.w;
		v3 = c2.y * c3.w - c3.y * c2.w;
		v4 = c2.y * c4.w - c4.y * c2.w;
		v5 = c3.y * c4.w - c4.y * c3.w;

		T d02 = + (v5 * c2.x - v4 * c3.x + v3 * c4.x) * invDet;
		T d12 = - (v5 * c1.x - v2 * c3.x + v1 * c4.x) * invDet;
		T d22 = + (v4 * c1.x - v2 * c2.x + v0 * c4.x) * invDet;
		T d32 = - (v3 * c1.x - v1 * c2.x + v0 * c3.x) * invDet;

		v0 = c2.z * c1.y - c1.z * c2.y;
		v1 = c3.z * c1.y - c1.z * c3.y;
		v2 = c4.z * c1.y - c1.z * c4.y;
		v3 = c3.z * c2.y - c2.z * c3.y;
		v4 = c4.z * c2.y - c2.z * c4.y;
		v5 = c4.z * c3.y - c3.z * c4.y;

		T d03 = - (v5 * c2.x - v4 * c3.x + v3 * c4.x) * invDet;
		T d13 = + (v5 * c1.x - v2 * c3.x + v1 * c4.x) * invDet;
		T d23 = - (v4 * c1.x - v2 * c2.x + v0 * c4.x) * invDet;
		T d33 = + (v3 * c1.x - v1 * c2.x + v0 * c3.x) * invDet;

		return matrix4(
			vector4<T>(d00, d10, d20, d30),
			vector4<T>(d01, d11, d21, d31),
			vector4<T>(d02, d12, d22, d32),
			vector4<T>(d03, d13, d23, d33));
	}


	
	static const matrix4 make_identity()
	{
		return matrix4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
	}

	template<class vector>
	static const matrix4 make_rotation(vector axis, T angle)
	{
		matrix4 res = make_identity();
		axis.normalize();

		T sinA = sin(angle);
		T cosA = cos(angle);
		T cosI = 1 - cosA;

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

	template<class vector>
	static const matrix4 make_scale(vector scale)
	{
		return matrix4(
			scale.x, 0, 0, 0,
			0, scale.y, 0, 0,
			0, 0, scale.z, 0,
			0, 0, 0, 1);
	}

	static const matrix4 make_scale(T scale)
	{
		return make_scale(vector3<T, SCALE_TYPE>(scale, scale, scale));
	}


	template<class vector>
	static const matrix4 make_translation(vector translation)
	{
		matrix4 res = make_identity();
		res.c4 = vector4<T>(translation.x, translation.y, translation.z, 1);
		return res;
	}

	static const matrix4 make_frustum(T left, T right, T bottom, T top,	T znear, T zfar)
	{
		T temp = 2.0f * znear;
		T temp2 = right - left;
		T temp3 = top - bottom;
		T temp4 = zfar - znear;
		
		return matrix4(
			vector4<T>(temp / temp2, 0.0f, 0.0f, 0.0f),
			vector4<T>(0.0f, temp / temp3, 0.0f, 0.0f),
			vector4<T>((right + left) / temp2, (top + bottom) / temp3, (-zfar - znear) / temp4, -1.0f),
			vector4<T>(0.0f, 0.0f, (-temp * zfar) / temp4, 0.0f));
	}

	static const matrix4 make_perspective(T fov, T aspect_ratio, T near, T far)
	{
		T ymax = near * tan(fov * T(M_PI / 360.0));
		T xmax = ymax * aspect_ratio;
		return make_frustum(-xmax, xmax, -ymax, ymax, near, far);	
	}

};

#endif