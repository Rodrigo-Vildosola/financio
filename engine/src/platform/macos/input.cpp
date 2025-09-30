#include "engpch.h"
#include "eng/input/input.h"

#include "eng/core/application.h"
#include <GLFW/glfw3.h>

namespace eng {

	bool Input::is_key_pressed(const KeyCode key)
	{
		auto* window = static_cast<GLFWwindow*>(Application::get().get_window().get_native_window());
		auto state = glfwGetKey(window, static_cast<int32_t>(key));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::is_mouse_button_pressed(const MouseCode button)
	{
		auto* window = static_cast<GLFWwindow*>(Application::get().get_window().get_native_window());
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}

	glm::vec2 Input::get_mouse_pos()
	{
		auto* window = static_cast<GLFWwindow*>(Application::get().get_window().get_native_window());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	float Input::get_mouse_x()
	{
		return get_mouse_pos().x;
	}

	float Input::get_mouse_y()
	{
		return get_mouse_pos().y;
	}

}
