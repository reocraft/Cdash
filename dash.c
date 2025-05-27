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

void countScore (int cube_x, int tube_x, int* score) {
  if (cube_x - tube_x <= 1 && cube_x - tube_x >= 0) {
    *score += 1;
  }
}


// Functions that resets tube positions back to start of game.
void tube_init (int* tube1ax, int* tube1ay, int* tube1bx, int* tube1by, int* tube2ax, int* tube2ay, int* tube2bx, int* tube2by, int* tube3ax, int* tube3ay, int* tube3bx, int* tube3by) {
    // Obstacle tubes
    *tube1ax = *tube1bx = WINDOW_WIDTH - SQUARE_SIZE + 283;
    *tube1ay = -300;
    *tube1by = *tube1ay + 600;

    *tube2ax = *tube2bx = WINDOW_WIDTH - SQUARE_SIZE;
    *tube2ay = -150;
    *tube2by = *tube2ay + 600;

    *tube3ax = *tube3bx = WINDOW_WIDTH - SQUARE_SIZE + 567;
    *tube3ay = 0;
    *tube3by = *tube3ay + 600;
}

void cube_init (int* cubex, int* cubey) {
  *cubex = (WINDOW_WIDTH -  SQUARE_SIZE) / 4;
  *cubey = (WINDOW_HEIGHT - SQUARE_SIZE) / 2;
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


    // First text that prints in the beginning of the game.
    SDL_Surface *surf = TTF_RenderText_Solid(font, "Press space to start the game!", (SDL_Color){255,255,255,255});
    SDL_Texture *text_tex = SDL_CreateTextureFromSurface(ren, surf);
    SDL_Rect text_rect = {WINDOW_WIDTH*2/7, WINDOW_HEIGHT/2, surf->w, surf->h};
    SDL_FreeSurface(surf);
    // Second text that prints in the end of the game.
    SDL_Surface *surf2 = TTF_RenderText_Solid(font, "Game over! Press Q to quit, R to play again.", (SDL_Color){255,255,255,255});
    SDL_Texture *text_tex2 = SDL_CreateTextureFromSurface(ren, surf2);
    SDL_Rect text_rect2 = {WINDOW_WIDTH*2/9, WINDOW_HEIGHT/2, surf2->w, surf2->h };
    SDL_FreeSurface(surf2);

    // Value to store velocity added when pressing `space`
    float sq_vel = 0;

    int frame = 0;
    int score = 0;
    int highscore = 0;
    bool running = true;
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
                    case 'q':
                      if (finish) {
                        running = false;
                      }
                      break;
                    case 'r':
                      if (finish) {
                        cube_init(&sq.x, &sq.y);
                        tube_init(&tube1a.x, &tube1a.y, &tube1b.x, &tube1b.y, &tube2a.x, &tube2a.y, &tube2b.x, &tube2b.y, &tube3a.x, &tube3a.y, &tube3b.x, &tube3b.y);
                        start = false;
                        finish = false;
                        collided = false;
                        sq_vel = 0;
                        score = 0;
                      }
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
          if (sq_vel >= 13) {
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
          if (sq.x > WINDOW_WIDTH  - SQUARE_SIZE) {
            sq.x = WINDOW_WIDTH  - SQUARE_SIZE;
          }
          if (sq.y > WINDOW_HEIGHT - SQUARE_SIZE) {
            sq.y = WINDOW_HEIGHT - SQUARE_SIZE;
            sq_vel = 0;
          }



          tube1a.x -= 2;
          tube1b.x -= 2;
          tube2a.x -= 2;
          tube2b.x -= 2;
          tube3a.x -= 2;
          tube3b.x -= 2;


          // When the tubes go off the screen, respawns a new tube.
          if (tube1a.x <= -SQUARE_SIZE) {
            respawn(&tube1a.x, &tube1a.y);
            tube1b.x = tube1a.x;
            tube1b.y = tube1a.y + 600;
          }
          if (tube2a.x <= -SQUARE_SIZE) {
            respawn(&tube2a.x, &tube2a.y);
            tube2b.x = tube2a.x;
            tube2b.y = tube2a.y + 600;
          }
          if (tube3a.x <= -SQUARE_SIZE) {
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
            finish = true;
            continue; // Makes sure to end loop here so that an extra point wouldn't be counted.
          }

          // Count the scores when they go through the tubes
          countScore(sq.x, tube1a.x, &score);
          countScore(sq.x, tube2a.x, &score);
          countScore(sq.x, tube3a.x, &score);

          // Update highscore
          if (score > highscore) {
            highscore = score;
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

        if (start && finish) {
          SDL_SetRenderDrawColor(ren, 0, 144, 48, 255);  // green background
          SDL_RenderClear(ren);

          SDL_RenderCopy(ren, text_tex2, NULL, &text_rect2); // Render the text
          SDL_RenderPresent(ren);

          char buf[64];
          snprintf(buf, sizeof(buf), "Current score: %d  High score: %d", score, highscore);
          SDL_Surface *surf3 = TTF_RenderText_Solid(font, buf, (SDL_Color){255,255,255,255});
          SDL_Texture *tex3 = SDL_CreateTextureFromSurface(ren, surf3);
          SDL_Rect dst3 = {10, 40, surf3->w, surf3->h};
          SDL_FreeSurface(surf3);
          SDL_RenderCopy(ren, tex3, NULL, &dst3);
          SDL_DestroyTexture(tex3);
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
