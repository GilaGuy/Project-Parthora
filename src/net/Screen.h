#ifndef SCREEN_H
#define SCREEN_H

#include "Protocol.h"

struct Client;

struct Screen
{
	Cross checkBeyondBoundaries(sf::Vector2f coords) const;
	Cross checkBeyondScreens(sf::Vector2f coords) const;

	Screen* prev;
	Screen* next;
	Client* owner;

	sf::Vector2u size;
	float boundaryLeft, boundaryRight;
};

class ScreenList
{
public:
	ScreenList();
	~ScreenList();

	Screen* add();
	Screen* getFirst();
	Screen* getLast();
	Screen* get(ClientID ownerID);
	bool rem(ClientID ownerID);
	size_t count();
	void clear();

private:
	Screen* m_first;
	Screen* m_last;
	size_t m_count;
};

#endif /* SCREEN_H */
