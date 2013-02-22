#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <vector>

#include "entity.hpp"

enum light_state
{
	state_normal,
	state_afraid,
	state_angry
};

struct light_t
{
	std::vector<light_t*> edges;
	bool occupied;
	int x, y;
};

struct inner_state
{
	light_state state;
	light_t *position;
	int aggression;
	int fear;
};

struct perception
{
	light_t *last_step;
	bool danger;
	bool step;
};

class light_mind : public alife::mind<inner_state, perception>
{
public:
	const static int aggression_threshold = 2;
	virtual std::unique_ptr<alife::mind<inner_state, perception>> think(inner_state &_state, const perception &_perception) override;
};

#endif
