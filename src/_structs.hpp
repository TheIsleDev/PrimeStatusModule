#pragma once

#include "FWeakObjectPtr.hpp"
#include <Unreal/AActor.hpp>
#include <Unreal/FText.hpp>
#include <Unreal/AGameModeBase.hpp>
#include <Unreal/PropertyMacros.hpp>

struct FEligiblePrimeElder
{
    bool bPrimeCondition1;
    bool bPrimeCondition2;
    bool bPrimeCondition3;
    bool bPrimeCondition4;
    bool bPrimeCondition5;
    bool bPrimeCondition6;
    bool bPrimeCondition7;
    bool bPrimeCondition8;
    bool bPrimeCondition9;
    bool bPrimeCondition10;
    bool bIsEligiblePrime;
};

class AController : public RC::Unreal::AActor {};
class APlayerController : public AController {};
class ATIPlayerController : public APlayerController {};

class ATIDinosaurBase : public RC::Unreal::AActor {};

struct ATIGameModeBase : public RC::Unreal::AGameModeBase {};

struct FClientShowNotificationParams {
    RC::Unreal::FText Message;
};