/******************************************************************************
 * profiler.h
 *
 * Lightweight profiling utility for Financio Project.
 *
 * Purpose:
 *   This profiler is used for testing, debugging, and performance
 *   optimization only. It allows you to annotate sections of code
 *   (scopes and functions) and gather timing information in JSON form
 *   suitable for Chrome’s tracing tools or other visualization front-ends.
 *
 *   **NOT** part of the core Financio API—include and enable this
 *   only when you need to drill into simulation performance or verify
 *   execution flow. When ENABLE_PROFILING is set to 0 (the default),
 *   all profiling macros compile away to nothing.
 *
 * Author:
 *   Rodrigo Vildósola
 *
 * Usage:
 *   - Wrap the top of your `main()` or test harness with:
 *       PROFILE_BEGIN_SESSION("MySession", "my_profile.json");
 *   - Annotate functions or code regions with:
 *       PROFILE_FUNCTION();
 *       // or
 *       PROFILE_SCOPE("SomeScopeName");
 *   - At shutdown or end of your session:
 *       PROFILE_END_SESSION();
 *
 *   The output file can be opened in Chrome:
 *     chrome://tracing → Load your JSON → Inspect timings.
 *
 ******************************************************************************/

#pragma once

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <string>
#include <thread>
#include <mutex>
#include <sstream>

namespace Profiler {

    using FloatingPointMicroseconds = std::chrono::duration<double, std::micro>;

    struct ProfileResult
    {
        std::string name;
        FloatingPointMicroseconds start;
        std::chrono::microseconds elapsed_time;
        std::thread::id thread_id;
    };

    struct InstrumentationSession
    {
        std::string name;
    };

    class Instrumentor
    {
    public:
        Instrumentor(const Instrumentor&) = delete;
        Instrumentor(Instrumentor&&) = delete;

        void begin_session(const std::string& name, const std::string& filepath = "results.json")
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (current_session)
            {
                internal_end_session();
            }
            output_stream.open(filepath);

            if (output_stream.is_open())
            {
                current_session = new InstrumentationSession({name});
                write_header();
            }
        }

        void end_session()
        {
            std::lock_guard<std::mutex> lock(mutex);
            internal_end_session();
        }

        void write_profile(const ProfileResult& result)
        {
            std::stringstream json;
            json << std::setprecision(3) << std::fixed;

            if (!is_first_entry) {
                json << ",";  // Add comma only after the first entry
            } else {
                is_first_entry = false; // Set flag after the first entry
            }

            json << "{";
            json << "\"cat\":\"function\",";
            json << "\"dur\":" << (result.elapsed_time.count()) << ',';
            json << "\"name\":\"" << result.name << "\",";
            json << "\"ph\":\"X\",";
            json << "\"pid\":0,";
            json << "\"tid\":\"" << result.thread_id << "\",";
            json << "\"ts\":" << result.start.count();
            json << "}";

            std::lock_guard<std::mutex> lock(mutex);
            if (current_session)
            {
                output_stream << json.str();
                output_stream.flush();
            }
        }

        static Instrumentor& get()
        {
            static Instrumentor instance;
            return instance;
        }

    private:

        bool is_first_entry = true;

        Instrumentor() : current_session(nullptr) {}
        ~Instrumentor() { end_session(); }

        void write_header()
        {
            output_stream << "{\"otherData\": {},\"traceEvents\":[";
            // output_stream.flush();
        }

        void write_footer()
        {
            output_stream << "]}";
            // output_stream.flush();
        }

        void internal_end_session()
        {
            if (current_session)
            {
                write_footer();
                output_stream.close();
                delete current_session;
                current_session = nullptr;
                is_first_entry = true; // Reset for next session
            }
        }

    private:
        std::mutex mutex;
        InstrumentationSession* current_session;
        std::ofstream output_stream;
    };

    class InstrumentationTimer
    {
    public:
        InstrumentationTimer(const char* name) : name(name), stopped(false)
        {
            start_timepoint = std::chrono::steady_clock::now();
        }

        ~InstrumentationTimer()
        {
            if (!stopped) stop();
        }

        void stop()
        {
            auto end_timepoint = std::chrono::steady_clock::now();
            auto high_res_start = FloatingPointMicroseconds{ start_timepoint.time_since_epoch() };
            auto elapsed_time = std::chrono::time_point_cast<std::chrono::microseconds>(end_timepoint).time_since_epoch() - std::chrono::time_point_cast<std::chrono::microseconds>(start_timepoint).time_since_epoch();

            Instrumentor::get().write_profile({ name, high_res_start, elapsed_time, std::this_thread::get_id() });
            stopped = true;
        }
    private:
        const char* name;
        std::chrono::time_point<std::chrono::steady_clock> start_timepoint;
        bool stopped;
    };

    namespace Utils {

        template <size_t N>
        struct CleanedString
        {
            char data[N];
        };

        template <size_t N, size_t K>
        constexpr auto cleanup_output_string(const char(&expr)[N], const char(&remove)[K])
        {
            CleanedString<N> result = {};

            size_t src_index = 0;
            size_t dst_index = 0;
            while (src_index < N)
            {
                size_t match_index = 0;
                while (match_index < K - 1 && src_index + match_index < N - 1 && expr[src_index + match_index] == remove[match_index])
                    match_index++;
                if (match_index == K - 1)
                    src_index += match_index;
                result.data[dst_index++] = expr[src_index] == '"' ? '\'' : expr[src_index];
                src_index++;
            }
            return result;
        }
    }
}

// Macros for profiling
#define ENABLE_PROFILING 0
#if ENABLE_PROFILING
	// Resolve which function signature macro will be used. Note that this only
	// is resolved when the (pre)compiler starts, so the syntax highlighting
	// could mark the wrong one in your editor!
    #if defined(__GNUC__) || defined(__clang__)
        #define FUNC_SIG __PRETTY_FUNCTION__
    #elif defined(_MSC_VER)
        #define FUNC_SIG __FUNCSIG__
    #else
        #define FUNC_SIG __func__
    #endif

    #define PROFILE_BEGIN_SESSION(name, filepath) ::Profiler::Instrumentor::get().begin_session(name, filepath)
    #define PROFILE_END_SESSION() ::Profiler::Instrumentor::get().end_session()
    #define PROFILE_SCOPE_LINE2(name, line) constexpr auto fixedName##line = ::Profiler::Utils::cleanup_output_string(name, "__cdecl ");\
											::Profiler::InstrumentationTimer timer##line(fixedName##line.data)
    #define PROFILE_SCOPE_LINE(name, line) PROFILE_SCOPE_LINE2(name, line)
    #define PROFILE_SCOPE(name) PROFILE_SCOPE_LINE(name, __LINE__)
    #define PROFILE_FUNCTION() PROFILE_SCOPE(FUNC_SIG)
#else
    #define PROFILE_BEGIN_SESSION(name, filepath)
    #define PROFILE_END_SESSION()
    #define PROFILE_SCOPE(name)
    #define PROFILE_FUNCTION()
#endif
