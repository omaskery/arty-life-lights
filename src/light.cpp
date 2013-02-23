#include <algorithm>
#include <iostream>

#include "utility.hpp"
#include "light.hpp"

light_mind_ptr light_mind_normal::think(inner_state &_state, const perception &_perception)
{
	light_mind_ptr result = nullptr;
	_state.colour = light_blue;

	_state.move_counter ++;
	if(_perception.danger)
	{
		result = light_mind_ptr(new light_mind_afraid());
		_state.aggression = 0;
		_state.fear = 10;
	}
	else if(_state.move_counter >= move_divider)
	{
		_state.move_counter = 0;
		int neighbour = (rand() % _state.position->edges.size());
		if(_state.position->edges[neighbour]->occupied == false) {
			_state.position->occupied = false;
			_state.position = _state.position->edges[neighbour];
			_state.position->occupied = true;
		}
	}

	return result;
}

light_mind_ptr light_mind_afraid::think(inner_state &_state, const perception &_perception)
{
	light_mind_ptr result = nullptr;
	_state.colour = light_yellow;

	_state.move_counter ++;
	if(!_perception.danger) _state.fear --;
	if(_perception.step) _state.aggression ++;

	if(_state.aggression >= aggression_threshold)
	{
		_state.aggression *= anger_multiplier;
		result = light_mind_ptr(new light_mind_angry());
	}
	else if(_state.fear <= 0)
	{
		result = light_mind_ptr(new light_mind_normal());
	}
	else if(_state.move_counter >= move_divider)
	{
		_state.move_counter = 0;
		light_t *furthest = nullptr;
		float furthest_distance;

		for(light_t *node : _state.position->edges)
		{
			if(node->occupied) continue;

			float check_distance = distance(node->x, node->y, _perception.last_step->x, _perception.last_step->y);
			if(furthest == nullptr || check_distance > furthest_distance)
			{
				furthest_distance = check_distance;
				furthest = node;
			}
		}

		if(furthest != nullptr) {
			_state.position->occupied = false;
			_state.position = furthest;
			_state.position->occupied = true;
		}
	}

	return result;
}

light_mind_ptr light_mind_angry::think(inner_state &_state, const perception &_perception)
{
	light_mind_ptr result = nullptr;
	_state.colour = light_red;

	_state.move_counter ++;
	_state.aggression --;
	if(_state.aggression <= 0)
	{
		_state.fear = 0;
		result = light_mind_ptr(new light_mind_normal());
	}
	else if(_state.move_counter >= move_divider)
	{
		_state.move_counter = 0;
		light_t *closest = nullptr;
		float closest_distance;

		for(light_t *node : _state.position->edges)
		{
			if(node->occupied) continue;

			float check_distance = distance(node->x, node->y, _perception.last_step->x, _perception.last_step->y);
			if(closest == nullptr || check_distance < closest_distance)
			{
				closest_distance = check_distance;
				closest = node;
			}
		}

		if(closest != nullptr) {
			_state.position->occupied = false;
			_state.position = closest;
			_state.position->occupied = true;
		}
	}

	return result;
}

bool lights_collide(const light_ptr &_a, const light_ptr &_b)
{
	return (distance(_a->x, _a->y, _b->x, _b->y) <= 32);
}

bool valid_light_position(const light_ptr &_light, const light_vector &_all)
{
	for(auto &other : _all)
	{
		if(lights_collide(_light, other))
			return false;
	}
	return true;
}

light_vector make_lights(int _number, int _width, int _height)
{
	light_vector lights;

	srand(time(nullptr));

	while(lights.size() < _number)
	{
		light_ptr light(new light_t());
		light->x = random_between(0, _width  - 32);
		light->y = random_between(0, _height - 32);
		light->occupied = false;

		if(valid_light_position(light, lights))
			lights.push_back(std::move(light));
	}

	return lights;
}

void connect_lights(light_vector &_lights)
{
	for(auto &node : _lights)
	{
		struct element
		{
			light_t *node;
			float dist;
		};

		std::vector<element> neighbours;

		for(auto &other : _lights)
		{
			if(node.get() == other.get()) continue;

			element el;
			el.dist = distance(node->x, node->y, other->x, other->y);
			el.node = other.get();

			neighbours.push_back(el);
		}

		std::sort(std::begin(neighbours), std::end(neighbours), [](const element &l, const element &r) -> bool {
			return (l.dist < r.dist);
		});

		for(int i = 0; i < 5; i++)
		{
			node->edges.push_back(neighbours[i].node);
		}
	}

	for(auto &node : _lights)
	{
		for(auto &neighbour : node->edges)
		{
			neighbour->edges.push_back(node.get());
		}
	}
}

light_entity_vector::iterator check_entity_clicked(light_entity_vector &_entities, int _mousex, int _mousey)
{
	for(auto entity = std::begin(_entities); entity != std::end(_entities); entity++)
	{
		if(distance((*entity)->state().position->x+16, (*entity)->state().position->y+16, _mousex, _mousey) < 16)
			return entity;
	}

	return _entities.end();
}

light_t *check_light_clicked(const light_vector &_lights, int _mousex, int _mousey)
{
	for(auto &light : _lights)
	{
		if(distance(light->x+16, light->y+16, _mousex, _mousey) < 16)
			return light.get();
	}
	return nullptr;
}
