
// Declare local debugging if you want more logging on test run
//#define LOCAL_DEBUGGING

#include <TheIsleHelpers/ConfigReader.hpp>
#include <PrimeStatus.hpp>


PrimeStatusSystem::PrimeStatusSystem() {
	ModName = STR("PrimeStatus");
	ModVersion = STR("1.0.2");
	ModDescription = STR("Hehe");
	ModAuthors = STR("Shiza");

	RC::ConfigLoader::LoadModConfig(&Config);
}

PrimeStatusSystem::~PrimeStatusSystem() {
}


void PrimeStatusSystem::on_unreal_init() {
	static StatusSubsystem Ticker{};
	TickingStatus = &Ticker;
}


#define MOD_API __declspec(dllexport)
extern "C" {
	MOD_API RC::CppUserModBase* start_mod() {
		return new PrimeStatusSystem();
	}

	MOD_API void uninstall_mod(RC::CppUserModBase* mod) {
		delete mod;
	}
}
