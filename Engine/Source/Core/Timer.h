#pragma once
#include <chrono>

namespace wf
{
	using Clock = std::chrono::high_resolution_clock;
	using TimePoint = std::chrono::time_point<Clock>;
	using Duration = std::chrono::duration<float>;

	class Timer
	{
	public:
		Timer() = default;
		~Timer() = default;

		void tick();

		bool isFixedUpdateReady();
		float getDeltaTime() const;
		float getFixedTime() const;
		float getFps() const;

	private:
		void refreshFps();

	private:
		// config
		float m_fixedTimeStep{ 1.f / 60.f };

		// state
		float m_deltaTime{ 0.f };
		float m_fixedAccumulator{ 0.f };
		size_t m_frameCount{ 0 };
		float m_fps{ 0.f };

		// internal
		TimePoint m_currentTime{};
		TimePoint m_lastTime{};
	};
}
