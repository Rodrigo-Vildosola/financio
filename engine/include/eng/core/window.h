#pragma once

#include "glm/fwd.hpp"
#include "engpch.h"
#include "eng/core/base.h"
#include "eng/events/event.h"

namespace eng {

	struct WindowProps
	{
		std::string title;
		u32 width;
		u32 height;

		WindowProps(const std::string& title = "Financio",
			        u32 width = 900,
			        u32 height = 900)
			: title(title), width(width), height(height) {}
	};

	// Interface representing a desktop system based Window
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void on_update() = 0;

		virtual u32 get_width() const = 0;
		virtual u32 get_height() const = 0;

		virtual std::pair<u32, u32> get_framebuffer_size() const = 0;

		virtual glm::vec2 get_mouse_position() const = 0;



		// Window attributes
		virtual void set_event_cb(const EventCallbackFn& callback) = 0;
		virtual void set_vsync(bool enabled) = 0;
		virtual bool is_vsync() const = 0;

		virtual void* get_native_window() const = 0;

		virtual wgpu::Surface get_surface(wgpu::Instance wgpu_instance) const = 0;


		static scope<Window> create(const WindowProps& props = WindowProps());
	};

}
