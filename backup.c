#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>

#define G 1000
#define PLAYER_JUMP_SPD 250.0f
#define PLAYER_HOR_SPD 200.0f
#define BG_SPEED 350.0f
#define ENV_ITEMS_LENGTH 3

typedef struct Player {
    Vector2 position;
    float speed;
    bool canJump;
} Player;

typedef struct EnvItem {
    Rectangle rect;
    int blocking;
    Texture2D texture;
} EnvItem;

//----------------------------------------------------------------------------------
// Module functions declaration
//----------------------------------------------------------------------------------
void UpdatePlayer(Player *player, EnvItem *envItems, float delta);
void UpdatePipePosition();
int RandInt();
//-----------------------------------------------------------------------------------
// Program main entry point
//-----------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //-------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Fish Bird");
    InitAudioDevice();
    Texture2D fish = LoadTexture("./images/peixe-padrao-pq.png");
    Texture2D pipeFloor = LoadTexture("./images/cano-baixo.png");
    Texture2D pipeCeiling = LoadTexture("./images/cano-cima.png");
    Music music = LoadMusicStream("./images/xuxa.mp3");
    PlayMusicStream(music);
    SetMusicVolume(music, 0.1f);

    Player player = { 0 };
    player.position = (Vector2){ -150, 150 };
    player.speed = 0;
    // player.canJump = false;
    EnvItem envItems[3][2];

    int distance = 0;
    for(int i = 0; i < 3; i++) {
        int randomY = RandInt();
        Rectangle rect = { distance, randomY, 140, 200 };
        envItems[i][0] = (EnvItem){ rect, 1, pipeFloor };
        rect.y -= 550;
        rect.height += 220;
        envItems[i][1] = (EnvItem){ rect, 1, pipeCeiling};
        distance += 350;
    }

    Camera2D camera = {};
    camera.target = (Vector2){50, 200};
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update
        //----------------------------------------------------------------------------------
        float deltaTime = GetFrameTime();

        UpdatePlayer(&player, *envItems, deltaTime);
        UpdateMusicStream(music);

        //-----------------------------------------------------------------------------

        // Draw 
        //----------------------------------------------------------------------------------
        // DrawTextureV(texture, player.position, WHITE); 
        DrawTexture(fish, player.position.x + 300, player.position.y, WHITE); 
        BeginDrawing();

            ClearBackground(BLUE);

            BeginMode2D(camera);
                float speed = 100.0f;
                for (int i = 0; i < ENV_ITEMS_LENGTH; i++) {
                    envItems[i][0].rect.x -= speed * GetFrameTime(); // Atualiza a posição x do item
                    envItems[i][1].rect.x -= speed * GetFrameTime(); // Atualiza a posição x do item

                    // Se o item saiu completamente da tela, move-o de volta para a direita
                    if (envItems[i][0].rect.x + envItems[i][0].rect.width < player.position.x - 200) {
                        // envItems[i][0].rect.x += screenWidth + envItems[i][0].rect.width;
                        // envItems[i][1].rect.x += screenWidth + envItems[i][1].rect.width;
                        UpdatePipePosition(envItems);
                    }
                    DrawTextureV(envItems[i][0].texture, (Vector2){envItems[i][0].rect.x - 20, envItems[i][0].rect.y}, WHITE);
                    DrawTextureV(envItems[i][1].texture, (Vector2){envItems[i][1].rect.x - 20, envItems[i][1].rect.y}, WHITE);
                }

                //Rectangle playerRect = { player.position.x - 20, player.position.y - 40, 40, 40 };
                // DrawRectangleRec(playerRect, RED);
                
                // DrawCircle(player.position.x, player.position.y, 5, GOLD);

            EndMode2D();
            
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(fish);
    UnloadTexture(pipeCeiling);
    UnloadTexture(pipeFloor);
    UnloadMusicStream(music);
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void UpdatePlayer(Player *player, EnvItem *envItems, float delta)
{
    if (IsKeyDown(KEY_SPACE))
    {
        player->speed = -PLAYER_JUMP_SPD;
    }
    // esse trecho de código verifica se atingiu um obstaculo
    bool hitObstacle = false;
    for (int i = 0; i < ENV_ITEMS_LENGTH; i++)
    {
        EnvItem *ei = envItems + i;
        Vector2 *p = &(player->position);
        if (CheckCollisionPointRec(player->position, ei->rect) || CheckCollisionPointRec(player->position, (ei + 1)->rect))
        {
            hitObstacle = true;
            player->speed = 0.0f;
            p->y = ei->rect.y;
            break;
        }
    }

    if (!hitObstacle)
    {
        player->position.y += player->speed*delta;
        player->speed += G*delta;
        player->canJump = false;
    }
    else player->canJump = true;
}

void UpdatePipePosition(EnvItem **pipes) {
    for (int i = 0; i < ENV_ITEMS_LENGTH - 1; i++) {
        pipes[i] = pipes[i + 1];
    }
    Texture2D pipeFloor = LoadTexture("./images/cano-baixo.png");
    Texture2D pipeCeiling = LoadTexture("./images/cano-cima.png");
    int randomY = RandInt();
    Rectangle rect = { 700, randomY, 140, 200 };
    EnvItem pipe1 = (EnvItem){ rect, 1, pipeFloor };
    rect.y -= 550;
    rect.height += 220;
    EnvItem pipe2 = (EnvItem){ rect, 1, pipeCeiling};
    UnloadTexture(pipeCeiling);
    UnloadTexture(pipeFloor);
    EnvItem array[] = {pipe1, pipe2};
    pipes[ENV_ITEMS_LENGTH - 1] = array; 
}

int RandInt() {
    return 120 + rand() % (250 + 1 - 120);     
}