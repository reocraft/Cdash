#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

const int WINDOW_WIDTH  = 800;
const int WINDOW_HEIGHT = 600;
const int SQUARE_SIZE   = 50;
const int SPEED         = 5;

int rng (int min, int max) {
  int random = rand();
  return random % (max - min + 1) + min;
}

void respawn (int* xpos, int* ypos) {
  *xpos = WINDOW_WIDTH;
  *ypos = rng(0, 4) * -100;
}

void collision (int cube_x, int cube_y, int tube_x, int tube_y, bool* collided) {
  if (cube_x - tube_x < SQUARE_SIZE && cube_x - tube_x > -SQUARE_SIZE) {
    if (cube_y - tube_y > -SQUARE_SIZE && cube_y - tube_y < 400) {
      *collided = true;
    }
  }
}

int main(int argc, char *argv[]) {

  bool collided = false;

  srand(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }
    if (TTF_Init() != 0) {
      fprintf(stderr, "TTF_Init Error: %s\n", TTF_GetError());
      SDL_Quit();
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
    TTF_Font *font = TTF_OpenFont("/Library/Fonts/Arial Unicode.ttf", 24);
    if (!font) {
      fprintf(stderr, "TTF_OpenFont Error: %s\n", TTF_GetError());
      /* cleanup and exit… */
    }

    // sq.x and sq.y keep track of the positions of the square
    SDL_Rect sq = {
        (WINDOW_WIDTH  - SQUARE_SIZE) / 4,
        (WINDOW_HEIGHT - SQUARE_SIZE) / 2,
        SQUARE_SIZE, SQUARE_SIZE
    };

    // Obstacle tubes
    SDL_Rect tube1a = {
      (WINDOW_WIDTH - SQUARE_SIZE) + 267,
      -300,
      SQUARE_SIZE, 400
    };
    SDL_Rect tube1b = {
      (WINDOW_WIDTH - SQUARE_SIZE) + 267,
      tube1a.y + 600,
      SQUARE_SIZE, 400
    };
    SDL_Rect tube2a = {
      (WINDOW_WIDTH - SQUARE_SIZE),
      -150,
      SQUARE_SIZE, 400
    };
    SDL_Rect tube2b = {
      (WINDOW_WIDTH - SQUARE_SIZE),
      tube2a.y + 600,
      SQUARE_SIZE, 400
    };
    SDL_Rect tube3a = {
      (WINDOW_WIDTH - SQUARE_SIZE) + 534,
      0,
      SQUARE_SIZE, 400
    };
    SDL_Rect tube3b = {
      (WINDOW_WIDTH - SQUARE_SIZE) + 534,
      tube3a.y + 600,
      SQUARE_SIZE, 400
    };

    SDL_Surface *surf = TTF_RenderText_Solid(font, "Press space to start the game!", (SDL_Color){255,255,255,255});
    SDL_Texture *text_tex = SDL_CreateTextureFromSurface(ren, surf);
    SDL_Rect text_rect = { 10, 10, surf->w, surf->h };
    SDL_FreeSurface(surf);

    // Value to store velocity added when pressing `space`
    float sq_vel = 0;

    int frame = 0;
    int running = 1;
    bool start = false;
    bool finish = false;
    SDL_Event e;
    while (running) {
        // handle events
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_SPACE:
                      if (sq_vel <= 16.0) {
                        sq_vel += 16.0;
                      };
                      start = true;
                      break;
                    default: 
                      break;
                }
            }
        }

        if (!start && !finish) {
          SDL_SetRenderDrawColor(ren, 0, 144, 48, 255);  // green background
          SDL_RenderClear(ren);
          SDL_RenderCopy(ren, text_tex, NULL, &text_rect); // Render the text
          SDL_RenderPresent(ren);
        }

        if (start && !finish) {
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

          // Checks for collisions.
          collision(sq.x, sq.y, tube1a.x, tube1a.y, &collided);
          collision(sq.x, sq.y, tube1b.x, tube1b.y, &collided);
          collision(sq.x, sq.y, tube2a.x, tube2a.y, &collided);
          collision(sq.x, sq.y, tube2b.x, tube2b.y, &collided);
          collision(sq.x, sq.y, tube3a.x, tube3a.y, &collided);
          collision(sq.x, sq.y, tube3b.x, tube3b.y, &collided);

          if (collided) {
            break;
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
        
    }

    SDL_DestroyTexture(text_tex);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
