#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <ctime>
#include <cmath>

inline float distance(float ax, float ay, float bx, float by)
{
	auto dx = (ax - bx);
	auto dy = (ay - by);
	return sqrt(dx*dx + dy*dy);
}

inline int millis()
{
	return (clock() / (CLOCKS_PER_SEC / 1000));
}

#endif
