#pragma once

#include "engpch.h"

namespace eng {

	class Timestep
	{
	public:
		Timestep(f32 time = 0.0f)
			: m_time(time)
		{
		}

		operator f32() const { return m_time; }

		f32 get_seconds() const { return m_time; }
		f32 get_milliseconds() const { return m_time * 1000.0f; }
	private:
		f32 m_time;
	};

}
