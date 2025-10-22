#pragma once

#include "environment.h"
#include <nlohmann/json.hpp>
#include <vector>
#include <unordered_map>
#include <string>

/**
 * @brief Forward declaration for feature container.
 * FeatureVector encapsulates all numeric and categorical inputs
 * observed at a given timestamp.
 */
class FeatureVector;

/**
 * @brief Base interface for any model component that participates
 *        in the strategy graph (return model, risk model, strategy, execution).
 *
 * Each model can adapt its internal behavior according to the active
 * environment (Research, Backtest, Live, etc.).
 */
class IModel {
public:
    virtual ~IModel() = default;

    /**
     * @brief Set the environment in which this model operates.
     *        Implementations may change internal logic accordingly.
     *
     * @param e Active environment enum.
     */
    virtual void set_environment(Environment e) = 0;
};

/**
 * @brief Interface for predictive models that estimate expected return μ.
 *
 * A ReturnModel transforms a FeatureVector into an expected return.
 * It may be statistical (e.g., linear regression) or machine learning–based.
 * During research it may also support training or online updating.
 */
class IReturnModel : public IModel {
public:
    virtual ~IReturnModel() = default;

    /**
     * @brief Initialize model from JSON specification.
     *        Typically contains hyperparameters, weights, or paths to assets.
     */
    virtual void load(const nlohmann::json& spec) = 0;

    /**
     * @brief Predict expected return μ given feature vector.
     * @param f Current feature observation.
     * @return Expected return.
     */
    virtual double predict(const FeatureVector& f) = 0;

    /**
     * @brief Optional online update step for adaptive models.
     *        Called after realized returns are known.
     * @param f Feature observation used for prediction.
     * @param target Realized return (training target).
     */
    virtual void update(const FeatureVector& f, double target) = 0;

    /**
     * @brief Serialize current state (weights, parameters) into JSON.
     * @return Serializable model snapshot.
     */
    virtual nlohmann::json save_state() const { return {}; }
};

/**
 * @brief Interface for risk models estimating volatility, variance,
 *        or any other risk-related measure σ.
 *
 * Risk models receive the same feature vectors as return models but
 * produce an uncertainty or risk estimate instead of an expected return.
 */
class IRiskModel : public IModel {
public:
    virtual ~IRiskModel() = default;

    /**
     * @brief Initialize risk model from JSON specification.
     */
    virtual void load(const nlohmann::json& spec) = 0;

    /**
     * @brief Estimate current risk metric σ from features.
     * @param f Feature observation.
     * @return Risk measure (e.g., volatility, VaR, etc.).
     */
    virtual double estimate(const FeatureVector& f) = 0;

    /**
     * @brief Optional adaptive update step, called after realized returns.
     * @param f Feature observation.
     * @param realized_return Actual portfolio or instrument return.
     */
    virtual void update(const FeatureVector& f, double realized_return) = 0;

    /**
     * @brief Serialize current risk model state (e.g., volatility window).
     */
    virtual nlohmann::json save_state() const { return {}; }
};

/**
 * @brief Interface for execution models that transform
 *        target portfolio weights into concrete trade instructions.
 *
 * Execution models handle order slicing, liquidity estimation,
 * and slippage control. They operate only in real-time or simulation contexts.
 */
class IExecutionModel : public IModel {
public:
    virtual ~IExecutionModel() = default;

    /**
     * @brief Initialize execution model from JSON specification.
     */
    virtual void load(const nlohmann::json& spec) = 0;

    /**
     * @brief Generate execution orders required to reach target portfolio weight.
     *
     * @param target_weight Desired weight for the current symbol or portfolio.
     * @param context Execution context: symbol, timestamp, portfolio metrics, etc.
     * @return JSON array of order dictionaries (price, size, side, etc.).
     */
    virtual nlohmann::json generate_orders(double target_weight, const Context& context) = 0;
};

/**
 * @brief Interface for full trading strategies.
 *
 * A Strategy coordinates one or more return and risk models and uses
 * their outputs to produce target portfolio weights.
 * It may also receive execution feedback and realized returns for adaptive updates.
 */
class IStrategy : public IModel {
public:
    virtual ~IStrategy() = default;

    /**
     * @brief Initialize strategy from JSON specification.
     *        This can define submodels, parameters, or structural composition.
     */
    virtual void load(const nlohmann::json& spec) = 0;

    /**
     * @brief Called once per data bar or tick.
     *        Must output the desired target portfolio weight.
     *
     * @param f Latest feature vector.
     * @return Target portfolio weight (fraction of capital).
     */
    virtual double on_bar(const FeatureVector& f) = 0;

    /**
     * @brief Called after returns are realized.
     *        Used for performance tracking or model adaptation.
     *
     * @param f Feature vector used for the previous prediction.
     * @param realized_return Actual realized return over that bar.
     */
    virtual void update(const FeatureVector& f, double realized_return) = 0;
};
