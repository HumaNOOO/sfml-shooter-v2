#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Bullet.hpp"

enum WeaponType
{
	RIFLE,
	SNIPER,
	GRENADE_LAUNCHER
};

class Player
{
public:
	Player(sf::Vector2f pos = { 666.f,398.f }, float radius = 20.f, sf::Color color = sf::Color(0, 255, 0)) : RADIUS{ radius }
	{
		player.setPosition(pos);
		player.setFillColor(color);
		player.setRadius(radius);
		player.setOrigin(radius, radius);
		pistol.setSize({ 20.f,5.f });
		pistol.setOrigin(0.f, 3.f);
		buffer.loadFromFile("gunshot.wav");
		gun_sound.setBuffer(buffer);
		gun_sound.setVolume(0.5);
		player_weapon = RIFLE;
	}

	inline sf::Vector2f getPosition()
	{
		return player.getPosition();
	}

	void setPlayerWeaponType(WeaponType weaponType)
	{
		player_weapon = weaponType;
	}

	inline void draw(sf::RenderTarget& r)
	{
		r.draw(player);
		r.draw(pistol);
	}

	void resetFollowDelay()
	{
		delay_to_follow = 0.5f;
	}

	inline float getFollowDelay()
	{
		return delay_to_follow;
	}

	void update(sf::Window& w, float dt, sf::Vector2f& mp, std::string& str)
	{
		shotCooldown -= dt;
		delay_to_follow -= dt;
		sf::Vector2f playerPos = player.getPosition();
		float angle_degrees = atan2f(mp.y - playerPos.y, mp.x - playerPos.x) / PI * 180.f;
		pistol.setRotation(angle_degrees);
		pistol.setPosition({ playerPos.x + cosf(angle_degrees * PI / 180.f) * RADIUS / 2.f, playerPos.y + sinf(angle_degrees * PI / 180.f) * RADIUS / 2.f });
		if (player_weapon == SNIPER)
		{
			str.append("\nweapon: SNIPER");
		}
		else if (player_weapon == RIFLE)
		{
			str.append("\nweapon: RIFLE");
		}
		else if (player_weapon == GRENADE_LAUNCHER)
		{
			str.append("\nweapon: GRENADE LAUNCHER");
		}
	}

	inline void move(float x, float y)
	{
		player.move(x, y);
	}

	inline float getShotCooldown()
	{
		return shotCooldown;
	}

	inline WeaponType getPlayerWeaponType()
	{
		return player_weapon;
	}

	void shoot(std::vector<Bullet>& bullets)
	{
		if (player_weapon == RIFLE)
		{
			shotCooldown = 0.04f;
			bullets.push_back(Bullet(pistol.getPosition(), pistol.getRotation(), 1500.f, 5.f, 1));
		}
		else if (player_weapon == SNIPER)
		{
			shotCooldown = 1.f;
			bullets.push_back(Bullet(pistol.getPosition(), pistol.getRotation(), 5000.f, 40.f, 100));
		}
		else if (player_weapon == GRENADE_LAUNCHER)
		{
			shotCooldown = 3.f;
			bullets.push_back(Bullet(pistol.getPosition(), pistol.getRotation(), 1000.f, 5.f, 1, GRENADE_LAUNCHER));
		}
		//gun_sound.play();
	}

	inline float getPistiolRotationRadians()
	{
		return pistol.getRotation() * PI / 180.f;
	}

	inline sf::Vector2f getPistolPosition()
	{
		return pistol.getPosition();
	}

private:
	const float RADIUS;
	float delay_to_follow = 0.5f;
	sf::Sound gun_sound;
	sf::SoundBuffer buffer;
	float shotCooldown = 0.04f;
	sf::CircleShape player;
	sf::RectangleShape pistol;
	WeaponType player_weapon;
};