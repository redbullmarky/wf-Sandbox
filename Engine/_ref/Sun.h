#pragma once
#include "Light.h"

namespace wf::scene
{
	/**
	 * Represents a sunlight source
	 * All operations are *very* non-scientific/accurate, but
	 * I only added this as I thought it'd be cool.
	 */
	struct Sun : Light
	{
		int sunrise{ 16200 };			// 4.30am ish
		int sunset{ 73800 };			// 8.30pm ish
		int timeOfDay{ 12 * 3600 };		// Noon by default, in seconds

		float cloudCoverage{ 0.f };		// 0.f clear, 1.f fully covered
		float stormFactor{ 0.f };		// 0.f clear, 1.f bricking it, mate.

		Sun() {
			updateLightProperties();
		}

		Sun(int timeSeconds) : timeOfDay(timeSeconds) {
			updateLightProperties();
		}


		/**
		 * @brief Time the sun rises
		 */
		Sun& setSunrise(int seconds);
		Sun& setSunrise(int hours, int minutes);

		/**
		 * @brief Time the sun sets
		 */
		Sun& setSunset(int seconds);
		Sun& setSunset(int hours, int minutes);

		/**
		 * @brief Time elapsed (seconds) since midnight
		 */
		Sun& setTimeOfDay(int seconds);
		Sun& setTimeOfDay(int hours, int minutes);

		/**
		 * @brief Adjust weather detail
		 */
		Sun& setWeather(float cloudCoverage, float stormFactor);

		/**
		 * @brief Refreshes the light based on what we have
		 */
		void updateLightProperties();
	};
}