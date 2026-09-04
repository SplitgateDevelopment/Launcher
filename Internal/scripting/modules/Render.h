#pragma once

#include <cmath>
#include <string>

#include <pybind11/embed.h>

#include "../../ue/Engine.h"
#include "../../render/Render.h"
#include "../../utils/WorldToScreen.h"
#include "Actors.h" // PlayerInfo + ResolveLive, for skeleton()

/**
 * @file
 * @brief pybind11 module letting user scripts draw through the active render backend as
 * `SplitgateInternal.Render.*` (line/text/circle in screen space, plus world_line/world_text that
 * project 3D points). Call these from a handler subscribed to `Events.Render` (dispatched each
 * frame by the UserScripts feature) so drawing lands in the current frame.
 */

namespace py = pybind11;

namespace Scripts
{
	namespace Modules
	{
		/// Parse an (r, g, b[, a]) 0-1 sequence into an FLinearColor (defaults to opaque white).
		inline FLinearColor ParseColor(const py::object& color)
		{
			FLinearColor c{1.f, 1.f, 1.f, 1.f};
			if (color.is_none()) return c;
			auto seq = color.cast<py::sequence>();
			const size_t n = py::len(seq);
			if (n > 0) c.R = seq[0].cast<float>();
			if (n > 1) c.G = seq[1].cast<float>();
			if (n > 2) c.B = seq[2].cast<float>();
			if (n > 3) c.A = seq[3].cast<float>();
			return c;
		}

		/// Registers the `Render` submodule.
		void Render(py::module_& m)
		{
			auto r = m.def_submodule("Render");

			r.def("line", [](float x1, float y1, float x2, float y2, py::object color, float thickness)
				  { ::Render::Line(FVector2D{x1, y1}, FVector2D{x2, y2}, thickness, ParseColor(color)); },
				  py::arg("x1"), py::arg("y1"), py::arg("x2"), py::arg("y2"), py::arg("color") = py::none(), py::arg("thickness") = 1.f);

			r.def("text", [](float x, float y, std::string text, py::object color, float scale)
				  { ::Render::Text(FVector2D{x, y}, text, scale, ParseColor(color)); },
				  py::arg("x"), py::arg("y"), py::arg("text"), py::arg("color") = py::none(), py::arg("scale") = 1.f);

			r.def("circle", [](float x, float y, float radius, py::object color, int segments, float thickness)
				  {
				const FLinearColor c = ParseColor(color);
				if (segments < 3) segments = 3;
				constexpr float twoPi = 6.28318530718f;
				FVector2D prev{x + radius, y};
				for (int i = 1; i <= segments; i++)
				{
					const float a = twoPi * i / segments;
					FVector2D cur{x + radius * std::cos(a), y + radius * std::sin(a)};
					::Render::Line(prev, cur, thickness, c);
					prev = cur;
				} },
				  py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("color") = py::none(), py::arg("segments") = 32, py::arg("thickness") = 1.f);

			// Project two world points and draw a line between them; skipped if either is off-screen.
			r.def("world_line", [](float x1, float y1, float z1, float x2, float y2, float z2, py::object color, float thickness) -> bool
				  {
				FVector2D a{}, b{};
				if (!Projection::WorldToScreen(FVector{x1, y1, z1}, a)) return false;
				if (!Projection::WorldToScreen(FVector{x2, y2, z2}, b)) return false;
				::Render::Line(a, b, thickness, ParseColor(color));
				return true; },
				  py::arg("x1"), py::arg("y1"), py::arg("z1"), py::arg("x2"), py::arg("y2"), py::arg("z2"), py::arg("color") = py::none(), py::arg("thickness") = 1.f);

			// Project a world point and draw text there; returns whether it was on-screen.
			r.def("world_text", [](float x, float y, float z, std::string text, py::object color, float scale) -> bool
				  {
				FVector2D p{};
				if (!Projection::WorldToScreen(FVector{x, y, z}, p)) return false;
				::Render::Text(p, text, scale, ParseColor(color));
				return true; },
				  py::arg("x"), py::arg("y"), py::arg("z"), py::arg("text"), py::arg("color") = py::none(), py::arg("scale") = 1.f);

			// Rectangle outline (x, y = top-left).
			r.def("rect", [](float x, float y, float w, float h, py::object color, float thickness)
				  {
				const FLinearColor c = ParseColor(color);
				const FVector2D tl{x, y}, tr{x + w, y}, br{x + w, y + h}, bl{x, y + h};
				::Render::Line(tl, tr, thickness, c);
				::Render::Line(tr, br, thickness, c);
				::Render::Line(br, bl, thickness, c);
				::Render::Line(bl, tl, thickness, c); },
				  py::arg("x"), py::arg("y"), py::arg("w"), py::arg("h"), py::arg("color") = py::none(), py::arg("thickness") = 1.f);

			// Filled rectangle (x, y = top-left). Fast on the ImGui renderer; approximated with
			// horizontal lines on the UE canvas.
			r.def("rect_filled", [](float x, float y, float w, float h, py::object color)
				  { ::Render::RectFilled(FVector2D{x, y}, FVector2D{x + w, y + h}, ParseColor(color)); },
				  py::arg("x"), py::arg("y"), py::arg("w"), py::arg("h"), py::arg("color") = py::none());

			r.def("circle_filled", [](float x, float y, float radius, py::object color)
				  { ::Render::CircleFilled(FVector2D{x, y}, radius, ParseColor(color)); },
				  py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("color") = py::none());

			// Vertical gradient rect (x, y = top-left): top_color at the top edge, bottom_color below.
			r.def("rect_gradient", [](float x, float y, float w, float h, py::object top_color, py::object bottom_color)
				  { ::Render::RectGradient(FVector2D{x, y}, FVector2D{x + w, y + h}, ParseColor(top_color), ParseColor(bottom_color)); },
				  py::arg("x"), py::arg("y"), py::arg("w"), py::arg("h"), py::arg("top_color") = py::none(), py::arg("bottom_color") = py::none());

			// One-call bone skeleton for a player snapshot (resolves it live; call players() first).
			r.def("skeleton", [](const PlayerInfo& player, py::object color, float thickness) -> bool
				  {
				auto* c = ResolveLive(player.address);
				if (!c || !c->Mesh) return false;
				auto* mesh = c->Mesh;
				const FLinearColor col = ParseColor(color);

				static constexpr int pairs[][2] = {
					{BoneFNames::head, BoneFNames::neck_01}, {BoneFNames::neck_01, BoneFNames::spine_03},
					{BoneFNames::spine_03, BoneFNames::spine_01}, {BoneFNames::spine_01, BoneFNames::pelvis},
					{BoneFNames::spine_03, BoneFNames::upperarm_l}, {BoneFNames::upperarm_l, BoneFNames::lowerarm_l},
					{BoneFNames::lowerarm_l, BoneFNames::hand_l}, {BoneFNames::spine_03, BoneFNames::upperarm_r},
					{BoneFNames::upperarm_r, BoneFNames::lowerarm_r}, {BoneFNames::lowerarm_r, BoneFNames::hand_r},
					{BoneFNames::pelvis, BoneFNames::thigh_l}, {BoneFNames::thigh_l, BoneFNames::calf_l},
					{BoneFNames::calf_l, BoneFNames::foot_l}, {BoneFNames::pelvis, BoneFNames::thigh_r},
					{BoneFNames::thigh_r, BoneFNames::calf_r}, {BoneFNames::calf_r, BoneFNames::foot_r}};

				for (const auto& pair : pairs)
				{
					FVector2D a{}, b{};
					if (!Projection::WorldToScreen(mesh->GetBoneMatrix(pair[0]), a)) continue;
					if (!Projection::WorldToScreen(mesh->GetBoneMatrix(pair[1]), b)) continue;
					::Render::Line(a, b, thickness, col);
				}
				return true; },
				  py::arg("player"), py::arg("color") = py::none(), py::arg("thickness") = 1.f);
		}
	} // namespace Modules
} // namespace Scripts
