globalVar struct {
    int framesCounter = 0;
    int finishScreen  = 0;
} titleData;

void InitTitleScreen() {
    titleData.framesCounter = 0;
    titleData.finishScreen  = 0;
}

void UpdateTitleScreen() {
    // Press enter or tap to change to GAMEPLAY screen
    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
        // titleData.finishScreen = 1;   // OPTIONS
        titleData.finishScreen = 2;  // GAMEPLAY
    }
}

void DrawTitleScreen() {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), GREEN);
    Vector2 pos = {20, 10};
    DrawTextEx(font, "TITLE SCREEN", pos, (float)font.baseSize * 3.0f, 4, DARKGREEN);
    DrawText("PRESS ENTER or TAP to JUMP to GAMEPLAY SCREEN", 120, 220, 20, DARKGREEN);
}

void UnloadTitleScreen() {}

// Title Screen should finish?
int FinishTitleScreen() {
    return titleData.finishScreen;
}
