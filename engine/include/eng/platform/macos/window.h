#pragma once

#include "eng/core/window.h"
#include "eng/core/context/context.h"
#include "eng/input/input.h"

#include <GLFW/glfw3.h>

namespace eng {

	class MacOSWindow : public Window
	{
	public:
		MacOSWindow(const WindowProps& props);
		virtual ~MacOSWindow();

		void on_update() override;

		inline u32 get_width() const override { return m_data.width; }
		inline u32 get_height() const override { return m_data.height; }

		std::pair<u32, u32> get_framebuffer_size() const override;

		glm::vec2 get_mouse_position() const override;


		// Window attributes
		inline void set_event_cb(const EventCallbackFn& callback) override { m_data.event_cb = callback; }
		void set_vsync(bool enabled) override;
		bool is_vsync() const override;

		inline virtual void* get_native_window() const override { return m_window; }
		inline virtual wgpu::Surface get_surface(wgpu::Instance wgpu_instance) const override;

	private:
		virtual void init(const WindowProps& props);
		virtual void shutdown();

        GLFWwindow* m_window;

		struct WindowData {
			std::string title;
			u32 width, height;
			bool vsync;

			EventCallbackFn event_cb;
		};

		WindowData m_data;
	};

}
