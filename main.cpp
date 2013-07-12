#include "classes.hpp"

/**
	classes.hpp already includes
	headers.hpp
**/

const int screenX = 640;
const int screenY = 480;

const int topBound = screenY * 0.10;
const int bottomBound = screenY * 0.90;

const int leftBound = screenX * 0.10;
const int rightBound = screenX * 0.90;

bool isInMovableScreen(const Ship& ship)
{
	if( ship.getX() > leftBound && ship.getY() > topBound && ship.getYHeight() < bottomBound && ship.getXWidth() < rightBound )
		return true;

	return false;
}

extern void populateCoordinates(const char* filename, int permBatchIncrement, vector<vector<int> >& meteorCoords);

int main(int argc , char** argv)
{
	ALLEGRO_DISPLAY* display = NULL ;
	ALLEGRO_BITMAP* shipBmp = NULL;
	ALLEGRO_BITMAP* backg = NULL;
	ALLEGRO_BITMAP* small_bullet = NULL;
	ALLEGRO_BITMAP* large_bullet = NULL;
	ALLEGRO_BITMAP* large_meteor = NULL;
	ALLEGRO_FONT* dylan_32 = NULL;
	ALLEGRO_FONT* dylan_16 = NULL;

	if(!al_init())
	{
		return -1;
	}
    
	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_primitives_addon();
	al_set_app_name("Rocket Game");

	display = al_create_display(screenX, screenY);

	if(!display)
	{
		return -1;
	}

	bool startComplete = false, gameComplete = false, exitComplete = false;
	bool draw = false;
	GameStatus gameStatus = PLAYING;

	shipBmp = al_load_bitmap("space/ship.tga");
	backg = al_load_bitmap("space/background.tga");
	small_bullet = al_load_bitmap("space/small_bullet.tga");
	large_bullet = al_load_bitmap("space/large_bullet.tga");
	large_meteor = al_load_bitmap("space/large_asteroid.tga");
	
	dylan_32 = al_load_font("font/dylan.ttf", 32, NULL);
	dylan_16 = al_load_font("font/dylan.ttf", 16, NULL);
	
	Ship ship( screenX / 9, ( screenY / 2 ) - ( al_get_bitmap_height(shipBmp) / 2 ) );
	vector<Bullet> bullets;
	vector<Meteor> meteors;
	vector< vector<int> > meteorCoordinates;
	vector<ALLEGRO_BITMAP* > explodingLargeMeteor;

	populateCoordinates("data/MeteoriteCoords.txt", 1000, meteorCoordinates);

	ship.setPic(al_load_bitmap("space/ship.tga"));

	for(unsigned int i = 0; i < 4; i++)
	{
		std::stringstream s;
		s << "space/large_explosion_" << i << ".tga";
		explodingLargeMeteor.push_back(al_load_bitmap(s.str().c_str()));
	}

	for(unsigned int i = 0; i < meteorCoordinates.size(); i++)
	{
		Meteor m (meteorCoordinates[i][0], meteorCoordinates[i][1]);
		m.setPic(large_meteor);
		m.setExplosionPic(explodingLargeMeteor);
		meteors.push_back(m);
	}

	al_install_keyboard();

	ALLEGRO_EVENT_QUEUE* que = al_create_event_queue();
	ALLEGRO_EVENT evt;
	
	ALLEGRO_TIMER* starttimer = al_create_timer( 1 / 30.0 );
	ALLEGRO_TIMER* inputtimer = al_create_timer( 1 / 240.0 );
	ALLEGRO_TIMER* frametimer = al_create_timer( 1 / 60.0 );
	ALLEGRO_TIMER* exittimer = al_create_timer( 1 / 30.0 );

	ALLEGRO_KEYBOARD_STATE kstate;

	al_register_event_source(que, al_get_keyboard_event_source());
	al_register_event_source(que, al_get_display_event_source(display));
	al_register_event_source(que, al_get_timer_event_source(starttimer));
	al_register_event_source(que, al_get_timer_event_source(inputtimer));
	al_register_event_source(que, al_get_timer_event_source(frametimer));
	al_register_event_source(que, al_get_timer_event_source(exittimer));


	// introductory loop starts
	int titleLocY = screenY * 0.42;
	int playLocY = screenY * 0.625;
	int exitLocY = screenY * 0.70;
	int recWidth = 100, recHeight = 50;
	bool isPlay = true;

	al_start_timer(starttimer);

	while(!startComplete)
	{
		al_wait_for_event(que, &evt);

		if(evt.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			startComplete = true;
		}

		if(evt.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch(evt.keyboard.keycode)
			{
			case ALLEGRO_KEY_UP:
				isPlay = true;
				break;
			case ALLEGRO_KEY_DOWN:
				isPlay = false;
				break;
			case ALLEGRO_KEY_ENTER:
				if(isPlay)
				{
					startComplete = true;
				}
				else
				{
					startComplete = gameComplete = exitComplete = true;
				}
				break;
			}
		}

		al_draw_bitmap(backg, 0, 0, NULL);
		al_draw_text(dylan_32, al_map_rgb(0, 0, 255), screenX / 2, titleLocY, ALLEGRO_ALIGN_CENTER, "Asteroid Belt");
		al_draw_text(dylan_16, al_map_rgb(0, 124, 240), screenX / 2, playLocY, ALLEGRO_ALIGN_CENTER, "PLAY");  
		al_draw_text(dylan_16, al_map_rgb(0, 124, 240), screenX / 2, exitLocY, ALLEGRO_ALIGN_CENTER, "EXIT");
		
		if(isPlay)
			al_draw_rectangle( (screenX / 2) - (recWidth / 2), playLocY, (screenX / 2) + (recWidth / 2), playLocY + (recHeight / 2), al_map_rgb( 0, 255, 0), 1.0);
		else
			al_draw_rectangle( (screenX / 2) - (recWidth / 2), exitLocY, (screenX / 2) + (recWidth / 2), exitLocY + (recHeight / 2), al_map_rgb( 0, 255, 0), 1.0);

		al_flip_display();

		al_clear_to_color(al_map_rgb(255,255,255));

	}

	al_unregister_event_source(que, al_get_timer_event_source(starttimer));
	
	// game loop starts
	al_start_timer(inputtimer);
	al_start_timer(frametimer);

	while(!gameComplete)
	{
		al_wait_for_event(que, &evt);

		if(evt.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			gameComplete = true;
		}

		if(evt.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch(evt.keyboard.keycode)
			{
			case ALLEGRO_KEY_R:
				ship.fire(bullets, small_bullet);
				break;
			case ALLEGRO_KEY_F:
				ship.fire(bullets, large_bullet);
				break;
			}
		}

		if(evt.type == ALLEGRO_EVENT_TIMER)
		{
			if(evt.timer.source == inputtimer)
			{
				al_get_keyboard_state(&kstate);

				// set the direction of ship!!
				if(al_key_down(&kstate, ALLEGRO_KEY_W))
				{
					ship.setDir(UP);
				}
				else if(al_key_down(&kstate, ALLEGRO_KEY_S))
				{
					ship.setDir(DOWN);
				}
				else if(al_key_down(&kstate, ALLEGRO_KEY_A))
				{
					ship.setDir(LEFT);
				}
				else if(al_key_down(&kstate, ALLEGRO_KEY_D))
				{
					ship.setDir(RIGHT);
				}
				else
				{
					ship.setDir(STAND);
				}

			}

			if(evt.timer.source == frametimer)
			{
				// move everything past here
				// move the ship!!
				ship.move();
				ship.decInvulTimer(); // this will decrease invulnerability if it is

				// move the bullets!!
				for(unsigned int i = 0; i < bullets.size(); i++)
				{
					bullets[i].move();
				}

				// move the meteors!!
				for(unsigned int i = 0; i < meteors.size(); i++)
				{
					meteors[i].move();
				}

				// done moving

				// check if ships collides on meteors
				

				for(unsigned int i = 0; i < meteors.size(); i++)
				{
					if(ship.collides(meteors[i]))
					{
						// the meteor explodes upon contact with ship
						meteors[i].setExploding(true);
						// decrease ship's available life if it's vulnerable
						// and summon it at default loc
						if(ship.getCollState() == VUL)
							ship.decHp(); // dec hp and life as well
					}
				}

				// check bullets and meteors collision
				for(unsigned int i = 0; i < bullets.size(); i++)
				{
					for(unsigned int j = 0; j < meteors.size(); j++)
					{
						if(bullets[i].collides(meteors[j]))
						{
							bullets[i].decHp();
							meteors[j].decHp();
						}
					}
				}

				// disactivate bullets that go through right bound or have collided,active = false
				for(unsigned int i = 0; i < bullets.size(); i++)
				{
					if( bullets[i].getX() > rightBound )
						bullets[i].setActive(false);
				}

				// disactivate meteors that go through left bound
				for(unsigned int i = 0; i < meteors.size(); i++)
				{
					if( ( meteors[i].getX() + al_get_bitmap_width(meteors[i].getBmp()) ) < leftBound )
						meteors[i].setActive(false);
				}

				// delete inactive bullets and meteors
				auto iNewBulletIter = std::remove_if(bullets.begin(), bullets.end(), 
					[](const Bullet& bullet){ return !bullet.getActive();});
				bullets.erase(iNewBulletIter, bullets.end());

				auto iNewMeteorIter = std::remove_if(meteors.begin(), meteors.end(), 
					[](const Meteor& meteor){ return !meteor.getActive();});
				meteors.erase(iNewMeteorIter, meteors.end());

				ship.updatePosition();

				for(unsigned int i = 0; i < bullets.size(); i++)
				{
					bullets[i].updatePosition();
				}

				for(unsigned int i = 0; i < meteors.size(); i++)
				{
					meteors[i].updatePosition();
				}

				// check if no more meteor
				if( meteors.size() == 0 )
				{
					gameStatus = WIN;
					gameComplete = true;
				}
				else if( !ship.isActive() )
				{
					gameStatus = LOSE;
					gameComplete = true;
				}

				draw = true;
			}

		}

		if(draw)
		{
			al_draw_bitmap(backg, 0, 0, NULL);
			// al_draw_bitmap(ship.getBmp(), ship.getX(), ship.getY(), NULL);

			al_draw_tinted_bitmap(ship.getBmp(), ship.getCollState() == INVUL ? al_map_rgb(255,204,204) : al_map_rgb(255,255,255) , ship.getX(), ship.getY(), NULL);

			for(unsigned int i = 0; i < bullets.size(); i++)
			{
				al_draw_bitmap(bullets[i].getBmp(), bullets[i].getX(), bullets[i].getY() - al_get_bitmap_height(bullets[i].getBmp()) / 2, NULL);
			}

			for(unsigned int i = 0; i < meteors.size(); i++)
			{
				al_draw_bitmap(meteors[i].getBmp(), meteors[i].getX(), meteors[i].getY(), NULL);
			}

			std::stringstream s;
			s << "0" << ship.getLifeCount();

			al_draw_text(dylan_16, al_map_rgb(0,0,255), screenX * 0.1, screenX * 0.1, ALLEGRO_ALIGN_CENTER, s.str().c_str());

			al_flip_display();
			al_clear_to_color(al_map_rgb(255,255,255));

			draw = false;
		}
	}

	// unregister the game loop's timer
	al_unregister_event_source(que, al_get_timer_event_source(inputtimer));
	al_unregister_event_source(que, al_get_timer_event_source(frametimer));

	// start of exit timer

	al_start_timer(exittimer);

	while(!exitComplete)
	{
		al_wait_for_event(que, &evt);

		if(evt.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			exitComplete = true;
		}

		al_draw_bitmap(backg, 0, 0, NULL);

		switch(gameStatus)
		{
		case WIN:
			al_draw_text(dylan_32, al_map_rgb(0, 255, 0), screenX / 2, screenY * 0.25, ALLEGRO_ALIGN_CENTER, "YOU HAVE ESCAPED");
			al_draw_text(dylan_32, al_map_rgb(0, 255, 0), screenX / 2, screenY * 0.50, ALLEGRO_ALIGN_CENTER, "THE ASTEROID BELT!");
			break;
		case LOSE:
			al_draw_text(dylan_32, al_map_rgb(255, 0, 0), screenX / 2, screenY * 0.25, ALLEGRO_ALIGN_CENTER, "YOU HAVE DIED IN");
			al_draw_text(dylan_32, al_map_rgb(255, 0, 0), screenX / 2, screenY * 0.50, ALLEGRO_ALIGN_CENTER, "THE ASTEROID BELT!");
			break;
		}

		al_flip_display();
		al_clear_to_color(al_map_rgb(0,0,0));

	}

	al_destroy_display(display);

	return 0;

}
