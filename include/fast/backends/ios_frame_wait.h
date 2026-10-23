#pragma once

#if defined(__IOS__)
namespace Fast {
// Install on the render/main thread. Return true after servicing the requested
// wait, or false to retain the normal frame-limiter sleep. No game work should
// run from the callback: a frame has been submitted but is not yet finished.
void SetIosFrameWaitCallback(bool (*callback)(double seconds));
} // namespace Fast
#endif
