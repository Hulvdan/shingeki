enum class GameScreen {
    UNKNOWN = -2,
    NOT_SET = -1,
    LOGO    = 0,
    TITLE,
    OPTIONS,
    GAMEPLAY,
    ENDING,
};

globalVar GameScreen currentScreen = GameScreen::UNKNOWN;
globalVar Font       font{};
globalVar Music      music{};
globalVar Sound      fxCoin{};
