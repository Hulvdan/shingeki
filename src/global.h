/**********************************************************************************************
 *
 *   raylib - Advance Game template
 *
 *   Screens Functions Declarations (Init, Update, Draw, Unload)
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

#ifndef SCREENS_H
#define SCREENS_H

#include "raylib_vector2.h"
#include "raylib_vector3.h"

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
enum class GameScreen {
    UNKNOWN = -2,
    NOT_SET = -1,
    LOGO    = 0,
    TITLE,
    OPTIONS,
    GAMEPLAY,
    ENDING,
};

//----------------------------------------------------------------------------------
// Global Variables Declaration (shared by several modules)
//----------------------------------------------------------------------------------
extern GameScreen currentScreen;
extern Font       font;
extern Music      music;
extern Sound      fxCoin;

#ifdef __cplusplus
extern "C" {  // Prevents name mangling of functions
#endif

//----------------------------------------------------------------------------------
// Logo Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitLogoScreen();
void UpdateLogoScreen();
void DrawLogoScreen();
void UnloadLogoScreen();
int  FinishLogoScreen();

//----------------------------------------------------------------------------------
// Title Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitTitleScreen();
void UpdateTitleScreen();
void DrawTitleScreen();
void UnloadTitleScreen();
int  FinishTitleScreen();

//----------------------------------------------------------------------------------
// Options Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitOptionsScreen();
void UpdateOptionsScreen();
void DrawOptionsScreen();
void UnloadOptionsScreen();
int  FinishOptionsScreen();

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitGameplayScreen();
void UpdateGameplayScreen();
void DrawGameplayScreen();
void UnloadGameplayScreen();
int  FinishGameplayScreen();

//----------------------------------------------------------------------------------
// Ending Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitEndingScreen();
void UpdateEndingScreen();
void DrawEndingScreen();
void UnloadEndingScreen();
int  FinishEndingScreen();

#ifdef __cplusplus
}
#endif

#endif  // SCREENS_H
