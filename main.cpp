#include <SFML/Graphics.hpp>
#include <chrono>
#include <thread>
#include <random>

constexpr float PLAYER_SPEED = 750.0f; // pixels per second
constexpr float BALL_SPEED = 1400.0f;   // pixels per second
constexpr int WINDOW_WIDTH = 1600;
constexpr int WINDOW_HEIGHT = 1200;
constexpr int MIDDLE_WIDTH = WINDOW_WIDTH / 2;
constexpr int MIDDLE_HEIGHT = WINDOW_HEIGHT / 2;
constexpr float TARGET_FPS = 120.0f;
constexpr auto TARGET_FRAME_TIME = std::chrono::duration<float>(1.0f / TARGET_FPS);

float getRandomNumber(float min, float max){
    std::random_device rd;

    // Create a Mersenne Twister engine seeded with the random device
    std::mt19937 gen(rd());

    // Create a uniform integer distribution for numbers between 1 and 100 (inclusive)
    std::uniform_int_distribution<> distrib(min, max);

    // Generate a random number
    return  distrib(gen);
}

int main() {
    using namespace sf;
    bool ball_move_allowed = false;
    const sf::Font font("arial.ttf");
    bool gameOver = false;
    int score = 0;
    sf::Text loseGame(font, "YOU SUCK", 200);
    sf::Text scoreText(font, std::to_string(score),200);
    loseGame.setPosition(Vector2<float>(MIDDLE_WIDTH-500, MIDDLE_HEIGHT-200));
    scoreText.setPosition({MIDDLE_WIDTH,0});

    // Create the main window
    RenderWindow window(VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "PONG");

    RectangleShape player(Vector2<float>(20, 100));
    player.setPosition(Vector2<float>(20, WINDOW_HEIGHT/2));

    RectangleShape opponent(Vector2<float>(20, 100));
    opponent.setPosition(Vector2<float>(WINDOW_WIDTH - 200, WINDOW_HEIGHT/2));

    CircleShape ball(15);
    ball.setPosition(Vector2<float>(WINDOW_WIDTH/2, WINDOW_HEIGHT/2));

    // Ball velocity in pixels per second
    Vector2<float> ballVelocity(-BALL_SPEED, -getRandomNumber(-BALL_SPEED,BALL_SPEED) * 0.5f);

    // Delta time tracking
    auto lastTime = std::chrono::high_resolution_clock::now();
    auto frameStart = lastTime;

    // Start the game loop
    while (window.isOpen()) {
        if(Keyboard::isKeyPressed(Keyboard::Key::Q) ||
           Keyboard::isKeyPressed(Keyboard::Key::Escape)) window.close();
        frameStart = std::chrono::high_resolution_clock::now();

        // Calculate delta time in seconds
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        // Process events
        while (const std::optional event = window.pollEvent()) {
            // Close window: exit
            if (event->is<Event::Closed>())
                window.close();
        }
        // Clear screen
        window.clear();

        // Check collisions
        std::optional<Rect<float>> playerCollide = player.getGlobalBounds().findIntersection(ball.getGlobalBounds());
        std::optional<Rect<float>> ballCollision;
        if(playerCollide == std::nullopt){
            ballCollision = opponent.getGlobalBounds().findIntersection(ball.getGlobalBounds());
        } else {
            ballCollision = playerCollide;
            score++;
        }

        // Handle collisions
        if(ballCollision != std::nullopt){
            ballVelocity.x *= -1;
            if(playerCollide != std::nullopt){
                ballVelocity.x = BALL_SPEED;
            }
        } else if(ball.getPosition().y <= 0 || ball.getPosition().y >= WINDOW_HEIGHT - 50){
            ballVelocity.y *= -1;
        } else if(ball.getPosition().x <= -10){
            gameOver = true;
            ball.setPosition(Vector2<float>(-200, WINDOW_HEIGHT/2));
        }

        // Accept player input and calculate movement
        Vector2<float> playerVelocity(0, 0);
        if (Keyboard::isKeyPressed(Keyboard::Key::W) ||
            Keyboard::isKeyPressed(Keyboard::Key::Up)) {
            playerVelocity.y = -PLAYER_SPEED;
            ball_move_allowed = true;
        } else if(Keyboard::isKeyPressed(Keyboard::Key::S) ||
                  Keyboard::isKeyPressed(Keyboard::Key::Down)){
            playerVelocity.y = PLAYER_SPEED;
            ball_move_allowed = true;
        }

        // Move player using delta time
        Vector2<float> playerMovement = playerVelocity * deltaTime;
        player.move(playerMovement);
        window.draw(player);

        // Set the opponents position to ball height (it's totally fair don't worry abt it)
        opponent.setPosition(Vector2<float>(WINDOW_WIDTH - 50, ball.getPosition().y));
        window.draw(opponent);

        // Move ball using delta time
        if(ball_move_allowed) {
            Vector2<float> ballMovement = ballVelocity * deltaTime;
            ball.move(ballMovement);
        }
        window.draw(ball);

        // Game over handling
        if(gameOver && Keyboard::isKeyPressed(Keyboard::Key::R)){
            player.setPosition(Vector2<float>(20, WINDOW_HEIGHT/2));
            opponent.setPosition(Vector2<float>(WINDOW_WIDTH - 200, WINDOW_HEIGHT/2));
            ball.setPosition(Vector2<float>(WINDOW_WIDTH/2, WINDOW_HEIGHT/2));
            ball_move_allowed = false;
            gameOver = false;
            score = 0;
            ballVelocity = Vector2<float>(-BALL_SPEED, -getRandomNumber(-BALL_SPEED,BALL_SPEED) * 0.5f);
        } else if(gameOver) {
            window.draw(loseGame);
            scoreText.setString(std::to_string(score));
            window.draw(scoreText);
        }

        // Display the window
        window.display();

        // Frame rate limiting to 120 FPS
        auto frameEnd = std::chrono::high_resolution_clock::now();
        auto frameDuration = frameEnd - frameStart;
        if (frameDuration < TARGET_FRAME_TIME) {
            std::this_thread::sleep_for(TARGET_FRAME_TIME - frameDuration);
        }
    }

    return 0;
}