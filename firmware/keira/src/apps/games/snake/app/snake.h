#include <lilka.h>
#include "app.h"
#include <vector>
#include <utility>

enum class GameState { Menu, Game, Options, Exit };

class SnakeApp : public App {
public:
    SnakeApp();
    void run() override;

private:
    // MARK: - Const Properties

    // Game title
    const char* gameTitle = "Snake";
    // Size set in pixels (height and width)
    const int baseSegmentSize = 8;
    // Dead zone around the edges of the screen as a multiple of the segment size
    const int deadZoneSegmentsMultiplicator = 4;

    // MARK: - General Properties

    // Actual game state
    GameState currentGameState = GameState::Menu;
    // Game speed delay in ms to control the speed of the snake and control responsiveness
    int gameSpeedDelay = 100;
    // Indexes for Main menu options
    int menuIndex = 0;
    // Indexes for Options menu options
    int optionsIndex = 1;
    // Game over flag
    bool gameOver = false;

    // MARK: - Actors Properties

    // Snake body represented as a vector of pairs of x, y coordinates
    std::vector<std::pair<int, int>> body;
    // Direction of the snake
    int dxSnake = baseSegmentSize;
    int dySnake = 0;
    // Apple position
    int appleX, appleY;
    // Current score
    int score = 0;

    // MARK: - Menu methods
    void displayMainMenu();
    void handleMainMenu();
    void displayOptionsMenu();
    void handleOptionsMenu();
    void drawBigTitle(int startX, int startY);

    // MARK: - Game Process methods
    void restartGame();
    void update();
    void spawnApple();
    void drawGame();
    void showGameOver();
};