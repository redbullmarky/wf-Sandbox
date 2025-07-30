#pragma once
#include "Math/Math.h"

#include <iomanip>
#include <sstream>

namespace wf
{
	struct ColourDenorm
	{
		unsigned char r{ 255 };
		unsigned char g{ 255 };
		unsigned char b{ 255 };
		unsigned char a{ 255 };
	};

	struct Colour
	{
		float r{ 1.f };
		float g{ 1.f };
		float b{ 1.f };
		float a{ 1.f };

		/**
		 * @brief Convert to a Vec3 (RGB)
		 */
		Vec3 toVec3() const
		{
			return Vec3{ r, g, b };
		}

		/**
		 * @brief Convert to a Vec4 (RGA)
		 */
		Vec4 toVec4() const
		{
			return Vec4{ r, g, b, a };
		}

		/**
		 * @brief Convert from a Vec3
		 */
		static Colour from(const Vec3& from)
		{
			return Colour{ from.x, from.y, from.z, 1.f };
		}

		/**
		 * @brief Convert from a Vec4
		 */
		static Colour from(const Vec4& from)
		{
			return Colour{ from.x, from.y, from.z, from.w };
		}

		/**
		 * @brief Convert from denormalised colour
		 */
		static Colour from(const ColourDenorm& from)
		{
			return { from.r / 255.f, from.g / 255.f, from.b / 255.f, from.a / 255.f };
		}

		ColourDenorm denormalised() const
		{
			return ColourDenorm{
				static_cast<unsigned char>(r * 255),
				static_cast<unsigned char>(g * 255),
				static_cast<unsigned char>(b * 255),
				static_cast<unsigned char>(a * 255)
			};
		}

		/**
		 * @brief Convert to a hex string
		 */
		std::string toHex() const
		{
			std::stringstream ss;
			auto denorm = denormalised();

			ss << "#"
				<< std::setw(2) << std::setfill('0') << std::hex << denorm.r
				<< std::setw(2) << std::setfill('0') << std::hex << denorm.g
				<< std::setw(2) << std::setfill('0') << std::hex << denorm.b
				<< std::setw(2) << std::setfill('0') << std::hex << denorm.a;
			return ss.str();
		}

		/**
		 * @brief Get the luminance of this colour
		 *
		 * Luminance formula based on WCAG 2.0
		 */
		float luminance() const
		{

			Colour lum(*this);

			lum.r = (lum.r <= 0.03928f) ? lum.r / 12.92f : pow((lum.r + .055f) / 1.055f, 2.4f);
			lum.g = (lum.g <= 0.03928f) ? lum.g / 12.92f : pow((lum.g + .055f) / 1.055f, 2.4f);
			lum.b = (lum.b <= 0.03928f) ? lum.b / 12.92f : pow((lum.b + .055f) / 1.055f, 2.4f);

			return 0.2126f * lum.r + 0.7152f * lum.g + 0.0722f * lum.b;
		}

		/**
		 * @brief Get contrast ratio with another col
		 */
		float contrastRatio(const Colour& otherCol) const
		{
			float luminance1 = luminance();
			float luminance2 = otherCol.luminance();

			float light = std::max(luminance1, luminance2) + 0.05f;
			float dark = std::min(luminance1, luminance2) + 0.05f;

			return light / dark;
		}

		/**
		 * @brief Determine if the contrast with another colour exceeds a threshold
		 */
		bool isContrastSufficient(const Colour& otherCol, float threshold = 4.5f) const
		{
			return contrastRatio(otherCol) >= threshold;
		}

		constexpr Colour() = default;
		constexpr Colour(float r, float g, float b) : Colour(r, g, b, 1.f) {}
		constexpr Colour(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
	};


	constexpr wf::Colour BLANK{ 0.f, 0.f, 0.f, 0.f };
	constexpr wf::Colour BLACK{ 0.f, 0.f, 0.f, 1.f };
	constexpr wf::Colour WHITE{ 1.f, 1.f, 1.f, 1.f };

	constexpr wf::Colour RED{ 0.90f, 0.16f, 0.22f, 1.f };
	constexpr wf::Colour ORANGE{ 1.f, 0.63f, 0.f, 1.f };
	constexpr wf::Colour YELLOW{ 0.99f, 0.98f, 0.f, 1.f };
	constexpr wf::Colour GREEN{ 0.f, 0.89f, 0.19f, 1.f };
	constexpr wf::Colour BLUE{ 0.f, 0.47f, 0.95f, 1.f };
	constexpr wf::Colour PURPLE{ 0.78f, 0.48f, 1.f, 1.f };

	constexpr wf::Colour GREY{ 0.51f, 0.51f, 0.51f, 1.f };
	constexpr wf::Colour PINK{ 1.f, 0.43f, 0.76f, 1.f };
	constexpr wf::Colour VIOLET{ 0.53f, 0.24f, 0.75f, 1.f };
	constexpr wf::Colour LIME{ 0.f, 0.62f, 0.18f, 1.f };
	constexpr wf::Colour BEIGE{ 0.83f, 0.69f, 0.51f, 1.f };
	constexpr wf::Colour BROWN{ 0.50f, 0.42f, 0.31f, 1.f };
	constexpr wf::Colour MAGENTA{ 1.f, 0.f, 1.f, 1.f };
	constexpr wf::Colour EGGSHELL{ 0.94f, 0.92f, 0.84f, 1.0f };
	constexpr wf::Colour GOLD{ 1.f, 0.80f, 0.f, 1.f };
	constexpr wf::Colour MAROON{ 0.75f, 0.13f, 0.22f, 1.f };

	constexpr wf::Colour LIGHTGREY{ 0.78f, 0.78f, 0.78f, 1.f };
	constexpr wf::Colour LIGHTBLUE{ 0.40f, 0.75f, 1.f, 1.f };
	constexpr wf::Colour DARKGREY{ 0.31f, 0.31f, 0.31f, 1.f };
	constexpr wf::Colour DARKGREEN{ 0.f, 0.46f, 0.17f, 1.f };
	constexpr wf::Colour DARKBLUE{ 0.f, 0.32f, 0.67f, 1.f };
	constexpr wf::Colour DARKPURPLE{ 0.44f, 0.12f, 0.49f, 1.f };
	constexpr wf::Colour DARKBROWN{ 0.30f, 0.25f, 0.18f, 1.f };
	constexpr wf::Colour OFFBLACK{ 0.1f, 0.1f, 0.1f, 1.f };

	static constexpr wf::Colour CUTTINGMAT{ 0.19f, 0.39f, 0.25f, 1.f };
}

