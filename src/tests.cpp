#pragma once

#ifdef TESTS
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

// TODO: Как-то прикрутить это к raylib_game.cpp
// #include "raylib_game.cpp"

#include "doctest.h"

TEST_CASE ("aboba") {
    CHECK(1 == 1);
}
