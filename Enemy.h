#pragma once
#include <SFML/Graphics.hpp>

class Enemy
{
public:
	Enemy(const sf::Vector2f& pos);

	float getRadius() const;
	bool shouldDie() const;
	void die();
	const sf::Vector2f& getPosition() const;
	void draw(sf::RenderTarget& rt) const;

private:
	sf::CircleShape enemy_;
	bool shouldDie_;

};
