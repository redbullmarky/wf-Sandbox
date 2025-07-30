#include "pch.h"
#include "Render/Colour.h"
#include "Sun.h"

#include <algorithm>

namespace wf::scene
{
	Sun& Sun::setSunrise(int seconds)
	{
		sunrise = seconds;
		return *this;
	}

	Sun& Sun::setSunrise(int hours, int minutes)
	{
		return setSunrise((minutes + (hours * 60)) * 60);
	}

	Sun& Sun::setSunset(int seconds)
	{
		sunset = seconds;
		return *this;
	}

	Sun& Sun::setSunset(int hours, int minutes)
	{
		return setSunset((minutes + (hours * 60)) * 60);
	}

	Sun& Sun::setWeather(float _cloudCoverage, float _stormFactor)
	{
		cloudCoverage = std::clamp(_cloudCoverage, 0.f, 1.f);
		stormFactor = std::clamp(_stormFactor, 0.f, 1.f);
		return *this;
	}

	Sun& Sun::setTimeOfDay(int seconds)
	{
		timeOfDay = seconds;
		return *this;
	}

	Sun& Sun::setTimeOfDay(int hours, int minutes)
	{
		return setTimeOfDay((minutes + (hours * 60)) * 60);
	}

	void Sun::updateLightProperties()
	{
		int twilightLength = 3600; // 30 mins
		Colour finalColour = { 1, 1, 1, 1 };

		int dayLightLength = sunset - sunrise;
		int nightTimeLength = 86400 - dayLightLength;
		float sunDegrees = (timeOfDay >= sunrise && timeOfDay <= sunset) ?
			180.f * float(timeOfDay - sunrise) / float(dayLightLength) :
			180.f + 180.f * float((timeOfDay > sunset ? timeOfDay - sunset : timeOfDay + (86400 - sunset))) / nightTimeLength;

		float theta = glm::radians(sunDegrees);
		Vec3 sunPos = Vec3{ cos(theta) * 100.f, sin(theta) * 100.f, 0.f };

		// handle twilight, tinging the sky red for half an hour
		bool twilight = (timeOfDay >= (sunrise - twilightLength) && timeOfDay < sunrise) ||
			(timeOfDay > sunset && timeOfDay <= (sunset + twilightLength));
		bool night = (timeOfDay < sunrise || timeOfDay > sunset);

		// for twilight:
		//		- if distance from sunrise/sunset is less than 50% of twilightLength, we lower the G/B
		//		- if distance from sunrise/sunset is greater than 50%, we lower the R.
		// beyond twilight (i.e. night)
		//		- full intensity applied to r,g and b
		if (twilight) {
			float distance = static_cast<float>((timeOfDay <= sunrise ? sunrise - timeOfDay : timeOfDay - sunset)) / static_cast<float>(twilightLength);

			if (distance < .5f) {
				finalColour.g *= 1.f - distance;
				finalColour.b *= 1.f - distance;
			}
			else {
				finalColour.r *= .5f + (1.f - distance);
				finalColour.g *= .5f;
				finalColour.b *= .5f;
			}

			finalColour = Colour::from(glm::mix(finalColour.toVec3(), Vec3{ 1.f, .45, 0.f }, distance < .5f ? distance * 2.f : 1.f - distance));
		}
		else if (night) {
			finalColour.r *= .5f;
			finalColour.g *= .5f;
			finalColour.b *= .5f;
		}

		// for initial sun ambience, we'll find out how far we are between our time and (dayLightLength/2)
		float dayMidpoint = sunrise + (dayLightLength / 2.f);
		float dayDist = std::clamp(fabs(timeOfDay - dayMidpoint) / (dayLightLength / 2.f), 0.f, 1.f);

		// now we know how far away from day's midpoint is, we can scale the ambience
		ambientLevel = (.2f * (1.f - dayDist));

		// for clouds, further degrade
		ambientLevel *= (1.f - cloudCoverage);

		// for storm, we'll mix a bit of nasty
		finalColour = Colour::from(glm::mix(finalColour.toVec3(), Vec3{ .5f, .5f, .5f }, stormFactor));

		direction = glm::normalize(glm::vec3(0.f) - sunPos);
		colour = finalColour;
	}
}