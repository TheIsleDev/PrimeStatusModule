
#pragma once

#include <Unreal/FString.hpp>
#include <Unreal/Hooks/Hooks.hpp>
#include <Unreal/FText.hpp>
#include <Unreal/AActor.hpp>
#include <Unreal/UObject.hpp>
#include <Unreal/UObjectArray.hpp>
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/CoreUObject/UObject/UnrealType.hpp>
#include <Unreal/Core/Containers/ContainerAllocationPolicies.hpp>
#include <TheIsle/_simple_structs.hpp>
#include <TheIsle/ATIPlayerController.hpp>
#include <TheIsle/ATIGameModeBase.hpp>


using namespace RC::Unreal;
using namespace RC::Unreal::TheIsle;

struct FEligiblePrimeElderHolder {
    int32 DinoID;
    FEligiblePrimeElder Holder;
};

struct ConditionField {
    bool FEligiblePrimeElder::* Member;
    const RC::StringType Name;
};

class StatusSubsystem {
private:
	/// How often it fires
	int TicksFired = 0;
	static constexpr int TickRate{10};

	static const ConditionField ConditionFields[];

	Hook::GlobalCallbackId FireCallBackID{};

	using CachedPrimeData = TMap<FString, FEligiblePrimeElderHolder>;
    CachedPrimeData Cached;

public:
	StatusSubsystem();
	~StatusSubsystem();

	void Tick(float DeltaSeconds, bool bIdleMode);
	void NotifyPrimeConditionDiff(ATIPlayerController* Player, const FEligiblePrimeElder& Old, const FEligiblePrimeElder& New);
};
