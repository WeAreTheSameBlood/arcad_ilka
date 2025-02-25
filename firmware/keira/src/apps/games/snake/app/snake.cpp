#include "snake.h"
#include <cstdlib>
#include <ctime>
#include "strings.h"

// MARK: - Static 
static int globalHighScore = 0;

// MARK: - SnakeApp
SnakeApp::SnakeApp() : App(game_title), dx(baseSegmentSize), dy(0), score(0), gameOver(false) {
    std::srand(std::time(nullptr));

    // Init snake body in the centre of screen
    int startX = (canvas->width() / 2 / baseSegmentSize) * baseSegmentSize;
    int startY = (canvas->height() / 2 / baseSegmentSize) * baseSegmentSize;

    // Build of start snake body as 3 segments length
    body.push_back({ startX, startY });
    body.push_back({ startX - baseSegmentSize, startY });
    body.push_back({ startX - 2 * baseSegmentSize, startY });

    // Spawn init apple
    spawnApple();
}

// MARK: - Run
void SnakeApp::run() {
    while (true) {
        if (!gameOver) {
            update();
            drawGame();
            
            // Show score
            char scoreDisplay[16];
            std::snprintf(
                scoreDisplay, sizeof(scoreDisplay),
                Strings::SCORE_FORMAT, score
            );
            canvas->setCursor(10, 16);
            canvas->setTextColor(lilka::colors::Green);
            canvas->print(scoreDisplay);
            
            queueDraw();
            vTaskDelay(100 / portTICK_PERIOD_MS);
        } else {
            showGameOver();
            queueDraw();
            
            // Wait for restart on A button pressed
            lilka::State buttonsState = lilka::controller.getState();
            if (buttonsState.a.justPressed) {
                // Reset game state
                body.clear();
                int startX = canvas->width() / 2;
                int startY = canvas->height() / 2;
                body.push_back({ startX, startY });
                body.push_back({ startX - baseSegmentSize, startY });
                body.push_back({ startX - 2 * baseSegmentSize, startY });
                score = 0;
                gameOver = false;
                spawnApple();
            } else if (buttonsState.b.justPressed) {
                return;
            }
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
    }
}

// MARK: - Spawn Apple
void SnakeApp::spawnApple() {
    // Calculate limits indesies for random position
    int minIndexX = deadZoneSegmentsMultiplicator;
    int maxIndexX = (canvas->width() / baseSegmentSize) - deadZoneSegmentsMultiplicator;

    int minIndexY = deadZoneSegmentsMultiplicator;
    int maxIndexY = (canvas->height() / baseSegmentSize) - deadZoneSegmentsMultiplicator;

    // Generate random position until it doesn't collide with the snake
    while (true) {
        int randX = std::rand() % (maxIndexX - minIndexX + 1) + minIndexX;
        int randY = std::rand() % (maxIndexY - minIndexY + 1) + minIndexY;
        appleX = randX * baseSegmentSize;
        appleY = randY * baseSegmentSize;
        bool collision = false;

        // Check collision with snake
        for (auto &seg : body) {
            if (seg.first == appleX && seg.second == appleY) {
                collision = true;
                break;
            }
        }
        if (!collision) break;
    }
}

// MARK: - Update
void SnakeApp::update() {
    if (gameOver)  return;

    // Get the current state of the control buttons
    lilka::State state = lilka::controller.getState();
    if (state.up.justPressed && dy == 0) { dx = 0; dy = -baseSegmentSize; }
    else if (state.down.justPressed && dy == 0) { dx = 0; dy = baseSegmentSize; }
    else if (state.left.justPressed && dx == 0) { dx = -baseSegmentSize; dy = 0; }
    else if (state.right.justPressed && dx == 0) { dx = baseSegmentSize; dy = 0; }
    
    // Calculate new head position
    int newX = body.front().first + dx;
    int newY = body.front().second + dy;
    
    // Overscreen moves processing (wrap-around)
    if (newX < 0) newX = canvas->width() - baseSegmentSize;
    if (newY < 0) newY = canvas->height() - baseSegmentSize;
    if (newX >= canvas->width()) newX = 0;
    if (newY >= canvas->height()) newY = 0;
    
    // Check collision with apple
    for (auto &seg : body) {
        if (seg.first == newX && seg.second == newY) {
            gameOver = true;
            if (score > globalHighScore) {
                globalHighScore = score;
            }
            return;
        }
    }
    
    body.insert(body.begin(), { newX, newY });
    
    if (newX == appleX && newY == appleY) {
        score++;
        spawnApple();
    } else {
        body.pop_back();
    }
}

// MARK: - Draw Game
void SnakeApp::drawGame() {
    // Clear screen
    canvas->fillScreen(canvas->color565(0, 0, 0));

    // Draw apple as a red square
    canvas->fillRect(
        appleX, appleY, 
        baseSegmentSize, baseSegmentSize,
        canvas->color565(255, 0, 0)
    );
    
    // Draw snake segments as white squares
    for (auto &seg : body) {
        canvas->fillRect(
            seg.first, seg.second,
            baseSegmentSize, baseSegmentSize,
            canvas->color565(255, 255, 255)
        );
    }
}

// MARK: - Game Over
void SnakeApp::showGameOver() {
    canvas->fillScreen(canvas->color565(0, 0, 0));
    
    // Game Over preview
    canvas->setCursor(
        canvas->width() / 2 - 44,
        canvas->height() / 2 - 40
    );
    canvas->setTextColor(lilka::colors::Red);
    canvas->print(Strings::GAME_OVER);

    // Your Score Preview
    char scoreText[16];
    std::snprintf(
        scoreText, sizeof(scoreText),
        Strings::YOUR_SCORE_FORMAT,
        score
    );
    canvas->setCursor(
        canvas->width() / 2 - 62,
        canvas->height() / 2 - 10
    );
    canvas->setTextColor(lilka::colors::White);
    canvas->print(scoreText);

    // High Score Preview
    char highScoreText[16];
    std::snprintf(
        highScoreText, sizeof(highScoreText),
        Strings::HIGH_SCORE_FORMAT,
        globalHighScore
    );
    canvas->setCursor(
        canvas->width() / 2 - 62,
        canvas->height() / 2 + 10
    );
    canvas->setTextColor(lilka::colors::Yellow);
    canvas->print(highScoreText);

    canvas->setCursor(
        canvas->width() / 2 - 100, 
        canvas->height() / 2 + 40
    );
    canvas->setTextColor(lilka::colors::Green);
    canvas->print(Strings::PRESS_A_TRY_AGAIN);
}