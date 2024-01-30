#include "raylib.h"

float radius = 20;

int main() {
    int screenWidth = 800;
    int screenHeight = 500;
    int vectorX = 50;
    int vectorY = 100;


    InitWindow(screenWidth, screenHeight, "Flappy Bird");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        //PARTE 1: Processar lógica do jogo
        //PARTE 2: Desenhar na tela
        BeginDrawing();
            HideCursor();
            ClearBackground(SKYBLUE);
            
            // DrawCircleV(position, radius, DARKGRAY);
            DrawCircle(vectorX, vectorY, radius, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
}