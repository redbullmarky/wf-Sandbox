#include "pch.h"
#include "Timer.h"

namespace wf
{
	void Timer::tick()
	{
		static bool ticked{ false };

		m_currentTime = wf::Clock::now();
		m_deltaTime = ticked ? wf::Duration(m_currentTime - m_lastTime).count() : 0.f;

		ticked = true;

		m_lastTime = m_currentTime;
		m_frameCount++;
		m_fixedAccumulator += m_deltaTime;

		refreshFps();
	}

	bool Timer::isFixedUpdateReady()
	{
		bool ret = m_fixedAccumulator >= m_fixedTimeStep;
		if (ret) m_fixedAccumulator -= m_fixedTimeStep;
		return ret;
	}

	float Timer::getDeltaTime() const
	{
		return m_deltaTime;
	}

	float Timer::getFixedTime() const
	{
		return m_fixedTimeStep;
	}

	float Timer::getFps() const
	{
		return m_fps;
	}

	void Timer::refreshFps()
	{
		static constexpr int sampleCount = 100;
		static float samples[sampleCount] = {};
		static int index = 0;
		static int filled = 0;

		samples[index] = m_deltaTime;
		index = (index + 1) % sampleCount;
		if (filled < sampleCount) filled++;

		float total = 0.f;
		for (int i = 0; i < filled; i++)
			total += samples[i];

		float avgDelta = total / filled;

		m_fps = avgDelta > 0.f ? 1.f / avgDelta : 0.f;
	}
}