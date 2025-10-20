#include "python/python_runtime.h"

#include <pybind11/embed.h>

#include <filesystem>
#include <iostream>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

namespace py = pybind11;
namespace fs = std::filesystem;

static fs::path get_executable_dir() {
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    return fs::path(buffer).parent_path();
#elif __APPLE__
    char buffer[1024];
    uint32_t size = sizeof(buffer);
    _NSGetExecutablePath(buffer, &size);
    return fs::path(buffer).parent_path();
#else
    return fs::read_symlink("/proc/self/exe").parent_path();
#endif
}

python_runtime::python_runtime() : m_guard{} {
    try {
        fs::path exe_dir = get_executable_dir();
        fs::path py_root = exe_dir / "python";
        fs::path py_models = py_root / "models";

        add_sys_path(py_root.string());
        add_sys_path(py_models.string());

        std::cerr << "[python_runtime] Python path added: " << py_root << "\n";
    } catch (const std::exception& e) {
        std::cerr << "[python_runtime] init error: " << e.what() << "\n";
    }
}
python_runtime::~python_runtime() = default;

void python_runtime::add_sys_path(const std::string& p) {
    // safe to call many times
    py::module_ sys = py::module_::import("sys");
    auto path = sys.attr("path");
    if (!py::bool_(py::module_::import("builtins").attr("any")(
            py::make_tuple(path.contains(p))))) {
        path.attr("append")(p);
    }
}
