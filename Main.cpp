#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <cmath>
#include <chrono>
#include <ctime>
#include <random>
#include <deque>
#include <thread>
#include <atomic>
#include <iostream>
#include <string>

#include "Player.h"
#include "Enemy.h"

std::atomic<bool> exit_thread = false;
std::atomic<bool> update_collision = false;

float distanceTwoPoints(sf::Vector2f, sf::Vector2f);

class Batch
{
public:
	void updateCollision()
	{
		for (Bullet* bullet : bullets)
		{
			const sf::Vector2f bulletPos = bullet->getPosition();
			for (Enemy* enemy : enemies)
			{
				if (distanceTwoPoints(bulletPos, enemy->getPosition()) < enemy->getRadius() + 1.f)
				{
					bullet->modifyPiercePower(-1);
					enemy->die();
				}
			}
		}
	}

	inline void reset()
	{
		enemies.clear();
		bullets.clear();
	}

	inline void addEnemy(Enemy* enemy)
	{
		enemies.push_back(enemy);
	}

	inline void addBullet(Bullet* bullet)
	{
		bullets.push_back(bullet);
	}
private:
	std::vector<Enemy*> enemies;
	std::vector<Bullet*> bullets;
};

class EnemyVertexArray : public sf::Drawable, sf::Transformable
{
public:
	EnemyVertexArray(const std::vector<Enemy>& enemies)
	{
		vertices.setPrimitiveType(sf::Triangles);
		vertices.resize(enemies.size() * 30 * 3);

		while (endIndex < enemies.size() * 30 * 3)
		{
			for (size_t i = startIndex; i < endIndex; i += 3)
			{
				vertices[i].position = enemies[enemy].getPosition();
				vertices[i + 1].position = vertices[i].position + sf::Vector2f{ cosf(angle) * 10.f, sinf(angle) * 10.f };
				angle += ANGLE_INCREASE_RADIAN;
				vertices[i + 2].position = vertices[i].position + sf::Vector2f{ cosf(angle) * 10.f, sinf(angle) * 10.f };
			}

			startIndex = endIndex;
			endIndex = startIndex + 90;
			enemy++;
		}
	}

	void update(std::vector<Enemy>& enemies)
	{
		startIndex = 0;
		endIndex = startIndex + 90;
		enemy = 0;

		while (endIndex < enemies.size() * 30 * 3)
		{
			for (size_t i = startIndex; i < endIndex; i += 3)
			{
				vertices[i].position = enemies[enemy].getPosition();
				vertices[i + 1].position = vertices[i].position + sf::Vector2f{ cosf(angle) * 10.f, sinf(angle) * 10.f };
				angle += ANGLE_INCREASE_RADIAN;
				vertices[i + 2].position = vertices[i].position + sf::Vector2f{ cosf(angle) * 10.f, sinf(angle) * 10.f };
			}

			startIndex = endIndex;
			endIndex = startIndex + 90;
			enemy++;
		}
	}

private:
	virtual void draw(sf::RenderTarget& rt, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		rt.draw(vertices, states);
	}

	size_t startIndex = 0;
	size_t endIndex = startIndex + 90;
	const float ANGLE_INCREASE_RADIAN = 0.22439947525510914f;
	float angle = 0.f;
	size_t enemy = 0;
	sf::VertexArray vertices;
};

class Timer
{
public:
	Timer(long long& dur) : duration_{ dur }
	{
		start_ = std::chrono::steady_clock::now();
	}

	~Timer()
	{
		duration_ = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start_).count();
	}

private:
	std::chrono::time_point<std::chrono::steady_clock> start_;
	long long& duration_;
};

class EnemyVertexArrayOptimized : public sf::Drawable, public sf::Transformable
{
public:
	explicit EnemyVertexArrayOptimized(float radius, int sides) : RADIUS{ radius }, SIDES{ sides }, ANGLE_INCREASE_RADIAN{ 360.f / sides * PI / 180.f }
	{
		vertices.setPrimitiveType(sf::Triangles);
		for (size_t i = 0; i < SIDES + 1; i++)
		{
			points.push_back({ cosf(ANGLE_INCREASE_RADIAN * i) * RADIUS, sinf(ANGLE_INCREASE_RADIAN * i) * RADIUS });
		}
	}

	void update(const std::vector<Enemy>& enemies, const sf::Vector2f& screenCenter, int& enemiesDrawn, std::string& str)
	{
		long long duration;
		{
			Timer t(duration);
			vertices.clear();
			for (const Enemy& enemy : enemies)
			{
				if (enemy.getPosition().x >= screenCenter.x + 700 ||
					enemy.getPosition().x <= screenCenter.x - 700 ||
					enemy.getPosition().y >= screenCenter.y + 400 ||
					enemy.getPosition().y <= screenCenter.y - 400)
					continue;

				const sf::Vector2f enemyPos = enemy.getPosition();
				for (int i = 0; i < SIDES; i++)
				{
					sf::Vertex v1(enemyPos, sf::Color::Green);
					sf::Vertex v2(enemyPos + points[i], sf::Color::Green);
					sf::Vertex v3(enemyPos + points[i + 1], sf::Color::Green);
					vertices.append(v1);
					vertices.append(v2);
					vertices.append(v3);
				}
				++enemiesDrawn;
			}
		}
		str.append("\nevaoUpdate: " + std::to_string(duration) + " us");
	}
private:
	virtual void draw(sf::RenderTarget& rt, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		rt.draw(vertices, states);
	}

	std::vector<sf::Vector2f> points;
	const float ANGLE_INCREASE_RADIAN;
	const float RADIUS;
	const int SIDES;
	sf::VertexArray vertices;
};

inline float distanceTwoPoints(sf::Vector2f v1, sf::Vector2f v2)
{
	return sqrtf(powf(v1.x - v2.x, 2) + powf(v1.y - v2.y, 2));
}

inline sf::Vector2f toPlayerVector(sf::Vector2f playerPos, sf::Vector2f viewCenter, float dt, std::string& text)
{
	float dist = distanceTwoPoints(viewCenter, playerPos);
	float angleRad = atan2f(playerPos.y - viewCenter.y, playerPos.x - viewCenter.x);
	sf::Vector2f ret = { cosf(angleRad) * dt * 2.3f * dist, sinf(angleRad) * dt * 2.3f * dist };
	text.append("\nview movement vector: " + std::to_string(ret.x) + ", " + std::to_string(ret.y));
	return ret;
}

void updateBullets(float dt, std::string& text, std::vector<Bullet>& bullets)
{
	long long duration;
	{
		Timer t(duration);
		bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [&](const Bullet& bullet) { return bullet.getLifetime() <= 0.f; }), bullets.end());

		for (Bullet& bullet : bullets)
		{
			bullet.modifyLifetime(-dt);
			bullet.move(dt);
		}
	}
	text.append("\nupdateBullets: " + std::to_string(duration) + " us");
}

void updateEnemies(std::vector<Enemy>& enemies, std::string& text)
{
	long long duration;
	{
		Timer t(duration);
		enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [&](Enemy& enemy) { return enemy.shouldDie(); }), enemies.end());
	}
	text.append("updateEnemies: " + std::to_string(duration) + " us");
}

void updateCollision(float dt, std::vector<Enemy>& enemies, std::vector<Bullet>& bullets, std::string& text, std::vector<sf::CircleShape>& explosionCircles)
{
	long long duration;
	{
		Timer t(duration);
		for (Bullet& bullet : bullets)
		{
			const sf::Vector2f bulletPos = bullet.getPosition();
			for (Enemy& enemy : enemies)
			{
				if (distanceTwoPoints(bulletPos, enemy.getPosition()) < enemy.getRadius() + 1.f)
				{
					enemy.die();
					if (bullet.getProjectileType() == WeaponType::GrenadeLauncher)
					{
						sf::CircleShape cs;
						cs.setPosition(bulletPos);
						cs.setFillColor(sf::Color::Transparent);
						cs.setOutlineColor(sf::Color::White);
						cs.setOutlineThickness(4.f);
						explosionCircles.push_back(cs);

						for (Enemy& enemy1 : enemies)
						{
							if (distanceTwoPoints(bulletPos, enemy1.getPosition()) < 150.f)
							{
								enemy1.die();
							}
						}
					}
					bullet.modifyPiercePower(-1);
				}
			}
		}
	}
	text.append("\nupdateCollision: " + std::to_string(duration) + " us");
}

Batch batches[2];
void threadWorker()
{
	while (!exit_thread)
	{
		if (update_collision)
		{
			batches[0].updateCollision();
		}
		update_collision = false;
	}
	std::cout << "STOPING COLLISION THREAD!\n";
}

int main()
{
	int framesSinceLastAverage = 0;
	float avgDt = 0;
	float timerClock = 0.1f;
	int enemiesTotal = 0;
	int enemiesDrawn = 0;

	std::vector<sf::CircleShape> explosionCircles;
	explosionCircles.reserve(10);

	std::deque<sf::RectangleShape> frameTimes;
	std::vector<Bullet> bullets;
	bullets.reserve(2000);

	//std::thread collisionThread(threadWorker);

	srand(time(0));

	constexpr int WINDOW_WIDTH = 1360;
	constexpr int WINDOW_HEIGHT = 768;

	std::string textStr;

	std::vector<Enemy> enemies;
	enemies.reserve(10000);
	for (int i = 0; i < 9000; i++)
	{
		enemies.push_back(Enemy({ (float)(rand() % WINDOW_WIDTH * 5), (float)(rand() % WINDOW_HEIGHT * 5) }));
	}

	EnemyVertexArray enemyVertexArray{ enemies };

	sf::Font font;
	font.loadFromFile("mono.ttf");

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(20);

	int nearby200x;
	int nearby200y;

	sf::RectangleShape lineVertical;
	lineVertical.setFillColor(sf::Color(100, 100, 100));
	lineVertical.setSize({ 2.f,1000.f });

	sf::RectangleShape lineHorizontal;
	lineHorizontal.setSize({ 1600.f,2.f });
	lineHorizontal.setFillColor(sf::Color(100, 100, 100));

	sf::RectangleShape FOV({ 1360,768 });
	FOV.setFillColor(sf::Color::Transparent);
	FOV.setOutlineThickness(5);
	FOV.setOutlineColor(sf::Color::Red);
	FOV.setOrigin({ 680, 384 });

	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Shooter", sf::Style::Default);
	sf::Event e;
	Player player;
	player.move(WINDOW_WIDTH * 2.5f, WINDOW_HEIGHT * 2.5f);
	sf::Clock clk;
	sf::View view;

	view.reset(sf::FloatRect(0, 0, 1360, 768));
	view.setCenter(player.getPosition());

	bool zoomed = false;
	bool canZoom = true;
	bool drawLine = false;
	bool drawFOV = false;
	bool drawDebug = false;
	sf::Vertex line[2];

	EnemyVertexArrayOptimized evao(10.f, 16);

	while (window.isOpen())
	{
		float dt = clk.restart().asSeconds();

		while (window.pollEvent(e))
		{
			switch (e.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				if (e.key.code == sf::Keyboard::C)
				{
					drawLine = !drawLine;
				}

				if (e.key.code == sf::Keyboard::F3)
				{
					drawDebug = !drawDebug;
				}
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
		{
			if (canZoom)
			{
				if (!zoomed)
				{
					view.zoom(2.f);
					zoomed = true;
					drawFOV = true;
				}
				else
				{
					view.zoom(0.5f);
					zoomed = false;
					drawFOV = false;
				}
				canZoom = false;
			}
		}
		else
		{
			canZoom = true;
		}


		// INPUT

		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && player.getShotCooldown() <= 0.f) player.shoot(bullets);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) player.move(0, -200.f * dt);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) player.move(0, 200.f * dt);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) player.move(-200.f * dt, 0.f);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) player.move(200.f * dt, 0.f);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) player.setWeaponType(WeaponType::Rifle);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) player.setWeaponType(WeaponType::Sniper);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) player.setWeaponType(WeaponType::GrenadeLauncher);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
		{
			enemies.clear();
			for (int i = 0; i < 9000; i++)
			{
				enemies.push_back(Enemy({ (float)(rand() % WINDOW_WIDTH * 5), (float)(rand() % WINDOW_HEIGHT * 5) }));
			}
		}

		// UPDATE

		window.setTitle("Shooter FPS: " + std::to_string(1 / dt));
		const sf::Vector2f viewCenter = view.getCenter();

		//for (Enemy& enemy : enemies)
		//{
		//	if (enemy.getPosition().x > viewCenter.x)
		//	{
		//		batches[0].addEnemy(&enemy);
		//	}
		//	else if (enemy.getPosition().x <= viewCenter.x)
		//	{
		//		batches[1].addEnemy(&enemy);
		//	}
		//}
		//
		//for (Bullet& bullet : bullets)
		//{
		//	if (bullet.getPosition().x > viewCenter.x)
		//	{
		//		batches[0].addBullet(&bullet);
		//	}
		//	else if (bullet.getPosition().x <= viewCenter.x)
		//	{
		//		batches[1].addBullet(&bullet);
		//	}
		//}

		updateEnemies(enemies, textStr);
		update_collision = true;
		//batches[0].updateCollision();
		//batches[1].updateCollision();
		updateCollision(dt, enemies, bullets, textStr, explosionCircles);
		updateBullets(dt, textStr, bullets);
		evao.update(enemies, view.getCenter(), enemiesDrawn, textStr);
		//while (update_collision) {};
		//batches[0].reset();
		//batches[1].reset();

		for (sf::CircleShape& cs : explosionCircles)
		{
			float increase = cs.getRadius() + 1000.f * dt;

			cs.move(-1000.f * dt, -1000.f * dt);

			cs.setRadius(increase);
		}

		explosionCircles.erase(std::ranges::begin(std::ranges::remove_if(explosionCircles, [](const sf::CircleShape& cs) { return cs.getRadius() >= 150.f; })), explosionCircles.end());


		if (drawLine)
		{
			line[0].position = player.getWeaponPosition();
			float pistolRotation = player.getWeaponRotationRadians();
			line[1].position = { line[0].position.x + cosf(pistolRotation) * 2000.f, line[0].position.y + sinf(pistolRotation) * 2000.f };
		}
		window.setView(view);
		sf::Vector2f mp = window.mapPixelToCoords(sf::Mouse::getPosition(window));
		player.update(dt, mp, textStr);

		if (player.getFollowDelay() <= 0.f)
		{
			if (distanceTwoPoints(view.getCenter(), player.getPosition()) <= 1.f)
			{
				view.setCenter(player.getPosition());
				player.resetFollowDelay();
			}
			else
			{
				view.move(toPlayerVector(player.getPosition(), view.getCenter(), dt, textStr));
			}
		}

		static constexpr int DIV = 200;
		nearby200x = std::nearbyintf(view.getCenter().x / DIV) * DIV;
		nearby200y = std::nearbyintf(view.getCenter().y / DIV) * DIV;
		avgDt += dt;
		timerClock -= dt;
		framesSinceLastAverage++;

		FOV.setPosition(view.getCenter());

		if (timerClock <= 0.f)
		{
			sf::RectangleShape rect({ 2.f, -avgDt / framesSinceLastAverage * 10000 });
			rect.setPosition(202, WINDOW_HEIGHT);
			rect.setFillColor(sf::Color::Red);
			frameTimes.push_back(rect);
			for (sf::RectangleShape& rect : frameTimes)
			{
				rect.move(-2, 0);
			}

			if (frameTimes.size() > 100)
			{
				frameTimes.pop_front();
			}

			timerClock = 0.01f;
			avgDt = 0.f;
			framesSinceLastAverage = 0;
		}

		// RENDERING

		window.clear();

		for (int i = -3; i <= 3; i++)
		{
			lineHorizontal.setPosition(nearby200x - 800, i * DIV + nearby200y);
			lineVertical.setPosition(i * DIV + nearby200x, nearby200y - 500);
			window.draw(lineHorizontal);
			window.draw(lineVertical);
		}

		for (Bullet& b : bullets)
		{
			b.draw(window);
		}
		player.draw(window);

		//sf::Vector2f viewCenter = view.getCenter();
		//for (Enemy& enemy : enemies)
		//{
		//	sf::Vector2f enemyPos = enemy.getPosition();
		//	if (!(viewCenter.x + 690 < enemyPos.x || viewCenter.x - 690 > enemyPos.x) && !(viewCenter.y + 390 < enemyPos.y || viewCenter.y - 390 > enemyPos.y))
		//	{
		//		enemy.draw(window);
		//		enemiesDrawn++;
		//	}
		//}

		enemiesTotal = enemies.size();

		textStr.append("\nenemies drawn/total: " + std::to_string(enemiesDrawn) + '/' + std::to_string(enemiesTotal));
		text.setString(textStr);
		textStr.clear();
		enemiesDrawn = 0;

		if (drawLine)
		{
			window.draw(line, 2, sf::Lines);
		}

		if (drawFOV)
		{
			window.draw(FOV);
		}
		window.draw(evao);

		for (const sf::CircleShape cs : explosionCircles)
		{
			if (cs.getPosition().x >= view.getCenter().x + 850 ||
				cs.getPosition().x <= view.getCenter().x - 850 ||
				cs.getPosition().y >= view.getCenter().y + 550 ||
				cs.getPosition().y <= view.getCenter().y - 550)
				continue;

			window.draw(cs);
		}

		window.setView(window.getDefaultView());

		if (drawDebug)
		{
			sf::RectangleShape rs;
			rs.setPosition(0, 0);
			rs.setSize({ 500,200 });
			rs.setFillColor(sf::Color(0, 0, 0, 190));

			for (sf::RectangleShape& rect : frameTimes)
			{
				window.draw(rect);
			}

			window.draw(rs);
			window.draw(text);
		}

		window.display();
	}
	//exit_thread = true;
	//collisionThread.join();
}