#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL.h>
#include <algorithm>
#include <iostream>

#include "light.hpp"
#include "utility.hpp"

int main(int _argc, char *_argv[])
{
	std::vector<std::unique_ptr<light_t>> lights;
	perception perceived;
	inner_state state;

	SDL_Surface *screen = nullptr;
	bool running = true;
	SDL_Event e;

	srand(time(nullptr));

	while(lights.size() < 100)
	{
		std::unique_ptr<light_t> light(new light_t());
		light->x = (rand() % (800 - 32)) + 1;
		light->y = (rand() % (600 - 32)) + 1;
		light->occupied = false;

		bool valid = true;

		for(auto &other : lights)
		{
			if(distance(light->x, light->y, other->x, other->y) < 32)
			{
				valid = false;
				break;
			}
		}

		if(valid)
			lights.push_back(std::move(light));
	}

	for(auto &node : lights)
	{
		struct element
		{
			light_t *node;
			float dist;
		};

		std::vector<element> neighbours;

		for(auto &other : lights)
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
	for(auto &node : lights)
	{
		for(auto &neighbour : node->edges)
		{
			neighbour->edges.push_back(node.get());
		}
	}

	state.position = lights.back().get();
	state.state = state_normal;
	state.aggression = 0;

	perceived.last_step = nullptr;
	perceived.danger = false;
	perceived.step = false;
	
	std::vector<alife::entity<inner_state, perception>> entities;

	SDL_Init(SDL_INIT_EVERYTHING);
	screen = SDL_SetVideoMode(800, 600, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);

	unsigned int nextThink = millis();
	unsigned int thinkDelay = 1000 / 1;
	unsigned int stepNumber = 0;

	unsigned int nextDraw = millis();
	unsigned int drawDelay = 1000 / 30;

	unsigned int lastDanger = 0;
	unsigned int dangerTimeout = 15000;

	unsigned int spawnDelay = 1000;
	unsigned int nextSpawn = millis() + spawnDelay;

	while(running)
	{
		while(SDL_PollEvent(&e)) {
			if(e.type == SDL_QUIT)
				running = false;
			else if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
			{
				std::vector<alife::entity<inner_state,perception>>::iterator dead;
				bool clicked = false;

				for(auto entity = std::begin(entities); entity != std::end(entities); entity++)
				{
					if(distance(entity->state().position->x+16, entity->state().position->y+16, e.button.x, e.button.y) < 16)
					{
						perceived.step = true;
						perceived.last_step = entity->state().position;
						perceived.danger = true;
						lastDanger = millis();

						std::cout << "danger occurred at (" << (entity->state().position->x + 16)
							<< ", " << (entity->state().position->y + 16) << ")" << std::endl;

						clicked = true;
						dead = entity;
					}
				}

				if(clicked) {
					dead->state().position->occupied = false;
					entities.erase(dead);
				}
				else
				{
					for(auto &light : lights)
					{
						if(distance(light->x+16, light->y+16, e.button.x, e.button.y) < 16)
						{
							perceived.last_step = light.get();
							std::cout << "danger occurred at (" << (light->x + 16)
								<< ", " << (light->y + 16) << ")" << std::endl;
						}
					}
				}
			}
		}

		if(perceived.danger && (millis() - lastDanger) >= dangerTimeout)
			perceived.danger = false;

		if(millis() >= nextSpawn)
		{
			nextSpawn += spawnDelay;

			int which_light = (rand() % lights.size());
			if(entities.size() < 15 && !lights[which_light]->occupied) {
				state.position = lights[which_light].get();
				state.position->occupied = true;
				auto mind = std::unique_ptr<light_mind>(new light_mind());
				alife::entity<inner_state, perception> ent(std::move(mind), state, perceived);
				entities.push_back(std::move(ent));
			}
		}

		if(millis() >= nextThink)
		{
			nextThink += thinkDelay;

			std::cout << "step " << stepNumber++ << std::endl;

			for(auto &entity : entities) {
				entity.update_perception(perceived);
				entity.think();
			}

			perceived.step = false;
		}

		if(millis() >= nextDraw)
		{
			nextDraw = millis() + drawDelay;

			SDL_FillRect(screen, nullptr, SDL_MapRGB(screen->format, 200,200,200));

			for(auto &node : lights)
			{
				for(light_t *edge : node->edges)
				{
					lineRGBA(screen, node->x+16, node->y+16, edge->x+16, edge->y+16, 25,25,25, 255);
				}
			}

			for(auto &node : lights)
			{
				if(node.get() == perceived.last_step)
					filledCircleRGBA(screen, node->x+16, node->y+16, 16, 150,150,150, 255);
				else
					filledCircleRGBA(screen, node->x+16, node->y+16, 16, 50,50,50, 255);
				if(node->occupied)
					circleRGBA(screen, node->x+16, node->y+16, 16, 255,0,0, 255);
			}

			for(auto &entity : entities) {
				int entColour = 0;
				switch(entity.state().state)
				{
				case state_normal: entColour = 0x0000FFFF; break;
				case state_afraid: entColour = 0xFFFF00FF; break;
				case state_angry:  entColour = 0xFF0000FF; break;
				}
				filledCircleColor(screen, entity.state().position->x+16, entity.state().position->y+16, 12, entColour);
			}

			SDL_Flip(screen);
		}
	}

	return 0;
}
