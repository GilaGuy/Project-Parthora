#ifndef TGO_H
#define TGO_H

#include "BGO.h"

class Renderer;

class TGO : public BGO
{
public:
	TGO();
	TGO(const sf::String& string, const sf::Font& font, unsigned int characterSize = 30);

	virtual ~TGO();

	sf::Text& text();
	const sf::Text& text() const;

	sf::Transform getLocalTransform() const override;

	void middleAnchorPoint(bool arg);

protected:
	void draw(Renderer& renderer, sf::RenderStates states) const override;

private:
	sf::Text data;
};

#endif // TGO_H
