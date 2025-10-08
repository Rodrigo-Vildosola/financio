#pragma once

#include "eng/core/layer/layer_stack.h"
#include "eng/hooks/hooks.h"

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

template<class AppCfg, class Ctx = Context<>, class Hooks = NoopHooks>
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

    Ctx& context();
    LayerStack<Ctx>& layers();
    CommandLineArgs args() const;

private:
    bool m_running{true};
    CommandLineArgs m_args;
    Ctx m_ctx{};
    LayerStack<Ctx> m_stack{&m_ctx};
};

}

#include "eng/core/application.tpp"
