#include "Bullet.h"

Bullet::Bullet(const sf::Vector2f& pos, const float rotation, const float speed, const float length, const int piercePower, const WeaponType projectileType) : piercePower_(piercePower), lifetime_(5.55f), projectileType_(projectileType)
{
	bullet_.setSize({ length, 5.f });
	bullet_.setFillColor(sf::Color::Red);
	bullet_.setPosition(pos);
	bullet_.setRotation(rotation);
	bullet_.setOrigin(0.f, 3.f);
	const float angleRadians = rotation * PI / 180.f;
	moveVector_ = { cosf(angleRadians) * speed, sinf(angleRadians) * speed };
}

float Bullet::getLifetime() const
{
	return lifetime_;
}

void Bullet::die()
{
	lifetime_ = 0;
}

void Bullet::modifyLifetime(const float modifier)
{
	lifetime_ += modifier;
}

const sf::Vector2f& Bullet::getPosition() const
{
	return bullet_.getPosition();
}

void Bullet::move(float speed)
{
	bullet_.move(moveVector_ * speed);
}

void Bullet::draw(sf::RenderTarget& rt) const
{
	rt.draw(bullet_);
}

void Bullet::modifyPiercePower(const int modifier)
{
	piercePower_ += modifier;

	if (piercePower_ <= 0)
	{
		die();
	}
}

WeaponType Bullet::getProjectileType() const
{
	return projectileType_;
}
