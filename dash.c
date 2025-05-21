#include <SDL.h>
#include <stdio.h>

const int WINDOW_WIDTH  = 800;
const int WINDOW_HEIGHT = 600;
const int SQUARE_SIZE   = 50;
const int SPEED         = 5;

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow(
        "Move the Square", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if (!win) {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!ren) {
        SDL_DestroyWindow(win);
        fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Rect sq = {
        (WINDOW_WIDTH  - SQUARE_SIZE) / 2,
        (WINDOW_HEIGHT - SQUARE_SIZE) / 2,
        SQUARE_SIZE, SQUARE_SIZE
    };

    int running = 1;
    SDL_Event e;
    while (running) {
        // handle events
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_w: sq.y -= SPEED; break;
                    case SDLK_s: sq.y += SPEED; break;
                    case SDLK_a: sq.x -= SPEED; break;
                    case SDLK_d: sq.x += SPEED; break;
                    default: break;
                }
                // keep square on-screen
                if (sq.x < 0) sq.x = 0;
                if (sq.y < 0) sq.y = 0;
                if (sq.x > WINDOW_WIDTH  - SQUARE_SIZE)
                    sq.x = WINDOW_WIDTH  - SQUARE_SIZE;
                if (sq.y > WINDOW_HEIGHT - SQUARE_SIZE)
                    sq.y = WINDOW_HEIGHT - SQUARE_SIZE;
            }
        }

        // render
        SDL_SetRenderDrawColor(ren, 0, 144, 48, 255);  // green background
        SDL_RenderClear(ren);

        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255); // white square
        SDL_RenderFillRect(ren, &sq);

        SDL_RenderPresent(ren);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
