#include "raylib.h"
#include "stdlib.h"
#include "time.h"

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
void UpdatePlayer();
void UpdatePipePosition();
int RandInt();
EnvItem* CreatePipe();
//-----------------------------------------------------------------------------------
// Program main entry point
//-----------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //-------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;
    srand(time(NULL));

    InitWindow(screenWidth, screenHeight, "Fish Bird");
    Texture2D fish = LoadTexture("./images/peixe-padrao-pq.png");
    Texture2D pipeFloor = LoadTexture("./images/cano-baixo.png");
    Texture2D pipeCeiling = LoadTexture("./images/cano-cima.png");

    InitAudioDevice();
    Music music = LoadMusicStream("./images/xuxa.mp3");
    PlayMusicStream(music);
    SetMusicVolume(music, 0.1f);

    Player player = { 0 };
    player.position = (Vector2){ -150, 150 };
    player.speed = 0;
    //player.canJump = true;

    EnvItem** envItems = malloc(3 * sizeof(EnvItem*));
    for(int i = 0; i < 3; i++) {
    envItems[i] = malloc(2 * sizeof(EnvItem));
    }
    
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
        camera.zoom += ((float)GetMouseWheelMove()*0.05f);

        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
        else if (camera.zoom < 0.25f) camera.zoom = 0.25f;
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
                        UpdatePipePosition(&envItems);
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

void UpdatePlayer(Player *player, EnvItem **envItems[][2], float delta)
{
    if (IsKeyDown(KEY_SPACE))
    {
        player->speed = -PLAYER_JUMP_SPD;
    }

    bool hitObstacle = false;
    for (int i = 0; i < ENV_ITEMS_LENGTH; i++)
    {
        // Acessa o array bidimensional e depois o EnvItem dentro dele
        EnvItem *ei1 = (*envItems[i])[0];
        EnvItem *ei2 = (*envItems[i])[1];

        if (CheckCollisionPointRec(player->position, ei1->rect) || CheckCollisionPointRec(player->position, ei2->rect))
        {
            TraceLog(LOG_INFO, "bateu");
            hitObstacle = true;
            player->speed = 0.0f;
            //player->position.y = ei1->rect.y;
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
    free(pipes[0]);
    for (int i = 0; i < ENV_ITEMS_LENGTH - 1; i++) {
        *pipes[i] = *pipes[i + 1];
    }

    pipes[ENV_ITEMS_LENGTH - 1] = CreatePipe();
}

int RandInt() {
    return 120 + rand() % (250 + 1 - 120);
}

EnvItem* CreatePipe() {

    EnvItem* envItems = (EnvItem*)malloc(2 * sizeof(EnvItem));
    Texture2D pipeFloor = LoadTexture("./images/cano-baixo.png");
    Texture2D pipeCeiling = LoadTexture("./images/cano-cima.png");

    int randomY = RandInt();
    Rectangle rect = { 700, randomY, 140, 200 };
    envItems[0] = (EnvItem){ rect, 1, pipeFloor };
    rect.y -= 550;
    rect.height += 220;
    envItems[1] = (EnvItem){ rect, 1, pipeCeiling};
    UnloadTexture(pipeCeiling);
    UnloadTexture(pipeFloor);
    return envItems;
}