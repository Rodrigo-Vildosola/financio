#pragma once
#include <nlohmann/json.hpp>
#include <pybind11/embed.h>
#include <memory>

class Py_Execution {
public:
    Py_Execution();
    ~Py_Execution();

    void load(const nlohmann::json& spec); // {"import","class","init":{...}}

    // context is passed as JSON (symbol, timestamp, current_weight, portfolio_notional,...)
    // returns JSON array of order dicts
    nlohmann::json generate_orders(double target_weight, const nlohmann::json& context);

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};
