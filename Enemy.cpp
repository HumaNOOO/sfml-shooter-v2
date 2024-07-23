#include "Enemy.h"

Enemy::Enemy(const sf::Vector2f& pos) : shouldDie_(false)
{
	enemy_.setFillColor(sf::Color::Green);
	enemy_.setRadius(10);
	enemy_.setOrigin(10, 10);
	enemy_.setPosition(pos);
}

float Enemy::getRadius() const
{
	return enemy_.getRadius();
}

bool Enemy::shouldDie() const
{
	return shouldDie_;
}

void Enemy::die()
{
	shouldDie_ = true;
}

const sf::Vector2f& Enemy::getPosition() const
{
	return enemy_.getPosition();
}

void Enemy::draw(sf::RenderTarget& rt) const
{
	rt.draw(enemy_);
}
