#include "eng/core/timer.h"

namespace eng {

	std::chrono::time_point<std::chrono::high_resolution_clock> Timer::s_start_time;

	void Timer::init() {
		s_start_time = std::chrono::high_resolution_clock::now();
	}

	float Timer::elapsed() {
		auto now = std::chrono::high_resolution_clock::now();
		auto delta = std::chrono::duration<float>(now - s_start_time);
		return delta.count(); // in seconds
	}

	float Timer::elapsed_millis() {
		auto now = std::chrono::high_resolution_clock::now();
		auto delta = std::chrono::duration<float, std::milli>(now - s_start_time);
		return delta.count(); // in milliseconds
	}

}
