#include "snake.h"
#include <cstdlib>
#include <ctime>
#include "../resouces/strings.h"

// MARK: - Static
static int globalHighScore = 0;

// MARK: - SnakeApp
SnakeApp::SnakeApp() : App(gameTitle) {
    std::srand(std::time(nullptr));

    // Init snake body in the centre of screen
    int startX = (canvas->width() / 2 / baseSegmentSize) * baseSegmentSize;
    int startY = (canvas->height() / 2 / baseSegmentSize) * baseSegmentSize;

    // Build of start snake body as 3 segments length
    body.push_back({startX, startY});
    body.push_back({startX - baseSegmentSize, startY});
    body.push_back({startX - (2 * baseSegmentSize), startY});

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
                    char scoreDisplay[6];
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
                        restartGame();
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

// MARK: - Restart
void SnakeApp::restartGame() {
    // Clear the snake body and reset game parameters
    body.clear();
    int startX = (canvas->width() / 2 / baseSegmentSize) * baseSegmentSize;
    int startY = (canvas->height() / 2 / baseSegmentSize) * baseSegmentSize;

    // Build the starting snake body with 3 segments
    body.push_back({startX, startY});
    body.push_back({startX - baseSegmentSize, startY});
    body.push_back({startX - 2 * baseSegmentSize, startY});
    score = 0;
    gameOver = false;

    spawnApple();
    currentGameState = GameState::Game;
}

// MARK: - Main Menu
void SnakeApp::handleMainMenu() {
    displayMainMenu();
    queueDraw();

    lilka::State state = lilka::controller.getState();

    if (state.up.justPressed) {
        if (menuIndex > 0) menuIndex--;
    } else if (state.down.justPressed) {
        if (menuIndex < 2) menuIndex++;
    } else if (state.a.justPressed) {
        switch (menuIndex) {
            case 0:
                // Start
                restartGame();
                break;

            case 1:
                // Options
                currentGameState = GameState::Options;
                break;

            case 2:
                // Exit
                currentGameState = GameState::Exit;
                break;
        }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
}

void SnakeApp::displayMainMenu() {
    canvas->fillScreen(canvas->color565(0, 0, 0));

    // Main menu title
    // Draw the big animated title "S N A K E" using custom drawing
    // Adjust startX as needed to center the title (example offset: canvas->width()/2 - 100)
    drawBigTitle(canvas->width() / 2 - 100, 20);

    // Main menu options
    const char* mainMenuItems[3] = {"Start", "Options", "Exit"};
    const int verticalSpacing = 20;
    int startY = canvas->height() / 2 + 10;

    for (int i = 0; i < sizeof(mainMenuItems); i++) {
        canvas->setCursor(canvas->width() / 2 - 40, startY + i * verticalSpacing);
        if (i == menuIndex) canvas->setTextColor(lilka::colors::Yellow);
        else canvas->setTextColor(lilka::colors::White);
        canvas->print(mainMenuItems[i]);
    }
}

// MARK: - Draw Big Title (manually drawn letters)
// This function draws the title "S N A K E" with a height of approximately 48 pixels.
// Each letter is drawn using fillRect primitives.
void SnakeApp::drawBigTitle(int startX, int startY) {
    // Set title drawing color (example: green)
    uint16_t titleColor = canvas->color565(0, 255, 0);

    // --- Draw Apple before S ---
    canvas->fillRect(startX - 16, startY + 40, 8, 8, canvas->color565(255, 0, 0));

    // --- Draw letter S ---
    // Top horizontal bar
    canvas->fillRect(startX, startY, 30, 8, titleColor);
    // Upper left vertical bar
    canvas->fillRect(startX, startY, 8, 24, titleColor);
    // Middle horizontal bar
    canvas->fillRect(startX, startY + 20, 30, 8, titleColor);
    // Lower right vertical bar
    canvas->fillRect(startX + 22, startY + 20, 8, 24, titleColor);
    // Bottom horizontal bar
    canvas->fillRect(startX, startY + 40, 30, 8, titleColor);

    int letterSpacing = 40; // Offset for next letter

    // --- Draw letter N ---
    int xN = startX + letterSpacing;
    // Left vertical bar
    canvas->fillRect(xN, startY, 8, 48, titleColor);
    // Right vertical bar
    canvas->fillRect(xN + 24, startY, 8, 48, titleColor);
    // Diagonal (approximation using small rectangles)
    for (int i = 0; i < 48; i += 4) {
        canvas->fillRect(xN + 8 + i / 3, startY + i, 4, 4, titleColor);
    }

    // --- Draw letter A ---
    int xA = xN + letterSpacing;
    // Right diagonal of A
    for (int i = 0; i < 48; i += 4) {
        if (i >= 40) {
            canvas->fillRect(14 + xA + i / 3, startY + i, 6, 4, titleColor);
        } else {
            canvas->fillRect(14 + xA + i / 3, startY + i, 6, 6, titleColor);
        }
    }
    // Left diagonal of A
    for (int i = 0; i < 48; i += 4) {
        if (i >= 40) {
            canvas->fillRect(xA + 16 - i / 3, startY + i, 6, 4, titleColor);
        } else {
            canvas->fillRect(xA + 16 - i / 3, startY + i, 6, 6, titleColor);
        }
    }
    // Horizontal crossbar of A
    canvas->fillRect(xA + (4 * 2.5), startY + 24 + 4, 16, 8, titleColor);

    // --- Draw letter K ---
    int xK = xA + letterSpacing;
    // Vertical bar for K
    canvas->fillRect(xK, startY, 8, 48, titleColor);
    // Upper diagonal
    for (int i = 0; i <= 24; i += 4) {
        if (i == 24) {
            canvas->fillRect(xK + 8 + i, startY + 22 - i, 4, 4, titleColor);
        } else {
            canvas->fillRect(xK + 8 + i, startY + 22 - i, 4, 6, titleColor);
        }
    }
    // Lower diagonal
    for (int i = 0; i < 24; i += 4) {
        canvas->fillRect(xK + 8 + i, startY + 22 + i, 4, 6, titleColor);
    }

    // --- Draw letter E ---
    int xE = xK + letterSpacing;
    // Vertical bar for E
    canvas->fillRect(xE, startY, 8, 48, titleColor);
    // Top horizontal bar
    canvas->fillRect(xE, startY, 30, 8, titleColor);
    // Middle horizontal bar
    canvas->fillRect(xE, startY + 20, 24, 8, titleColor);
    // Bottom horizontal bar
    canvas->fillRect(xE, startY + 40, 30, 8, titleColor);
}

// MARK: - Options Menu
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

    if (state.b.justPressed && currentGameState == GameState::Game) {
        GameState::Menu;
        return;
    }

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
    canvas->setCursor(canvas->width() / 2 - 48, canvas->height() / 2 - 50);
    canvas->setTextColor(lilka::colors::Red);
    canvas->print(Strings::GAME_OVER);

    // Your Score Preview
    char scoreText[16];
    std::snprintf(scoreText, sizeof(scoreText), Strings::YOUR_SCORE_FORMAT, score);
    canvas->setCursor(canvas->width() / 2 - 68, canvas->height() / 2 - 20);
    canvas->setTextColor(lilka::colors::White);
    canvas->print(scoreText);

    // High Score Preview
    char highScoreText[16];
    std::snprintf(highScoreText, sizeof(highScoreText), Strings::HIGH_SCORE_FORMAT, globalHighScore);
    canvas->setCursor(canvas->width() / 2 - 68, canvas->height() / 2);
    canvas->setTextColor(lilka::colors::Yellow);
    canvas->print(highScoreText);

    // Press A Preview
    canvas->setCursor(canvas->width() / 2 - 100, canvas->height() / 2 + 30);
    canvas->setTextColor(lilka::colors::Green);
    canvas->print(Strings::PRESS_A_TRY_AGAIN);

    // Press B Preview
    canvas->setCursor(canvas->width() / 2 - 80, canvas->height() / 2 + 50);
    canvas->setTextColor(lilka::colors::Red_orange);
    canvas->print(Strings::PRESS_B_EXIT);
}