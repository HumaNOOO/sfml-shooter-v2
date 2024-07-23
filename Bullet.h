#pragma once

#include <SFML/Graphics.hpp>

static constexpr float PI = 3.141592f;

enum class WeaponType
{
	Rifle,
	Sniper,
	GrenadeLauncher
};

class Bullet
{
public:
	Bullet(const sf::Vector2f& pos, float rotation, float speed, float length, int piercePower, WeaponType projectileType = WeaponType::Rifle);

	float getLifetime() const;
	void die();
	void modifyLifetime(float modifier);
	const sf::Vector2f& getPosition() const;
	void move(float speed);
	void draw(sf::RenderTarget& rt) const;
	void modifyPiercePower(int modifier);
	WeaponType getProjectileType() const;

private:
	int piercePower_;
	float lifetime_;
	sf::RectangleShape bullet_;
	sf::Vector2f moveVector_;
	WeaponType projectileType_;
};
