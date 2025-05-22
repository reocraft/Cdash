#include <SDL.h>
#include <stdio.h>
#include <time.h>

const int WINDOW_WIDTH  = 800;
const int WINDOW_HEIGHT = 600;
const int SQUARE_SIZE   = 50;
const int SPEED         = 5;

typedef struct vec {
  float x;
  float y;
} vec_t;

int rng (int min, int max) {
  int random = rand();
  return random % (max - min + 1) + min;
}

void respawn (int* xpos, int* ypos) {
  *xpos = WINDOW_WIDTH;
  *ypos = rng(0, 4) * -100;
}

int main(int argc, char *argv[]) {

  srand(time(NULL));

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

    // sq.x and sq.y keep track of the positions of the square
    SDL_Rect sq = {
        (WINDOW_WIDTH  - SQUARE_SIZE) / 4,
        (WINDOW_HEIGHT - SQUARE_SIZE) / 2,
        SQUARE_SIZE, SQUARE_SIZE
    };

    // Obstacle tubes
    SDL_Rect tube1a = {
      (WINDOW_WIDTH - SQUARE_SIZE) - 267,
      -300,
      SQUARE_SIZE, 400
    };
    SDL_Rect tube1b = {
      (WINDOW_WIDTH - SQUARE_SIZE) - 267,
      tube1a.y + 600,
      SQUARE_SIZE, 400
    };
    SDL_Rect tube2a = {
      (WINDOW_WIDTH - SQUARE_SIZE) - 534,
      -150,
      SQUARE_SIZE, 400
    };
    SDL_Rect tube2b = {
      (WINDOW_WIDTH - SQUARE_SIZE) - 534,
      tube2a.y + 600,
      SQUARE_SIZE, 400
    };
    SDL_Rect tube3a = {
      (WINDOW_WIDTH - SQUARE_SIZE),
      0,
      SQUARE_SIZE, 400
    };
    SDL_Rect tube3b = {
      (WINDOW_WIDTH - SQUARE_SIZE),
      tube3a.y + 600,
      SQUARE_SIZE, 400
    };

    // MAYBE HAVE LIKE 5 DIFFERENT TUBES IN ROTATION AND WHENEVER IT HITS THE LEFT EDGE AND GOES OFF THE SCREEN,
    // WE CALL FOR A FUNCTION WHICH RANDOMIZES THE HEIGHT OF THESE TUBES

    // Value to store velocity added when pressing `space`
    float sq_vel = 0;

    int frame = 0;
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
                    case SDLK_w: 
                      sq.y -= SPEED; 
                      break;
                    case SDLK_s: 
                      sq.y += SPEED; 
                      break;
                    case SDLK_a: 
                      sq.x -= SPEED; 
                      break;
                    case SDLK_d: 
                      sq.x += SPEED; 
                      break;
                    case SDLK_SPACE:
                      if (sq_vel <= 16.0) {
                        sq_vel += 16.0;
                      };
                      break;
                    default: 
                      break;
                }
            }
        }

        // Gravity like effect of cube falling down
        sq.y -= sq_vel;
        sq.y += 5;
        if (sq_vel > 13) {
          sq_vel -= 0.5;
        }
        else if (sq_vel > 0) {
          sq_vel -= 1;
        }

        // keep square on-screen
        if (sq.x < 0) {
          sq.x = 0;
        }
        if (sq.y < 0) {
          sq.y = 0;
        } 
        if (sq.x > WINDOW_WIDTH  - SQUARE_SIZE)
            sq.x = WINDOW_WIDTH  - SQUARE_SIZE;
        if (sq.y > WINDOW_HEIGHT - SQUARE_SIZE)
            sq.y = WINDOW_HEIGHT - SQUARE_SIZE;


        tube1a.x -= 1.3;
        tube1b.x -= 1.3;
        tube2a.x -= 1.3;
        tube2b.x -= 1.3;
        tube3a.x -= 1.3;
        tube3b.x -= 1.3;


        // When the tubes go off the screen, respawns a new tube.
        if (tube1a.x <= 0) {
          respawn(&tube1a.x, &tube1a.y);
          tube1b.x = tube1a.x;
          tube1b.y = tube1a.y + 600;
        }
        if (tube2a.x <= 0) {
          respawn(&tube2a.x, &tube2a.y);
          tube2b.x = tube2a.x;
          tube2b.y = tube2a.y + 600;
        }
        if (tube3a.x <= 0) {
          respawn(&tube3a.x, &tube3a.y);
          tube3b.x = tube3a.x;
          tube3b.y = tube3a.y + 600;
        }

        
        // render
        SDL_SetRenderDrawColor(ren, 0, 144, 48, 255);  // green background
        SDL_RenderClear(ren);

        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255); // white square
        SDL_RenderFillRect(ren, &sq);
        
        SDL_SetRenderDrawColor(ren, 255, 255, 0, 255); // yellow tube
        SDL_RenderFillRect(ren, &tube1a);
        SDL_RenderFillRect(ren, &tube1b);
        SDL_RenderFillRect(ren, &tube2a);
        SDL_RenderFillRect(ren, &tube2b);
        SDL_RenderFillRect(ren, &tube3a);
        SDL_RenderFillRect(ren, &tube3b);



        SDL_RenderPresent(ren);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
