#pragma once


#include <string>
#include <iostream>
#include "SDK/SDK.hpp"
#include "Abilities.h"
using namespace std;
using namespace SDK;

inline FGameplayAbilitySpecHandle* (*InternalGiveAbility)(UAbilitySystemComponent*, FGameplayAbilitySpecHandle*, const FGameplayAbilitySpec& Spec) = decltype(InternalGiveAbility)(InSDKUtils::GetImageBase() + 0x6B19E0);
inline bool (*InternalTryActivateAbility)(UAbilitySystemComponent*, FGameplayAbilitySpecHandle Handle, FPredictionKey InPredictionKey, UGameplayAbility** OutInstancedAbility, void* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData) = decltype(InternalTryActivateAbility)((InSDKUtils::GetImageBase() + 0x6B33F0));
inline void (*SpecConstructor)(FGameplayAbilitySpec*, UGameplayAbility*, int, int, UObject*) = decltype(SpecConstructor)(InSDKUtils::GetImageBase() + 0x6D6DD0);
inline FGameplayAbilitySpecHandle(*GiveAbilityAndActivateOnce)(UAbilitySystemComponent* ASC, FGameplayAbilitySpecHandle*, FGameplayAbilitySpec) = decltype(GiveAbilityAndActivateOnce)(InSDKUtils::GetImageBase() + 0x6B1B00);
static void (*FGameplayAbilitySpecCtor)(FGameplayAbilitySpec*, UGameplayAbility*, int, int, UObject*) = decltype(FGameplayAbilitySpecCtor)(__int64(GetModuleHandleW(0)) + 0x6D6DD0);
static FGameplayAbilitySpecHandle* (__fastcall* GiveAbilityOG)(UAbilitySystemComponent*, FGameplayAbilitySpecHandle*, FGameplayAbilitySpec) = decltype(GiveAbilityOG)(__int64(GetModuleHandleW(0)) + 0x6B19E0);

inline void InternalServerTryActivateAbility(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle, bool InputPressed, const FPredictionKey& PredictionKey, const FGameplayEventData* TriggerEventData)
{
    FGameplayAbilitySpec* Spec = nullptr;

    for (int i = 0; i < AbilitySystemComponent->ActivatableAbilities.Items.Num(); ++i) {
        auto& Handle = AbilitySystemComponent->ActivatableAbilities.Items[i].Handle;

        if (Handle.Handle == Handle.Handle)
        {
            Spec = &AbilitySystemComponent->ActivatableAbilities.Items[i];
        }
    }

    if (!Spec) { AbilitySystemComponent->ClientActivateAbilityFailed(Handle, PredictionKey.Current); return; }

    Spec->InputPressed = true;
    UGameplayAbility* InstancedAbility = nullptr;

    if (!InternalTryActivateAbility(AbilitySystemComponent, Handle, PredictionKey, &InstancedAbility, nullptr, TriggerEventData)) {
        AbilitySystemComponent->ClientActivateAbilityFailed(Handle, PredictionKey.Current);
        Spec->InputPressed = false;
        AbilitySystemComponent->ActivatableAbilities.MarkItemDirty(*Spec);
    }
}

void EonGiveAbility(AFortPlayerControllerAthena* PC, UClass* Ability)
{
    if (!PC || !PC->PlayerState || !PC->MyFortPawn) { return; }
    auto AbilitySystemComponent = PC->MyFortPawn->AbilitySystemComponent;

    if (!AbilitySystemComponent)
        return;

    if (!Ability)
        return;

    FGameplayAbilitySpec Spec{};
    FGameplayAbilitySpecCtor(&Spec, (UGameplayAbility*)Ability->DefaultObject, 1, -1, nullptr);
    GiveAbilityOG(AbilitySystemComponent, &Spec.Handle, Spec);
}

void GiveAbilitySet(AFortPlayerControllerAthena* PC, UFortAbilitySet* Set)
{
    if (Set)
    {
        for (size_t i = 0; i < Set->GameplayAbilities.Num(); i++)
        {
            UClass* Ability = Set->GameplayAbilities[i].Get();
            if (Ability) { EonGiveAbility(PC, Ability); }
        }
    }
}

inline void GiveAbility(AFortPlayerPawnAthena* Pawn, UGameplayAbility* GameplayAbility)
{
    if (!Pawn || !GameplayAbility)
    {
        std::cout << "Pawn || GameplayAbility is null" << std::endl;
        return;
    }

    auto AbilitySystemComponent = Pawn->AbilitySystemComponent;

    if (!AbilitySystemComponent) {
        std::cout << "AbilitySystemComponent is null" << std::endl;
        return;
    }

    FGameplayAbilitySpec Spec;
    SpecConstructor(&Spec, GameplayAbility, 1, -1, nullptr);
    InternalGiveAbility(AbilitySystemComponent, &Spec.Handle, Spec);
}
