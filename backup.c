#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <string.h>

#define G 1000
#define PLAYER_JUMP_SPD 250.0f
#define PLAYER_HOR_SPD 200.0f
#define BG_SPEED 350.0f
#define ENV_ITEMS_LENGTH 4

typedef struct Player {
    Vector2 position;
    float speed;
    int score;
    bool canJump;
} Player;

typedef struct EnvItem {
    Rectangle rect;
    int blocking;
    bool passed;
    Texture2D texture;
} EnvItem;

//----------------------------------------------------------------------------------
// Module functions declaration
//----------------------------------------------------------------------------------
int UpdatePlayer(Player *player, float delta);
void UpdatePipePosition();
int RandInt();
void CreatePipe();
//-----------------------------------------------------------------------------------
// Program main entry point
//-----------------------------------------------------------------------------------
EnvItem envItems[ENV_ITEMS_LENGTH][2];
int main(void)
{
    // Initialization
    //-------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Fish Bird");
    InitAudioDevice();
    Texture2D fish = LoadTexture("./images/fish-fin-default.png");
    Texture2D pipeFloor = LoadTexture("./images/cano-baixo.png");
    Texture2D pipeCeiling = LoadTexture("./images/cano-cima.png");
    Music music = LoadMusicStream("./images/xuxa.mp3");
    Sound music2 = LoadSound("./audio/point.ogg");
    Sound hit = LoadSound("./audio/hit.ogg");
    SetSoundVolume(hit, 0.3f);
    SetSoundVolume(music2, 0.3f);
    PlayMusicStream(music);
    SetMusicVolume(music, 0.2f);

    Player player = { 0 };
    player.position = (Vector2){ -150, 150 };
    player.score = 0;
    player.speed = 0;
    // player.canJump = false;

    CreatePipe(pipeFloor, pipeCeiling);
    Camera2D camera = {};
    camera.target = (Vector2){80, 200};
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    int screen  = -1;
    while (!WindowShouldClose())
    {
        // Update
        //----------------------------------------------------------------------------------
        float deltaTime = GetFrameTime();
        
        camera.zoom += ((float)GetMouseWheelMove()*0.05f);

        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
        else if (camera.zoom < 0.25f) camera.zoom = 0.25f;
        UpdateMusicStream(music);

        //-----------------------------------------------------------------------------

        // Draw 
        //----------------------------------------------------------------------------------
        // DrawTextureV(texture, player.position, WHITE); 
        
        BeginDrawing();

        if (screen == -1) {
            ClearBackground(BLUE);
            DrawTexture(fish, player.position.x + 300, player.position.y, WHITE); 
            DrawText("Press Enter to start", 300, 170, 20, WHITE);
            if (IsKeyPressed(KEY_ENTER)){
                player.position = (Vector2){ -150, 150 };
                player.score = 0;
                CreatePipe(pipeFloor, pipeCeiling);
                screen = 0;
            }         
        } 

        if (screen == 0) {
            screen = UpdatePlayer(&player, deltaTime);
            DrawTexture(fish, player.position.x + 300, player.position.y, WHITE); 
            ClearBackground(BLUE);

            BeginMode2D(camera);
                float speed = 100.0f;
                for (int i = 0; i < ENV_ITEMS_LENGTH; i++) {
                    envItems[i][0].rect.x -= speed * GetFrameTime(); // Atualiza a posição x do item
                    envItems[i][1].rect.x -= speed * GetFrameTime(); // Atualiza a posição x do item

                    // Se o item saiu completamente da tela, move-o de volta para a direita
                    
                    DrawTextureV(envItems[i][0].texture, (Vector2){envItems[i][0].rect.x - 20, envItems[i][0].rect.y}, WHITE);
                    DrawTextureV(envItems[i][1].texture, (Vector2){envItems[i][1]. rect.x - 20, envItems[i][1].rect.y}, WHITE);
                }
                if (envItems[0][0].rect.x + envItems[0][0].rect.width < player.position.x - 150) {
                        UpdatePipePosition();
                    }
                if (envItems[0][0].rect.x + envItems[0][0].rect.width < player.position.x) {
                    if (!envItems[0][0].passed) {
                        envItems[0][0].passed = true;
                        player.score++;
                        PlaySound(music2);
                    }
                }
        

            EndMode2D();
            DrawText(TextFormat("Score: %i", player.score), 100, 30, 20, BLACK);
            PlaySound(hit);
        }

        if (screen == 1) {
            ClearBackground(BLUE);
            DrawText("Game Over", 300, 170, 40, WHITE);
            DrawText(TextFormat("Score: %i", player.score), 360, 225, 20, WHITE);

            if (IsKeyPressed(KEY_ENTER)){
                player.position = (Vector2){ -150, 150 };
                player.score = 0;
                CreatePipe(pipeFloor, pipeCeiling);
                screen = -1;
            }          
        }

        EndDrawing();
        //               ----------------------------------------------------------------------------------
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

int UpdatePlayer(Player *player, float delta)
{
    if (IsKeyDown(KEY_SPACE))
    {
        player->speed = -PLAYER_JUMP_SPD;
    }
    // esse trecho de código verifica se atingiu um obstaculo
    int hitObstacle = 0;
    for (int i = 0; i < ENV_ITEMS_LENGTH; i++)
    {
        EnvItem *ei = &envItems[i][0];
        if (CheckCollisionPointRec(player->position, ei->rect) || CheckCollisionPointRec(player->position, (ei + 1)->rect))
        {
            hitObstacle = 1;
            player->speed = 0.0f;
            return hitObstacle;
        }
    }

    if (!hitObstacle)
    {
        player->position.y += player->speed*delta;
        player->speed += G*delta;
        player->canJump = false;
    }
    else player->canJump = true;
    return hitObstacle;
}

void UpdatePipePosition() {
    for (int i = 0; i < ENV_ITEMS_LENGTH - 1; i++) {
        memcpy(&envItems[i], &envItems[i + 1], sizeof(EnvItem));
        memcpy(&envItems[i][1], &envItems[i + 1][1], sizeof(EnvItem));
    }
    Texture2D pipeFloor = LoadTexture("./images/cano-baixo.png");
    Texture2D pipeCeiling = LoadTexture("./images/cano-cima.png");
    int randomY = RandInt();
    Rectangle rect = { 570, randomY, 140, 200 };
    envItems[3][0]= (EnvItem){ rect, 1, false,pipeFloor };
    rect.y -= 550;
    rect.height += 220;
    envItems[3][1] = (EnvItem){ rect, 1, false,pipeCeiling};
}

int RandInt() {
    return 100 + rand() % (400 + 1 - 100);     
}

void CreatePipe(Texture2D pipeFloor, Texture2D pipeCeiling) {
    int distance = 0;
    for(int i = 0; i < ENV_ITEMS_LENGTH; i++) {
        int randomY = RandInt();
        Rectangle rect = { distance, randomY, 140, 200 };
        envItems[i][0] = (EnvItem){ rect, 1, false, pipeFloor };
        rect.y -= 550;
        rect.height += 220;
        envItems[i][1] = (EnvItem){ rect, 1, false, pipeCeiling};
        distance += 250;
    }
}

void DrawFish() {

}