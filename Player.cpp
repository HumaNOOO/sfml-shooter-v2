#include "Player.h"

Player::Player(const sf::Vector2f pos, const float radius, const sf::Color color) : radius_(radius), cameraFollowDelay_(0.5f), shotCooldown_(0.04f), weaponType_(WeaponType::Rifle)
{
	player_.setPosition(pos);
	player_.setFillColor(color);
	player_.setRadius(radius);
	player_.setOrigin(radius, radius);
	weapon_.setSize({ 20.f,5.f });
	weapon_.setOrigin(0.f, 3.f);
	soundBuffer_.loadFromFile("gunshot.wav");
	gunSound_.setBuffer(soundBuffer_);
	gunSound_.setVolume(0.5);
}

const sf::Vector2f& Player::getPosition() const
{
	return player_.getPosition();
}

void Player::setWeaponType(const WeaponType weaponType)
{
	weaponType_ = weaponType;
}

void Player::draw(sf::RenderTarget& rt) const
{
	rt.draw(player_);
	rt.draw(weapon_);
}

void Player::resetFollowDelay()
{
	cameraFollowDelay_ = 0.5f;
}

float Player::getFollowDelay() const
{
	return cameraFollowDelay_;
}

void Player::update(const float dt, const sf::Vector2f& mp, std::string& str)
{
	shotCooldown_ -= dt;
	cameraFollowDelay_ -= dt;
	const sf::Vector2f playerPos = player_.getPosition();
	const float angleDegrees = atan2f(mp.y - playerPos.y, mp.x - playerPos.x) / PI * 180.f;
	weapon_.setRotation(angleDegrees);
	weapon_.setPosition({ playerPos.x + cosf(angleDegrees * PI / 180.f) * radius_ / 2.f, playerPos.y + sinf(angleDegrees * PI / 180.f) * radius_ / 2.f });

	switch (weaponType_)
	{
	case WeaponType::Rifle:
		str.append("\nweapon: Rifle");
		break;
	case WeaponType::Sniper:
		str.append("\nweapon: Sniper");
		break;
	case WeaponType::GrenadeLauncher:
		str.append("\nweapon: Grenade launcher");
	}
}

void Player::move(const float x, const float y)
{
	player_.move(x, y);
}

float Player::getShotCooldown() const
{
	return shotCooldown_;
}

void Player::shoot(std::vector<Bullet>& bullets)
{
	switch (weaponType_)
	{
	case WeaponType::Rifle:
		shotCooldown_ = 0.04f; //0.04f
		bullets.emplace_back(weapon_.getPosition(), weapon_.getRotation(), 1500.f, 5.f, 1);
		break;
	case WeaponType::Sniper:
		shotCooldown_ = 0.04f; //1.f
		bullets.emplace_back(weapon_.getPosition(), weapon_.getRotation(), 5000.f, 40.f, 100);
		break;
	case WeaponType::GrenadeLauncher:
		shotCooldown_ = 0.04f; //0.5f
		bullets.emplace_back(weapon_.getPosition(), weapon_.getRotation(), 1000.f, 5.f, 1, weaponType_);
	}
}

float Player::getWeaponRotationRadians() const
{
	return weapon_.getRotation() * PI / 180.f;
}

const sf::Vector2f& Player::getWeaponPosition() const
{
	return weapon_.getPosition();
}
