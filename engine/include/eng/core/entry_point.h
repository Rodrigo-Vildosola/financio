#pragma once

#include "eng/core/logger.h"
#include "eng/core/application.h"
#include "eng/debug/profiler.h"

#ifdef FNC_PLATFORM_MACOS

extern eng::Application* eng::create_application(eng::CommandLineArgs args);

int main(int argc, char** argv) {
    eng::logger::init();

    PROFILE_BEGIN_SESSION("FinancioInit", "eng_init.json");
    auto app = eng::create_application({ argc, argv });
    PROFILE_END_SESSION();

    PROFILE_BEGIN_SESSION("FinancioRuntime", "eng_runtime.json");
    app->run();
    PROFILE_END_SESSION();

    PROFILE_BEGIN_SESSION("FinancioDestruction", "eng_destruction.json");
    delete app;
    PROFILE_END_SESSION();
}

#endif
