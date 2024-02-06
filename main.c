#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <string.h>

#define G 1000
#define PLAYER_JUMP_SPD 230.0f
#define PLAYER_HOR_SPD 200.0f
#define BG_SPEED 350.0f
#define ENV_ITEMS_LENGTH 4
#define NUM_TEXTURES 4

typedef struct Player {
    Vector2 position;
    float speed;
    int score;
} Player;

typedef struct EnvItem {
    Rectangle rect;
    bool passed;
    Texture2D texture;
} EnvItem;

int UpdatePlayer(Player *player, float delta);
void UpdatePipePosition();
int RandInt();
void CreatePipe();

EnvItem envItems[ENV_ITEMS_LENGTH][2];
int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Fish Bird");
    InitAudioDevice();
    Texture2D fishTextures[NUM_TEXTURES];
    fishTextures[0] = LoadTexture("./images/fish-fin-default.png");
    fishTextures[1] = LoadTexture("./images/fish-fin-up.png");
    fishTextures[2] = LoadTexture("./images/fish-fin-down.png");
    Texture2D pipeFloor = LoadTexture("./images/cano-baixo.png");
    Texture2D pipeCeiling = LoadTexture("./images/cano-cima.png");
    Texture2D background = LoadTexture("./images/bg-picture.png");
    Image icon = LoadImage("./images/fish-fin-default.png");
    Music music = LoadMusicStream("./audio/xuxa.mp3");
    Sound point = LoadSound("./audio/point.ogg");
    Sound hit = LoadSound("./audio/hit.ogg");
    SetSoundVolume(hit, 0.3f);
    SetSoundVolume(point, 0.3f);
    PlayMusicStream(music);
    SetMusicVolume(music, 0.2f);
    SetWindowIcon(icon);

    int frameCounter = 0;
    Player player = { 0 };
    player.position = (Vector2){ -150, 150 };
    player.score = 0;
    player.speed = 0;

    CreatePipe(pipeFloor, pipeCeiling);
    Camera2D camera = {};
    camera.target = (Vector2){80, 200};
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    SetTargetFPS(60);

    int screen  = -1;
    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        
        UpdateMusicStream(music);
        
        BeginDrawing();
        
        DrawTexture(background, 0, 0, WHITE);

        if (screen == -1) 
        {
            
            HideCursor();
            DrawTexture(fishTextures[0], player.position.x + 300, player.position.y, WHITE); 
            DrawText("Press Enter to start", 80, 210, 20, WHITE);
            for (int i = 0; i < ENV_ITEMS_LENGTH; i++) 
            {
                DrawTextureV(envItems[i][0].texture, (Vector2){envItems[i][0].rect.x + 300, envItems[i][0].rect.y}, WHITE);
                DrawTextureV(envItems[i][1].texture, (Vector2){envItems[i][1].rect.x + 300, envItems[i][1].rect.y}, WHITE);
                }
            if (IsKeyPressed(KEY_ENTER)) screen = 0;        
        } 

        if (screen == 0) 
        
        {
            
            HideCursor();
            screen = UpdatePlayer(&player, deltaTime);
            frameCounter++;
            Texture2D currentFishTexture = fishTextures[(frameCounter / 8) % 2];
            DrawTexture(currentFishTexture, player.position.x + 300, player.position.y, WHITE);

            BeginMode2D(camera);
                float speed = 100.0f;
                for (int i = 0; i < ENV_ITEMS_LENGTH; i++) {
                     
                    envItems[i][0].rect.x -= speed * GetFrameTime(); // Atualiza a posição x do item
                    envItems[i][1].rect.x -= speed * GetFrameTime(); // Atualiza a posição x do item

                    // Se o item saiu completamente da tela, move-o de volta para a direita
                    
                    DrawTextureV(envItems[i][0].texture, (Vector2){envItems[i][0].rect.x, envItems[i][0].rect.y}, WHITE);
                    DrawTextureV(envItems[i][1].texture, (Vector2){envItems[i][1].rect.x, envItems[i][1].rect.y}, WHITE);
                }
                if (envItems[0][0].rect.x + envItems[0][0].rect.width < player.position.x - 170) {
                        UpdatePipePosition();
                    }
                if (envItems[0][0].rect.x + envItems[0][0].rect.width < player.position.x) {
                    if (!envItems[0][0].passed) {
                        envItems[0][0].passed = true;
                        player.score++;
                        PlaySound(point);
                    }
                }


            EndMode2D();
            DrawText(TextFormat("Score: %i", player.score), 100, 30, 20, BLACK);
            PlaySound(hit);
        }

        if (screen == 1) 
        
        {
            HideCursor();
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
    }
    UnloadTexture(fishTextures[0]);
    UnloadTexture(fishTextures[1]);
    UnloadTexture(fishTextures[2]);
    UnloadTexture(pipeCeiling);
    UnloadTexture(pipeFloor);
    UnloadTexture(background);
    UnloadMusicStream(music);
    UnloadSound(point);
    UnloadSound(hit);
    UnloadImage(icon);
    CloseWindow();
    return 0;
}

int UpdatePlayer(Player *player, float delta)
{
    if (IsKeyDown(KEY_SPACE))
    {
        player->speed = -PLAYER_JUMP_SPD;
    }  
    for (int i = 0; i < ENV_ITEMS_LENGTH; i++)
    {
        EnvItem *ei = &envItems[i][0];
        if (CheckCollisionPointRec(player->position, ei->rect) || CheckCollisionPointRec(player->position, (ei + 1)->rect) || player->position.y >= 440)
        {
            player->speed = 0.0f;
            return 1;
        }
    }

    player->position.y += player->speed*delta;
    player->speed += G*delta;

    return 0;
}

void UpdatePipePosition() {
    for (int i = 0; i < ENV_ITEMS_LENGTH - 1; i++) {
        memcpy(&envItems[i], &envItems[i + 1], sizeof(EnvItem));
        memcpy(&envItems[i][1], &envItems[i + 1][1], sizeof(EnvItem));
    }
    Texture2D pipeFloor = LoadTexture("./images/cano-baixo.png");
    Texture2D pipeCeiling = LoadTexture("./images/cano-cima.png");
    int randomY = RandInt();
    Rectangle rect = { 570, randomY, 135 , 200 };
    envItems[3][0]= (EnvItem){ rect, false, pipeFloor };
    rect.y -= 575;
    rect.height += 250;
    envItems[3][1] = (EnvItem){ rect, false, pipeCeiling};
}

int RandInt() {
    return 150 + rand() % (400 + 1 - 150);     
}

void CreatePipe(Texture2D pipeFloor, Texture2D pipeCeiling) {
    int distance = 0;
    for(int i = 0; i < ENV_ITEMS_LENGTH; i++) {
        int randomY = RandInt();
        Rectangle rect = { distance, randomY, 135 , 200 };
        envItems[i][0] = (EnvItem){ rect, false, pipeFloor };
        rect.y -= 575;
        rect.height += 250;
        envItems[i][1] = (EnvItem){ rect, false, pipeCeiling};
        distance += 250;
    }
}