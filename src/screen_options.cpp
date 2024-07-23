globalVar struct {
    int framesCounter = 0;
    int finishScreen  = 0;
} optionsData;

// Options Screen Initialization logic
void InitOptionsScreen() {
    optionsData.framesCounter = 0;
    optionsData.finishScreen  = 0;
}

// Options Screen Update logic
void UpdateOptionsScreen() {}

// Options Screen Draw logic
void DrawOptionsScreen() {}

// Options Screen Unload logic
void UnloadOptionsScreen() {}

// Options Screen should finish?
int FinishOptionsScreen() {
    return optionsData.finishScreen;
}
