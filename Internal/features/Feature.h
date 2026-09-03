#pragma once

/// @file
/// Defines the Feature base class: the abstract interface every gameplay
/// feature implements plus the lifecycle flags Features::RunFeature uses to
/// drive it. See the class comment below for the full Init/Check/Run/Destroy
/// contract.

#include "../settings/Settings.h"
#include "../utils/Logger.h"
#include "../scripting/Events.h"

// Base class for a gameplay feature. Features::Execute drives every registered
// feature once per rendered frame:
//
//   UpdateEnabled();               // refresh Enabled from settings
//   if (!Initialized) Init();
//   if (!Check()) continue;        // Check() == "is it valid/safe to act right now?"
//   if (Enabled) Run();            // once, if OneTime
//   else if (applied) Destroy();   // revert, once, on the enabled -> disabled edge
//
// Contract for subclasses:
//   Check()   - return whether it is currently valid to act (pointers ready, in
//               game, ...). Do NOT gate this on Enabled and do NOT return Enabled;
//               the loop decides Run vs Destroy. (Legacy features that still
//               `return Enabled` keep working, but their Destroy() never runs.)
//   Run()     - apply the effect. Called every frame while enabled, or once per
//               enable if OneTime is set.
//   Destroy() - revert whatever Run() applied. Called once when the feature goes
//               enabled -> disabled while still valid.
class Feature
{
  public:
	bool Enabled = false;	  // toggled from settings via UpdateEnabled()
	bool Initialized = false; ///< set true by Init() once setup succeeds; features guard on this
	bool OneTime = false;	  // Run() fires once per enable instead of every frame

	std::string Name = "BaseFeature"; ///< human-readable id, used in Log() output
	// Which event drives this feature. Render runs every frame (the fast loop in
	// Features::Execute); any other event subscribes it to the event bus and runs
	// it when that event is dispatched (e.g. Events::Type::PlayerDeath).
	Events::Type Event = Events::Type::Render;

	// Bookkeeping owned by Features::Execute; subclasses should not touch these.
	bool applied = false; // Run() has been applied and not yet reverted by Destroy()
	bool hasRun = false;  // a OneTime feature has already run this enable cycle

	Feature() {};

	/// One-time setup (resolve game objects, cache originals). Must set
	/// Initialized to reflect success; the runner calls it on first use.
	virtual void Init() = 0;
	/// Refresh Enabled from the backing setting. Called at registration and on
	/// every Events::Type::SettingsChanged, so features never poll per frame.
	virtual void UpdateEnabled() = 0;
	/// Whether it is currently valid to act (pointers ready, in game, ...).
	/// Must NOT gate on or return Enabled; the runner decides Run vs Destroy.
	virtual bool Check() = 0;
	/// Revert whatever Run() applied. Called once on the enabled -> disabled edge.
	virtual void Destroy() = 0;
	/// Apply the effect. Called every frame while enabled (or once per enable if OneTime).
	virtual void Run() = 0;

	/// Emit a "[Name] message" line, but only when Settings.DEBUG.FeaturesLogging is on.
	void Log(std::string message)
	{
		if (!Settings.DEBUG.FeaturesLogging) return;
		Logger::Log("FEATURE", std::format("[{}] {}", Name, message));
	};
};
