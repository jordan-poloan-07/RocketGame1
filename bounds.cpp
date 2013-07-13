#include "classes.hpp"

void isInScreenBounds(Ship& ship, const ScreenBounds& screen)
{
	if( ship.getX() < screen.getLeft() )
	{
		ship.reverseMove(REVERSE_L);
	}
	else if( ship.getXWidth() > screen.getRight() )
	{
		ship.reverseMove(REVERSE_R);
	}
	else if( ship.getY() < screen.getTop() )
	{
		ship.reverseMove(REVERSE_T);	
	}
	else if( ship.getYHeight() > screen.getBottom() )
	{
		ship.reverseMove(REVERSE_B);
	}
}