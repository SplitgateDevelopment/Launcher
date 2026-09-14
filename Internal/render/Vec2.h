#pragma once

/// @file
/// @brief Render::Vec2 — a backend-neutral 2D position/size. The core carries only two floats and
/// has zero dependency on ImGui or the UE SDK, so it can back a game that uses neither. Conversions
/// to/from a platform vector type (FVector2D, ImVec2, ...) are added out-of-line by specializing the
/// Vec2Traits customization point in an adapter header (see render/adapters/); the core is never
/// edited to gain a new backend.

#include <type_traits>
#include <utility>

namespace Render
{
	/// Customization point: specialize `Vec2Traits<T>` in an adapter header to bridge a platform
	/// 2D-vector type @p T (e.g. FVector2D, ImVec2) to/from Render::Vec2. The core declares it but
	/// defines no specialization, so it depends on nothing.
	///
	/// A specialization provides `static Vec2 From(const T&)` and `static T To(const Vec2&)`.
	template <class T>
	struct Vec2Traits;

	/// A screen-space position or size, in pixels. Neutral currency of the render API.
	struct Vec2
	{
		float x = 0.f; ///< horizontal, pixels
		float y = 0.f; ///< vertical, pixels

		constexpr Vec2() = default;
		constexpr Vec2(float x, float y) : x(x), y(y) {}

		/// Implicit inbound conversion from any platform vector @p T that has a `Vec2Traits<T>`
		/// specialization in scope. Lets call sites pass an FVector2D / ImVec2 straight into the
		/// render API; SFINAE removes this ctor for types without a specialization.
		template <class T, class = decltype(Vec2Traits<std::decay_t<T>>::From(std::declval<const T&>()))>
		Vec2(const T& v) : Vec2(Vec2Traits<std::decay_t<T>>::From(v))
		{
		}

		/// Explicit outbound conversion to a platform vector, e.g. `v.To<FVector2D>()` /
		/// `v.To<ImVec2>()`. Requires a `Vec2Traits<T>` specialization in scope.
		template <class T>
		T To() const
		{
			return Vec2Traits<T>::To(*this);
		}
	};
} // namespace Render
