#pragma once

#include <glm/glm.hpp>

#include "eng/core/base.h"
#include "eng/input/key_codes.h"
#include "eng/input/mouse_codes.h"

namespace eng {

	class Input
	{
	public:
		static bool is_key_pressed(KeyCode key);

		static bool is_mouse_button_pressed(MouseCode button);
		static glm::vec2 get_mouse_pos();
		static float get_mouse_x();
		static float get_mouse_y();
	};

}
