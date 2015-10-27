/**
 * Screen and its manager.
 *
 * @date       October 26, 2015
 *
 * @revisions
 *
 * @designer   Melvin Loho
 *
 * @programmer Melvin Loho
 *
 * @notes      Represents a client's screen.
 *             It is also used as a doubly linked list element for easy management by the ScreenManager.
 */

#include "Screen.h"

#include <assert.h>
#include "Client.h"

Cross Screen::checkBeyondBoundaries(sf::Vector2f coords) const
{
	if (coords.x < boundaryLeft)
	{
		return CROSS_LEFT;
	}
	else if (coords.x > boundaryRight)
	{
		return CROSS_RIGHT;
	}
	else
	{
		return NO_CROSS;
	}
}

Cross Screen::checkBeyondScreens(sf::Vector2f coords) const
{
	if (coords.x < 0)
	{
		return CROSS_LEFT;
	}
	else if (coords.x > size.x)
	{
		return CROSS_RIGHT;
	}
	else
	{
		return NO_CROSS;
	}
}

ScreenManager::ScreenManager() :
	m_first(nullptr),
	m_last(nullptr),
	m_count(0)
{}

ScreenManager::~ScreenManager()
{
	clear();
}

Screen* ScreenManager::add()
{
	Screen* newScreen = new Screen();
	newScreen->prev = nullptr;
	newScreen->next = nullptr;
	newScreen->owner = nullptr;

	if (m_count == 0)
	{
		m_first = newScreen;
		m_last = newScreen;
	}
	else
	{
		Screen* curr = m_first;

		while (curr->next) curr = curr->next;

		curr->next = newScreen;
		newScreen->prev = curr;

		m_last = newScreen;
	}

	++m_count;

	return newScreen;
}

Screen* ScreenManager::getFirst()
{
	return m_first;
}

Screen* ScreenManager::getLast()
{
	return m_last;
}

Screen* ScreenManager::get(ClientID ownerID)
{
	Screen* curr = m_first;

	while (curr)
	{
		if (curr->owner->id == ownerID) return curr;

		curr = curr->next;
	}

	return nullptr;
}

bool ScreenManager::rem(ClientID ownerID)
{
	Screen* toRemove = get(ownerID);

	if (!toRemove) return false;

	if (toRemove->prev)
		toRemove->prev->next = toRemove->next;
	if (toRemove->next)
		toRemove->next->prev = toRemove->prev;

	if (toRemove == m_first)
		m_first = toRemove->next;
	if (toRemove == m_last)
		m_last = toRemove->prev;

	delete toRemove; toRemove = nullptr;

	--m_count;

	return true;
}

size_t ScreenManager::count()
{
	return m_count;
}

void ScreenManager::clear()
{
	Screen* curr = m_first;

	size_t deleteCount = 0;

	while (curr)
	{
		curr = curr->next;
		delete curr->prev; curr->prev = nullptr;

		++deleteCount;
	}

	assert(deleteCount == m_count);

	m_first = nullptr;
	m_last = nullptr;
	m_count = 0;
}
