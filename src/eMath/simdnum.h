
#include <assert.h>

#include <emmintrin.h>

class _declspec(align(16)) simdNum {
public:

	static simdNum True()
	{
		return simdNum((int)0xffffffff);
	}

	static simdNum Zero()
	{
		return simdNum(_mm_setzero_ps());
	}

	union 
	{
		__m128 data;
		const float f[4];
	};

	/*const float& operator [] (const size_t i) const
	{
		assert(i < 4);
		return *(&data + i);
	}

	const float& operator [] (const unsigned int i) const
	{
		assert(i < 3);
		return *(&x + i);
	}

	const float& operator [] (const int i) const
	{
		assert(i < 3 && i >= 0);
		return *(&x + i);
	}

	float& operator [] (const size_t i)
	{
		assert(i < 3);
		return *(&x + i);
	}

	float& operator [] (const unsigned int i)
	{
		assert(i < 3);
		return *(&x + i);
	}

	float& operator [] (const int i)
	{
		assert(i < 3 && i >= 0);
		return *(&x + i);
	}*/

	simdNum() {}
	simdNum(const simdNum &other) : data(other.data) {}
	simdNum(float a) : data(_mm_set_ps1(a)) {}
	simdNum(float a, float b, float c, float d) : data(_mm_set_ps(a, b, c, d)) {}
	simdNum(float *a) : data(_mm_load_ps(a)) {}
	simdNum(int a) : data(_mm_castsi128_ps(_mm_set1_epi32(a))) {}
	simdNum(int a, int b, int c, int d) : data(_mm_castsi128_ps(_mm_set_epi32(a, b, c, d))) {}
	simdNum(__m128 a) : data(a) {}
	
	const float& operator [] (const size_t i) const
	{
		assert(i < 4);
		return f[i];
	}

	simdNum operator - () const
	{
		return *this * simdNum(-1.0f);
	}

	const simdNum& operator = (const simdNum& n)
	{
		data = n.data;
		return *this;
	}

	const simdNum& operator += (const simdNum& n)
	{
		data = _mm_add_ps(data, n.data);
		return *this;
	}

	const simdNum& operator -= (const simdNum& n)
	{
		data = _mm_sub_ps(data, n.data);
		return *this;
	}

	const simdNum& operator *= (const simdNum& n)
	{
		data = _mm_mul_ps(data, n.data);
		return *this;
	}

	const simdNum& operator /= (const simdNum& n)
	{
		data = _mm_div_ps(data, n.data);
		return *this;
	}

	simdNum operator + (const simdNum& n) const
	{
		return simdNum(_mm_add_ps(data, n.data));
	}

	simdNum operator - (const simdNum &n) const
	{
		return simdNum(_mm_sub_ps(data, n.data));
	}

	simdNum operator * (const simdNum &n) const
	{
		return simdNum(_mm_mul_ps(data, n.data));
	}

	simdNum operator / (const simdNum &n) const
	{
		return simdNum(_mm_div_ps(data, n.data));
	}



	simdNum operator == (const simdNum& n) const
	{
		return simdNum(_mm_cmpeq_ps(data, n.data));
	}

	simdNum operator != (const simdNum& n) const
	{
		return simdNum(_mm_cmpneq_ps(data, n.data));
	}

	simdNum operator < (const simdNum& n) const
	{
		return simdNum(_mm_cmplt_ps(data, n.data));
	}

	simdNum operator <= (const simdNum& n) const
	{
		return simdNum(_mm_cmple_ps(data, n.data));
	}

	simdNum operator > (const simdNum& n) const
	{
		return simdNum(_mm_cmpgt_ps(data, n.data));
	}

	simdNum operator >= (const simdNum& n) const
	{
		return simdNum(_mm_cmpge_ps(data, n.data));
	}


	simdNum operator && (const simdNum& n) const
	{
		return simdNum(_mm_and_ps(data, n.data));
	}

	simdNum operator || (const simdNum& n) const
	{
		return simdNum(_mm_or_ps(data, n.data));
	}
	simdNum operator ^ (const simdNum& n) const
	{
		return simdNum(_mm_xor_ps(data, n.data));
	}

	simdNum operator ! () const
	{
		return simdNum(_mm_andnot_ps(data, True().data));
	}
};