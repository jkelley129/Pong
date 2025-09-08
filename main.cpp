#include <SFML/Graphics.hpp>
#include <chrono>
#include <thread>

constexpr float PLAYER_SPEED = 0.2;
constexpr float BALL_SPEED = 0.1;
constexpr int WINDOW_WIDTH = 1600;
constexpr int WINDOW_HEIGHT = 1200;
constexpr int MIDDLE_WIDTH = WINDOW_WIDTH / 2;
constexpr int MIDDLE_HEIGHT = WINDOW_HEIGHT / 2;
constexpr int SLEEP_NANOSECONDS = 100;

int main() {
    using namespace sf;
    bool ball_move_allowed = false;
    const sf::Font font("arial.ttf");
    bool gameOver = false;
    sf::Text loseGame(font, "YOU SUCK", 200);
    loseGame.setPosition(Vector2<float>(MIDDLE_WIDTH-500, MIDDLE_HEIGHT-200));

    // Create the main window
    RenderWindow window(VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "PONG");

    RectangleShape player(Vector2<float>(20, 100));
    player.setPosition(Vector2<float>(20,WINDOW_HEIGHT/2));

    RectangleShape opponent(Vector2<float>(20, 100));
    opponent.setPosition(Vector2<float>(WINDOW_WIDTH - 200,WINDOW_HEIGHT/2));

    CircleShape ball(15);
    ball.setPosition(Vector2<float>(WINDOW_WIDTH/2,WINDOW_HEIGHT/2));

    //Player speed
    Vector2<float> playerMove;
    //Ball speed
    Vector2<float> ballMove = (Vector2<float>(-BALL_SPEED, -BALL_SPEED / 2));


    // Start the game loop
    while (window.isOpen()) {
        // Process events
        while (const std::optional event = window.pollEvent()) {
            // Close window: exit
            if (event->is<Event::Closed>())
                window.close();
        }
        // Clear screen
        window.clear();

        //Check collisions
        std::optional<Rect<float>> playerCollide = player.getGlobalBounds().findIntersection(ball.getGlobalBounds());
        std::optional<Rect<float>> ballCollision;
        if(playerCollide == std::nullopt){
            ballCollision = opponent.getGlobalBounds().findIntersection(ball.getGlobalBounds());
        }else{
            ballCollision = playerCollide;
        }

        // Handle collisions
        if(ballCollision != std::nullopt){
            ballMove.x *= -1;
            if(playerCollide != std::nullopt){ ballMove.x = BALL_SPEED;}
        }else if(ball.getPosition().y <= 0 || ball.getPosition().y >= WINDOW_HEIGHT - 50){
            ballMove.y *= -1;
        }else if(ball.getPosition().x <= -10){
            gameOver = true;
            ball.setPosition(Vector2<float>(-200, WINDOW_HEIGHT/2));
        }


        // Accept player input
        if (Keyboard::isKeyPressed(Keyboard::Key::W) ||
            Keyboard::isKeyPressed(Keyboard::Key::Up)) {
            playerMove = (Vector2<float>(0, -PLAYER_SPEED));
            ball_move_allowed = true;
        }else if(Keyboard::isKeyPressed(Keyboard::Key::S) ||
                 Keyboard::isKeyPressed(Keyboard::Key::Down)){
            playerMove = (Vector2<float>(0, PLAYER_SPEED));
            ball_move_allowed = true;
        }else{
            playerMove = (Vector2<float>(0, 0));
        }
        //Move and draw the player
        player.move(playerMove);
        window.draw(player);

        //Set the opponents position to ball height(its totally fair dont worry abt it)
        opponent.setPosition(Vector2<float>(WINDOW_WIDTH - 50, ball.getPosition().y));
        window.draw(opponent);

        //Move and draw the ball
        ball.move(ball_move_allowed ? ballMove : Vector2<float>(0, 0));
        window.draw(ball);

        if(gameOver && Keyboard::isKeyPressed(Keyboard::Key::R)){
            player.setPosition(Vector2<float>(20,WINDOW_HEIGHT/2));
            opponent.setPosition(Vector2<float>(WINDOW_WIDTH - 200,WINDOW_HEIGHT/2));
            ball.setPosition(Vector2<float>(WINDOW_WIDTH/2,WINDOW_HEIGHT/2));
            ball_move_allowed = false;
            loseGame.setScale(Vector2f(0,0));
        }else if(gameOver) window.draw(loseGame);


        //Display the window
        //kind of important
        window.display();
        std::this_thread::sleep_for(std::chrono::nanoseconds(SLEEP_NANOSECONDS));
    }
}