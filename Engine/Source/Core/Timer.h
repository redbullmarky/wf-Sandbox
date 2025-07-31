#pragma once
#include <chrono>
#include <functional>
#include <vector>

namespace wf
{
	struct CustomTimer;

	using Clock = std::chrono::high_resolution_clock;
	using TimePoint = std::chrono::time_point<Clock>;
	using Duration = std::chrono::duration<float>;
	using TimerCallback = std::function<void(CustomTimer&)>;

	struct CustomTimer
	{
		friend class Timer;
	public:
		float duration;
		bool autoRenew{ false };
		int renewCount{ -1 };

		bool running{ true };
		bool expired{ false };
		float remaining;
		int renewals{ 0 };
		int remainingRenewals{ 0 };

		/**
		 * @brief Create a new timer that begins immediately
		 */
		CustomTimer(float duration, TimerCallback expiryCallback, bool autoRenew = false, int renewCount = -1)
			: duration(duration), expiryCallback(expiryCallback), autoRenew(autoRenew), renewCount(renewCount) {
			remaining = duration;
			if (renewCount > 0) {
				remainingRenewals = renewCount;
			}
		}

		void pause() { running = false; }
		void Resume() { running = true; }
		void expire() { running = false; expired = true; }

	private:
		TimerCallback expiryCallback;
	};

	class Timer
	{
	public:
		Timer() = default;
		~Timer() = default;

		void tick(bool tickCustomTimers = true);

		bool isFixedUpdateReady();
		float getDeltaTime() const;
		float getFixedTimestep() const;
		void setFixedTimestep(float timestep);
		float getFps() const;

		/**
		 * @brief Create a new callback timer
		 */
		void createTimer(float duration, TimerCallback callback, bool autoRenew = false, int renewCount = -1);

		/**
		 * @brief Prevents further accumulation of timers or resumes.
		 */
		void pauseTimers(bool pause = true);

		/**
		 * @brief Clear timers. Does not invoke expiry callbacks as a result
		 */
		void clearTimers();

	private:
		void refreshFps();
		void updateCustomTimers();

	private:
		// config
		float m_fixedTimestep{ 1.f / 60.f };

		// state
		float m_deltaTime{ 0.f };
		float m_fixedAccumulator{ 0.f };
		size_t m_frameCount{ 0 };
		float m_fps{ 0.f };

		// custom timers
		std::vector<CustomTimer> m_timers;
		std::vector<CustomTimer> m_pendingTimers;
		bool m_timersPaused{ false };

		// internal
		TimePoint m_currentTime{};
		TimePoint m_lastTime{};
	};
}
