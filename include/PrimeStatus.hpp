
#pragma once

#include <string>
#include <Mod/CppUserModBase.hpp>
#include <Subsystem.hpp>


struct PrimeStatusConfig {
};

class PrimeStatusSystem : public RC::CppUserModBase {
private:
	PrimeStatusConfig Config{};

public:
	StatusSubsystem* TickingStatus{};

    PrimeStatusSystem();
	~PrimeStatusSystem() override;

	void on_unreal_init() override;
};
