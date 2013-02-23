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

inline int random_between(int _min, int _max)
{
	return ((rand() % (_max - _min)) + _min);
}

struct timer
{
	std::function<void()> tick;
	unsigned int next;
	unsigned int delay;

	inline timer(unsigned int _delay)
		: next(millis()), delay(_delay), tick(nullptr) {}
	inline timer(unsigned int _delay, std::function<void()> _tick)
		: next(millis()), delay(_delay), tick(_tick) {}

	inline void reset() {
		next = millis() + delay;
	}
	inline void think() {
		think(tick);
	}
	inline void think(std::function<void()> _tick) {
		if(millis() >= next) {
			next += delay;
			_tick();
		}
	}
};

#endif
