#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include "player.h"
#include "platform.h"
#include "contactlistener.h"
#include <sinxml/sinxml.h>

typedef std::vector<Platform*> Platforms;

void Load(const char* filename, Platforms* platforms)
{
	for(Platforms::iterator i = platforms->begin(); i!=platforms->end(); ++i)
	{
		delete *i;
	}
	platforms->clear();
	
	sinxml::Document doc("1.0");
	doc.Load_file(filename);
	sinxml::Children& mapchildren = doc.Get_root()->Get_children();
	for(sinxml::Children::iterator i = mapchildren.begin(); i != mapchildren.end(); ++i)
	{
		Platform* platform = new Platform;
		sinxml::Children& platformchildren = (*i)->Get_children();
		for(sinxml::Children::iterator j = platformchildren.begin(); j != platformchildren.end(); ++j)
		{
			float x = sinxml::fromstring<float>((*j)->Get_child("x")->Get_value());
			float y = sinxml::fromstring<float>((*j)->Get_child("y")->Get_value());
			platform->Add_collision_vertex(b2Vec2(x, y));
		}
		platforms->push_back(platform);
	}
}

int main(int argc, const char* argv[])
{	
	al_init();

	al_init_image_addon();

	ALLEGRO_DISPLAY *display;
	al_set_new_display_flags(ALLEGRO_WINDOWED);
	display = al_create_display(800, 600);
	
	al_install_keyboard();

	ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
	al_register_event_source(event_queue, (ALLEGRO_EVENT_SOURCE *)display);
	al_register_event_source(event_queue, al_get_keyboard_event_source());

	b2Vec2 gravity(0.0f, 40.0f);
	b2World world(gravity, true);

	Player player;
	player.Create_body(&world);

	Platforms platforms;
	if(argc==2)
	{
		Load(argv[1], &platforms);
		for(Platforms::iterator i = platforms.begin(); i!=platforms.end(); ++i)
			(*i)->Create_body(world);
	}
	else
	{
		Load("data/test.map", &platforms);
		for(Platforms::iterator i = platforms.begin(); i!=platforms.end(); ++i)
			(*i)->Create_body(world);
	}
	
	ContactListener cl;
	world.SetContactListener(&cl);

	float acctime = 0;
	float32 timeStep = 1.0f / 60.0f;
	
	b2Vec2 camera(0.0f, 0.0f);

	float last_update = al_current_time();
	float dt;

	while(1)
	{
		float current_time = al_current_time();
		dt = current_time - last_update;
		last_update = current_time;
		
		ALLEGRO_EVENT event;
		if (al_get_next_event(event_queue, &event))
		{
			if (ALLEGRO_EVENT_DISPLAY_CLOSE == event.type ||
					ALLEGRO_EVENT_KEY_DOWN == event.type &&
					ALLEGRO_KEY_ESCAPE == event.keyboard.keycode)
			{
				break;
			}
			player.Event(event);
		}

		acctime += dt;
		while(acctime>timeStep)
		{
			player.Update(timeStep);

			acctime-=timeStep;
			world.Step(timeStep, 6, 2);
			world.ClearForces();

			camera = 10*player.Get_position();
			camera.x -= 400;
			camera.y -= 300;
		}


		for(Platforms::iterator i = platforms.begin(); i!=platforms.end(); ++i)
			(*i)->Draw(camera);
//		platform.Draw(camera);
		player.Draw(camera);
		al_flip_display();
		al_clear_to_color(al_map_rgb(0, 0, 0));

		al_rest(0.001);
	}

	al_destroy_event_queue(event_queue);
	al_destroy_display(display);
}
