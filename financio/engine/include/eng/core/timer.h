#pragma once

#include <chrono>

namespace eng {

	class Timer {
	public:
		// Initialize the timer
		static void init();

		// Time since init() in seconds
		static float elapsed();

		// Time since init() in milliseconds
		static float elapsed_millis();

	private:
		static std::chrono::time_point<std::chrono::high_resolution_clock> s_start_time;
	};

}
