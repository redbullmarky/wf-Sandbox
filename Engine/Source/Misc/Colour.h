#pragma once
#include "Math/Math.h"

namespace wf
{
	struct Colour
	{
		float r{ 1.f };
		float g{ 1.f };
		float b{ 1.f };
		float a{ 1.f };

		Vec3 toVec3() const
		{
			return Vec3{ r, g, b };
		}

		Vec4 toVec4() const
		{
			return Vec4{ r, g,b,a };
		}

		static Colour from(const Vec3& from)
		{
			return Colour{ from.x, from.y, from.z, 1.f };
		}

		static Colour from(const Vec4& from)
		{
			return Colour{ from.x, from.y, from.z, from.w };
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

