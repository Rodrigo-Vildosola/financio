#pragma once

#include <memory>
#include <webgpu/webgpu_cpp_print.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace eng {

class logger {
public:
    static void init();

    inline static std::shared_ptr<spdlog::logger>& get_core_logger() { return core_logger; }
    inline static std::shared_ptr<spdlog::logger>& get_client_logger() { return client_logger; }
    inline static std::shared_ptr<spdlog::logger>& get_only_file_logger() { return only_file_logger; }

private:
    static std::shared_ptr<spdlog::logger> core_logger;
    static std::shared_ptr<spdlog::logger> client_logger;
    static std::shared_ptr<spdlog::logger> only_file_logger;

};

} // namespace eng

// Core macros
#define FNC_CORE_TRACE(...)    ::eng::logger::get_core_logger()->trace(__VA_ARGS__)
#define FNC_CORE_DEBUG(...)    ::eng::logger::get_core_logger()->debug(__VA_ARGS__)
#define FNC_CORE_INFO(...)     ::eng::logger::get_core_logger()->info(__VA_ARGS__)
#define FNC_CORE_WARN(...)     ::eng::logger::get_core_logger()->warn(__VA_ARGS__)
#define FNC_CORE_ERROR(...)    ::eng::logger::get_core_logger()->error(__VA_ARGS__)
#define FNC_CORE_CRITICAL(...) ::eng::logger::get_core_logger()->critical(__VA_ARGS__)

// CLient macros
#define FNC_TRACE(...)     ::eng::logger::get_client_logger()->trace(__VA_ARGS__)
#define FNC_DEBUG(...)     ::eng::logger::get_client_logger()->debug(__VA_ARGS__)
#define FNC_INFO(...)      ::eng::logger::get_client_logger()->info(__VA_ARGS__)
#define FNC_WARN(...)      ::eng::logger::get_client_logger()->warn(__VA_ARGS__)
#define FNC_ERROR(...)     ::eng::logger::get_client_logger()->error(__VA_ARGS__)
#define FNC_CRITICAL(...)  ::eng::logger::get_client_logger()->critical(__VA_ARGS__)

#define FNC_FILE_TRACE(...)     ::eng::logger::get_only_file_logger()->trace(__VA_ARGS__)
#define FNC_FILE_DEBUG(...)     ::eng::logger::get_only_file_logger()->debug(__VA_ARGS__)
#define FNC_FILE_INFO(...)      ::eng::logger::get_only_file_logger()->info(__VA_ARGS__)
#define FNC_FILE_WARN(...)      ::eng::logger::get_only_file_logger()->warn(__VA_ARGS__)
#define FNC_FILE_ERROR(...)     ::eng::logger::get_only_file_logger()->error(__VA_ARGS__)
#define FNC_FILE_CRITICAL(...)  ::eng::logger::get_only_file_logger()->critical(__VA_ARGS__)
