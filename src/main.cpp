#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL.h>
#include <iostream>

#include "statefulmind.hpp"
#include "utility.hpp"
#include "light.hpp"

int main(int _argc, char *_argv[])
{
	light_entity_vector entities;
	unsigned int stepNumber = 0;
	perception perceived;
	light_vector lights;
	inner_state state;

	SDL_Surface *screen = nullptr;
	int width = 800, height = 600;
	bool running = true;
	SDL_Event e;

	timer tmrThink(1000 / 4);
	timer tmrDraw(1000 / 30);
	timer tmrSpawn(1000 / 1);
	timer tmrDanger(1000 * 15);

	srand(time(nullptr));

	lights = make_lights(100, width, height);
	connect_lights(lights);

	SDL_Init(SDL_INIT_EVERYTHING);
	screen = SDL_SetVideoMode(width, height, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);

	while(running)
	{
		while(SDL_PollEvent(&e)) {
			if(e.type == SDL_QUIT)
				running = false;
			else if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
			{
				light_entity_vector::iterator clicked = check_entity_clicked(entities, e.button.x, e.button.y);

				if(clicked != entities.end())
				{
					perceived.step = true;
					perceived.last_step = (*clicked)->state().position;
					perceived.danger = true;
					tmrDanger.reset();

					std::cout << "danger occurred at (" << ((*clicked)->state().position->x + 16)
						<< ", " << ((*clicked)->state().position->y + 16) << ")" << std::endl;

					(*clicked)->state().position->occupied = false;
					entities.erase(clicked);
				}
				else
				{
					light_t *clicked_light = check_light_clicked(lights, e.button.x, e.button.y);
					if(clicked_light != nullptr)
					{
						perceived.last_step = clicked_light;
						std::cout << "danger occurred at (" << (clicked_light->x + 16)
							<< ", " << (clicked_light->y + 16) << ")" << std::endl;
					}
				}
			}
		}

		tmrDanger.think([&]() {
			perceived.danger = false;
		});

		tmrSpawn.think([&]() {
			int which_light = (rand() % lights.size());

			if(entities.size() < 20 && !lights[which_light]->occupied) {
				state.position = lights[which_light].get();
				state.position->occupied = true;

				light_mind_ptr first_state(new light_mind_normal());
				light_mind_ptr mind(new alife::stateful_mind<inner_state,perception>(std::move(first_state)));

				light_entity_ptr ent(new light_entity(std::move(mind), state, perceived));

				entities.push_back(std::move(ent));
			}
		});

		tmrThink.think([&](){
			std::cout << "step " << stepNumber++ << std::endl;

			for(auto &entity : entities) {
				entity->update_perception(perceived);
				entity->think();
			}

			perceived.step = false;
		});

		tmrDraw.think([&]() {
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
				//if(node->occupied)
				//	circleRGBA(screen, node->x+16, node->y+16, 16, 255,0,0, 255);
			}

			for(auto &entity : entities) {
				int entColour = 0;
				switch(entity->state().colour)
				{
				case light_blue: entColour = 0x0000FFFF; break;
				case light_yellow: entColour = 0xFFFF00FF; break;
				case light_red:  entColour = 0xFF0000FF; break;
				}
				filledCircleColor(screen, entity->state().position->x+16, entity->state().position->y+16, 12, entColour);
			}

			SDL_Flip(screen);
		});
	}

	return 0;
}
