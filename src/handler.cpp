#include <SDKGenerator/UEHeaderGenerator.hpp>

#include <String/StringType.hpp>
#include <Unreal/AActor.hpp>
#include <Unreal/UObject.hpp>
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/CoreUObject/UObject/UnrealType.hpp>
#include <Unreal/Core/Containers/ContainerAllocationPolicies.hpp>

#include "Containers/FString.hpp"
#include "Structs/TheIsleStructs.hpp"

struct ConditionField {
	bool IsleStructs::FEligiblePrimeElder::* Member;
	const wchar_t* Name;// StringType maybe, I made it when didn't realized I can use it.
};

namespace PrimeStatusComponent {
	using namespace RC::Unreal;

	static UClass* GameModeBaseClass{};
	static FProperty* GameModeAllPlayers{};

	static UClass* PlayerControllerBaseClass{};
	static FProperty* PlayerControllerPawn{};

	static UClass* DinoClass{};
	static FProperty* DinoPrimeDataProp{};
	static FProperty* DinoIDProp{};

	static UFunction* ClientNotifyFunc{};

	static UObject* GameMode{};

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

	auto NotifyPrimeConditionDiff(
		IsleStructs::ATIDinosaurBase* Dino,
		IsleStructs::ATIPlayerController* PC,
		const IsleStructs::FEligiblePrimeElder& Old,
		const IsleStructs::FEligiblePrimeElder& New
	) -> void {
		using namespace RC::Unreal;

		int CompletedTasks{0};
		StringType ChangeAnnounce = STR("");
		for (const auto& Field : Fields) {
			bool OldVal = Old.*Field.Member;
			bool NewVal = New.*Field.Member;
			if (NewVal) CompletedTasks++;
			if (OldVal == NewVal) continue;
			ChangeAnnounce += fmt::format(STR("[{}: {}]"), Field.Name, NewVal);
		}

		StringType MessageStr = fmt::format(STR("Prime Tasks [{}/5] {}"), CompletedTasks, ChangeAnnounce);
		IsleStructs::FClientShowNotificationParams Notif{FText(MessageStr)};
		PC->ProcessEvent(ClientNotifyFunc, &Notif);
	}

	static TMap<int32, IsleStructs::FEligiblePrimeElder> Cached;

	auto Fire() -> void {
		if (!GameMode) {
			GameMode = UObjectGlobals::FindFirstOf(STR("BP_SurvivalGameMode_C"));
			if(!GameMode) return;
		}

		TMap<int32, IsleStructs::FEligiblePrimeElder> NewCached;
		TSet<IsleStructs::ATIPlayerController*>* ActivePlayers = GameModeAllPlayers->ContainerPtrToValuePtr<TSet<IsleStructs::ATIPlayerController*>>(GameMode);
		for (IsleStructs::ATIPlayerController* Player : *ActivePlayers) {
			IsleStructs::APawn* Pawn = *PlayerControllerPawn->ContainerPtrToValuePtr<IsleStructs::APawn*>(Player);;
			if (!Pawn || !Pawn->IsA(DinoClass)) continue;// Make sure it's actually dino, not a fucking damn human

			IsleStructs::ATIDinosaurBase* Dino = static_cast<IsleStructs::ATIDinosaurBase*>(Pawn);
			int32 DinoID = *DinoIDProp->ContainerPtrToValuePtr<int32>(Dino);
			IsleStructs::FEligiblePrimeElder& NewData = *DinoPrimeDataProp->ContainerPtrToValuePtr<IsleStructs::FEligiblePrimeElder>(Dino);
			if (!Cached.Contains(DinoID)) {
				NewCached.Add(DinoID, NewData);
				continue;
			}

			IsleStructs::FEligiblePrimeElder OldData = *Cached.Find(DinoID);
			NewCached.Add(DinoID, OldData);
			if (!std::memcmp(&OldData, &NewData, sizeof(IsleStructs::FEligiblePrimeElder))) continue;

			NotifyPrimeConditionDiff(Dino, Player, OldData, NewData);
			NewCached.Add(DinoID, NewData);
		}
		Cached = NewCached;
	}

	auto Initialize() -> void {
		GameModeBaseClass = UObjectGlobals::StaticFindObject<UClass*>(nullptr, nullptr, STR("/Script/TheIsle.TIGameModeBase"));
		GameModeAllPlayers = GameModeBaseClass->GetPropertyByNameInChain(STR("AllPlayerControllers"));

		PlayerControllerBaseClass = UObjectGlobals::StaticFindObject<UClass*>(nullptr, nullptr, STR("/Script/TheIsle.TIPlayerController"));
		PlayerControllerPawn = PlayerControllerBaseClass->GetPropertyByNameInChain(STR("Pawn"));// Dinos/Humans/Spectator

		DinoClass = UObjectGlobals::StaticFindObject<UClass*>(nullptr, nullptr, STR("/Script/TheIsle.TIDinosaurBase"));
		DinoPrimeDataProp = DinoClass->GetPropertyByNameInChain(STR("EligiblePrimeElderData"));
		DinoIDProp = DinoClass->GetPropertyByNameInChain(STR("ID"));

		ClientNotifyFunc = UObjectGlobals::StaticFindObject<UFunction*>(nullptr, nullptr, STR("/Script/TheIsle.TIPlayerController:ClientShowNotification"));
	}
}