#pragma once
#include "model.h"
#include <nlohmann/json.hpp>
#include <pybind11/embed.h>
#include <memory>

class Py_ExecutionModel : IExecutionModel {
public:
    Py_ExecutionModel();
    ~Py_ExecutionModel();

    void load(const nlohmann::json& spec) override; // {"import","class","init":{...}}

    // context is passed as JSON (symbol, timestamp, current_weight, portfolio_notional,...)
    // returns JSON array of order dicts
    nlohmann::json generate_orders(double target_weight, const Context& context) override;

    void set_environment(Environment e) override;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};
