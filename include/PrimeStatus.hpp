
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
	std::unique_ptr<StatusSubsystem> TickingStatus{};

    PrimeStatusSystem();
	~PrimeStatusSystem() override;

	void on_unreal_init() override;
};
