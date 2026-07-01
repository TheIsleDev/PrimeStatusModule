#pragma once


#include <SDKGenerator/UEHeaderGenerator.hpp>

#include <String/StringType.hpp>
#include <Unreal/AActor.hpp>
#include <Unreal/UObject.hpp>
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/CoreUObject/UObject/UnrealType.hpp>
#include <Unreal/Core/Containers/ContainerAllocationPolicies.hpp>

#include "Containers/FString.hpp"
#include "Structs/TheIsleStructs.hpp"

using namespace RC::Unreal;

struct ConditionField {
	bool IsleStructs::FEligiblePrimeElder::* Member;
	const wchar_t* Name;// StringType maybe, I made it when didn't realized I can use it.
};

namespace PrimeStatusModule {
	static UClass* GameModeBaseClass = nullptr;
	static FProperty* GameModeAllPlayers = nullptr;

	static UClass* DinoClass = nullptr;
	static FProperty* DinoPrimeDataProp = nullptr;
	static FProperty* DinoIDProp = nullptr;
	static FProperty* DinoPlayerControllerProp = nullptr;

	static UFunction* ClientNotifyFunc = nullptr;

	static const ConditionField Fields[] = {
		{&IsleStructs::FEligiblePrimeElder::bPrimeCondition1, STR("Visit a Sanctuary")},
		{&IsleStructs::FEligiblePrimeElder::bPrimeCondition2, STR("Get Nested In")},
		{&IsleStructs::FEligiblePrimeElder::bPrimeCondition3, STR("Get Perfect Diet")},
		{&IsleStructs::FEligiblePrimeElder::bPrimeCondition4, STR("Visit Mass Migration Zone")},
		{&IsleStructs::FEligiblePrimeElder::bPrimeCondition5, STR("Visit 2 Migration Zones")},
		{&IsleStructs::FEligiblePrimeElder::bPrimeCondition6, STR("Visit 4 Patrol Zones")},
		{&IsleStructs::FEligiblePrimeElder::bPrimeCondition7, STR("Never be infertile")},
		{&IsleStructs::FEligiblePrimeElder::bPrimeCondition8, STR("Never get muscle spasms")},
		{&IsleStructs::FEligiblePrimeElder::bPrimeCondition9, STR("Raise children to subadult")},
		{&IsleStructs::FEligiblePrimeElder::bPrimeCondition10, STR("Be a Hypsi, Troodon, Beipi, Dryo or Deino.")},
		{&IsleStructs::FEligiblePrimeElder::bIsEligiblePrime, STR("Prime Eligibility")},
	};

	static TMap<int32, IsleStructs::FEligiblePrimeElder> Cached;

	auto NotifyPrimeConditionDiff(
		IsleStructs::ATIDinosaurBase* Dino,
		IsleStructs::ATIPlayerController* PC,
		const IsleStructs::FEligiblePrimeElder& Old,
		const IsleStructs::FEligiblePrimeElder& New
	) -> void {
		using namespace RC::Unreal;

		for (const auto& Field : Fields) {
			bool OldVal = Old.*Field.Member;
			bool NewVal = New.*Field.Member;
			if (OldVal == NewVal) continue;

			auto MessageStr = fmt::format(STR("Your prime task status changed [{}: {}]"), Field.Name, NewVal);
			IsleStructs::FClientShowNotificationParams Notif{FText(MessageStr)};
			PC->ProcessEvent(ClientNotifyFunc, &Notif);
		}
	}

	auto Fire() -> void {
		auto* GameMode = UObjectGlobals::FindFirstOf(STR("BP_SurvivalGameMode_C"));
		if (!GameMode) return;

		TMap<int32, IsleStructs::FEligiblePrimeElder> NewCached;
		TArray<IsleStructs::ATIDinosaurBase*>* ActiveDinos = GameModeAllPlayers->ContainerPtrToValuePtr<TArray<IsleStructs::ATIDinosaurBase*>>(GameMode);
		for (IsleStructs::ATIDinosaurBase* Dino : *ActiveDinos) {
			if (!Dino || !Dino->IsA(DinoClass)) continue;

			int32 DinoId = *DinoIDProp->ContainerPtrToValuePtr<int32>(Dino);
			IsleStructs::FEligiblePrimeElder& NewData = *DinoPrimeDataProp->ContainerPtrToValuePtr<IsleStructs::FEligiblePrimeElder>(Dino);
			if (!Cached.Contains(DinoId)) {
				NewCached.Add(DinoId, NewData);
				continue;
			}

			IsleStructs::FEligiblePrimeElder OldData = *Cached.Find(DinoId);
			IsleStructs::ATIPlayerController* PlayerControllerPtr = *DinoPlayerControllerProp->ContainerPtrToValuePtr<IsleStructs::ATIPlayerController*>(Dino);
			if (!PlayerControllerPtr) {
				NewCached.Add(DinoId, OldData);
				continue;
			};

			NewCached.Add(DinoId, OldData);
			if (!std::memcmp(&OldData, &NewData, sizeof(IsleStructs::FEligiblePrimeElder))) continue;

			NotifyPrimeConditionDiff(Dino, PlayerControllerPtr, OldData, NewData);
			NewCached.Add(DinoId, NewData);
		}
		Cached = NewCached;
	}

	auto Initialize() -> void {
		GameModeBaseClass = UObjectGlobals::StaticFindObject<UClass*>(nullptr, nullptr, STR("/Script/TheIsle.TIGameModeBase"));
		GameModeAllPlayers = GameModeBaseClass->GetPropertyByNameInChain(STR("AllPlayerCharacters"));

		DinoClass = UObjectGlobals::StaticFindObject<UClass*>(nullptr, nullptr, STR("/Script/TheIsle.TIDinosaurBase"));
		DinoPrimeDataProp = DinoClass->GetPropertyByNameInChain(STR("EligiblePrimeElderData"));
		DinoIDProp = DinoClass->GetPropertyByNameInChain(STR("ID"));
		DinoPlayerControllerProp = DinoClass->GetPropertyByNameInChain(STR("PlayerController"));

		ClientNotifyFunc = UObjectGlobals::StaticFindObject<UFunction*>(nullptr, nullptr, STR("/Script/TheIsle.TIPlayerController:ClientShowNotification"));
	}
}