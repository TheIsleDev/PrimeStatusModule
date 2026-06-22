#pragma once

#include "String/StringType.hpp"
#include <DynamicOutput/Output.hpp>
#include <DynamicOutput/OutputDevice.hpp>

#include <SDKGenerator/UEHeaderGenerator.hpp>

#include <Unreal/UObject.hpp>
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/CoreUObject/UObject/UnrealType.hpp>
#include <Unreal/Core/Containers/ContainerAllocationPolicies.hpp>

#include "_structs.hpp"

using namespace RC::Unreal;

namespace PrimeChecker {
    static UClass* DinoClass = nullptr;
    static FProperty* DinoPrimeDataProp = nullptr;
    static FProperty* DinoPlayerControllerProp = nullptr;

    static FProperty* FuncParamProp = nullptr;

    static UFunction* SetEligibleFunc = nullptr;
    static UFunction* ClientNotifyFunc = nullptr;
    static UFunction* SetPrimeFunc = nullptr;

    auto NotifyPrimeConditionDiff(UObject* Dino, const FEligiblePrimeElder& Old, const FEligiblePrimeElder& New) -> void {
        using namespace RC::Unreal;

        auto* PCSlot = DinoPlayerControllerProp->ContainerPtrToValuePtr<UObject*>(Dino);
        if (!PCSlot || !*PCSlot) return;
        UObject* PC = *PCSlot;

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

    auto HandleSetEligiblePrimeElderData(UnrealScriptFunctionCallableContext& FuncContext) -> void {
        Output::send<LogLevel::Error>(STR("HOOK FIRED"));
        FSetEligiblePrimeElderDataParams& Params = FuncContext.GetParams<FSetEligiblePrimeElderDataParams>();
        FEligiblePrimeElder& NewData = Params.NewData;

        UObject* Dino = FuncContext.Context;
        FEligiblePrimeElder* OldData = DinoPrimeDataProp->ContainerPtrToValuePtr<FEligiblePrimeElder>(Dino);

        NotifyPrimeConditionDiff(Dino, *OldData, NewData);
    }

    auto Initialize() -> void {
        DinoClass = UObjectGlobals::StaticFindObject<UClass*>(nullptr, nullptr, STR("/Script/TheIsle.TIDinosaurBase"));
        DinoPrimeDataProp = DinoClass->GetPropertyByNameInChain(STR("EligiblePrimeElderData"));
        DinoPlayerControllerProp = DinoClass->GetPropertyByNameInChain(STR("PlayerController"));

        ClientNotifyFunc = UObjectGlobals::StaticFindObject<UFunction*>(nullptr, nullptr, STR("/Script/TheIsle.TIPlayerController:ClientShowNotification"));

        SetPrimeFunc = UObjectGlobals::StaticFindObject<UFunction*>(nullptr, nullptr, STR("/Script/TheIsle.TIDinosaurBase:SetEligiblePrimeElderData"));
        FuncParamProp = SetPrimeFunc->GetPropertyByNameInChain(STR("NewData"));
        SetPrimeFunc->RegisterPostHook([](UnrealScriptFunctionCallableContext& FuncContext, void*) -> void {
            HandleSetEligiblePrimeElderData(FuncContext);
        });
    }
}





/*
those are the TINotificationWidget / UTINotificationWidget notifications.
The player-controller call that drives them is ATIPlayerController::ClientShowNotification(FText Message),
which ends up showing text through UTINotificationWidget::ShowNotification(FText NotificationText).
If you are searching dumps/SDK, search for ClientShowNotification, TINotificationWidget, NotificationWidget, and ShowNotification.
build FText first then call ClientShowNotification if using lua
*/


/*
void ATIDinosaurBase::SetNumberOfPrimeCondition6()
void ATIDinosaurBase::SetNumberOfPrimeCondition5()
void ATIDinosaurBase::SetEligiblePrimeElderData(FEligiblePrimeElder NewData)
bool ATIDinosaurBase::GetIsEligiblePrimeElder()
FEligiblePrimeElder ATIDinosaurBase::GetEligiblePrimeElderData() const {
    return FEligiblePrimeElder{};
}
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsPrimeElder();

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, meta=(AllowPrivateAccess=true))
    FEligiblePrimeElder EligiblePrimeElderData;

DOREPLIFETIME(ATIDinosaurBase, EligiblePrimeElderData);
*/

/*
FindAllOf("TIDinosaurBase")
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bPrimeCondition1;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bPrimeCondition2;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bPrimeCondition3;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bPrimeCondition4;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bPrimeCondition5;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bPrimeCondition6;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bPrimeCondition7;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bPrimeCondition8;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bPrimeCondition9;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bPrimeCondition10;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
    bool bIsEligiblePrime;

(ATICharacterBase) class ATIDinosaurBase : public ATICharacterBase
    class ATIPlayerController* PlayerController;

    void ClientShowNotification(const FText& Message);
*/