#pragma once

#include "eng/events/event.h"
#include "eng/platform/key_codes.h"


namespace eng {

    class KeyEvent : public Event {
    public:
	    KeyCode get_key_code() const { return m_key_code; }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
    protected:
		KeyEvent(const KeyCode keycode)
            : m_key_code(keycode) {}

		KeyCode m_key_code;
    };

    class KeyPressedEvent : public KeyEvent {
    public:
		KeyPressedEvent(const KeyCode keycode, const u16 repeatCount)
            : KeyEvent(keycode), m_repeat_count(repeatCount) {}

        u16 get_repeat_count() const { return m_repeat_count; }

        std::string to_string() const override {
            std::stringstream ss;
            ss << "KeyPressedEvent: " << m_key_code << " (" << m_repeat_count << " repeats)";
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyPressed)
    private:
        u16 m_repeat_count;
    };

    class KeyReleasedEvent : public KeyEvent {
    public:
		KeyReleasedEvent(const KeyCode keycode)
            : KeyEvent(keycode) {}

        std::string to_string() const override {
            std::stringstream ss;
            ss << "KeyReleasedEvent: " << m_key_code;
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyReleased)
    };

    class KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(const KeyCode keycode)
			: KeyEvent(keycode) {}

		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_key_code;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	};
}
