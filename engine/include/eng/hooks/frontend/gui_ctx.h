#pragma once

#include "eng/core/base.h"
#include "eng/core/context.h"
#include "eng/platform/window.h"
#include "eng/renderer/context/context.h"

// Minimal systems available in GUI apps
namespace eng {

struct AppState { 
    bool running=true; 
    bool minimized=false; 
};

struct WindowSys { 
    scope<Window> win; 
};

struct GfxSys    { 
    scope<GraphicsContext> ctx; 
};

// Your concrete GUI context
using GuiCtx = Context<AppState, WindowSys, GfxSys>;

} // namespace eng
