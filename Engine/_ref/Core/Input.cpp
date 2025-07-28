#include "pch.h"
#include "Input.h"

namespace wf
{
	Input::Input()
	{
		refresh();
	}

	void Input::refresh()
	{
		m_previous = m_current;
		m_current.refresh();
	}

	void Input::processEvent(SDL_Event* event)
	{
		switch (event->type)
		{
		case SDL_EVENT_KEY_DOWN:
		{
			m_current.keyboard.keys[event->key.scancode] = true;
			m_current.keyboard.repeat[event->key.scancode] = event->key.repeat;
			break;
		}
		case SDL_EVENT_KEY_UP:
		{
			m_current.keyboard.keys[event->key.scancode] = false;
			m_current.keyboard.repeat[event->key.scancode] = false;
			break;
		}
		case SDL_EVENT_MOUSE_MOTION:
		{
			m_current.mouse.position = { event->motion.x, event->motion.y };
			m_current.mouse.delta += Vec2{ event->motion.xrel, event->motion.yrel };
			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		{
			m_current.mouse.buttons[event->button.button] = true;
			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_UP:
		{
			m_current.mouse.buttons[event->button.button] = false;
			break;
		}
		case SDL_EVENT_MOUSE_WHEEL:
		{
			m_current.mouse.wheel += Vec2{ event->wheel.x, event->wheel.y };
			break;
		}
		}
	}

	bool Input::isAnyKeyPressed() const
	{
		for (int i = 0; i < NUM_KEYS; ++i) {
			if (isKeyPressed(static_cast<KeyCode>(i))) return true;
		}
		return false;
	}

	bool Input::isAnyKeyHeld() const
	{
		for (int i = 0; i < NUM_KEYS; ++i) {
			if (isKeyHeld(static_cast<KeyCode>(i))) return true;
		}
		return false;
	}

	bool Input::isKeyPressed(KeyCode key) const
	{
		return m_current.keyboard.keys[key] && !m_previous.keyboard.keys[key];
	}

	bool Input::isKeyHeld(KeyCode key) const
	{
		return m_current.keyboard.keys[key] && m_previous.keyboard.keys[key];
	}

	bool Input::isKeyReleased(KeyCode key) const
	{
		return !m_current.keyboard.keys[key] && m_previous.keyboard.keys[key];
	}

	std::vector<int> Input::getPressedKeys() const
	{
		std::vector<int> keys;
		for (int i = 0; i < NUM_KEYS; ++i) {
			if (isKeyPressed(static_cast<KeyCode>(i))) {
				keys.push_back(i);
			}
		}
		return keys;
	}

	std::vector<int> Input::getHeldKeys() const
	{
		std::vector<int> keys;
		for (int i = 0; i < NUM_KEYS; ++i) {
			if (isKeyHeld(static_cast<KeyCode>(i))) {
				keys.push_back(i);
			}
		}
		return keys;
	}

	std::vector<int> Input::getReleasedKeys() const
	{
		std::vector<int> keys;
		for (int i = 0; i < NUM_KEYS; ++i) {
			if (!m_current.keyboard.keys[i] && m_previous.keyboard.keys[i]) {
				keys.push_back(i);
			}
		}
		return keys;
	}

	bool Input::isMouseButtonPressed(int button) const
	{
		return m_current.mouse.buttons[button] && !m_previous.mouse.buttons[button];
	}

	bool Input::isMouseButtonReleased(int button) const
	{
		return !m_current.mouse.buttons[button] && m_previous.mouse.buttons[button];
	}

	bool Input::isMouseButtonHeld(int button) const
	{
		return m_current.mouse.buttons[button] && m_previous.mouse.buttons[button];
	}

	Vec2 Input::getMousePosition() const
	{
		return m_current.mouse.position;
	}

	Vec2 Input::getMouseWheel() const
	{
		return m_current.mouse.wheel;
	}

	Vec2 Input::getMouseDelta() const
	{
		return m_current.mouse.delta;
	}
}