#include <citro2d.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "color.h"

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

void render()
{
    for (int y = 0; y < 32; y++)
    {
        for (int x = 0; x < 32; x++)
        {
            C2D_DrawSolidSquare(x * 7 + SCREEN_OFFSET_LEFT, y * 7, PIXEL_SIZE, getColor(x+y));
            // C2D_DrawRectangle(x*7, x*7, PIXEL_SIZE, PIXEL_SIZE, clrRed, clrRed, clrRed, clrRed);
        }
    }
}

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

    printf("Hello, world!\n");

    // Main loop
    while (aptMainLoop())
    {
        // gspWaitForVBlank();
        // gfxSwapBuffers();
        hidScanInput();

        u32 kDown = hidKeysDown();
        if (kDown & KEY_START)
            break; // break in order to return to hbmenu

        //diagnostics
        printf("\x1b[2;1HCPU:     %6.2f%%\x1b[K", C3D_GetProcessingTime() * 6.0f);
        printf("\x1b[3;1HGPU:     %6.2f%%\x1b[K", C3D_GetDrawingTime() * 6.0f);
        printf("\x1b[4;1HCmdBuf:  %6.2f%%\x1b[K", C3D_GetCmdBufUsage() * 100.0f);

        // Render the scene
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(top, clrClear);
        C2D_SceneBegin(top);

        render();

        C3D_FrameEnd(0);
    }

    C2D_Fini();
    C3D_Fini();
    gfxExit();
    return 0;
}
