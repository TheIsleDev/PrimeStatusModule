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
#include "_structs.hpp"

using namespace RC::Unreal;

namespace PrimeChecker {
  static UClass* GameModeBaseClass = nullptr;
  static FProperty* GameModeAllPlayers = nullptr;

  static UClass* DinoClass = nullptr;
  static FProperty* DinoPrimeDataProp = nullptr;
  static FProperty* DinoIdPrefixProp = nullptr;
  static FProperty* DinoPlayerControllerProp = nullptr;

  static UFunction* ClientNotifyFunc = nullptr;

  static TMap<FString, FEligiblePrimeElder> Cached;

  auto NotifyPrimeConditionDiff(ATIDinosaurBase* Dino, ATIPlayerController* PC, const FEligiblePrimeElder& Old, const FEligiblePrimeElder& New) -> void {
    using namespace RC::Unreal;

    struct ConditionField { bool FEligiblePrimeElder::* Member; const wchar_t* Name; };
    static const ConditionField Fields[] = {
      {&FEligiblePrimeElder::bPrimeCondition1, STR("Condition1")},
      {&FEligiblePrimeElder::bPrimeCondition2, STR("Condition2")},
      {&FEligiblePrimeElder::bPrimeCondition3, STR("Condition3")},
      {&FEligiblePrimeElder::bPrimeCondition4, STR("Condition4")},
      {&FEligiblePrimeElder::bPrimeCondition5, STR("Condition5")},
      {&FEligiblePrimeElder::bPrimeCondition6, STR("Condition6")},
      {&FEligiblePrimeElder::bPrimeCondition7, STR("Condition7")},
      {&FEligiblePrimeElder::bPrimeCondition8, STR("Condition8")},
      {&FEligiblePrimeElder::bPrimeCondition9, STR("Condition9")},
      {&FEligiblePrimeElder::bPrimeCondition10, STR("Condition10")},
      {&FEligiblePrimeElder::bIsEligiblePrime, STR("IsEligiblePrime")},
    };

    for (const auto& Field : Fields) {
      bool OldVal = Old.*Field.Member;
      bool NewVal = New.*Field.Member;
      if (OldVal == NewVal) continue;

      auto MessageStr = fmt::format(STR("Your prime task status changed [{}: {}]"), Field.Name, NewVal);
      FClientShowNotificationParams Notif{FText(MessageStr)};

      PC->ProcessEvent(ClientNotifyFunc, &Notif);
    }
  }

  auto Fire() -> void {
    auto* GameMode = UObjectGlobals::FindFirstOf(STR("BP_SurvivalGameMode_C"));
    if (!GameMode) return;

    static TMap<FString, FEligiblePrimeElder> NewCached;
    TArray<ATIDinosaurBase*>* ActiveDinos = GameModeAllPlayers->ContainerPtrToValuePtr<TArray<ATIDinosaurBase*>>(GameMode);
    for (ATIDinosaurBase* Dino : *ActiveDinos) {
      FString IdPrefix = *DinoIdPrefixProp->ContainerPtrToValuePtr<FString>(Dino);
      FEligiblePrimeElder& NewData = *DinoPrimeDataProp->ContainerPtrToValuePtr<FEligiblePrimeElder>(Dino);
      if (!Cached.Contains(IdPrefix)) {
        NewCached.Add(IdPrefix, NewData);
        continue;
      }
      ATIPlayerController* PlayerControllerPtr = *DinoPlayerControllerProp->ContainerPtrToValuePtr<ATIPlayerController*>(Dino);
      if (!PlayerControllerPtr) continue;

      FEligiblePrimeElder OldData = *Cached.Find(IdPrefix);
      NewCached.Add(IdPrefix, OldData);
      if (!std::memcmp(&OldData, &NewData, sizeof(FEligiblePrimeElder))) continue;

      NotifyPrimeConditionDiff(Dino, PlayerControllerPtr, OldData, NewData);
      NewCached.Add(IdPrefix, NewData);
    }
    Cached = NewCached;
  }

  auto Initialize() -> void {
    GameModeBaseClass = UObjectGlobals::StaticFindObject<UClass*>(nullptr, nullptr, STR("/Script/TheIsle.TIGameModeBase"));
    GameModeAllPlayers = GameModeBaseClass->GetPropertyByNameInChain(STR("AllPlayerCharacters"));

    DinoClass = UObjectGlobals::StaticFindObject<UClass*>(nullptr, nullptr, STR("/Script/TheIsle.TIDinosaurBase"));
    DinoPrimeDataProp = DinoClass->GetPropertyByNameInChain(STR("EligiblePrimeElderData"));
    DinoIdPrefixProp = DinoClass->GetPropertyByNameInChain(STR("IdPrefix"));
    DinoPlayerControllerProp = DinoClass->GetPropertyByNameInChain(STR("PlayerController"));

    ClientNotifyFunc = UObjectGlobals::StaticFindObject<UFunction*>(nullptr, nullptr, STR("/Script/TheIsle.TIPlayerController:ClientShowNotification"));
  }
}