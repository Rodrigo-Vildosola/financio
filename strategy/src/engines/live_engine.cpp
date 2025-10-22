// engine/live_engine.cpp
#include "engines/live_engine.h"
#include <iostream>

void LiveEngine::initialize() {
    std::cerr << "[LiveEngine] initialize()\n";
}

void LiveEngine::run() {
    std::cerr << "[LiveEngine] run() — live mode stub\n";
}

void LiveEngine::finalize() {
    std::cerr << "[LiveEngine] finalize()\n";
}
