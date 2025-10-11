#pragma once
#include "eng/events/event.h"

namespace eng {

template<class Ctx> class LayerStack; // fwd

// Headless/default hooks. Core-only.
struct NoopHooks {
    template<class Ctx>
    static void init(Ctx&, LayerStack<Ctx>*) {}

    template<class Ctx>
    static void shutdown(Ctx&) {}

    template<class Ctx>
    static void begin_frame(Ctx&) {}

    template<class Ctx>
    static void end_frame(Ctx&) {}

    template<class Ctx>
    static void pump_platform(Ctx&) {}

    template<class Ctx>
    static bool poll_event(Ctx&, Event&) { 
        return false; 
    }
};

} // namespace eng
