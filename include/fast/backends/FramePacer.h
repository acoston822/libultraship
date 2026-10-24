#pragma once

#include <cmath>
#include <cstdint>

namespace Fast {
// Plan frame deadlines from an epoch, not the previous sleep's wake-up time.
// Call once per frame, then wait until the returned monotonic timestamp. A late
// frame starts a fresh cadence instead of forcing catch-up frames in a burst.
class FramePacer {
  public:
    void Reset() {
        *this = FramePacer{};
    }

    double NextDeadline(double now, uint32_t fps) {
        if (!std::isfinite(now) || fps == 0) {
            Reset();
            return std::isfinite(now) ? now : 0;
        }
        const double pending = mStarted ? mEpoch + static_cast<double>(mNextFrame) / mFps : now;
        if (!mStarted || fps != mFps || now < mPrevious || now >= pending) {
            mEpoch = now;
            mNextFrame = 0;
            mFps = fps;
            mStarted = true;
        }
        mPrevious = now;
        return mEpoch + static_cast<double>(mNextFrame++) / mFps;
    }

  private:
    bool mStarted = false;
    uint32_t mFps = 0;
    uint64_t mNextFrame = 0;
    double mEpoch = 0, mPrevious = 0;
};
} // namespace Fast
