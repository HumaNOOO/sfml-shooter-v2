#include <SFML/Graphics.hpp>

static constexpr float PI = 3.141592f;

class Bullet
{
public:
	Bullet(sf::Vector2f pos, float rotation, float speed, float length, int piercePower, int projectileType = 0)
	{
		bullet.setSize({ length, 5.f });
		bullet.setFillColor(sf::Color::Red);
		bullet.setPosition(pos);
		bullet.setRotation(rotation);
		bullet.setOrigin(0.f, 3.f);
		float angleRadians = rotation * PI / 180.f;
		move_vector = { cosf(angleRadians) * speed, sinf(angleRadians) * speed };
		pierce_count = piercePower;
		projectile_type = projectileType;
	}

	inline float getLifetime() const
	{
		return lifetime;
	}

	inline void die()
	{
		lifetime = 0.f;
	}

	inline void modifyLifetime(float modifier)
	{
		lifetime += modifier;
	}

	inline sf::Vector2f getPosition()
	{
		return bullet.getPosition();
	}

	inline void move(float modifier)
	{
		bullet.move(move_vector * modifier);
	}

	inline void draw(sf::RenderTarget& rt)
	{
		rt.draw(bullet);
	}

	inline void modifyPierceCount(int modifier)
	{
		pierce_count += modifier;
		if (pierce_count <= 0)
		{
			die();
		}
	}

	inline int getProjectileType()
	{
		return projectile_type;
	}

private:
	int pierce_count;
	float lifetime = 0.55f; //seconds
	sf::RectangleShape bullet;
	sf::Vector2f move_vector;
	int projectile_type;
};