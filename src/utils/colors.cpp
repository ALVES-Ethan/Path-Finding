#include "colors.h"

#include <algorithm>

int rgbToAnsi(int r, int g, int b) {
    r = std::max(0, std::min(255, r));
    g = std::max(0, std::min(255, g));
    b = std::max(0, std::min(255, b));

    // Convert RGB to ANSI 256 color mode
    // pour correctement formuler penser a rajoute un nouveau define j'appele oss = #define oss "\033[38;5;" puis mettre la fonction et finir avec m tres important
    // Formula: 16 + 36 * R + 6 * G + B
    int ansiCode = 16 + (36 * (r / 51)) + (6 * (g / 51)) + (b / 51);
    return ansiCode;
}