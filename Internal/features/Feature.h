#pragma once

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
	bool Enabled = false;      // toggled from settings via UpdateEnabled()
	bool Initialized = false;
	bool OneTime = false;      // Run() fires once per enable instead of every frame

	std::string Name = "BaseFeature";
	// Which event drives this feature. Render runs every frame (the fast loop in
	// Features::Execute); any other event subscribes it to the event bus and runs
	// it when that event is dispatched (e.g. Events::Type::PlayerDeath).
	Events::Type Event = Events::Type::Render;

	// Bookkeeping owned by Features::Execute; subclasses should not touch these.
	bool applied = false;      // Run() has been applied and not yet reverted by Destroy()
	bool hasRun = false;       // a OneTime feature has already run this enable cycle

	Feature() {};

	virtual void Init() = 0;
	virtual void UpdateEnabled() = 0;
	virtual bool Check() = 0;
	virtual void Destroy() = 0;
	virtual void Run() = 0;

	void Log(std::string message)
	{
		if (!Settings.DEBUG.FeaturesLogging) return;
		Logger::Log("FEATURE", std::format("[{}] {}", Name, message));
	};
};
