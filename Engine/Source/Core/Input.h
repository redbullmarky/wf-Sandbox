#pragma once
#include "Math/Math.h"

#include <array>
#include <cstdint>
#include <SDL3/SDL.h>
#include <vector>

namespace wf
{
	using KeyCode = SDL_Scancode;
	using ButtonCode = uint8_t;
	constexpr int NUM_KEYS = SDL_SCANCODE_COUNT;

	class Input
	{
	public:
		Input();
		~Input() = default;

		void processEvent(SDL_Event* event);
		void refresh();

		bool isAnyKeyPressed() const;
		bool isAnyKeyHeld() const;
		bool isKeyPressed(KeyCode key) const;
		bool isKeyReleased(KeyCode key) const;
		bool isKeyHeld(KeyCode key) const;

		std::vector<int> getPressedKeys() const;
		std::vector<int> getReleasedKeys() const;
		std::vector<int> getHeldKeys() const;

		bool isMouseButtonPressed(int button) const;
		bool isMouseButtonReleased(int button) const;
		bool isMouseButtonHeld(int button) const;

		Vec2 getMousePosition() const;
		Vec2 getMouseWheel() const;
		Vec2 getMouseDelta() const;

	private:
		struct InputState
		{
			struct InputKeyboardState
			{
				std::array<bool, NUM_KEYS> keys;
				std::array<bool, NUM_KEYS> repeat;

				void refresh()
				{
					// no need, as far as i can work out, to reset the current keys...because we're 
					// capturing keyup events anyway, so this also avoids the keyboard delay thing.
					// @tbc
					std::fill(repeat.begin(), repeat.end(), false);
				}
			} keyboard;

			struct InputMouseState
			{
				Vec2 position{};
				Vec2 delta{};
				std::array<bool, 10> buttons; // let's support a freaky mouse
				Vec2 wheel{};

				void refresh()
				{
					delta = {};
					wheel = {};
				}
			} mouse;

			void refresh()
			{
				keyboard.refresh();
				mouse.refresh();
			}
		};

	private:
		InputState m_current{};
		InputState m_previous{};
	};

	///////////////
	// KEY CODES //
	///////////////
	constexpr wf::KeyCode KEY_ESCAPE{ SDL_SCANCODE_ESCAPE };
	constexpr wf::KeyCode KEY_SHIFT_LEFT{ SDL_SCANCODE_LSHIFT };
	constexpr wf::KeyCode KEY_SHIFT_RIGHT{ SDL_SCANCODE_RSHIFT };
	constexpr wf::KeyCode KEY_ALT_LEFT{ SDL_SCANCODE_LALT };
	constexpr wf::KeyCode KEY_ALT_RIGHT{ SDL_SCANCODE_RALT };
	constexpr wf::KeyCode KEY_CTRL_LEFT{ SDL_SCANCODE_LCTRL };
	constexpr wf::KeyCode KEY_CTRL_RIGHT{ SDL_SCANCODE_RCTRL };
	constexpr wf::KeyCode KEY_CTRL_RETURN{ SDL_SCANCODE_RETURN };

	constexpr wf::KeyCode KEY_A{ SDL_SCANCODE_A };
	constexpr wf::KeyCode KEY_D{ SDL_SCANCODE_D };
	constexpr wf::KeyCode KEY_S{ SDL_SCANCODE_S };
	constexpr wf::KeyCode KEY_W{ SDL_SCANCODE_W };
	constexpr wf::KeyCode KEY_SPACE{ SDL_SCANCODE_SPACE };

	///////////////////
	// MOUSE BUTTONS //
	///////////////////
	constexpr wf::ButtonCode BUTTON_LEFT{ SDL_BUTTON_LEFT };
	constexpr wf::ButtonCode BUTTON_MIDDLE{ SDL_BUTTON_MIDDLE };
	constexpr wf::ButtonCode BUTTON_RIGHT{ SDL_BUTTON_RIGHT };
}