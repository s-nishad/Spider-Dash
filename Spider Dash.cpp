#include "raylib.h"
#include <string>

struct AnimeData {
    Rectangle rect;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

bool isOnGround(AnimeData playerData, int windowHeight) {
    return playerData.pos.y >= windowHeight - playerData.rect.height;
}

AnimeData updateAnimeData(AnimeData animeData, float dt, int maxFrame) {
    animeData.runningTime += dt;
    if (animeData.runningTime >= animeData.updateTime) {
        animeData.runningTime = 0.0;
        animeData.rect.x = animeData.frame * animeData.rect.width;
        animeData.frame++;
        if (animeData.frame > maxFrame) {
            animeData.frame = 0;
        }
    }
    return animeData;
}

int main() {
    // Declare the window size by array
    const int windowSize[2] = {800, 450};

    std::string gameTitle = "Spider Dash";

    InitWindow(windowSize[0], windowSize[1], gameTitle.c_str());

    // Gravity (pixels/sec)/sec
    const int gravity = 1000;

    // Player texture
    Texture2D player = LoadTexture("textures/spider.png");

    AnimeData playerData;
    playerData.rect.width = player.width / 6;
    playerData.rect.height = player.height;
    playerData.rect.x = 0;
    playerData.rect.y = 0;
    playerData.pos.x = windowSize[0] / 2 - playerData.rect.width / 2;
    playerData.pos.y = windowSize[1] - playerData.rect.height;
    playerData.frame = 0;
    playerData.updateTime = 1.0 / 12.0;
    playerData.runningTime = 0.0;

    // enemy texture
    Texture2D enemy = LoadTexture("textures/enemy.png");

    const int sizeOfAnime = 60;
    AnimeData enemies[sizeOfAnime] = {};
    for (int i = 0; i < sizeOfAnime; i++) {
        enemies[i].rect.width = enemy.width / 8;
        enemies[i].rect.height = enemy.height / 8;
        enemies[i].rect.x = 0;
        enemies[i].rect.y = 0;
        enemies[i].pos.x = windowSize[0] + i * 300;
        enemies[i].pos.y = windowSize[1] - enemy.height / 8;
        enemies[i].frame = 0;
        enemies[i].updateTime = 1.0 / 12.0;
        enemies[i].runningTime = 0.0;
    }

    float finishLine = {enemies[sizeOfAnime - 1].pos.x};

    // enemy velocity
    const int animeVelocity = -200;

    bool isInAir = false;
    const int jumpVelocity = -600;
    int velocity = 0;

    // Background
    Texture2D background = LoadTexture("textures/background.png");
    float bgX = 0;

    Texture2D midground = LoadTexture("textures/midground.png");
    float mgX = 0;

    Texture2D foreground = LoadTexture("textures/foreground.png");
    float fgX = 0;

    bool collision = false;

    bool gameStarted = false;  // Flag to indicate if the game has started
    bool gameOver = false;    // Flag to indicate if the game is over
    int score = 0;            // Player score
    int highScore = 0;        // Player high score
    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose()) {

        // Delta time is the time in seconds since the last frame
        const float dt = GetFrameTime();

        // Check if the game has started
        if (!gameStarted) {
            if (IsKeyPressed(KEY_SPACE)) {
                gameStarted = true;
                gameOver = false;
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("Click space to start!", 190, 200, 20, LIGHTGRAY);
            EndDrawing();

            continue;  // Skip the rest of the game loop until game starts
        }

        // Game logic and rendering
        if (!gameOver) {
            BeginDrawing();

            ClearBackground(RAYWHITE);

            bgX -= 20 * dt;
            if (bgX <= -background.width * 2) {
                bgX = 0.0;
            }

            Vector2 bgPos = {bgX, 0.0};
            DrawTextureEx(background, bgPos, 0.0, 2.0, WHITE);
            Vector2 bg2Pos = {bgX + background.width * 2, 0.0};
            DrawTextureEx(background, bg2Pos, 0.0, 2.0, WHITE);

            mgX -= 40 * dt;
            if (mgX <= -midground.width * 2) {
                mgX = 0.0;
            }

            Vector2 mgPos = {mgX, 0.0};
            DrawTextureEx(midground, mgPos, 0.0, 2.0, WHITE);
            Vector2 mg2Pos = {mgX + midground.width * 2, 0.0};
            DrawTextureEx(midground, mg2Pos, 0.0, 2.0, WHITE);

            fgX -= 80 * dt;
            if (fgX <= -foreground.width * 2) {
                fgX = 0.0;
            }

            Vector2 fgPos = {fgX, 0.0};
            DrawTextureEx(foreground, fgPos, 0.0, 2.0, WHITE);
            Vector2 fg2Pos = {fgX + foreground.width * 2, 0.0};
            DrawTextureEx(foreground, fg2Pos, 0.0, 2.0, WHITE);
            
            DrawText(gameTitle.c_str(), 10, 10, 20, WHITE);
            DrawText(("Score: " + std::to_string(score)).c_str(), 10, 40, 20, WHITE);
            DrawText(("High Score: " + std::to_string(highScore)).c_str(), 10, 70, 20, WHITE);

            // Ground check
            if (isOnGround(playerData, windowSize[1])) {
                velocity = 0;
                isInAir = false;
            } else {
                velocity += gravity * dt;
                isInAir = true;
            }

            if (IsKeyPressed(KEY_SPACE) && !isInAir) {
                velocity += jumpVelocity;
            }

            // Update enemy position
            for (int i = 0; i < sizeOfAnime; i++) {
                enemies[i].pos.x += animeVelocity * dt;
            }

            // finish line
            finishLine += animeVelocity + dt;

            // Update player position
            playerData.pos.y += velocity * dt;

            // If not in the air, update animation
            if (!isInAir) {
                playerData = updateAnimeData(playerData, dt, 5);
            }

            // update enemy animation
            for (int i = 0; i < sizeOfAnime; i++) {
                enemies[i] = updateAnimeData(enemies[i], dt, 7);
            }

            for (AnimeData enemy : enemies) {
                float padding = 10.0;
                Rectangle enemyRect = {
                    enemy.pos.x + padding,
                    enemy.pos.y + padding,
                    enemy.rect.width - padding * 2,
                    enemy.rect.height - padding * 2
                };
                Rectangle playerRect = {
                    playerData.pos.x,
                    playerData.pos.y,
                    playerData.rect.width,
                    playerData.rect.height
                };
                if (CheckCollisionRecs(enemyRect, playerRect)) {
                    collision = true;
                }
            }

            if (collision) {
                // Handle collision
                gameOver = true;
                if (score > highScore) {
                    highScore = score;
                }
            } else {
                DrawTextureRec(player, playerData.rect, playerData.pos, WHITE);

                for (int i = 0; i < sizeOfAnime; i++) {
                    DrawTextureRec(enemy, enemies[i].rect, enemies[i].pos, WHITE);
                }
                score++;
            }

            EndDrawing();
        } else {
            // Game over logic
            if (IsKeyPressed(KEY_SPACE)) {
                // Reset game
                gameStarted = true;
                gameOver = false;
                playerData.pos.x = windowSize[0] / 2 - playerData.rect.width / 2;
                playerData.pos.y = windowSize[1] - playerData.rect.height;
                for (int i = 0; i < sizeOfAnime; i++) {
                    enemies[i].pos.x = windowSize[0] + i * 300;
                }
                finishLine = enemies[sizeOfAnime - 1].pos.x;
                collision = false;
                score = 0;
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);
            // show score and high score
            DrawText(gameTitle.c_str(), 10, 10, 20, WHITE);
            DrawText(("Score: " + std::to_string(score)).c_str(), 10, 40, 20, WHITE);
            DrawText(("High Score: " + std::to_string(highScore)).c_str(), 10, 70, 20, WHITE);
            DrawText("Game Over!", windowSize[0] / 4, windowSize[1] / 2, 40, RED);
            DrawText("Press SPACE to Restart", windowSize[0] / 4 + 20, windowSize[1] / 2 + 40, 20, RED);
            EndDrawing();
        }
    }

    UnloadTexture(player);
    UnloadTexture(enemy);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    CloseWindow();

    return 0;
}
