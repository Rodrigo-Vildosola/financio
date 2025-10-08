#pragma once

#include "eng/core/timestep.h"
#include "eng/events/event.h"

namespace eng {

// Default no-op hooks. Lives in eng-core. No windowing symbols here.
struct NoopHooks {
    template<class Ctx>
    static void init(Ctx&) {}

    template<class Ctx>
    static void shutdown(Ctx&) {}

    template<class Ctx>
    static void begin_frame(Ctx&) {}

    template<class Ctx>
    static void end_frame(Ctx&) {}

    template<class Ctx>
    static void pump_platform(Ctx&) {}           // headless: nothing

    template<class Ctx>
    static bool poll_event(Ctx&, Event&) {
        return false;  // optional: event feed
    }
};

} // namespace eng
