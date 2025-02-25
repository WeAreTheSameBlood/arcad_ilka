#include <lilka.h>
#include "app.h"
#include <vector>
#include <utility>

class SnakeApp : public App {
public:
    SnakeApp();
    void run() override;

private:    
    // MARK: - Properties
    const char* game_title = "Snake";
    // Size set in pixels (height and width)
    const int baseSegmentSize = 8;
    // Dead zone around the edges of the screen as a multiple of the segment size
    const int deadZoneSegmentsMultiplicator = 4;
    // Snake body represented as a vector of pairs of x, y coordinates
    std::vector<std::pair<int, int>> body;
    // Direction of the snake
    int dx, dy;
    // Apple position
    int appleX, appleY;
    // Current score
    int score;
    // Game over flag
    bool gameOver;
    
    // MARK: - Functions
    void spawnApple();
    void update();
    void drawGame();
    void showGameOver();
};