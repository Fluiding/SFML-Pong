#include <string>
#include <random>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "BallHitData.h"
#include "NESData.h"

#define TARGET_FPS 60.f
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define PADDLE_SIZE sf::Vector2f(20, 125)
#define PADDLE_SPEED 6
#define BALL_RADIUS 10.f

#define P1_UP sf::Keyboard::Z
#define P1_DOWN sf::Keyboard::S
#define P2_UP sf::Keyboard::I
#define P2_DOWN sf::Keyboard::K

int main()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist(0, 1);

	sf::RenderWindow window(
		sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 8),
		"Pong",
		sf::Style::Titlebar | sf::Style::Close
	);
	window.setVerticalSyncEnabled(true);

	sf::RectangleShape line(sf::Vector2f(10, WINDOW_HEIGHT));
	line.setPosition(WINDOW_WIDTH / 2.f - 5.f, 0.f);

	sf::RectangleShape p1(PADDLE_SIZE);
	p1.setPosition(sf::Vector2f(50.f, WINDOW_HEIGHT / 2.f - PADDLE_SIZE.y / 2.f));
	p1.setFillColor(sf::Color::White);

	sf::RectangleShape p2(PADDLE_SIZE);
	p2.setPosition(sf::Vector2f(WINDOW_WIDTH - 50.f - PADDLE_SIZE.x, p1.getPosition().y));
	p2.setFillColor(sf::Color::White);

	sf::CircleShape ball(BALL_RADIUS);
	ball.setOrigin(10.f, 10.f);
	ball.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f);
	ball.setFillColor(sf::Color::White);

	sf::SoundBuffer ballHitBuffer;
	ballHitBuffer.loadFromMemory(ball_wav, ball_wav_len);
	sf::Sound ballHit;
	ballHit.setBuffer(ballHitBuffer);

	sf::Font font;
	font.loadFromMemory(NES_otf, NES_otf_len);

	sf::Text p1Score("0", font, 50u);
	p1Score.setPosition(WINDOW_WIDTH / 2.f - 25.f, 50.f);

	sf::Text p2Score("0", font, 50u);
	p2Score.setPosition(WINDOW_WIDTH / 2.f + 25.f, 100.f / 3.f);

	sf::Text welcome("PRESS SPACE TO BEGIN", font, 60u);
	sf::FloatRect textBounds = welcome.getLocalBounds();
	welcome.setPosition((window.getSize().x - textBounds.width) / 2.f, (window.getSize().y - textBounds.height) / 2.f);

	bool isPlaying = false;
	const sf::Time delta = sf::seconds(1.f / TARGET_FPS);
	sf::Clock clock;

	char xVel = 5;
	char yVel = -5;

	while (true)
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				window.close();
				break;
			}
		}

		p1Score.setOrigin(p1Score.getLocalBounds().width, p1Score.getLocalBounds().height / 2.f);

		if (!isPlaying)
		{
			ball.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f);

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
			{
				xVel = (dist(gen) == 0) ? -5 : 5;
				yVel = (dist(gen) == 0) ? -5 : 5;
				isPlaying = true;
			}

			window.clear(sf::Color(20, 20, 20));
			window.draw(welcome);
			window.draw(p1Score);
			window.draw(p2Score);
			window.display();
		}
		else
		{
			if (sf::Keyboard::isKeyPressed(P1_UP) && p1.getPosition().y > 0)
			{
				p1.setPosition(sf::Vector2f(p1.getPosition().x, p1.getPosition().y - PADDLE_SPEED));
			}
			if (sf::Keyboard::isKeyPressed(P1_DOWN) && p1.getPosition().y + PADDLE_SIZE.y < WINDOW_HEIGHT)
			{
				p1.setPosition(sf::Vector2f(p1.getPosition().x, p1.getPosition().y + PADDLE_SPEED));
			}
			if (sf::Keyboard::isKeyPressed(P2_UP) && p2.getPosition().y > 0)
			{
				p2.setPosition(sf::Vector2f(p2.getPosition().x, p2.getPosition().y - PADDLE_SPEED));
			}
			if (sf::Keyboard::isKeyPressed(P2_DOWN) && p2.getPosition().y + PADDLE_SIZE.y < WINDOW_HEIGHT)
			{
				p2.setPosition(sf::Vector2f(p2.getPosition().x, p2.getPosition().y + PADDLE_SPEED));
			}

			if (ball.getGlobalBounds().intersects(p1.getGlobalBounds()) ||
				ball.getGlobalBounds().intersects(p2.getGlobalBounds()))
			{
				xVel *= -1;
				ballHit.play();
			}

			if (ball.getPosition().y >= WINDOW_HEIGHT || ball.getPosition().y <= 0)
			{
				yVel *= -1;
				ballHit.play();
			}

			if (ball.getPosition().x <= 0)
			{
				p2Score.setString(std::to_string(std::stoi((std::string)p2Score.getString()) + 1));
				isPlaying = false;
			}
			else if (ball.getPosition().x >= WINDOW_WIDTH)
			{
				p1Score.setString(std::to_string(std::stoi((std::string)p1Score.getString()) + 1));
				isPlaying = false;
			}

			ball.setPosition(sf::Vector2f(ball.getPosition().x + xVel, ball.getPosition().y + yVel));

			window.clear(sf::Color(20, 20, 20));
			window.draw(p1);
			window.draw(p2);
			window.draw(ball);
			window.draw(p1Score);
			window.draw(p2Score);
			window.display();
		}

		sf::Time elapsed = clock.restart();
        if (elapsed < delta) {
            sf::sleep(delta - elapsed);
        }
	}

	return EXIT_SUCCESS;
}