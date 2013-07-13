#include "headers.hpp"

#ifndef CLASSES_HPP
#define CLASSES_HPP

enum ShipCollState { VUL, INVUL };
enum GameStatus { PLAYING, WIN, LOSE };
enum Direction  { STAND, UP, DOWN, LEFT, RIGHT };
enum Reverse { REVERSE_T, REVERSE_B, REVERSE_L, REVERSE_R };

class ScreenBounds 
{
public:
	ScreenBounds(int top, int bottom, int left, int right) :
		topBound(top), bottomBound(bottom), leftBound(left), rightBound(right)
	{
	}
	~ScreenBounds()
	{
	}

	int getTop() const
	{
		return topBound;
	}

	int getBottom() const
	{
		return bottomBound;
	}

	int getLeft() const
	{
		return leftBound;
	}

	int getRight() const
	{
		return rightBound;
	}

private:
	int topBound, bottomBound, leftBound, rightBound;

};

class GameChar
{
public:
	GameChar(float x, float y)
	{
		this->x = x;
		this->y = y;

		dx = dy = 0.0;
		ms = 5.0;
		hp = 1.0;
		active = true;
		collided = false;

		pic = NULL;
	}

	GameChar()
	{
		x = y = dx = dy = 0.0;
		ms = 5.0;

		active = true;
		collided = false;

		pic = NULL;
	}

	virtual ~GameChar()
	{
	}

	virtual void move() = 0;

	virtual void decHp() = 0;

	void setPic(ALLEGRO_BITMAP* bmp)
	{
		pic = bmp;
	}

	void updatePosition()
	{
		x += dx;
		y += dy;
	}

	void setActive(bool active)
	{
		this->active = active;
	}


	float getX() const
	{
		return x;
	}

	float getY() const
	{
		return y;
	}

	float getMS() const
	{
		return ms;
	}
	
	
	float getXWidth() const
	{
		return this->x + al_get_bitmap_width(this->pic);
	}

	float getYHeight() const
	{
		return this->y + al_get_bitmap_height(this->pic);
	}
	
	bool getActive() const
	{
		return this->active;
	}

	ALLEGRO_BITMAP* getBmp() const
	{
		return pic;
	}

	bool collides(const GameChar& collidee)
	{
		if( x + al_get_bitmap_width(pic) < collidee.getX() || 
			x > collidee.getX() + al_get_bitmap_width(collidee.getBmp()) || 
			y + al_get_bitmap_height(pic) < collidee.getY() || 
			y > collidee.getY() + al_get_bitmap_height(collidee.getBmp()) )
		{
			// no collision
			return false;
		}

		else 
		{
			int char1_w = al_get_bitmap_width(pic);
			int char2_w = al_get_bitmap_width(collidee.getBmp());
			int char1_h = al_get_bitmap_height(pic);
			int char2_h = al_get_bitmap_height(collidee.getBmp());

			int top = max(y, collidee.getY());
			int bottom = min(y + char1_h, collidee.getY() + char2_h);
			int left = max(x, collidee.getX());
			int right = min(x + char1_w, collidee.getX() + char2_w);

			for(int i = top; i < bottom; i++)
			{
				for(int j = left; j < right; j++)
				{
					al_lock_bitmap(pic, al_get_bitmap_format(pic), ALLEGRO_LOCK_READONLY);
					al_lock_bitmap(collidee.getBmp(), al_get_bitmap_format(collidee.getBmp()), ALLEGRO_LOCK_READONLY);

					ALLEGRO_COLOR color1 = al_get_pixel(pic, j - x, i - y);
					ALLEGRO_COLOR color2 = al_get_pixel(collidee.getBmp(), j - collidee.getX(), i - collidee.getY());

					if(color1.a != 0 && color2.a != 0)
					{
						return true;
					}
				}
			}

			return false;
		}
	}

protected:
	float x, y, dx, dy, ms, hp;
	bool active;
	bool collided;
	ALLEGRO_BITMAP* pic;

};

class Bullet : public GameChar
{
public: 

	Bullet(float x, float y, ALLEGRO_BITMAP* bullet, float hp) : GameChar(x, y)
	{
		this->pic = bullet;
		this->hp = hp;
		this->ms = 3;
	}

	void move()
	{
		this->dx = ms;
	}

	void decHp()
	{
		this->hp -= 1.0;

		if( hp == 0.0 )
		{
			this->active = false;
		}
	}
};

class Meteor : public GameChar
{
public:
	Meteor(float x, float y) : GameChar(x,y)
	{
		this->ms = 3.0;
		this->hp = 3.0;
		this->explodingCounter = 0;
		this->exploding = false;
	}

	void move()
	{
		if(!exploding)
			this->dx = 0 - ms;
		else
		{
			if(explodingCounter < (explosion.size() - 1))
			{
				this->setPic(explosion[explodingCounter]);
				explodingCounter++;
			}
			else
			{
				this->active = false; 
			}
		}
	}

	void decHp()
	{
		this->hp -= 1.0;

		if( hp == 0.0 )
		{
			this->exploding = true;
			this->ms = 0;
		}
	}

	void setExplosionPic(const std::vector<ALLEGRO_BITMAP*>& explosionPics)
	{
		this->explosion = explosionPics;
	}

	void setExploding(bool exploding)
	{
		this->exploding = exploding;
	}

private:
	unsigned int explodingCounter;
	bool exploding;
	std::vector<ALLEGRO_BITMAP*> explosion;
};

class Ship : public GameChar
{
public:
	Ship(float x, float y) : GameChar(x, y)
	{
		ms = 5.0;
		dir = STAND;
		life = 3;
		collState = VUL;
		collStateDur = 0; // 0 is equal to 'it stays like that'
	}

	void fire( std::vector<Bullet>& bulletCont, ALLEGRO_BITMAP* bullet, float hp = 1.0)
	{
		bulletCont.push_back(Bullet(getXWidth(), (getYHeight() + y) / 2, bullet, hp));
	}

	void decHp()
	{
		this->hp -= 1.0;

		if( hp == 0.0 )
		{
			this->life -= 1;

			if( life < 0 )
				this->active = false;
			else
			{
				hp = 1.0;
				collState = INVUL;
				collStateDur = 240;
			}
		}
	}

	void move()
	{
		switch(dir)
		{
		case UP:
			this->dx = 0;
			this->dy = -ms;
			break;
		case DOWN:
			this->dx = 0;
			this->dy = ms;
			break;
		case LEFT:
			this->dx = -ms;
			this->dy = 0;
			break;
		case RIGHT:
			this->dx = ms;
			this->dy = 0;
			break;
		case STAND:
			this->dx = this->dy = 0.0;
			break;
		}

	}

	void reverseMove(Reverse r)
	{
		switch(r)
		{
		case REVERSE_L:
			x += ms;
			break;
		case REVERSE_R:
			x += -ms;
			break;
		case REVERSE_T:
			y += ms;
			break;
		case REVERSE_B:
			y += -ms;
			break;
		}
	}

	void decInvulTimer()
	{
		if( collStateDur > 0 )
			collStateDur--;
		else 
		{
			collState = VUL;
		}
	}

	void setDir(Direction dir)
	{
		this->dir = dir;
	}

	ShipCollState getCollState() const
	{
		return collState;
	}

	bool isActive() const
	{
		return this->active;
	}

	int getLifeCount() const
	{
		return this->life;
	}

private:
	Direction dir;
	int life;
	ShipCollState collState;
	int collStateDur;
};


#endif

