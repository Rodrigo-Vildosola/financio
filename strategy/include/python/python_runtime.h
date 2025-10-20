#pragma once

#include <pybind11/embed.h>

#include <mutex>
#include <string>

class python_runtime {
public:
    static python_runtime& instance() {
        static python_runtime g_inst;
        return g_inst;
    }
    // add a path once (e.g., "strategy/python")
    void add_sys_path(const std::string& p);

private:
    python_runtime();
    ~python_runtime();
    python_runtime(const python_runtime&) = delete;
    python_runtime& operator=(const python_runtime&) = delete;

    pybind11::scoped_interpreter m_guard;
    std::once_flag m_path_once;
};
