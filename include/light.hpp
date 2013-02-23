#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <vector>

#include "entity.hpp"

enum light_colour
{
	light_blue,
	light_yellow,
	light_red
};

struct light_t
{
	std::vector<light_t*> edges;
	bool occupied;
	int x, y;
};

typedef std::unique_ptr<light_t> light_ptr;
typedef std::vector<light_ptr> light_vector;

struct inner_state
{
	light_colour colour;
	light_t *position;
	int move_counter;
	int aggression;
	int fear;

	inline inner_state()
		: colour(light_blue), position(nullptr), move_counter(0), aggression(0), fear(0) {}
};

struct perception
{
	light_t *last_step;
	bool danger;
	bool step;

	inline perception()
		: last_step(nullptr), danger(false), step(false) {}
};

typedef alife::entity<inner_state, perception> light_entity;
typedef std::unique_ptr<light_entity> light_entity_ptr;
typedef std::vector<light_entity_ptr> light_entity_vector;

typedef alife::mind<inner_state, perception> light_mind;
typedef std::unique_ptr<light_mind> light_mind_ptr;

class light_mind_normal : public light_mind
{
public:
	const static int move_divider = 5;
	virtual light_mind_ptr think(inner_state &_state, const perception &_perception) override;
};

class light_mind_afraid : public light_mind
{
public:
	const static int move_divider = 1;
	const static int aggression_threshold = 2;
	const static int anger_multiplier = 20;

	virtual light_mind_ptr think(inner_state &_state, const perception &_perception) override;
};

class light_mind_angry : public light_mind
{
public:
	const static int move_divider = 2;
	virtual light_mind_ptr think(inner_state &_state, const perception &_perception) override;
};

light_vector make_lights(int _number, int _width, int _height);
void connect_lights(light_vector &_lights);
light_entity_vector::iterator check_entity_clicked(light_entity_vector &_entities, int _mousex, int _mousey);
light_t *check_light_clicked(const light_vector &_lights, int _mousex, int _mousey);

#endif
