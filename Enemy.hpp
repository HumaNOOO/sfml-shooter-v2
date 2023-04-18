#include "SFML/Graphics.hpp"

class Enemy
{
public:
	Enemy(sf::Vector2f pos)
	{
		mEnemy.setFillColor(sf::Color::Green);
		mEnemy.setRadius(10.f);
		mEnemy.setOrigin(10.f, 10.f);
		mEnemy.setPosition(pos);
		mShould_die = false;
	}

	inline float getRadius()
	{
		return mEnemy.getRadius();
	}

	inline bool shouldDie()
	{
		return mShould_die;
	}

	void die()
	{
		mShould_die = true;
	}

	inline sf::Vector2f getPosition() const
	{
		return mEnemy.getPosition();
	}

	inline void draw(sf::RenderTarget& rt)
	{
		rt.draw(mEnemy);
	}

private:
	sf::CircleShape mEnemy;
	bool mShould_die;
};