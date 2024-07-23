#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>

#include "Bullet.h"

class Player
{
public:
	Player(sf::Vector2f pos = { 0.f, 0.f }, float radius = 20.f, sf::Color color = sf::Color(0, 0, 255));

	const sf::Vector2f& getPosition() const;
	void setWeaponType(WeaponType weaponType);
	void draw(sf::RenderTarget& rt) const;
	void resetFollowDelay();
	float getFollowDelay() const;
	void update(float dt, const sf::Vector2f& mp, std::string& str);
	void move(float x, float y);
	float getShotCooldown() const;
	void shoot(std::vector<Bullet>& bullets);
	float getWeaponRotationRadians() const;
	const sf::Vector2f& getWeaponPosition() const;

private:
	const float radius_;
	float cameraFollowDelay_;
	sf::Sound gunSound_;
	sf::SoundBuffer soundBuffer_;
	float shotCooldown_;
	sf::CircleShape player_;
	sf::RectangleShape weapon_;
	WeaponType weaponType_;

};
