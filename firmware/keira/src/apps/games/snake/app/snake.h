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
    const char* game_title = "Snake";
    // Size set in pixels (height and width)
    const int baseSegmentSize = 8;
    // Dead zone around the edges of the screen as a multiple of the segment size
    const int deadZoneSegmentsMultiplicator = 4;

    // MARK: - General Properties

    // Actual game state
    GameState currentGameState;
    // Game speed delay in ms to control the speed of the snake and control responsiveness
    int gameSpeedDelay;
    // Indexes for Main menu options
    int menuIndex;
    // Indexes for Options menu options
    int optionsIndex;

    // MARK: - Actors Properties

    // Snake body represented as a vector of pairs of x, y coordinates
    std::vector<std::pair<int, int>> body;
    // Direction of the snake
    int dxSnake, dySnake;
    // Apple position
    int appleX, appleY;
    // Current score
    int score;
    // Game over flag
    bool gameOver;

    // MARK: - Menu methods
    void displayMainMenu();
    void handleMainMenu();
    void displayOptionsMenu();
    void handleOptionsMenu();

    // MARK: - Game Process methods
    void update();
    void spawnApple();
    void drawGame();
    void showGameOver();
};