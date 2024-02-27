#include "raylib.h"
#include <stdio.h>

//------------------------------------------------------------------------------------------
// Types and Structures Definition
//------------------------------------------------------------------------------------------

#define CELL_SIZE 50
#define GRID_WIDTH 16
#define GRID_HEIGHT 12
#define SNAKE_MAX_LENGTH 192

typedef enum GameScreen { LOGO = 0, TITLE, GAMEPLAY, ENDING } GameScreen;

typedef enum Direction { UP = 1, RIGHT, DOWN, LEFT } Direction;

typedef struct SnakeSegment {
  int x;
  int y;
  Direction dir;
} SnakeSegment;

static bool canMove = true;
static Vector2 food = {0};
static SnakeSegment snake[SNAKE_MAX_LENGTH] = {0};
static int snakeLength = 1;
static int highscore = 1;
static bool newHighscore = false;

static void Load();

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 800;
  const int screenHeight = 600;

  InitAudioDevice();

  Sound gameOverSound = LoadSound("sounds/jingles-saxophone_00.ogg");
  Sound eatSound = LoadSound("sounds/powerUp2.ogg");

  SetRandomSeed(GetTime());

  InitWindow(screenWidth, screenHeight, "Snake");

  GameScreen currentScreen = LOGO;

  int framesCounter = 0; // Useful to count frames
  int animCounter = -1;

  float dt = 0.0f;
  float timer = 0.0f;

  FILE *fptr;

  fptr = fopen("save.txt", "r");
  if (fptr != NULL) {
    char buffer[10];
    fgets(buffer, sizeof(buffer), fptr);
    sscanf(buffer, "%d", &highscore);
    fclose(fptr);
  }

  SetTargetFPS(60); // Set desired framerate (frames-per-second)
  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    // Update
    //----------------------------------------------------------------------------------

    dt = GetFrameTime();
    switch (currentScreen) {
    case LOGO: {
      framesCounter++; // Count frames

      // Wait for 2 seconds (120 frames) before jumping to TITLE screen
      if (framesCounter % 10 == 0) {
        animCounter++;
      }
      if (framesCounter > 200) {
        currentScreen = TITLE;
      }
    } break;
    case TITLE: {
      if (IsKeyPressed(KEY_ENTER)) {
        currentScreen = GAMEPLAY;
        Load();
      }
    } break;
    case GAMEPLAY: {
      timer += dt;
      if (IsKeyPressed(KEY_UP) && snake[0].dir != UP && snake[0].dir != DOWN &&
          canMove) {
        snake[0].dir = UP;
        canMove = false;
      }
      if (IsKeyPressed(KEY_DOWN) && snake[0].dir != UP &&
          snake[0].dir != DOWN && canMove) {
        snake[0].dir = DOWN;
        canMove = false;
      }
      if (IsKeyPressed(KEY_RIGHT) && snake[0].dir != RIGHT &&
          snake[0].dir != LEFT && canMove) {
        snake[0].dir = RIGHT;
        canMove = false;
      }
      if (IsKeyPressed(KEY_LEFT) && snake[0].dir != RIGHT &&
          snake[0].dir != LEFT && canMove) {
        snake[0].dir = LEFT;
        canMove = false;
      }

      if (timer >= 0.15) {
        timer = 0;

        SnakeSegment tail = snake[snakeLength - 1];

        Direction prevSnakeSegDirection = snake[0].dir;
        for (int i = 0; i < snakeLength; i++) {
          SnakeSegment nextPosition = snake[i];
          if (snake[i].dir == UP) {
            nextPosition.y--;
            if (nextPosition.y < 0) {
              nextPosition.y = GRID_HEIGHT - 1;
            }
          } else if (snake[i].dir == DOWN) {
            nextPosition.y++;
            if (nextPosition.y >= GRID_HEIGHT) {
              nextPosition.y = 0;
            }
          } else if (snake[i].dir == RIGHT) {
            nextPosition.x++;
            if (nextPosition.x >= GRID_WIDTH) {
              nextPosition.x = 0;
            }
          } else if (snake[i].dir == LEFT) {
            nextPosition.x--;
            if (nextPosition.x < 0) {
              nextPosition.x = GRID_WIDTH - 1;
            }
          }

          Direction currentSegmentDirection = snake[i].dir;
          snake[i].dir = prevSnakeSegDirection;

          prevSnakeSegDirection = currentSegmentDirection;

          snake[i].x = nextPosition.x;
          snake[i].y = nextPosition.y;
        }

        for (int i = 1; i < snakeLength; i++) {
          if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            PlaySound(gameOverSound);
            if (snakeLength > highscore) {
              newHighscore = true;
              highscore = snakeLength;
              FILE *fptr;
              fptr = fopen("save.txt", "w");
              if (fptr != NULL) {
                fprintf(fptr, "%d\n", highscore);
                fclose(fptr);
              }
            }
            currentScreen = ENDING;
            break;
          }
        }

        if (snake[0].x == food.x && snake[0].y == food.y) {
          PlaySound(eatSound);
          snake[snakeLength] = tail;
          snakeLength++;
          food.x = GetRandomValue(0, GRID_WIDTH - 1);
          food.y = GetRandomValue(0, GRID_HEIGHT - 1);
        }

        canMove = true;
      }

    } break;
    case ENDING: {
      if (IsKeyPressed(KEY_ENTER)) {
        currentScreen = TITLE;
      }
    } break;
    default:
      break;
    }

    //----------------------------------------------------------------------------------
    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    ClearBackground(RAYWHITE);

    switch (currentScreen) {
    case LOGO: {
      int textWidth = MeasureText("SNAKE", 80);
      DrawText("SNAKE", (screenWidth - textWidth) / 2, 20, 80, LIGHTGRAY);
      textWidth = MeasureText("Intro animation", 20);
      DrawText("Intro animation", (screenWidth - textWidth) / 2, 120, 20,
               LIGHTGRAY);

      if (animCounter < 10) {
        DrawRectangle(500, 300, CELL_SIZE, CELL_SIZE, RED);
      } else {

        DrawRectangle((animCounter - 1) * CELL_SIZE, 300, CELL_SIZE, CELL_SIZE,
                      GREEN);
      }
      DrawRectangle(animCounter * CELL_SIZE, 300, CELL_SIZE, CELL_SIZE, GREEN);
    } break;
    case TITLE: {
      DrawRectangle(0, 0, screenWidth, screenHeight, GREEN);
      const char *text = "PRESS ENTER TO START";
      int textWidth = MeasureText(text, 20);
      DrawText(text, (screenWidth - textWidth) / 2, 240, 20, DARKGREEN);

      text = "SNAKE";
      textWidth = MeasureText(text, 80);
      DrawText(text, (screenWidth - textWidth) / 2, 20, 80, WHITE);
    } break;
    case GAMEPLAY: {
      DrawRectangle(0, 0, screenWidth, screenHeight, GRAY);
      for (int i = 0; i < snakeLength; i++) {
        SnakeSegment ss = snake[i];
        DrawRectangle(ss.x * CELL_SIZE, ss.y * CELL_SIZE, CELL_SIZE, CELL_SIZE,
                      GREEN);
      }

      DrawRectangle(food.x * CELL_SIZE, food.y * CELL_SIZE, CELL_SIZE,
                    CELL_SIZE, RED);

      const char *scoreText = TextFormat("Score: %d", snakeLength);
      int textWidth = MeasureText(scoreText, 20);
      DrawText(scoreText, (screenWidth - textWidth) / 2, 20, 20, WHITE);

    } break;
    case ENDING: {
      DrawRectangle(0, 0, screenWidth, screenHeight, BLUE);

      const char *text = "PRESS ENTER TO RETURN TO TITLE SCREEN";
      int textWidth = MeasureText(text, 20);
      DrawText(text, (screenWidth - textWidth) / 2, 240, 20, DARKBLUE);

      text = TextFormat("Score: %d", snakeLength);
      textWidth = MeasureText(text, 40);
      DrawText(text, (screenWidth - textWidth) / 2, 120, 40, WHITE);

      if (newHighscore) {
        text = "NEW HIGHSCORE!";
        textWidth = MeasureText(text, 60);
        DrawText(text, (screenWidth - textWidth) / 2, 20, 60, WHITE);
      } else {
        text = TextFormat("Score to beat: %d", highscore);
        textWidth = MeasureText(text, 60);
        DrawText(text, (screenWidth - textWidth) / 2, 20, 60, WHITE);
      }

    } break;
    default:
      break;
    }

    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  UnloadSound(eatSound);
  UnloadSound(gameOverSound);

  CloseAudioDevice();

  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}

void Load() {
  canMove = true;
  newHighscore = false;
  food = (Vector2){GetRandomValue(0, GRID_WIDTH - 1),
                   GetRandomValue(0, GRID_HEIGHT - 1)};

  snakeLength = 1;

  snake[0] = (SnakeSegment){GetRandomValue(0, GRID_WIDTH - 1),
                            GetRandomValue(0, GRID_HEIGHT - 1), RIGHT};
}
