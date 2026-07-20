#include <SDKGenerator/UEHeaderGenerator.hpp>

#include <Unreal/FText.hpp>
#include <Unreal/AActor.hpp>
#include <Unreal/UObject.hpp>
#include <Unreal/UObjectArray.hpp>
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/CoreUObject/UObject/UnrealType.hpp>
#include <Unreal/Core/Containers/ContainerAllocationPolicies.hpp>

#include <Reflection/_include_custom.hpp>

namespace PrimeStatusComponent {
	using namespace RC::Unreal;

	struct FEligiblePrimeElderHolder {
		int32 DinoID;
		FEligiblePrimeElder Holder;
	};

	struct ConditionField {
		bool FEligiblePrimeElder::* Member;
		const RC::StringType Name;
	};

	static ATIGameModeBase* GameMode{};

	static const ConditionField Fields[] = {
		{&FEligiblePrimeElder::bPrimeCondition1, STR("Visit a Sanctuary")},
		{&FEligiblePrimeElder::bPrimeCondition2, STR("Get Nested In")},
		{&FEligiblePrimeElder::bPrimeCondition3, STR("Get Perfect Diet")},
		{&FEligiblePrimeElder::bPrimeCondition4, STR("Visit Mass Migration Zone")},
		{&FEligiblePrimeElder::bPrimeCondition5, STR("Visit 2 Migration Zones")},
		{&FEligiblePrimeElder::bPrimeCondition6, STR("Visit 4 Patrol Zones")},
		{&FEligiblePrimeElder::bPrimeCondition7, STR("Never be infertile")},
		{&FEligiblePrimeElder::bPrimeCondition8, STR("Never get muscle spasms")},
		{&FEligiblePrimeElder::bPrimeCondition9, STR("Raise children to subadult")},
		{&FEligiblePrimeElder::bPrimeCondition10, STR("Be a Hypsi, Troodon, Beipi, Dryo or Deino.")},
		{&FEligiblePrimeElder::bIsEligiblePrime, STR("Prime Eligibility")},
	};

	auto NotifyPrimeConditionDiff(ATIDinosaurBase* Dino, ATIPlayerController* Player, const FEligiblePrimeElder& Old, const FEligiblePrimeElder& New) -> void {
		using namespace RC::Unreal;

		int CompletedTasks{0};
		RC::StringType ChangeAnnounce;
		for (const auto& Field : Fields) {
			bool OldVal = Old.*Field.Member;
			bool NewVal = New.*Field.Member;
			if (NewVal) CompletedTasks++;
			if (OldVal == NewVal) continue;
			ChangeAnnounce += fmt::format(STR("[{}: {}]"), Field.Name, NewVal);
		}

		RC::StringType MessageStr = fmt::format(STR("Prime Tasks [{}/5] {}"), CompletedTasks, ChangeAnnounce);
		Player->ClientShowNotification(FText(MessageStr));
	}

	static TMap<FString, FEligiblePrimeElderHolder> Cached;

	auto Fire() -> void {
		if (!GameMode) {
			GameMode = static_cast<ATIGameModeBase*>(UObjectGlobals::FindFirstOf(STR("BP_SurvivalGameMode_C")));
			if(!GameMode) return;
		}

		TMap<FString, FEligiblePrimeElderHolder> NewCached;
		TSet<ATIPlayerController*> ActivePlayers = GameMode->GetAllPlayerControllers();
		for (ATIPlayerController* Player : ActivePlayers) {
			APawn* Pawn = Player->GetPawn();
			if (!Pawn || !Pawn->IsA(ATIDinosaurBase::StaticClass())) continue;

			FString SteamID = Player->GetSteamId();
			ATIDinosaurBase* Dino = static_cast<ATIDinosaurBase*>(Pawn);
			int32 DinoID = Dino->GetID();
			FEligiblePrimeElder& NewData = Dino->GetEligiblePrimeElderData();
			FEligiblePrimeElderHolder* OldData = Cached.Find(SteamID);
			if (!OldData || OldData->DinoID != DinoID) {
				NewCached.Add(SteamID, {DinoID, NewData});
				continue;
			}

			NewCached.Add(SteamID, *OldData);
			if (!std::memcmp(&OldData->Holder, &NewData, sizeof(FEligiblePrimeElder))) continue;

			NotifyPrimeConditionDiff(Dino, Player, OldData->Holder, NewData);
			NewCached.Add(SteamID, {DinoID, NewData});
		}
		Cached = NewCached;
	}

	auto Initialize() -> void {
	}
}