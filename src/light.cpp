#include <iostream>

#include "utility.hpp"
#include "light.hpp"

std::unique_ptr<alife::mind<inner_state, perception>> light_mind::think(inner_state &_state, const perception &_perception) {
	switch(_state.state)
	{
	case state_normal:
		{
			if(_perception.danger)
			{
				std::cout << "became afraid" << std::endl;
				_state.state = state_afraid;
				_state.aggression = 0;
				_state.fear = 10;
			}
			else
			{
				int neighbour = (rand() % _state.position->edges.size());
				if(_state.position->edges[neighbour]->occupied == false) {
					_state.position->occupied = false;
					_state.position = _state.position->edges[neighbour];
					_state.position->occupied = true;
					std::cout << "moved to (" << _state.position->x << ", " << _state.position->y << ")" << std::endl;
				}
			}
		} break;
	case state_afraid:
		{
			if(!_perception.danger) _state.fear --;
			if(_perception.step) _state.aggression ++;

			if(_state.aggression >= aggression_threshold)
			{
				_state.aggression *= 20;
				std::cout << "became angry" << std::endl;
				_state.state = state_angry;
			}
			else if(_state.fear <= 0)
			{
				std::cout << "stopped being afraid" << std::endl;
				_state.state = state_normal;
			}
			else
			{
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
		} break;
	case state_angry:
		{
			_state.aggression --;
			if(_state.aggression <= 0)
			{
				std::cout << "stopped being angry" << std::endl;
				_state.state = state_normal;
			}
			else
			{
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
		} break;
	}

	return std::unique_ptr<alife::mind<inner_state, perception>>(nullptr);
}
