#pragma once

#include <String/StringType.hpp>
#include <DynamicOutput/Output.hpp>
#include <DynamicOutput/OutputDevice.hpp>

#include <SDKGenerator/UEHeaderGenerator.hpp>

#include <Unreal/AActor.hpp>
#include <Unreal/UObject.hpp>
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/CoreUObject/UObject/UnrealType.hpp>
#include <Unreal/Core/Containers/ContainerAllocationPolicies.hpp>

#include "Containers/FString.hpp"
#include "Structs/TheIsleStructs.hpp"

using namespace RC::Unreal;

namespace PrimeChecker {
  static UClass* GameModeBaseClass = nullptr;
  static FProperty* GameModeAllPlayers = nullptr;

  static UClass* DinoClass = nullptr;
  static FProperty* DinoPrimeDataProp = nullptr;
  static FProperty* DinoIDProp = nullptr;
  static FProperty* DinoPlayerControllerProp = nullptr;

  static UFunction* ClientNotifyFunc = nullptr;

  static TMap<int32, IsleStructs::FEligiblePrimeElder> Cached;

  auto NotifyPrimeConditionDiff(
    IsleStructs::ATIDinosaurBase* Dino,
    IsleStructs::ATIPlayerController* PC,
    const IsleStructs::FEligiblePrimeElder& Old,
    const IsleStructs::FEligiblePrimeElder& New
  ) -> void {
    using namespace RC::Unreal;

    struct ConditionField { bool IsleStructs::FEligiblePrimeElder::* Member; const wchar_t* Name; };
    static const ConditionField Fields[] = {
      {&IsleStructs::FEligiblePrimeElder::bPrimeCondition1, STR("Condition1")},
      {&IsleStructs::FEligiblePrimeElder::bPrimeCondition2, STR("Condition2")},
      {&IsleStructs::FEligiblePrimeElder::bPrimeCondition3, STR("Condition3")},
      {&IsleStructs::FEligiblePrimeElder::bPrimeCondition4, STR("Condition4")},
      {&IsleStructs::FEligiblePrimeElder::bPrimeCondition5, STR("Condition5")},
      {&IsleStructs::FEligiblePrimeElder::bPrimeCondition6, STR("Condition6")},
      {&IsleStructs::FEligiblePrimeElder::bPrimeCondition7, STR("Condition7")},
      {&IsleStructs::FEligiblePrimeElder::bPrimeCondition8, STR("Condition8")},
      {&IsleStructs::FEligiblePrimeElder::bPrimeCondition9, STR("Condition9")},
      {&IsleStructs::FEligiblePrimeElder::bPrimeCondition10, STR("Condition10")},
      {&IsleStructs::FEligiblePrimeElder::bIsEligiblePrime, STR("IsEligiblePrime")},
    };

    for (const auto& Field : Fields) {
      bool NewVal = New.*Field.Member;
      if (Old.*Field.Member == NewVal) continue;

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
      IsleStructs::ATIPlayerController* PlayerControllerPtr = *DinoPlayerControllerProp->ContainerPtrToValuePtr<IsleStructs::ATIPlayerController*>(Dino);
      if (!PlayerControllerPtr) continue;

      IsleStructs::FEligiblePrimeElder OldData = *Cached.Find(DinoId);
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