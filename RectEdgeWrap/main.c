#include <stdio.h>
#include <SDL3/SDL.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT  600

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

void cleanup(SDLResources *resources, CleanupFlags flags);

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
    resources.window = SDL_CreateWindow("Title", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
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

    SDL_Log("SDL3 Initialized");

    SDL_Event event;
    int quit = 0;

    // Rectangle set up
    float rect_width = 200.0f;
    float rect_height = 200.0f;
    float rect_x = (WINDOW_WIDTH - rect_width) / 2.0f;
    float rect_y = (WINDOW_HEIGHT - rect_height) / 2.0f;
    float rect_speed = 10.0f;

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
                        case SDLK_UP:
                            rect_y -= rect_speed;
                            break;
                        case SDLK_DOWN:
                            rect_y += rect_speed;
                            break;
                        case SDLK_LEFT:
                            rect_x -= rect_speed;
                            break;
                        case SDLK_RIGHT:
                            rect_x += rect_speed;
                            break;
                    }
                    break;
                
                case SDL_EVENT_QUIT:
                    SDL_Log("SDL3 event quit");
                    quit = 1;
                    break;
            }
        }

        // Draw Black Background and clear
        SDL_SetRenderDrawColor(resources.renderer, 0, 0, 0, 255);
        SDL_RenderClear(resources.renderer);
        // Set Rect Color
        SDL_SetRenderDrawColor(resources.renderer, 109, 109, 109, 255);
        
        // Check if Rectangle goes out of bounds
        // Left Edge
        if (rect_x < 0) {
            if (rect_x + rect_width <= 0) {
                rect_x += WINDOW_WIDTH;
            }
            else {
                SDL_FRect rect_left = {rect_x, rect_y, rect_width, rect_height};
                SDL_FRect rect_right_wrap = {WINDOW_WIDTH + rect_x, rect_y, rect_width, rect_height};
                SDL_RenderFillRect(resources.renderer, &rect_left);
                SDL_RenderFillRect(resources.renderer, &rect_right_wrap);
            }
        }
        // Right Edge
        else if (rect_x + rect_width > WINDOW_WIDTH) {
            if (rect_x >= WINDOW_WIDTH) {
                rect_x -= WINDOW_WIDTH;
            }
            else {
                SDL_FRect rect_right = {rect_x, rect_y, rect_width, rect_height};
                SDL_FRect rect_left_wrap = {rect_x - WINDOW_WIDTH, rect_y, rect_width, rect_height};
                SDL_RenderFillRect(resources.renderer, &rect_right);
                SDL_RenderFillRect(resources.renderer, &rect_left_wrap);
            }
        }
        // Top
        else if (rect_y <= 0) {
            if (rect_y + rect_height <= 0) {
                rect_y += WINDOW_HEIGHT;
            }
            else {
                SDL_FRect rect_top = {rect_x, rect_y, rect_width, rect_height};
                SDL_FRect rect_bot_wrap = {rect_x, WINDOW_HEIGHT + rect_y, rect_width, rect_height};
                SDL_RenderFillRect(resources.renderer, &rect_top);
                SDL_RenderFillRect(resources.renderer, &rect_bot_wrap);
            }
        }
        // Bottom
        else if (rect_y + rect_height > WINDOW_HEIGHT) {
            if (rect_y >= WINDOW_HEIGHT) {
                rect_y -= WINDOW_HEIGHT;
            }
            else {
                SDL_FRect rect_bot = {rect_x, rect_y, rect_width, rect_height};
                SDL_FRect rect_top_wrap = {rect_x, rect_y - WINDOW_HEIGHT, rect_width, rect_height};
                SDL_RenderFillRect(resources.renderer, &rect_bot);
                SDL_RenderFillRect(resources.renderer, &rect_top_wrap);
            }
        }
        // Render normally
        else {
            SDL_FRect rect = {rect_x,rect_y,rect_width,rect_height};
            SDL_RenderFillRect(resources.renderer, &rect);
        }
        
        // Present the Renderer
        SDL_RenderPresent(resources.renderer);

        SDL_Delay(1);
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