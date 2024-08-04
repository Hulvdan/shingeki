globalVar struct {
    int framesCounter = 0;
    int finishScreen  = 0;
} endingData;

void InitEndingScreen() {
    endingData.framesCounter = 0;
    endingData.finishScreen  = 0;
}

void UpdateEndingScreen() {
    // Press enter or tap to return to TITLE screen
    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
        endingData.finishScreen = 1;
}

void DrawEndingScreen() {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLUE);

    Vector2 pos = {20, 10};
    DrawTextEx(font, "ENDING SCREEN", pos, (float)font.baseSize * 3.0f, 4, DARKBLUE);
    DrawText("PRESS ENTER or TAP to RETURN to TITLE SCREEN", 120, 220, 20, DARKBLUE);
}

void UnloadEndingScreen() {}

// Ending Screen should finish?
int FinishEndingScreen() {
    return endingData.finishScreen;
}
