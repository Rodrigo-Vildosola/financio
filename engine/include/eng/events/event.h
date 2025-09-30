#pragma once

#include <functional>
#include <sstream>

#include "eng/core/base.h"

namespace eng {

    enum class EventType {
        None = 0,
        WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
        AppTick, AppUpdate, AppRender,
        KeyPressed, KeyReleased, KeyTyped,
        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
    };

    enum EventCategory {
        None = 0,
        EventCategoryApplication    = BIT(0),
        EventCategoryInput          = BIT(1),
        EventCategoryKeyboard       = BIT(2),
        EventCategoryMouse          = BIT(3),
        EventCategoryMouseButton    = BIT(4)
    };

    #define EVENT_CLASS_TYPE(type)  static EventType get_static_type() { return EventType::type; }                  \
                                    virtual EventType get_event_type() const override { return get_static_type(); } \
                                    virtual const char* get_name() const override { return #type; }

    #define EVENT_CLASS_CATEGORY(category) virtual int get_category_flags() const override { return category; }

    class Event {
        friend class EventDispatcher;
    public:
		virtual ~Event() = default;

        bool handled = false;


        virtual EventType get_event_type() const = 0;
        virtual const char* get_name() const = 0;
        virtual int get_category_flags() const = 0;
        virtual std::string to_string() const { return get_name(); }

        inline bool is_in_category(EventCategory category) {
            return get_category_flags() & category;
        }
    protected:
        bool m_handled = false;
    };

    class EventDispatcher {
    public:
        EventDispatcher(Event& event)
            : m_event(event) {}

		// F will be deduced by the compiler
		template<typename T, typename F>
		bool dispatch(const F& func)
		{
			if (m_event.get_event_type() == T::get_static_type())
			{
				m_event.handled |= func(static_cast<T&>(m_event)); 
				return true;
			}
			return false;
		}

    private:
        Event& m_event;
    };

    inline std::ostream& operator<<(std::ostream& os, const Event& e) {
        return os << e.to_string();
    }
}


// Implement custom formatter for eng::Event
namespace fmt {
    template <>
    struct formatter<eng::Event> {
        template <typename ParseContext>
        constexpr auto parse(ParseContext& ctx) {
            return ctx.begin();
        }

        template <typename FormatContext>
        auto format(const eng::Event& event, FormatContext& ctx) {
            return format_to(ctx.out(), "{}", event.to_string());
        }
    };
}
