#include "engpch.h"
#include "eng/core/window.h"

#ifdef ENG_PLATFORM_WINDOWS
    #include "Platform/Windows/WindowsWindow.h"
#endif
#ifdef ENG_PLATFORM_LINUX
    #include "Platform/Linux/LinuxWindow.h"
#endif
#ifdef ENG_PLATFORM_MACOS
    #include "eng/platform/macos/window.h"
#endif


namespace eng {

scope<Window> Window::create(const WindowProps& props)
{
    #ifdef ENG_PLATFORM_WINDOWS
        return create_scope<WindowsWindow>(props);
    #elif defined(ENG_PLATFORM_LINUX)
        return create_scope<LinuxWindow>(props);
    #elif defined(ENG_PLATFORM_MACOS)
        return create_scope<MacOSWindow>(props);
    #else
        ENG_CORE_ASSERT(false, "Unknown platform!");
        return nullptr;
    #endif
}
}
