#include <string>

#include <Mod/CppUserModBase.hpp>

#include "handler.cpp"

class PrimeStatus : public RC::CppUserModBase {
private:
	int TicksFired{0};
	static constexpr int PerTicksFired{720};// 120 per sec, 30 game ticks

public:
	PrimeStatus() : CppUserModBase() {
		ModName = STR("PrimeStatus");
		ModVersion = STR("1.0");
		ModDescription = STR("Tell players when they do something to prime tasks related");
		ModAuthors = STR("Shiza");
	}

	auto on_unreal_init() -> void override {
		PrimeStatusComponent::Initialize();
	}

	// Maybe I should do on gametick it or even better, intrusive game exe edit, so I won't even have to do on tick and just wait for calls on updates via callback
	auto on_update() -> void override {
		if (++TicksFired < PerTicksFired) return;
		TicksFired = 0;

		PrimeStatusComponent::Fire();
	}
};

#define MOD_API __declspec(dllexport)
extern "C" {
	MOD_API RC::CppUserModBase* start_mod() {
		return new PrimeStatus();
	}

	MOD_API void uninstall_mod(RC::CppUserModBase* mod) {
		delete mod;
	}
}