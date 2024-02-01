#include "raylib.h"
#include "raymath.h"

#define G 1000
#define PLAYER_JUMP_SPD 250.0f
#define PLAYER_HOR_SPD 200.0f
#define BG_SPEED 350.0f

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
void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, float delta);
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
    Texture2D pipe = LoadTexture("./images/cano.png");
    Music music = LoadMusicStream("./images/xuxa.mp3");
    PlayMusicStream(music);
    SetMusicVolume(music, 0.1f);

    Player player = { 0 };
    player.position = (Vector2){ -150, 150 };
    player.speed = 0;
    // player.canJump = false;
    EnvItem envItems[] = {
        {{ 0, 400, 140, 200 }, 1, pipe},
        {{ 300, 200, 140, 10 }, 1, pipe},
        {{ 250, 300, 140, 10 }, 1, pipe},
        {{ 650, 300, 140, 10 }, 1, pipe}
    };

    int envItemsLength = sizeof(envItems)/sizeof(envItems[0]);

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

        UpdatePlayer(&player, envItems, envItemsLength, deltaTime);
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
                for (int i = 0; i < envItemsLength; i++) {
                    envItems[i].rect.x -= speed * GetFrameTime(); // Atualiza a posição x do item

                    // Se o item saiu completamente da tela, move-o de volta para a direita
                    if (envItems[i].rect.x + envItems[i].rect.width < player.position.x - 200) {
                    envItems[i].rect.x += screenWidth + envItems[i].rect.width;
                    }
                    DrawTextureV(envItems[i].texture, (Vector2){envItems[i].rect.x - 20, envItems[i].rect.y}, WHITE);
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
    UnloadTexture(pipe);
    UnloadMusicStream(music);
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, float delta)
{
    if (IsKeyDown(KEY_SPACE))
    {
        player->speed = -PLAYER_JUMP_SPD;
    }
    // esse trecho de código verifica se atingiu um obstaculo
    bool hitObstacle = false;
    for (int i = 0; i < envItemsLength; i++)
    {
        EnvItem *ei = envItems + i;
        Vector2 *p = &(player->position);
        if (ei->blocking &&
    ei->rect.x <= p->x &&
    ei->rect.x + ei->rect.width >= p->x &&
    p->y <= ei->rect.y + ei->rect.height &&
    p->y + player->speed*delta >= ei->rect.y + ei->rect.height)
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