#include <Logger/logger.h>
#include <UI/window.h>
#include <Input/input.h>
#include <Util/Math/angle.h>
#include <Util/Math/vector.h>

#include <format>
#include <string>
#include <random>
#include <algorithm>

using namespace cgame::logger;

// game values
double ballSpeed = 200.0;
const double playerPaddleSpeed = 300.0;

class GameWindow : public cgame::ui::Window
{
private:
    cgame::ui::SolidColorObject rightPaddle;
    cgame::ui::SolidColorObject leftPaddle;
    cgame::ui::SolidColorObject ball;
    cgame::util::math::Vec2d ballSpeedVec;
    double leftPaddleYDeltaSpeed;
    bool gameOver = false;

    // This is where the game loop happens
    void update(int frameTimeMiliseconds) override
    {
        if (gameOver) return;

        double deltaTimeSeconds = (double)frameTimeMiliseconds / 1000;

        auto width = getWidth();
        auto height = getHeight();

        // enemy AI
        double paddleY;
        
        if (getHeight() > 100)
            paddleY = std::clamp<double>(ball.get_position().y, 100, height - 100);
        else
            paddleY = getHeight() / 2;

        rightPaddle.set_position(width - 10, paddleY);

        // player control
        auto newLeftPaddleY = leftPaddle.get_position().y + leftPaddleYDeltaSpeed * deltaTimeSeconds;

        if (getHeight() > 100)
            newLeftPaddleY = std::clamp<double>(newLeftPaddleY, 100, height - 100);
        else
            newLeftPaddleY = height / 2;

        leftPaddle.set_position(10, newLeftPaddleY);

        // move ball
        auto deltaBall = ballSpeedVec * deltaTimeSeconds;
        ball.move(deltaBall);

        // detect collision
        if (ball.get_position().y <= 30 and ballSpeedVec.y < 0)         // ceiling
        {
            ballSpeedVec.y *= -1;
        }

        if (ball.get_position().y >= height - 30 and ballSpeedVec.y > 0) // floor
        {
            ballSpeedVec.y *= -1;
        }

        if (ball.get_position().x >= width - 50 and ballSpeedVec.x > 0)  // enemy paddle
        {
            auto randomAngleValue = 135 + (static_cast<double>(std::rand()) / RAND_MAX) * 90;
            auto newBallAngle = cgame::util::math::Angle::degrees(randomAngleValue);

            ballSpeed += 20;
            ballSpeedVec = Vec2d(ballSpeed * newBallAngle.cos(), ballSpeed * newBallAngle.sin());
        }

        if (ball.get_position().x <= 50 and ballSpeedVec.x < 0)          // player
        {
            if ((ball.get_position().y <= newLeftPaddleY + 100 and ball.get_position().y >= newLeftPaddleY - 100))
            {
                auto randomAngleValue = -45 + (static_cast<double>(std::rand()) / RAND_MAX) * 90;
                auto newBallAngle = cgame::util::math::Angle::degrees(randomAngleValue);

                ballSpeedVec = Vec2d(ballSpeed * newBallAngle.cos(), ballSpeed * newBallAngle.sin());
            }
            else
            {
                set_title("GAME OVER!!! restart the game to play again");
                gameOver = true;
            }
        }

        // draw
        draw_background(cgame::util::color::Color::white);
        draw_solid_color_object(rightPaddle);
        draw_solid_color_object(leftPaddle);
        draw_solid_color_object(ball);
    }

    static void HandlePlayerInput(cgame::ui::Window& window, cgame::input::KeyboardKey key, cgame::input::KeyPressAction action, int mods)
    {
        auto gameWindow = dynamic_cast<GameWindow*>(&window);


        if (key == cgame::input::KeyboardKey::W)
        {
            if (action == cgame::input::KeyPressAction::PRESS)
                gameWindow->leftPaddleYDeltaSpeed -= playerPaddleSpeed;
            else if (action == cgame::input::KeyPressAction::RELEASE)
                gameWindow->leftPaddleYDeltaSpeed += playerPaddleSpeed;
        }
        else if (key == cgame::input::KeyboardKey::S)
        {
            if (action == cgame::input::KeyPressAction::PRESS)
                gameWindow->leftPaddleYDeltaSpeed += playerPaddleSpeed;
            else if (action == cgame::input::KeyPressAction::RELEASE)
                gameWindow->leftPaddleYDeltaSpeed -= playerPaddleSpeed;
        }
    }

public:
    GameWindow() : 
    cgame::ui::Window("pong", 800, 800),
    rightPaddle(cgame::ui::Rectangle(20, 200, Color::black)),
    leftPaddle(cgame::ui::Rectangle(20, 200, Color::black)),
    ball(cgame::ui::RegularPolygon(30, 12, Color::black))
    {
        // set up random gen
        std::srand(time(0));

        // set random angle to start the ball speed
        auto randomAngleValue = (static_cast<double>(std::rand()) / RAND_MAX) * 360;
        while (randomAngleValue == 90 or randomAngleValue == 270)
        {
            randomAngleValue = (static_cast<double>(std::rand()) / RAND_MAX) * 360;
        }
        auto startingAngle = cgame::util::math::Angle::degrees(randomAngleValue);

        // set ball speed
        ballSpeedVec = Vec2d(ballSpeed * startingAngle.cos(), ballSpeed * startingAngle.sin());

        // place the objects in the starting position
        leftPaddle.set_position(10, getHeight() / 2);
        rightPaddle.set_position(getWidth() - 10, getHeight() / 2);
        ball.set_position(getWidth() / 2, getHeight() / 2);
    }

    void start() override
    {
        Window::start();

        while (!isRunning());

        cgame::input::setKeyPressCallback(*this, HandlePlayerInput);
    }
};

int main()
{
    cgame::ui::Window::start_glfw();

    GameWindow gameWindow;

    gameWindow.start();

    while (gameWindow.isRunning());

    cgame::ui::Window::terminate_glfw();

    return 0;
}
