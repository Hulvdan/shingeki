#pragma once

#include "assert.h"

#include "raylib.h"
#include "raymath.h"

#include "raylib_vector2.cpp"
#include "raylib_vector3.cpp"

#include "base.cpp"
#include "math.cpp"

#include "debug_text.cpp"

#include "screens.cpp"
#include "screen_gameplay.cpp"
#include "screen_ending.cpp"
#include "screen_logo.cpp"
#include "screen_options.cpp"
#include "screen_title.cpp"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

globalVar float      transAlpha      = 0.0f;
globalVar bool       onTransition    = false;
globalVar bool       transFadeOut    = false;
globalVar GameScreen transFromScreen = GameScreen::NOT_SET;
globalVar GameScreen transToScreen   = GameScreen::NOT_SET;

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------

// Change to screen, no transition effect
// void ChangeToScreen(GameScreen screen);

// Request transition to next screen
void TransitionToScreen(GameScreen screen);

// Update transition effect
void UpdateTransition();

// Draw transition effect (full-screen rectangle)
void DrawTransition();

// Update and draw one frame
void UpdateDrawFrame();

int main() {
    // Initialization
    //---------------------------------------------------------

    // ref: https://www.reddit.com/r/raylib/comments/a19a67/resizable_window_questions/
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(800, 450, "raylib game template");
    MaximizeWindow();

    InitAudioDevice();  // Initialize audio device

    // Load global data (assets that must be available in all screens, i.e. font)
    font   = LoadFont("resources/mecha.png");
    music  = LoadMusicStream("resources/ambient.ogg");
    fxCoin = LoadSound("resources/coin.wav");

    SetMusicVolume(music, 1.0f);
    PlayMusicStream(music);

    // Setup and init first screen
    // currentScreen = GameScreen::TITLE;
    // currentScreen = GameScreen::LOGO;
    currentScreen = GameScreen::GAMEPLAY;
    // InitLogoScreen();
    // InitTitleScreen();
    InitGameplayScreen();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);  // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())  // Detect window close button or ESC key
    {
        UpdateDrawFrame();
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    // Unload current screen data before closing
    switch (currentScreen) {
    case GameScreen::LOGO:
        UnloadLogoScreen();
        break;
    case GameScreen::TITLE:
        UnloadTitleScreen();
        break;
    case GameScreen::GAMEPLAY:
        UnloadGameplayScreen();
        break;
    case GameScreen::ENDING:
        UnloadEndingScreen();
        break;
    default:
        break;
    }

    // Unload global data loaded
    UnloadFont(font);
    UnloadMusicStream(music);
    UnloadSound(fxCoin);

    CloseAudioDevice();  // Close audio context

    CloseWindow();  // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------
// Change to next screen, no transition
// void ChangeToScreen(GameScreen screen) {
//     // Unload current screen
//     switch (currentScreen) {
//     case GameScreen::LOGO:
//         UnloadLogoScreen();
//         break;
//     case GameScreen::TITLE:
//         UnloadTitleScreen();
//         break;
//     case GameScreen::GAMEPLAY:
//         UnloadGameplayScreen();
//         break;
//     case GameScreen::ENDING:
//         UnloadEndingScreen();
//         break;
//     default:
//         break;
//     }
//
//     // Init next screen
//     switch (screen) {
//     case GameScreen::LOGO:
//         InitLogoScreen();
//         break;
//     case GameScreen::TITLE:
//         InitTitleScreen();
//         break;
//     case GameScreen::GAMEPLAY:
//         InitGameplayScreen();
//         break;
//     case GameScreen::ENDING:
//         InitEndingScreen();
//         break;
//     default:
//         break;
//     }
//
//     currentScreen = screen;
// }

// Request transition to next screen
void TransitionToScreen(GameScreen screen) {
    onTransition    = true;
    transFadeOut    = false;
    transFromScreen = currentScreen;
    transToScreen   = screen;
    transAlpha      = 0.0f;
    PlaySound(fxCoin);
}

// Update transition effect (fade-in, fade-out)
void UpdateTransition() {
    if (!transFadeOut) {
        transAlpha += 0.05f;

        // NOTE: Due to float internal representation, condition jumps on 1.0f instead
        // of 1.05f For that reason we compare against 1.01f, to avoid last frame loading
        // stop
        if (transAlpha > 1.01f) {
            transAlpha = 1.0f;

            // Unload current screen
            switch (transFromScreen) {
            case GameScreen::LOGO:
                UnloadLogoScreen();
                break;
            case GameScreen::TITLE:
                UnloadTitleScreen();
                break;
            case GameScreen::OPTIONS:
                UnloadOptionsScreen();
                break;
            case GameScreen::GAMEPLAY:
                UnloadGameplayScreen();
                break;
            case GameScreen::ENDING:
                UnloadEndingScreen();
                break;
            default:
                break;
            }

            // Load next screen
            switch (transToScreen) {
            case GameScreen::LOGO:
                InitLogoScreen();
                break;
            case GameScreen::TITLE:
                InitTitleScreen();
                break;
            case GameScreen::GAMEPLAY:
                InitGameplayScreen();
                break;
            case GameScreen::ENDING:
                InitEndingScreen();
                break;
            default:
                break;
            }

            currentScreen = transToScreen;

            // Activate fade out effect to next loaded screen
            transFadeOut = true;
        }
    }
    else  // Transition fade out logic
    {
        transAlpha -= 0.02f;

        if (transAlpha < -0.01f) {
            transAlpha      = 0.0f;
            transFadeOut    = false;
            onTransition    = false;
            transFromScreen = GameScreen::NOT_SET;
            transToScreen   = GameScreen::UNKNOWN;
        }
    }
}

// Draw transition effect (full-screen rectangle)
void DrawTransition() {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, transAlpha));
}

// Update and draw game frame
void UpdateDrawFrame() {
    // Update
    //----------------------------------------------------------------------------------
    UpdateMusicStream(music);  // NOTE: Music keeps playing between screens

    if (IsKeyPressed(KEY_F11))
        ToggleBorderlessWindowed();

    if (!onTransition) {
        switch (currentScreen) {
        case GameScreen::LOGO: {
            UpdateLogoScreen();

            if (FinishLogoScreen())
                TransitionToScreen(GameScreen::TITLE);

        } break;
        case GameScreen::TITLE: {
            UpdateTitleScreen();

            if (FinishTitleScreen() == 1)
                TransitionToScreen(GameScreen::OPTIONS);
            else if (FinishTitleScreen() == 2)
                TransitionToScreen(GameScreen::GAMEPLAY);

        } break;
        case GameScreen::OPTIONS: {
            UpdateOptionsScreen();

            if (FinishOptionsScreen())
                TransitionToScreen(GameScreen::TITLE);

        } break;
        case GameScreen::GAMEPLAY: {
            UpdateGameplayScreen();

            if (FinishGameplayScreen() == 1)
                TransitionToScreen(GameScreen::ENDING);
            // else if (FinishGameplayScreen() == 2) TransitionToScreen(TITLE);

        } break;
        case GameScreen::ENDING: {
            UpdateEndingScreen();

            if (FinishEndingScreen() == 1)
                TransitionToScreen(GameScreen::TITLE);

        } break;
        default:
            break;
        }
    }
    else
        UpdateTransition();  // Update transition (fade-in, fade-out)
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(RAYWHITE);

    switch (currentScreen) {
    case GameScreen::LOGO:
        DrawLogoScreen();
        break;
    case GameScreen::TITLE:
        DrawTitleScreen();
        break;
    case GameScreen::OPTIONS:
        DrawOptionsScreen();
        break;
    case GameScreen::GAMEPLAY:
        DrawGameplayScreen();
        break;
    case GameScreen::ENDING:
        DrawEndingScreen();
        break;
    default:
        break;
    }

    // Draw full screen rectangle in front of everything
    if (onTransition)
        DrawTransition();

    // DrawFPS(10, 10);

    EndDrawing();
    //----------------------------------------------------------------------------------
}
