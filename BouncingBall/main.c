#include <stdio.h>
#include <SDL3/SDL.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT  600
#define GRAVITY 1
#define DAMPING 0.95f
#define FORCE 50


// SDL resources
typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
} SDLResources;

// Cleanup flags
typedef enum {
    CLEANUP_WINDOW = 0x01,
    CLEANUP_RENDERER = 0x02,
    CLEANUP_SDL = 0x04
} CleanupFlags;

// Ball struct
typedef struct {
    int x;
    int y;
    int radius;
    int vx;
    int vy;
} Ball;

void cleanup(SDLResources *resources, CleanupFlags flags);

void draw_filled_circle(SDL_Renderer *renderer, int x_center, int y_center, int radius) {
    int x = 0;
    int y = radius;
    int p = 1 - radius; // Initialize decision param

    while (x <= y) {
        SDL_RenderLine(renderer, x_center - x, y_center + y, x_center + x, y_center + y);
        SDL_RenderLine(renderer, x_center - x, y_center - y, x_center + x, y_center - y);
        SDL_RenderLine(renderer, x_center - y, y_center + x, x_center + y, y_center + x);
        SDL_RenderLine(renderer, x_center - y, y_center - x, x_center + y, y_center - x);

        if (p < 0) {
            p += 2*x + 1;
        }
        else {
            p += 2 * (x-y) + 1;
            y--;
        }
        x++;
    }
}

int main(int argc, char *argv[])
{
    SDLResources resources = {0}; // Initialize all to NULL

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        SDL_Log("SDL_Init error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create window
    resources.window = SDL_CreateWindow("Bouncing Ball", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (resources.window == NULL) {
        SDL_Log("SDL_CreateWindow: %s", SDL_GetError());
        cleanup(&resources, CLEANUP_SDL);
        return 1;
    }

    // Create renderer
    resources.renderer = SDL_CreateRenderer(resources.window, NULL);
    if (resources.renderer == NULL) {
        SDL_Log("SDL_CreateRenderer: %s", SDL_GetError());
        cleanup(&resources, CLEANUP_WINDOW | CLEANUP_SDL);
        return 1;
    }

    // Ball set up
    Ball ball = {
        WINDOW_WIDTH/2,
        WINDOW_HEIGHT-20,
        50,
        0,
        0    
    };

    SDL_Log("SDL3 Initialized");

    SDL_Event event;
    int quit = 0;

    // Main loop
    while (!quit) {
        while(SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_KEY_DOWN:
                    SDL_Log("Key: %s, Scancode: %d, Modifiers: %d",
                        SDL_GetKeyName(event.key.key),
                        event.key.scancode,
                        event.key.mod);
                    switch (event.key.key) {
                        case SDLK_SPACE:
                            ball.vy = -FORCE;
                            break;
                    }
                    break;
                case SDL_EVENT_QUIT:
                    SDL_Log("SDL3 event quit");
                    quit = 1;
                    break;
            }
        }

        // Update ball position
        ball.vy += GRAVITY;
        ball.y += ball.vy;

        // Bounce off the bottom
        if (ball.y + ball.radius >= WINDOW_HEIGHT) {
            ball.y = WINDOW_HEIGHT - ball.radius; // Snap to bottom
            ball.vy = -ball.vy * DAMPING; // Reverse and dampen
        }
        // Bounce off the top
        else if (ball.y - ball.radius <= 0 ) {
            ball.y = ball.radius;
            ball.vy = -ball.vy * DAMPING;
        }

        // Stop small movements
        if (ball.vy > -1 && ball.vy < 1) { 
            ball.vy = 0;
        }

        // Draw Black Background and clear
        SDL_SetRenderDrawColor(resources.renderer, 0, 0, 0, 255);
        SDL_RenderClear(resources.renderer);

        // Set Ball Color
        SDL_SetRenderDrawColor(resources.renderer, 109, 109, 109, 255);
        draw_filled_circle(resources.renderer, ball.x, ball.y, ball.radius); 
        
        // Present the Renderer
        SDL_RenderPresent(resources.renderer);

        SDL_Delay(10);
    }

    // Cleanup
    SDL_Log("SDL3 shutdown");
    cleanup(&resources, CLEANUP_RENDERER | CLEANUP_WINDOW | CLEANUP_SDL);
    return 0;
}

void cleanup(SDLResources *resources, CleanupFlags flags) {
    if ((flags & CLEANUP_RENDERER) && resources->renderer) {
        SDL_Log("Destroying Renderer...");
        SDL_DestroyRenderer(resources->renderer);
        resources->renderer = NULL;
    }
    if ((flags & CLEANUP_WINDOW) && resources->window) {
        SDL_Log("Destroying Window...");
        SDL_DestroyWindow(resources->window);
        resources->window = NULL;
    }
    if ((flags & CLEANUP_SDL)) {
        SDL_Log("Quitting SDL...");
        SDL_Quit();
    }
}