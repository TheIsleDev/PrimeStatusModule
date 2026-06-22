#pragma once

#include <Unreal/FText.hpp>

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

struct FSetEligiblePrimeElderDataParams {
    FEligiblePrimeElder NewData;
};

struct FClientShowNotificationParams {
    RC::Unreal::FText Message;
};