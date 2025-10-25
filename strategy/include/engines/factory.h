#pragma once

#include "engine.h"
#include "engines/research_engine.h"
#include "engines/live_engine.h"
#include "environment.h"

#include <memory>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>

/**
 * @brief Factory class that creates the appropriate engine implementation
 *        based on the environment specified in the JSON configuration.
 */
class EngineFactory {
public:
    /**
     * @brief Create an engine from a JSON configuration.
     *
     * Example JSON:
     * {
     *   "environment": "Research",
     *   "strategy": { ... },
     *   "execution_model": { ... }
     * }
     *
     * @param spec JSON configuration defining environment and strategy.
     * @return Unique pointer to an IEngine implementation.
     */
    static std::unique_ptr<IEngine> create(const nlohmann::json& spec) {
        if (!spec.contains("environment"))
            throw std::runtime_error("EngineFactory: missing 'environment' in spec");

        std::string env_str = spec.at("environment").get<std::string>();
        Environment env = parse_environment(env_str);

        switch (env) {
            case Environment::Research:
            case Environment::Backtest:
            case Environment::Simulation:
                return std::make_unique<ResearchEngine>();

            case Environment::Live:
            case Environment::Paper:
                return std::make_unique<LiveEngine>();

            default:
                throw std::runtime_error("EngineFactory: unknown environment " + env_str);
        }
    }

private:
    /**
     * @brief Convert string to Environment enum.
     */
    static Environment parse_environment(const std::string& s) {
        if (s == "Research")   return Environment::Research;
        if (s == "Backtest")   return Environment::Backtest;
        if (s == "Simulation") return Environment::Simulation;
        if (s == "Paper")      return Environment::Paper;
        if (s == "Live")       return Environment::Live;
        throw std::runtime_error("EngineFactory: invalid environment string '" + s + "'");
    }
};
