#include "raylib.h"

int main() {
    InitWindow(800, 450, "Hello World");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        //PARTE 1: Processar lógica do jogo

        //PARTE 2: Desenhar na tela
        BeginDrawing();

        EndDrawing();
    }

    CloseWindow();
}