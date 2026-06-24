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

#include "_structs.hpp"

using namespace RC::Unreal;

namespace PrimeChecker {
  static UClass* GameModeBaseClass = nullptr;
  static FProperty* GameModeAllPlayers = nullptr;

  static UClass* DinoClass = nullptr;
  static FProperty* DinoPrimeDataProp = nullptr;
  static FProperty* DinoPlayerControllerProp = nullptr;

  static UFunction* ClientNotifyFunc = nullptr;

  static UFunction* PossesPawnFunc = nullptr;
  static FProperty* FuncParamProp = nullptr;

  static std::vector<CachedDinoHolder> Cached{};

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

  auto HandleCallBack(UnrealScriptFunctionCallableContext& FuncContext) -> void {
    Output::send<LogLevel::Error>(STR("HOOK FIRED"));
    ATIDinosaurBase* Dino = *FuncParamProp->ContainerPtrToValuePtr<ATIDinosaurBase*>(FuncContext.TheStack.Locals());
    FEligiblePrimeElder* Data = DinoPrimeDataProp->ContainerPtrToValuePtr<FEligiblePrimeElder>(Dino);
    Cached.push_back(CachedDinoHolder{Dino, *Data});
  }

  auto try_find_func() -> void {
    PossesPawnFunc = UObjectGlobals::StaticFindObject<UFunction*>(nullptr, nullptr, STR("/Game/TheIsle/Core/Player/BP_PlayerController.BP_PlayerController_C:ReceivePossess"));
    if (!PossesPawnFunc) return;
    FuncParamProp = PossesPawnFunc->GetPropertyByNameInChain(STR("PossessedPawn"));
    PossesPawnFunc->RegisterPreHook(
      [](UnrealScriptFunctionCallableContext& FuncContext, void*) -> void {
        HandleCallBack(FuncContext);
      },
      nullptr
    );
  }

  auto Fire() -> void {
    if (!PossesPawnFunc) try_find_func();
    for (size_t i = Cached.size(); i-- > 0; ) {
      CachedDinoHolder& Holder = Cached[i];
      ATIDinosaurBase* Dino = Holder.Dino;
      if (!Dino || !Dino->IsA(DinoClass)) {
        Cached.erase(Cached.begin() + i);
        continue;
      }

      FEligiblePrimeElder* LiveDataPtr = DinoPrimeDataProp->ContainerPtrToValuePtr<FEligiblePrimeElder>(Dino);
      const FEligiblePrimeElder OldData = Holder.EligiblePrimeData;
      const FEligiblePrimeElder& NewData = *LiveDataPtr;
      Output::send<LogLevel::Error>(STR("FIRED 3"));
 
      if (!std::memcmp(&OldData, &NewData, sizeof(FEligiblePrimeElder))) continue;
 
      ATIPlayerController* PlayerControllerPtr = *DinoPlayerControllerProp->ContainerPtrToValuePtr<ATIPlayerController*>(Dino);

      if (!PlayerControllerPtr) continue;

      NotifyPrimeConditionDiff(Dino, PlayerControllerPtr, OldData, NewData);
      Holder.EligiblePrimeData = NewData;
    }
  }

  auto Initialize() -> void {
    GameModeBaseClass = UObjectGlobals::StaticFindObject<UClass*>(nullptr, nullptr, STR("/Script/TheIsle.TIGameModeBase"));
    GameModeAllPlayers = GameModeBaseClass->GetPropertyByNameInChain(STR("AllPlayerCharacters"));

    DinoClass = UObjectGlobals::StaticFindObject<UClass*>(nullptr, nullptr, STR("/Script/TheIsle.TIDinosaurBase"));
    DinoPrimeDataProp = DinoClass->GetPropertyByNameInChain(STR("EligiblePrimeElderData"));
    DinoPlayerControllerProp = DinoClass->GetPropertyByNameInChain(STR("PlayerController"));

    ClientNotifyFunc = UObjectGlobals::StaticFindObject<UFunction*>(nullptr, nullptr, STR("/Script/TheIsle.TIPlayerController:ClientShowNotification"));

    try_find_func();
  }
}