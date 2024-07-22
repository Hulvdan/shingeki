/**********************************************************************************************
 *
 *   raylib - Advance Game template
 *
 *   Logo Screen Functions Definitions (Init, Update, Draw, Unload)
 *
 *   Copyright (c) 2014-2022 Ramon Santamaria (@raysan5)
 *
 *   This software is provided "as-is", without any express or implied warranty. In no
 *event will the authors be held liable for any damages arising from the use of this
 *software.
 *
 *   Permission is granted to anyone to use this software for any purpose, including
 *commercial applications, and to alter it and redistribute it freely, subject to the
 *following restrictions:
 *
 *     1. The origin of this software must not be misrepresented; you must not claim that
 *you wrote the original software. If you use this software in a product, an
 *acknowledgment in the product documentation would be appreciated but is not required.
 *
 *     2. Altered source versions must be plainly marked as such, and must not be
 *misrepresented as being the original software.
 *
 *     3. This notice may not be removed or altered from any source distribution.
 *
 **********************************************************************************************/

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static struct {
    int framesCounter = 0;
    int finishScreen  = 0;

    int logoPositionX = 0;
    int logoPositionY = 0;

    int lettersCount = 0;

    int topSideRecWidth    = 0;
    int leftSideRecHeight  = 0;
    int bottomSideRecWidth = 0;
    int rightSideRecHeight = 0;

    int   state = 0;     // Logo animation states
    float alpha = 1.0f;  // Useful for fading
} logoData;

//----------------------------------------------------------------------------------
// Logo Screen Functions Definition
//----------------------------------------------------------------------------------

// Logo Screen Initialization logic
void InitLogoScreen() {
    logoData.finishScreen  = 0;
    logoData.framesCounter = 0;
    logoData.lettersCount  = 0;

    logoData.logoPositionX = GetScreenWidth() / 2 - 128;
    logoData.logoPositionY = GetScreenHeight() / 2 - 128;

    logoData.topSideRecWidth    = 16;
    logoData.leftSideRecHeight  = 16;
    logoData.bottomSideRecWidth = 16;
    logoData.rightSideRecHeight = 16;

    logoData.state = 0;
    logoData.alpha = 1.0f;
}

// Logo Screen Update logic
void UpdateLogoScreen() {
    if (logoData.state == 0)  // State 0: Top-left square corner blink logic
    {
        logoData.framesCounter++;

        if (logoData.framesCounter == 80) {
            logoData.state         = 1;
            logoData.framesCounter = 0;  // Reset counter... will be used later...
        }
    }
    else if (logoData.state == 1)  // State 1: Bars animation logic: top and left
    {
        logoData.topSideRecWidth += 8;
        logoData.leftSideRecHeight += 8;

        if (logoData.topSideRecWidth == 256)
            logoData.state = 2;
    }
    else if (logoData.state == 2)  // State 2: Bars animation logic: bottom and right
    {
        logoData.bottomSideRecWidth += 8;
        logoData.rightSideRecHeight += 8;

        if (logoData.bottomSideRecWidth == 256)
            logoData.state = 3;
    }
    else if (logoData.state == 3)  // State 3: "raylib" text-write animation logic
    {
        logoData.framesCounter++;

        if (logoData.lettersCount < 10) {
            if (logoData.framesCounter / 12)  // Every 12 frames, one more letter!
            {
                logoData.lettersCount++;
                logoData.framesCounter = 0;
            }
        }
        else  // When all letters have appeared, just fade out everything
        {
            if (logoData.framesCounter > 200) {
                logoData.alpha -= 0.02f;

                if (logoData.alpha <= 0.0f) {
                    logoData.alpha        = 0.0f;
                    logoData.finishScreen = 1;  // Jump to next screen
                }
            }
        }
    }
}

// Logo Screen Draw logic
void DrawLogoScreen() {
    if (logoData.state == 0)  // Draw blinking top-left square corner
    {
        if ((logoData.framesCounter / 10) % 2)
            DrawRectangle(logoData.logoPositionX, logoData.logoPositionY, 16, 16, BLACK);
    }
    else if (logoData.state == 1)  // Draw bars animation: top and left
    {
        DrawRectangle(
            logoData.logoPositionX,
            logoData.logoPositionY,
            logoData.topSideRecWidth,
            16,
            BLACK
        );
        DrawRectangle(
            logoData.logoPositionX,
            logoData.logoPositionY,
            16,
            logoData.leftSideRecHeight,
            BLACK
        );
    }
    else if (logoData.state == 2)  // Draw bars animation: bottom and right
    {
        DrawRectangle(
            logoData.logoPositionX,
            logoData.logoPositionY,
            logoData.topSideRecWidth,
            16,
            BLACK
        );
        DrawRectangle(
            logoData.logoPositionX,
            logoData.logoPositionY,
            16,
            logoData.leftSideRecHeight,
            BLACK
        );

        DrawRectangle(
            logoData.logoPositionX + 240,
            logoData.logoPositionY,
            16,
            logoData.rightSideRecHeight,
            BLACK
        );
        DrawRectangle(
            logoData.logoPositionX,
            logoData.logoPositionY + 240,
            logoData.bottomSideRecWidth,
            16,
            BLACK
        );
    }
    else if (logoData.state == 3)  // Draw "raylib" text-write animation + "powered by"
    {
        DrawRectangle(
            logoData.logoPositionX,
            logoData.logoPositionY,
            logoData.topSideRecWidth,
            16,
            Fade(BLACK, logoData.alpha)
        );
        DrawRectangle(
            logoData.logoPositionX,
            logoData.logoPositionY + 16,
            16,
            logoData.leftSideRecHeight - 32,
            Fade(BLACK, logoData.alpha)
        );

        DrawRectangle(
            logoData.logoPositionX + 240,
            logoData.logoPositionY + 16,
            16,
            logoData.rightSideRecHeight - 32,
            Fade(BLACK, logoData.alpha)
        );
        DrawRectangle(
            logoData.logoPositionX,
            logoData.logoPositionY + 240,
            logoData.bottomSideRecWidth,
            16,
            Fade(BLACK, logoData.alpha)
        );

        DrawRectangle(
            GetScreenWidth() / 2 - 112,
            GetScreenHeight() / 2 - 112,
            224,
            224,
            Fade(RAYWHITE, logoData.alpha)
        );

        DrawText(
            TextSubtext("raylib", 0, logoData.lettersCount),
            GetScreenWidth() / 2 - 44,
            GetScreenHeight() / 2 + 48,
            50,
            Fade(BLACK, logoData.alpha)
        );

        if (logoData.framesCounter > 20)
            DrawText(
                "powered by",
                logoData.logoPositionX,
                logoData.logoPositionY - 27,
                20,
                Fade(DARKGRAY, logoData.alpha)
            );
    }
}

// Logo Screen Unload logic
void UnloadLogoScreen() {
    // Unload LOGO screen variables here!
}

// Logo Screen should finish?
int FinishLogoScreen() {
    return logoData.finishScreen;
}
