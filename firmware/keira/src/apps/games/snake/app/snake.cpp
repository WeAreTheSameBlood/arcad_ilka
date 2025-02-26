#include "snake.h"
#include <cstdlib>
#include <ctime>
#include "../resouces/strings.h"

// MARK: - Static
static int globalHighScore = 0;

// MARK: - SnakeApp
SnakeApp::SnakeApp() :
    App(game_title),
    dxSnake(baseSegmentSize),
    dySnake(0),
    score(0),
    gameOver(false),
    currentGameState(GameState::Menu),
    gameSpeedDelay(100),
    menuIndex(0),
    optionsIndex(1) {
    std::srand(std::time(nullptr));

    // Init snake body in the centre of screen
    int startX = (canvas->width() / 2 / baseSegmentSize) * baseSegmentSize;
    int startY = (canvas->height() / 2 / baseSegmentSize) * baseSegmentSize;

    // Build of start snake body as 3 segments length
    body.push_back({startX, startY});
    body.push_back({startX - baseSegmentSize, startY});
    body.push_back({startX - 2 * baseSegmentSize, startY});

    // Spawn init apple
    spawnApple();
}

// MARK: - Run
void SnakeApp::run() {
    while (true) {
        switch (currentGameState) {
            case GameState::Menu:
                handleMainMenu();
                break;

            case GameState::Options:
                handleOptionsMenu();
                break;

            case GameState::Game:
                if (!gameOver) {
                    update();
                    drawGame();

                    // Show current score
                    char scoreDisplay[16];
                    std::snprintf(scoreDisplay, sizeof(scoreDisplay), Strings::SCORE_FORMAT, score);
                    canvas->setCursor(10, 16);
                    canvas->setTextColor(lilka::colors::Green);
                    canvas->print(scoreDisplay);

                    queueDraw();
                    vTaskDelay(gameSpeedDelay / portTICK_PERIOD_MS);
                } else {
                    showGameOver();
                    queueDraw();

                    lilka::State buttonsState = lilka::controller.getState();
                    if (buttonsState.a.justPressed) {
                        // Restart game
                        body.clear();
                        int startX = (canvas->width() / 2 / baseSegmentSize) * baseSegmentSize;
                        int startY = (canvas->height() / 2 / baseSegmentSize) * baseSegmentSize;
                        body.push_back({startX, startY});
                        body.push_back({startX - baseSegmentSize, startY});
                        body.push_back({startX - 2 * baseSegmentSize, startY});
                        score = 0;
                        gameOver = false;

                        spawnApple();
                        currentGameState = GameState::Game;
                    } else if (buttonsState.b.justPressed) {
                        // Return to main menu
                        currentGameState = GameState::Menu;
                    }
                    vTaskDelay(100 / portTICK_PERIOD_MS);
                }
                break;

            case GameState::Exit:
                stop();
                return;
        }
    }
}

void SnakeApp::displayMainMenu() {
    canvas->fillScreen(canvas->color565(0, 0, 0));

    // Main menu options
    const char* items[3] = {"Start", "Options", "Exit"};
    int startY = canvas->height() / 2 - 20;

    for (int i = 0; i < 3; i++) {
        canvas->setCursor(canvas->width() / 2 - 40, startY + i * 20);
        if (i == menuIndex) canvas->setTextColor(lilka::colors::Yellow);
        else canvas->setTextColor(lilka::colors::White);
        canvas->print(items[i]);
    }
}

void SnakeApp::handleMainMenu() {
    displayMainMenu();
    queueDraw();
    lilka::State state = lilka::controller.getState();

    if (state.up.justPressed) {
        if (menuIndex > 0) menuIndex--;
    } else if (state.down.justPressed) {
        if (menuIndex < 2) menuIndex++;
    } else if (state.a.justPressed) {
        if (menuIndex == 0) {
            body.clear();
            int startX = (canvas->width() / 2 / baseSegmentSize) * baseSegmentSize;
            int startY = (canvas->height() / 2 / baseSegmentSize) * baseSegmentSize;
            body.push_back({startX, startY});
            body.push_back({startX - baseSegmentSize, startY});
            body.push_back({startX - 2 * baseSegmentSize, startY});
            score = 0;
            gameOver = false;
            spawnApple();
            currentGameState = GameState::Game;
        } else if (menuIndex == 1) {
            currentGameState = GameState::Options;
        } else if (menuIndex == 2) {
            currentGameState = GameState::Exit;
        }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
}

void SnakeApp::displayOptionsMenu() {
    canvas->fillScreen(canvas->color565(0, 0, 0));

    // Options menu options -_-
    const char* options[3] = {"Slow", "Normal", "Fast"};
    int startY = canvas->height() / 2 - 20;
    for (int i = 0; i < 3; i++) {
        canvas->setCursor(canvas->width() / 2 - 40, startY + i * 20);
        if (i == optionsIndex) canvas->setTextColor(lilka::colors::Yellow);
        else canvas->setTextColor(lilka::colors::White);
        canvas->print(options[i]);
    }
}

void SnakeApp::handleOptionsMenu() {
    displayOptionsMenu();
    queueDraw();
    lilka::State state = lilka::controller.getState();

    if (state.up.justPressed) {
        if (optionsIndex > 0) optionsIndex--;
    } else if (state.down.justPressed) {
        if (optionsIndex < 2) optionsIndex++;
    } else if (state.a.justPressed) {
        switch (optionsIndex) {
            case 0:
                // Slow
                gameSpeedDelay = 150;
                break;

            case 1:
                // Normal
                gameSpeedDelay = 100;
                break;

            case 2:
                // Fast
                gameSpeedDelay = 50;
                break;
        }

        // Return to main menu
        currentGameState = GameState::Menu;
    } else if (state.b.justPressed) {
        // Return to main menu
        currentGameState = GameState::Menu;
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
}

// MARK: - Spawn Apple
void SnakeApp::spawnApple() {
    // Calculate limits indesies for random position
    int minIndexX = deadZoneSegmentsMultiplicator;
    int maxIndexX = (canvas->width() / baseSegmentSize) - deadZoneSegmentsMultiplicator - 1;

    int minIndexY = deadZoneSegmentsMultiplicator;
    int maxIndexY = (canvas->height() / baseSegmentSize) - deadZoneSegmentsMultiplicator - 1;

    // Generate random position until it doesn't collide with the snake
    while (true) {
        int randX = std::rand() % (maxIndexX - minIndexX + 1) + minIndexX;
        int randY = std::rand() % (maxIndexY - minIndexY + 1) + minIndexY;
        appleX = randX * baseSegmentSize;
        appleY = randY * baseSegmentSize;
        bool collision = false;

        // Check collision with snake
        for (auto& seg : body) {
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
    if (gameOver) return;

    // Get the current state of the control buttons
    lilka::State state = lilka::controller.getState();
    if (state.up.justPressed && dySnake == 0) {
        dxSnake = 0;
        dySnake = -baseSegmentSize;
    } else if (state.down.justPressed && dySnake == 0) {
        dxSnake = 0;
        dySnake = baseSegmentSize;
    } else if (state.left.justPressed && dxSnake == 0) {
        dxSnake = -baseSegmentSize;
        dySnake = 0;
    } else if (state.right.justPressed && dxSnake == 0) {
        dxSnake = baseSegmentSize;
        dySnake = 0;
    }

    // Calculate new head position
    int newX = body.front().first + dxSnake;
    int newY = body.front().second + dySnake;

    // Overscreen moves processing (wrap-around)
    if (newX < 0) newX = canvas->width() - baseSegmentSize;
    if (newY < 0) newY = canvas->height() - baseSegmentSize;
    if (newX >= canvas->width()) newX = 0;
    if (newY >= canvas->height()) newY = 0;

    // Check collision with apple
    for (size_t i = 0; i < body.size() - 1; i++) {
        if (body[i].first == newX && body[i].second == newY) {
            gameOver = true;
            if (score > globalHighScore) {
                globalHighScore = score;
            }
            return;
        }
    }

    body.insert(body.begin(), {newX, newY});

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
    canvas->fillRect(appleX, appleY, baseSegmentSize, baseSegmentSize, canvas->color565(255, 0, 0));

    // Draw snake segments as white squares
    for (auto& seg : body) {
        canvas->fillRect(seg.first, seg.second, baseSegmentSize, baseSegmentSize, canvas->color565(255, 255, 255));
    }
}

// MARK: - Game Over
void SnakeApp::showGameOver() {
    canvas->fillScreen(canvas->color565(0, 0, 0));

    // Game Over preview
    canvas->setCursor(canvas->width() / 2 - 44, canvas->height() / 2 - 50);
    canvas->setTextColor(lilka::colors::Red);
    canvas->print(Strings::GAME_OVER);

    // Your Score Preview
    char scoreText[16];
    std::snprintf(scoreText, sizeof(scoreText), Strings::YOUR_SCORE_FORMAT, score);
    canvas->setCursor(canvas->width() / 2 - 62, canvas->height() / 2 - 20);
    canvas->setTextColor(lilka::colors::White);
    canvas->print(scoreText);

    // High Score Preview
    char highScoreText[16];
    std::snprintf(highScoreText, sizeof(highScoreText), Strings::HIGH_SCORE_FORMAT, globalHighScore);
    canvas->setCursor(canvas->width() / 2 - 62, canvas->height() / 2);
    canvas->setTextColor(lilka::colors::Yellow);
    canvas->print(highScoreText);

    // Press A Preview
    canvas->setCursor(canvas->width() / 2 - 96, canvas->height() / 2 + 30);
    canvas->setTextColor(lilka::colors::Green);
    canvas->print(Strings::PRESS_A_TRY_AGAIN);

    // Press B Preview
    canvas->setCursor(canvas->width() / 2 - 80, canvas->height() / 2 + 50);
    canvas->setTextColor(lilka::colors::Red_orange);
    canvas->print(Strings::PRESS_B_EXIT);
}