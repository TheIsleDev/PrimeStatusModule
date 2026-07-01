#include <string>

#include <Mod/CppUserModBase.hpp>

#include "handler.cpp"

class PrimeStatus : public RC::CppUserModBase {
private:
	int last_tick_attempt = 0;
	int current_tick = 0;
	const int tick_delay = 1200;

public:
	PrimeStatus() : CppUserModBase() {
		ModName = STR("PrimeStatus");
		ModVersion = STR("1.0");
		ModDescription = STR("Tell players when they do something to prime tasks related");
		ModAuthors = STR("Shiza");
	}

	auto on_unreal_init() -> void override {
		PrimeStatusModule::Initialize();
	}

	// Maybe I should do on gametick it or even better, intrusive game exe edit, so I won't even have to do on tick and just wait for calls on updates via callback
	auto on_update() -> void override {
		if (++current_tick < last_tick_attempt + tick_delay) return;
		last_tick_attempt = current_tick;
		PrimeStatusModule::Fire();
	}
};

#define KISMET_DEBUGGER_MOD_API __declspec(dllexport)
extern "C" {
	KISMET_DEBUGGER_MOD_API RC::CppUserModBase* start_mod() {
		return new PrimeStatus();
	}

	KISMET_DEBUGGER_MOD_API void uninstall_mod(RC::CppUserModBase* mod) {
		delete mod;
	}
}