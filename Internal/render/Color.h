#pragma once

/// @file
/// @brief Render::Color — a backend-neutral RGBA color (0-1 floats). Like Render::Vec2, the core has
/// zero dependency on ImGui or the UE SDK; conversions to/from a platform color type (FLinearColor,
/// ImVec4, the settings ::Color, ...) are added out-of-line by specializing the ColorTraits
/// customization point in an adapter header (see render/adapters/). To support a new backend you
/// write one adapter — the core is never edited.

#include <type_traits>
#include <utility>

namespace Render
{
	/// Customization point: specialize `ColorTraits<T>` in an adapter header to bridge a platform
	/// color type @p T (e.g. FLinearColor, ImVec4, ::Color) to/from Render::Color. The core declares
	/// it but defines no specialization.
	///
	/// A specialization provides `static Color From(const T&)` and (when a reverse conversion is
	/// wanted) `static T To(const Color&)`.
	template <class T>
	struct ColorTraits;

	/// An RGBA color with 0-1 float components. Neutral currency of the render API.
	struct Color
	{
		float r = 1.f; ///< red, 0-1
		float g = 1.f; ///< green, 0-1
		float b = 1.f; ///< blue, 0-1
		float a = 1.f; ///< alpha, 0-1

		constexpr Color() = default;
		constexpr Color(float r, float g, float b, float a = 1.f) : r(r), g(g), b(b), a(a) {}

		/// Implicit inbound conversion from any platform color @p T that has a `ColorTraits<T>`
		/// specialization in scope. Lets call sites pass an FLinearColor / ImVec4 / settings ::Color
		/// straight into the render API; SFINAE removes this ctor for types without a specialization.
		template <class T, class = decltype(ColorTraits<std::decay_t<T>>::From(std::declval<const T&>()))>
		Color(const T& c) : Color(ColorTraits<std::decay_t<T>>::From(c))
		{
		}

		/// Explicit outbound conversion to a platform color, e.g. `c.To<FLinearColor>()` /
		/// `c.To<ImVec4>()`. Requires a `ColorTraits<T>` specialization in scope.
		template <class T>
		T To() const
		{
			return ColorTraits<T>::To(*this);
		}

		/// A copy with the alpha replaced — handy for a palette color reused at several opacities.
		constexpr Color Alpha(float na) const { return {r, g, b, na}; }
	};
} // namespace Render
