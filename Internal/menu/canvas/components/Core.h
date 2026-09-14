#pragma once

/// @file
/// @brief UCanvasGUI core: the shared immediate-mode foundation for the UE-canvas menu backend — the
/// deferred draw queue (PostRenderer), the single-window layout state, the Render:: draw primitives
/// (routed to whichever backend is active — canvas or the ImGui recorder), the software cursor, the
/// layout-cursor helpers (SameLine/PushNextElementY/NextPos/
/// EndElement), and the frame-end Render() drain. Every widget component in this folder includes it.
/// Everything is `inline` — this header is pulled into multiple translation units.

#include <Windows.h>
#include <string>
#include <cstdio>
#include <cstring>
#include <cmath>

#include "../../../utils/Input.h"
#include "../../../ue/Engine.h"
#include "../../../render/Render.h"
#include "../Colors.h"

namespace UCanvasGUI
{
	/// Deferred draw queue: pop-ups (combo dropdowns, color-picker swatches) enqueue here so they
	/// replay last, on top of the widgets drawn earlier in the frame. Render() drains it.
	namespace PostRenderer
	{
		/// One queued primitive. @ref kind selects which fields are meaningful.
		struct Command
		{
			enum Kind
			{
				None = -1,
				Rect,
				TextLeft,
				TextCenter,
				Line
			};

			Kind kind = None;
			FVector2D pos;
			FVector2D size;
			FLinearColor color;
			const char* text;
			bool outline;

			FVector2D from;
			FVector2D to;
			int thickness;
		};

		inline constexpr int Capacity = 128;
		inline Command queue[Capacity];

		/// Reserve the next free slot, or nullptr when the queue is full.
		inline Command* Next()
		{
			for (Command& cmd : queue)
				if (cmd.kind == Command::None)
					return &cmd;
			return nullptr;
		}

		inline void DrawRect(FVector2D pos, float w, float h, FLinearColor color)
		{
			if (Command* cmd = Next())
				*cmd = {Command::Rect, pos, FVector2D{w, h}, color};
		}
		inline void TextLeft(const char* text, FVector2D pos, FLinearColor color, bool outline)
		{
			if (Command* cmd = Next())
			{
				cmd->kind = Command::TextLeft;
				cmd->text = text;
				cmd->pos = pos;
				cmd->outline = outline;
				cmd->color = color;
			}
		}
		inline void TextCenter(const char* text, FVector2D pos, FLinearColor color, bool outline)
		{
			if (Command* cmd = Next())
			{
				cmd->kind = Command::TextCenter;
				cmd->text = text;
				cmd->pos = pos;
				cmd->outline = outline;
				cmd->color = color;
			}
		}
		inline void DrawLine(FVector2D from, FVector2D to, int thickness, FLinearColor color)
		{
			if (Command* cmd = Next())
			{
				cmd->kind = Command::Line;
				cmd->from = from;
				cmd->to = to;
				cmd->thickness = thickness;
				cmd->color = color;
			}
		}
	} // namespace PostRenderer

	// --- Immediate-mode layout state (single window, rebuilt every frame). ---
	inline bool elementHovered = false;	 ///< set when the cursor is over a widget (suppresses window drag)
	inline FVector2D menuPos = {0, 0};	 ///< top-left of the window this frame
	inline float offsetX = 0.0f;		 ///< running layout cursor, relative to menuPos
	inline float offsetY = 0.0f;		 ///< running layout cursor, relative to menuPos

	inline FVector2D lastElementPos = {0, 0};  ///< last widget's position (for SameLine)
	inline FVector2D lastElementSize = {0, 0}; ///< last widget's size (for SameLine)

	inline int activeElement = -1; ///< elementCount of the widget currently being dragged (-1 = none)
	inline int elementCount = 0;   ///< per-frame widget counter, doubles as each widget's id

	inline bool sameLine = false; ///< when set, the next widget is placed to the right of the last

	inline bool pushY = false;		///< when set, the next widget's Y is forced to pushYValue
	inline float pushYValue = 0.0f; ///< absolute Y forced by PushNextElementY

	/// The cursor position in client (window) pixels.
	inline FVector2D CursorPos()
	{
		POINT cursor;
		GetCursorPos(&cursor);
		ScreenToClient(GetActiveWindow(), &cursor);
		return FVector2D{(float)cursor.x, (float)cursor.y};
	}

	/// Whether the cursor is inside the [pos, pos + size] rectangle.
	inline bool MouseInZone(FVector2D pos, FVector2D size)
	{
		FVector2D cursor = CursorPos();
		return cursor.X > pos.X && cursor.Y > pos.Y && cursor.X < pos.X + size.X && cursor.Y < pos.Y + size.Y;
	}

	// --- Draw primitives (draw through the active Render backend, not the canvas directly). ---
	inline void DrawLine(FVector2D from, FVector2D to, int thickness, FLinearColor color)
	{
		Render::Line(from, to, (float)thickness, color);
	}
	inline void DrawRect(FVector2D pos, float w, float h, FLinearColor color)
	{
		Render::RectFilled(pos, FVector2D{pos.X + w, pos.Y + h}, color);
	}
	inline void DrawCircle(FVector2D pos, float radius, FLinearColor color)
	{
		Render::CircleFilled(pos, radius, color);
	}
	inline void TextLeft(const char* text, FVector2D pos, FLinearColor color, bool /*outline*/)
	{
		Render::Text(pos, std::string(text), 0.97f, color, false);
	}
	inline void TextCenter(const char* text, FVector2D pos, FLinearColor color, bool /*outline*/)
	{
		Render::Text(pos, std::string(text), 0.97f, color, true);
	}

	/// Software arrow cursor built from line segments (the Canvas menu owns its cursor; it does not
	/// borrow ImGui's software cursor, keeping the two backends fully decoupled).
	inline void DrawCursor()
	{
		constexpr FLinearColor tint{0.30f, 0.30f, 0.80f, 1.0f};
		const FVector2D origin = CursorPos();

		// Fan of segments from the tip, sweeping the arrow's inner edge.
		for (int x = 35, y = 10; y <= 30; x = x > 15 ? x - 1 : 15, y++)
			DrawLine(origin, FVector2D{origin.X + x, origin.Y + y}, 1, tint);

		DrawLine(FVector2D{origin.X + 35, origin.Y + 10}, FVector2D{origin.X + 15, origin.Y + 30}, 1, tint);
	}

	// --- Layout cursor helpers. ---
	inline void SameLine()
	{
		sameLine = true;
	}
	inline void PushNextElementY(float y, bool fromLastElement = true)
	{
		pushY = true;
		pushYValue = fromLastElement ? lastElementPos.Y + lastElementSize.Y + y : y;
	}

	/// Resolve the next widget's top-left, honoring SameLine / PushNextElementY, and (unless SameLine)
	/// advance the vertical cursor by @p advance. @p sameLineY offsets the Y when placed on the same
	/// line (used by widgets that vertically center against the previous element).
	inline FVector2D NextPos(FVector2D padding, float advance, float sameLineY = 0.0f)
	{
		FVector2D pos{menuPos.X + padding.X + offsetX, menuPos.Y + padding.Y + offsetY};
		if (sameLine)
		{
			pos.X = lastElementPos.X + lastElementSize.X + padding.X;
			pos.Y = lastElementPos.Y + sameLineY;
		}
		if (pushY)
		{
			pos.Y = pushYValue;
			pushY = false;
			pushYValue = 0.0f;
			offsetY = pos.Y - menuPos.Y;
		}
		if (!sameLine)
			offsetY += advance;
		return pos;
	}

	/// Close the current widget, recording its position (and, in the two-arg form, its size) for the
	/// next SameLine, and clearing the SameLine request.
	inline void EndElement(FVector2D pos)
	{
		sameLine = false;
		lastElementPos = pos;
	}
	inline void EndElement(FVector2D pos, FVector2D size)
	{
		lastElementSize = size;
		EndElement(pos);
	}

	/// Drain the deferred draw queue (dropdowns / swatches) so they land on top. Call at frame end.
	inline void Render()
	{
		for (PostRenderer::Command& cmd : PostRenderer::queue)
		{
			switch (cmd.kind)
			{
			case PostRenderer::Command::Rect:
				DrawRect(cmd.pos, cmd.size.X, cmd.size.Y, cmd.color);
				break;
			case PostRenderer::Command::TextLeft:
				TextLeft(cmd.text, cmd.pos, cmd.color, cmd.outline);
				break;
			case PostRenderer::Command::TextCenter:
				TextCenter(cmd.text, cmd.pos, cmd.color, cmd.outline);
				break;
			case PostRenderer::Command::Line:
				DrawLine(cmd.from, cmd.to, cmd.thickness, cmd.color);
				break;
			default:
				continue;
			}
			cmd.kind = PostRenderer::Command::None;
		}
	}
} // namespace UCanvasGUI
