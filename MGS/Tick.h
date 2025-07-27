#pragma once
#include "Utils.h"
#include "Bosses.h"
static void ApplySiphonEffectToEveryone()
{
	for (int i = 0; i < UWorld::GetWorld()->NetDriver->ClientConnections.Num(); i++)
	{
		auto PlayerState = (AFortPlayerState*)UWorld::GetWorld()->NetDriver->ClientConnections[i]->PlayerController->PlayerState;

		auto AbilitySystemComponent = PlayerState->AbilitySystemComponent;

		auto Handle = AbilitySystemComponent->MakeEffectContext();
		AbilitySystemComponent->NetMulticast_InvokeGameplayCueAdded(FGameplayTag(UKismetStringLibrary::GetDefaultObj()->Conv_StringToName(L"GameplayCue.Shield.PotionConsumed")), FPredictionKey(), Handle);
		AbilitySystemComponent->NetMulticast_InvokeGameplayCueExecuted(FGameplayTag(UKismetStringLibrary::GetDefaultObj()->Conv_StringToName(L"GameplayCue.Shield.PotionConsumed")), FPredictionKey(), Handle);
	}
}
inline static TArray<AActor*> PlayerStarts;
inline bool bShouldDestroyAllPlayerBuilds = false;
static void (*ServerReplicateActors)(void*) = decltype(ServerReplicateActors)(__int64(GetModuleHandleW(0)) + 0x1023F60);
void (*TickFlushOG)(UNetDriver* Driver, float a2);
void TickFlushHook(UNetDriver* Driver, float a2)
{
    static bool Done = false;
    static bool Done2 = false;
    if (!Driver)
        return;

    if (!Done)
    {
        if (GetGameState()->WarmupCountdownEndTime - GetStatics()->GetTimeSeconds(UWorld::GetWorld()) <= 10)
        {
            Done = true;

        }
    }

    if (BotsEnabled)
    {
        if (!Done2)
        {
            if (GetGameState()->WarmupCountdownEndTime - GetStatics()->GetTimeSeconds(UWorld::GetWorld()) <= 0)
            {
                Done2 = true;

                StartAircraftPhase(GetGameMode(), 0);
            }
        }

    }

    if (Driver && Driver->ReplicationDriver && Driver->ClientConnections.Num() > 0 && !Driver->ClientConnections[0]->InternalAck)
        ServerReplicateActors(Driver->ReplicationDriver);

    return TickFlushOG(Driver, a2);
}

inline float GetMaxTickRate() 
{
	return 30.f;
}