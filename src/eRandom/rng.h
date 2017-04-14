
#ifndef RNG_H
#define RNG_H

#include <random>
#include <limits>

template<class N>
class rng
{
public:

	rng(unsigned int seed) : m_generator(seed) {}

	N operator()() { return s_distribution(m_generator); }

private:

	// std::generate_canonical?
	static const std::uniform_real_distribution<N> s_distribution;
	//static const std::generate_canonical<N, 1, std::ranlux24_base> s_distribution;
	//std::minstd_rand m_generator;
	std::ranlux24_base m_generator;
};

template <typename N>
const std::uniform_real_distribution<N> rng<N>::s_distribution(0,1);

#endif
