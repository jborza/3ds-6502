#include <citro2d.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "color.h"
#include "emu_3ds.h"
#include "disassembler.h"

#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 240
#define SCREEN_OFFSET_LEFT (SCREEN_WIDTH - SCREEN_HEIGHT) / 2
//for 32x32 we need 7x7 large pixels
#define PIXEL_SIZE 7

void C2D_DrawSolidSquare(float x, float y, float size, u32 color)
{
    const float z = 0;
    C2D_DrawRectangle(x, y, z, size, size, color, color, color, color);
}

void render_memory()
{
    for (int y = 0; y < 32; y++)
    {
        for (int x = 0; x < 32; x++)
        {
            C2D_DrawSolidSquare(x * 7 + SCREEN_OFFSET_LEFT, y * 7, PIXEL_SIZE, getColor(state.memory[0x200 + x + y * DISP_WIDTH]));
        }
    }
}

void check_input(u32 kDown)
{
    //hackity hack, we simulate WASD with D-pad
    if (kDown & KEY_LEFT)
        last_key = 'A';
    else if (kDown & KEY_DOWN)
        last_key = 'S';
    else if (kDown & KEY_RIGHT)
        last_key = 'D';
    else if (kDown & KEY_UP)
        last_key = 'W';
    else
        last_key = 0;
}

int frame = 0;

int main(int argc, char *argv[])
{
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    // Create screens
    C3D_RenderTarget *top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    consoleInit(GFX_BOTTOM, NULL);

    //create colors
    initializePalette();
    // Create colors
    u32 clrClear = palette[0];

    //initialize emulator
    initialize_state();
    load_bin();
    reset_pc();
    emu_tick();

    printf("\x1b[27;1HPress B to exit");
    printf("\x1b[20;1HUse arrow keys as input");
    printf("\x1b[21;1HSTART to pause emulation");
    printf("\x1b[22;1HA to step in paused mode");
    printf("\x1b[23;1HL to show/hide diagnostics");
    printf("\x1b[24;1X to show/hide memory (ZP)");
    printf("\x1b[25;1HR to reset ROM");

    printf("\x1b[1;25H\x1b[31m%s\x1b[0m", paused ? "PAUSE" : "     ");

    // Main loop
    while (aptMainLoop())
    {
        // gspWaitForVBlank();
        // gfxSwapBuffers();
        hidScanInput();

        u32 kDown = hidKeysDown();
        if (kDown & KEY_B)
            break; // break in order to return to hbmenu

        if (paused && (kDown & KEY_A)){
            emu_tick();
            print_state();
        }
        if (kDown & KEY_R)
            reset_emulation();
        if (kDown & KEY_L)
            show_diagnostics = !show_diagnostics;
        if (kDown & KEY_START)
        {
            paused = !paused;
            printf("\x1b[1;25H\x1b[31m%s\x1b[0m", paused ? "PAUSE" : "     ");
        }
        if (kDown & KEY_X)
            show_memory = !show_memory;

        //diagnostics
        printf("\x1b[29;1HCPU:     %6.2f%%\x1b[K GPU:     %6.2f%%", C3D_GetProcessingTime() * 6.0f, C3D_GetDrawingTime() * 6.0f);
        printf("\x1b[28;1Hframe:     %d  frameskip: %d", frame++, frameskip);

        // Render the scene
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(top, clrClear);
        C2D_SceneBegin(top);

        check_input(kDown);
        if (!paused)
        {
            for(int i =0; i < frameskip; i++)
                emu_tick();
            print_state();
        }
        render_memory();
        C3D_FrameEnd(0);
    }

    C2D_Fini();
    C3D_Fini();
    gfxExit();
    return 0;
}
