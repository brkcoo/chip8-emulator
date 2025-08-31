#define SDL_MAIN_HANDLED
#include "SDL/x86_64-w64-mingw32/include/SDL2/SDL.h"
#include "SDL/x86_64-w64-mingw32/include/SDL2/SDL_opengl.h"
#include "chip8.h"

using namespace std;

// input key map
uint8_t keymap[16] = {
    SDLK_x,
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_q,
    SDLK_w,
    SDLK_e,
    SDLK_a,
    SDLK_s,
    SDLK_d,
    SDLK_z,
    SDLK_c,
    SDLK_4,
    SDLK_r,
    SDLK_f,
    SDLK_v,
};

int main(int argc, char **argv)
{
    // Command usage
    if (argc != 2)
    {
        cout << "Usage: chip8 <ROM file>" << endl;
        return 1;
    }

    Chip8 chip8 = Chip8(); // Initialise Chip8

    int w = 1024; // Window width
    int h = 512;  // Window height

    // The window we'll be rendering to
    SDL_Window *window = NULL;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }
    // Create window
    window = SDL_CreateWindow(
        "CHIP-8 Emulator",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        w, h, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        printf("Window could not be created! SDL_Error: %s\n",
               SDL_GetError());
        exit(2);
    }

    // Create renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetLogicalSize(renderer, w, h);

    // Create texture that stores frame buffer
    SDL_Texture *sdlTexture = SDL_CreateTexture(renderer,
                                                SDL_PIXELFORMAT_ARGB8888,
                                                SDL_TEXTUREACCESS_STREAMING,
                                                64, 32);

    // Temporary pixel buffer
    uint32_t pixels[2048];

load:
    // Attempt to load ROM
    chip8.ResetCPU(argv[1]);

    // Emulation loop
    while (true)
    {
        chip8.Cycle();

        // Process SDL events
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                exit(0);

            // Process keydown events
            if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_ESCAPE)
                    exit(0);

                if (e.key.keysym.sym == SDLK_F1)
                    goto load; // *gasp*, a goto statement!
                               // Used to reset/reload ROM

                for (int i = 0; i < 16; ++i)
                {
                    if (e.key.keysym.sym == keymap[i])
                    {
                        chip8.inputKeys[i] = 1;
                    }
                }
            }
            // Process keyup events
            if (e.type == SDL_KEYUP)
            {
                for (int i = 0; i < 16; ++i)
                {
                    if (e.key.keysym.sym == keymap[i])
                    {
                        chip8.inputKeys[i] = 0;
                    }
                }
            }
        }

        // If draw occurred, redraw SDL screen
        // Store pixels in temporary buffer
        for (int j = 0; j < 32; ++j)
        {
            for (int i = 0; i < 64; ++i)
            {
                uint32_t pixel = chip8.video[i][j];
                pixels[(j * 64) + i] = (0x00FFFFFF * pixel) | 0xFF000000;
            }
        } // Update SDL texture
        SDL_UpdateTexture(sdlTexture, NULL, pixels, 64 * sizeof(Uint32));
        // Clear screen and render
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, sdlTexture, NULL, NULL);
        SDL_RenderPresent(renderer);

        // Sleep to slow down emulation speed
        Sleep(2);
    }
}