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
	constexpr wf::KeyCode KEY_B{ SDL_SCANCODE_B };
	constexpr wf::KeyCode KEY_C{ SDL_SCANCODE_C };
	constexpr wf::KeyCode KEY_D{ SDL_SCANCODE_D };
	constexpr wf::KeyCode KEY_E{ SDL_SCANCODE_E };
	constexpr wf::KeyCode KEY_F{ SDL_SCANCODE_F };
	constexpr wf::KeyCode KEY_G{ SDL_SCANCODE_G };
	constexpr wf::KeyCode KEY_H{ SDL_SCANCODE_H };
	constexpr wf::KeyCode KEY_I{ SDL_SCANCODE_I };
	constexpr wf::KeyCode KEY_J{ SDL_SCANCODE_J };
	constexpr wf::KeyCode KEY_K{ SDL_SCANCODE_K };
	constexpr wf::KeyCode KEY_L{ SDL_SCANCODE_L };
	constexpr wf::KeyCode KEY_M{ SDL_SCANCODE_M };
	constexpr wf::KeyCode KEY_N{ SDL_SCANCODE_N };
	constexpr wf::KeyCode KEY_O{ SDL_SCANCODE_O };
	constexpr wf::KeyCode KEY_P{ SDL_SCANCODE_P };
	constexpr wf::KeyCode KEY_Q{ SDL_SCANCODE_Q };
	constexpr wf::KeyCode KEY_R{ SDL_SCANCODE_R };
	constexpr wf::KeyCode KEY_S{ SDL_SCANCODE_S };
	constexpr wf::KeyCode KEY_T{ SDL_SCANCODE_T };
	constexpr wf::KeyCode KEY_U{ SDL_SCANCODE_U };
	constexpr wf::KeyCode KEY_V{ SDL_SCANCODE_V };
	constexpr wf::KeyCode KEY_W{ SDL_SCANCODE_W };
	constexpr wf::KeyCode KEY_X{ SDL_SCANCODE_X };
	constexpr wf::KeyCode KEY_Y{ SDL_SCANCODE_Y };
	constexpr wf::KeyCode KEY_Z{ SDL_SCANCODE_Z };

	constexpr wf::KeyCode KEY_0{ SDL_SCANCODE_0 };
	constexpr wf::KeyCode KEY_1{ SDL_SCANCODE_1 };
	constexpr wf::KeyCode KEY_2{ SDL_SCANCODE_2 };
	constexpr wf::KeyCode KEY_3{ SDL_SCANCODE_3 };
	constexpr wf::KeyCode KEY_4{ SDL_SCANCODE_4 };
	constexpr wf::KeyCode KEY_5{ SDL_SCANCODE_5 };
	constexpr wf::KeyCode KEY_6{ SDL_SCANCODE_6 };
	constexpr wf::KeyCode KEY_7{ SDL_SCANCODE_7 };
	constexpr wf::KeyCode KEY_8{ SDL_SCANCODE_8 };
	constexpr wf::KeyCode KEY_9{ SDL_SCANCODE_9 };

	constexpr wf::KeyCode KEY_KP0{ SDL_SCANCODE_KP_0 };
	constexpr wf::KeyCode KEY_KP1{ SDL_SCANCODE_KP_1 };
	constexpr wf::KeyCode KEY_KP2{ SDL_SCANCODE_KP_2 };
	constexpr wf::KeyCode KEY_KP3{ SDL_SCANCODE_KP_3 };
	constexpr wf::KeyCode KEY_KP4{ SDL_SCANCODE_KP_4 };
	constexpr wf::KeyCode KEY_KP5{ SDL_SCANCODE_KP_5 };
	constexpr wf::KeyCode KEY_KP6{ SDL_SCANCODE_KP_6 };
	constexpr wf::KeyCode KEY_KP7{ SDL_SCANCODE_KP_7 };
	constexpr wf::KeyCode KEY_KP8{ SDL_SCANCODE_KP_8 };
	constexpr wf::KeyCode KEY_KP9{ SDL_SCANCODE_KP_9 };

	constexpr wf::KeyCode KEY_UP{ SDL_SCANCODE_UP };
	constexpr wf::KeyCode KEY_DOWN{ SDL_SCANCODE_DOWN };
	constexpr wf::KeyCode KEY_LEFT{ SDL_SCANCODE_LEFT };
	constexpr wf::KeyCode KEY_RIGHT{ SDL_SCANCODE_RIGHT };

	constexpr wf::KeyCode KEY_F1{ SDL_SCANCODE_F1 };
	constexpr wf::KeyCode KEY_F2{ SDL_SCANCODE_F2 };
	constexpr wf::KeyCode KEY_F3{ SDL_SCANCODE_F3 };
	constexpr wf::KeyCode KEY_F4{ SDL_SCANCODE_F4 };
	constexpr wf::KeyCode KEY_F5{ SDL_SCANCODE_F5 };
	constexpr wf::KeyCode KEY_F6{ SDL_SCANCODE_F6 };
	constexpr wf::KeyCode KEY_F7{ SDL_SCANCODE_F7 };
	constexpr wf::KeyCode KEY_F8{ SDL_SCANCODE_F8 };
	constexpr wf::KeyCode KEY_F9{ SDL_SCANCODE_F9 };
	constexpr wf::KeyCode KEY_F10{ SDL_SCANCODE_F10 };
	constexpr wf::KeyCode KEY_F11{ SDL_SCANCODE_F11 };
	constexpr wf::KeyCode KEY_F12{ SDL_SCANCODE_F12 };

	constexpr wf::KeyCode KEY_SPACE{ SDL_SCANCODE_SPACE };
	constexpr wf::KeyCode KEY_BACKSPACE{ SDL_SCANCODE_BACKSPACE };
	constexpr wf::KeyCode KEY_DELETE{ SDL_SCANCODE_DELETE };
	constexpr wf::KeyCode KEY_TAB{ SDL_SCANCODE_TAB };
	constexpr wf::KeyCode KEY_CAPS{ SDL_SCANCODE_CAPSLOCK };

	///////////////////
	// MOUSE BUTTONS //
	///////////////////
	constexpr wf::ButtonCode BUTTON_LEFT{ SDL_BUTTON_LEFT };
	constexpr wf::ButtonCode BUTTON_MIDDLE{ SDL_BUTTON_MIDDLE };
	constexpr wf::ButtonCode BUTTON_RIGHT{ SDL_BUTTON_RIGHT };
}