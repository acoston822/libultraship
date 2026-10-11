// iosFrameRate.mm
// Asks iOS to keep the display at a given rate while the game runs. On ProMotion phones the system otherwise
// picks a rate for a Metal layer on its own and can step between 120 and ~80 Hz. This is only a hint: the
// system may ignore it (Low Power Mode, non-ProMotion screens).
#import <TargetConditionals.h>

#if TARGET_OS_IOS
#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>

@interface LUSFrameRateTarget : NSObject
- (void)tick:(CADisplayLink*)link;
@end

@implementation LUSFrameRateTarget
- (void)tick:(CADisplayLink*)link {
    // Intentionally empty: the display link exists only to carry the preferred frame rate range.
}
@end

static CADisplayLink* sLink = nil;
static LUSFrameRateTarget* sTarget = nil;

extern "C" void iOS_SetPreferredFrameRate(int fps) {
    dispatch_async(dispatch_get_main_queue(), ^{
        if (sLink != nil) {
            [sLink invalidate];
            sLink = nil;
        }
        if (fps <= 0) {
            return;
        }
        if (sTarget == nil) {
            sTarget = [[LUSFrameRateTarget alloc] init];
        }
        sLink = [CADisplayLink displayLinkWithTarget:sTarget selector:@selector(tick:)];
        if (@available(iOS 15.0, *)) {
            sLink.preferredFrameRateRange = CAFrameRateRangeMake((float) fps, (float) fps, (float) fps);
        } else {
            sLink.preferredFramesPerSecond = fps;
        }
        [sLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSRunLoopCommonModes];
    });
}
#endif
