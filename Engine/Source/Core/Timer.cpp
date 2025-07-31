#include "pch.h"
#include "Timer.h"

namespace wf
{
	void Timer::tick(bool tickCustomTimers)
	{
		static bool ticked{ false };

		m_currentTime = wf::Clock::now();
		m_deltaTime = ticked ? wf::Duration(m_currentTime - m_lastTime).count() : 0.f;

		ticked = true;

		m_lastTime = m_currentTime;
		m_frameCount++;
		m_fixedAccumulator += m_deltaTime;

		// so that the fixed update runs at least once on the first frame
		if (m_frameCount == 1) {
			m_fixedAccumulator = m_fixedTimestep;
		}

		refreshFps();

		if (tickCustomTimers) {
			updateCustomTimers();
		}
	}

	bool Timer::isFixedUpdateReady()
	{
		bool ret = m_fixedAccumulator >= m_fixedTimestep;
		if (ret) m_fixedAccumulator -= m_fixedTimestep;
		return ret;
	}

	float Timer::getDeltaTime() const
	{
		return m_deltaTime;
	}

	float Timer::getFixedTimestep() const
	{
		return m_fixedTimestep;
	}

	void Timer::setFixedTimestep(float timestep)
	{
		m_fixedTimestep = timestep;
	}

	float Timer::getFps() const
	{
		return m_fps;
	}

	void Timer::createTimer(float duration, TimerCallback callback, bool autoRenew, int renewCount)
	{
		CustomTimer t(duration, callback, autoRenew, renewCount);
		m_pendingTimers.push_back(t);
	}

	void Timer::pauseTimers(bool pause)
	{
		m_timersPaused = pause;
	}

	void Timer::clearTimers()
	{
		m_timers.clear();
		m_pendingTimers.clear();
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

	void Timer::updateCustomTimers()
	{
		if (!m_timersPaused && m_timers.size()) {
			for (size_t i = 0; i < m_timers.size(); i++) {
				auto& timer = m_timers[i];
				if (timer.running) {
					timer.remaining -= m_deltaTime;
					if (timer.remaining <= 0.f) {
						timer.expired = true;

						if (timer.autoRenew) {
							if (timer.renewCount == -1 || timer.remainingRenewals > 0) {
								timer.remaining += timer.duration;
								timer.expired = false;
								timer.renewals++;

								if (timer.renewCount > 0) {
									timer.remainingRenewals--;
								}
							}
						}

						if (timer.expired) {
							timer.running = false;
						}

						timer.expiryCallback(timer);
					}
				}
			}

			std::erase_if(m_timers, [](const CustomTimer& timer) {
				return timer.expired;
				});
		}

		if (!m_pendingTimers.empty()) {
			m_timers.insert(m_timers.end(), m_pendingTimers.begin(), m_pendingTimers.end());
			m_pendingTimers.clear();
		}
	}
}