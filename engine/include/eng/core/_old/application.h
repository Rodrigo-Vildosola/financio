#pragma once

#include "eng/core/layer/layer_stack.h"
#include "eng/core/caps.h"
#include "eng/core/base.h"
#include "eng/core/timer.h"
#include "spdlog/logger.h"

namespace eng {

struct CommandLineArgs {
    i32 count = 0;
    char** args = nullptr;

    const char* operator[](i32 index) const {
        ENG_CORE_ASSERT(index < count, "Index out of bounds for CommandLineArgs");
        return args[index];
    }
};

// Simple hooks interface for platform integration
struct NoopHooks {
    static void init() {}
    static void shutdown() {}
    static void begin_frame() {}
    static void end_frame() {}
    static void pump_platform() {}
};

template<class AppCfg, class Hooks = NoopHooks>
class Application {
public:
    explicit Application(CommandLineArgs args = {});
    ~Application();

    template<class L, class... Args>
    L& push_root_layer(Args&&... a);

    template<class L, class... Args>
    L& push_overlay(Args&&... a);

    void run();
    void close();

    LayerStack<AppCfg>& layers();
    CommandLineArgs args() const;

private:
    bool m_running{true};
    CommandLineArgs m_args;
    LayerStack<AppCfg> m_stack{};
};

}

#include "eng/core/application.tpp"
