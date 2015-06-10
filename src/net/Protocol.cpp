/**
 * Protocol specifications.
 *
 * @date       April 18, 2015
 *
 * @revisions
 *
 * @designer   Melvin Loho
 *
 * @programmer Melvin Loho
 *
 * @notes      This file contains all of the common things that will be used by the server and client.
 */

#include "Protocol.h"

// HELPER FUNCTIONS ------------------------------------------------------------

Cross checkBeyondBoundaries(sf::Vector2f coords, const Screen& s)
{
	if (coords.x < s.boundaryLeft)
	{
		return CROSS_LEFT;
	}
	else if (coords.x > s.boundaryRight)
	{
		return CROSS_RIGHT;
	}
	else
	{
		return NO_CROSS;
	}
}

Cross checkBeyondScreens(sf::Vector2f coords, const Screen& s)
{
	if (coords.x < 0)
	{
		return CROSS_LEFT;
	}
	else if (coords.x > s.size.x)
	{
		return CROSS_RIGHT;
	}
	else
	{
		return NO_CROSS;
	}
}

//-----------------------------------------------------------------------------<
