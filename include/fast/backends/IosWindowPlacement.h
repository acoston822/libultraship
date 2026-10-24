#pragma once

#include <SDL.h>

namespace Fast {
// SDL chooses a display from the requested window's center. Saved renderer
// dimensions are physical pixels, while UIKit display bounds are points. A
// Retina-sized rectangle can land on an already-connected AirPlay display.
inline void ConfigureIosMainWindow(int& x, int& y, int& width, int& height) {
    // UIScreen.mainScreen is SDL's first display. Explicit display placement
    // also ignores desktop positions saved by an older build.
    x = SDL_WINDOWPOS_CENTERED_DISPLAY(0);
    y = SDL_WINDOWPOS_CENTERED_DISPLAY(0);
    SDL_Rect bounds{};
    if (SDL_GetDisplayBounds(0, &bounds) == 0 && bounds.w > 0 && bounds.h > 0) {
        width = bounds.w;
        height = bounds.h;
    }
}
} // namespace Fast
