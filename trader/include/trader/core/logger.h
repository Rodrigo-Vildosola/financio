#pragma once

#include <memory>
#include <spdlog/fwd.h>
// #include <spdlog/spdlog.h>
// #include <spdlog/fmt/ostr.h>

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

// Core macros
#define TR_CORE_TRACE(...)    ::logger::get_core_logger()->trace(__VA_ARGS__)
#define TR_CORE_DEBUG(...)    ::logger::get_core_logger()->debug(__VA_ARGS__)
#define TR_CORE_INFO(...)     ::logger::get_core_logger()->info(__VA_ARGS__)
#define TR_CORE_WARN(...)     ::logger::get_core_logger()->warn(__VA_ARGS__)
#define TR_CORE_ERROR(...)    ::logger::get_core_logger()->error(__VA_ARGS__)
#define TR_CORE_CRITICAL(...) ::logger::get_core_logger()->critical(__VA_ARGS__)

// CLient macros
#define TR_TRACE(...)     ::logger::get_client_logger()->trace(__VA_ARGS__)
#define TR_DEBUG(...)     ::logger::get_client_logger()->debug(__VA_ARGS__)
#define TR_INFO(...)      ::logger::get_client_logger()->info(__VA_ARGS__)
#define TR_WARN(...)      ::logger::get_client_logger()->warn(__VA_ARGS__)
#define TR_ERROR(...)     ::logger::get_client_logger()->error(__VA_ARGS__)
#define TR_CRITICAL(...)  ::logger::get_client_logger()->critical(__VA_ARGS__)

#define TR_FILE_TRACE(...)     ::logger::get_only_file_logger()->trace(__VA_ARGS__)
#define TR_FILE_DEBUG(...)     ::logger::get_only_file_logger()->debug(__VA_ARGS__)
#define TR_FILE_INFO(...)      ::logger::get_only_file_logger()->info(__VA_ARGS__)
#define TR_FILE_WARN(...)      ::logger::get_only_file_logger()->warn(__VA_ARGS__)
#define TR_FILE_ERROR(...)     ::logger::get_only_file_logger()->error(__VA_ARGS__)
#define TR_FILE_CRITICAL(...)  ::logger::get_only_file_logger()->critical(__VA_ARGS__)
