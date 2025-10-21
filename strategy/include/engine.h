#pragma once

#include "environment.h"
#include "feature_vector.h"
#include "model.h"

#include <nlohmann/json.hpp>
#include <memory>
#include <vector>
#include <string>

/**
 * @brief Base interface for all runtime engines that drive model execution.
 *
 * Examples:
 *  - ResearchEngine: runs offline training and analysis
 *  - BacktestEngine: simulates execution on historical data
 *  - LiveEngine: consumes real-time market data and produces live orders
 *
 * Each engine owns or references strategy components (return/risk/execution models)
 * and coordinates data flow, feature extraction, and updates.
 */
class IEngine {
public:
    virtual ~IEngine() = default;

    /**
     * @brief Configure engine from JSON spec.
     *
     * Example:
     * {
     *   "environment": "Research",
     *   "strategy": { "class": "MultiModelStrategy", ... },
     *   "data": { "path": "data/AAPL.csv" }
     * }
     */
    virtual void load(const nlohmann::json& spec) = 0;

    /**
     * @brief Initialize subcomponents (models, feature extractors, etc.).
     *        Called after load() but before run().
     */
    virtual void initialize() = 0;

    /**
     * @brief Main execution loop of the engine.
     *        Research/Backtest engines read historical data;
     *        Live engines subscribe to market streams.
     */
    virtual void run() = 0;

    /**
     * @brief Shutdown sequence and optional persistence.
     */
    virtual void finalize() = 0;

    /**
     * @brief Return environment type (Research, Backtest, Live, ...).
     */
    virtual Environment environment() const = 0;
};
