#include "engpch.h"
#include "eng/debug/profiler.h"
#include "eng/platform/macos/window.h"

#include "eng/events/application_event.h"
#include "eng/events/mouse_event.h"
#include "eng/events/key_event.h"
#include "eng/renderer/renderer.h"

#include <GLFW/glfw3.h>
#include <glfw3webgpu.h>


namespace eng {

	static uint8_t s_GLFWWindowCount = 0;

	static void GLFWErrorCallback(int error, const char* description)
	{
		ENG_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	MacOSWindow::MacOSWindow(const WindowProps& props)
	{
		init(props);
	}

	MacOSWindow::~MacOSWindow()
	{
		// shutdown();
	}

	wgpu::Surface MacOSWindow::get_surface(wgpu::Instance wgpu_instance) const {
		return glfwCreateWindowWGPUSurface(wgpu_instance.Get(), m_window);
	}

	void MacOSWindow::init(const WindowProps& props)
	{
		m_data.title = props.title;
		m_data.width = props.width;
		m_data.height = props.height;

		ENG_CORE_INFO("Creating window {0} ({1}, {2})", props.title, props.width, props.height);

		if (s_GLFWWindowCount == 0)
		{
			ENG_CORE_INFO("Initializing GLFW");

			int success = glfwInit();
			ENG_CORE_ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		{
			m_window = glfwCreateWindow((int)props.width, (int)props.height, m_data.title.c_str(), nullptr, nullptr);
			ENG_CORE_ASSERT(m_window, "Failed to create GLFW window");
			++s_GLFWWindowCount;
		}

		glfwSetWindowUserPointer(m_window, &m_data);
		set_vsync(true);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.width = width;
			data.height = height;

			WindowResizeEvent event(width, height);
			data.event_cb(event);
		});

		glfwSetWindowContentScaleCallback(m_window, [](GLFWwindow*, float xscale, float yscale) {
			ENG_CORE_INFO("DPI scale changed: {}, {}", xscale, yscale);
			// Resize render targets, UI, etc.
		});

		glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.event_cb(event);
		});

		glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.event_cb(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.event_cb(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data.event_cb(event);
					break;
				}
			}
		});

		glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int keycode)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event(keycode);
			data.event_cb(event);
		});

		glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.event_cb(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.event_cb(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.event_cb(event);
		});

		glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.event_cb(event);
		});
	}

	void MacOSWindow::shutdown()
	{
		glfwDestroyWindow(m_window);
		--s_GLFWWindowCount;
		if (s_GLFWWindowCount == 0)
		{
			ENG_CORE_INFO("Terminating GLFW");
			glfwTerminate();
		}
	}

	void MacOSWindow::on_update() {
		PROFILE_FUNCTION();

        glfwPollEvents();
	}

	std::pair<u32, u32> MacOSWindow::get_framebuffer_size() const {
		int width, height;
		glfwGetFramebufferSize(m_window, &width, &height);
		return { (u32) width, (u32) height };
	}

	glm::vec2 MacOSWindow::get_mouse_position() const {
		double x, y;
		glfwGetCursorPos(m_window, &x, &y);
		return { (f32) x, (f32) y };
	}


	void MacOSWindow::set_vsync(bool enabled)
	{
		m_data.vsync = enabled;
	}

	bool MacOSWindow::is_vsync() const
	{
		return m_data.vsync;
	}

}
