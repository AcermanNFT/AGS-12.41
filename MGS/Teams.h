#pragma once
#include "Utils.h"
#include "Abilities.h"
// andreu proper teams mode!!!!!!!

void ServerReviveFromDBNO(AFortPlayerPawnAthena* Pawn, AFortPlayerControllerAthena* Instigator)
{
	float ServerTime = UGameplayStatics::GetTimeSeconds(UWorld::GetWorld());
	printf("ServerReviveFromDBNO called\n");
	if (!Pawn || !Instigator)
		return;

	AFortPlayerControllerAthena* PC = (AFortPlayerControllerAthena*)Pawn->Controller;
	if (!PC || !PC->PlayerState)
		return;
	auto PlayerState = (AFortPlayerStateAthena*)PC->PlayerState;
	auto AbilitySystemComp = (UFortAbilitySystemComponentAthena*)PlayerState->AbilitySystemComponent;

	//Pawn->ReviveFromDBNOTime = 30;
	//Pawn->ServerWorldTimeRevivalTime = 30;
	//Pawn->DBNORevivalStacking = 0;

	FGameplayEventData Data{};
	Data.EventTag = Pawn->EventReviveTag;
	Data.ContextHandle = PlayerState->AbilitySystemComponent->MakeEffectContext();
	Data.Instigator = Instigator;
	Data.Target = Pawn;
	Data.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Pawn);
	Data.TargetTags = Pawn->GameplayTags;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Pawn, Pawn->EventReviveTag, Data);

	for (auto& Ability : AbilitySystemComp->ActivatableAbilities.Items)
	{
		if (Ability.Ability->Class == UGAB_AthenaDBNO_C::StaticClass())
		{
			printf("UGAB_AthenaDBNO_C\n");
			AbilitySystemComp->ServerCancelAbility(Ability.Handle, Ability.ActivationInfo);
			AbilitySystemComp->ServerEndAbility(Ability.Handle, Ability.ActivationInfo, Ability.ActivationInfo.PredictionKeyWhenActivated);
			AbilitySystemComp->ClientCancelAbility(Ability.Handle, Ability.ActivationInfo);
			AbilitySystemComp->ClientEndAbility(Ability.Handle, Ability.ActivationInfo);
			break;
		}
	}

	Pawn->bIsDBNO = false;
	Pawn->OnRep_IsDBNO();
	Pawn->SetHealth(30);
	PlayerState->DeathInfo = {};
	PlayerState->OnRep_DeathInfo();

	PC->ClientOnPawnRevived(Instigator);
}

inline __int64 PickTeam(__int64 a1, unsigned __int8 a2, __int64 a3)
{
	return 3;
};

char GetSquadIdForCurrentPlayer(AFortGameSessionDedicatedAthena* GameSession, FUniqueNetIdRepl UniqueId)
{
	TArray<AActor*> PCs;
	UGameplayStatics::GetDefaultObj()->GetAllActorsOfClass(UWorld::GetWorld(), AFortPlayerController::StaticClass(), &PCs);

	for (int i = 0; i < PCs.Num(); i++)
	{
		auto PC = (AFortPlayerController*)PCs[i];
		auto PlayerState = (AFortPlayerStateAthena*)PC->PlayerState;

		if (PlayerState && PlayerState->AreUniqueIDsIdentical(PC->PlayerState->UniqueId, UniqueId))
		{
			return PlayerState->TeamIndex - 2;
		}
	}

	PCs.Free();
	return 0;
}

void ServerPlaySquadQuickChatMessage(AFortPlayerControllerAthena* PlayerController, FAthenaQuickChatActiveEntry ChatEntry, __int64)
{
	auto PlayerState = reinterpret_cast<AFortPlayerStateAthena*>(PlayerController->PlayerState);

	switch (ChatEntry.Index)
	{
	case 0:
		PlayerState->ReplicatedTeamMemberState = ETeamMemberState::ChatBubble;
		break;
	case 1:
		PlayerState->ReplicatedTeamMemberState = ETeamMemberState::EnemySpotted;
		break;
	case 2:
		PlayerState->ReplicatedTeamMemberState = ETeamMemberState::NeedMaterials;
		break;
	case 3:
		PlayerState->ReplicatedTeamMemberState = ETeamMemberState::NeedBandages;
		break;
	case 4:
		PlayerState->ReplicatedTeamMemberState = ETeamMemberState::NeedShields;
		break;
	case 5:
		PlayerState->ReplicatedTeamMemberState = ETeamMemberState::NeedAmmoHeavy;
		break;
	case 6:
		PlayerState->ReplicatedTeamMemberState = ETeamMemberState::NeedAmmoLight;
		break;
	case 7:
		PlayerState->ReplicatedTeamMemberState = ETeamMemberState::FIRST_CHAT_MESSAGE;
		break;
	case 8:
		PlayerState->ReplicatedTeamMemberState = ETeamMemberState::NeedAmmoMedium;
		break;
	case 9:
		PlayerState->ReplicatedTeamMemberState = ETeamMemberState::NeedAmmoShells;
		break;
	default:
		break;
	}

	PlayerState->OnRep_ReplicatedTeamMemberState();

	static auto EmojiComm = StaticFindObject<UAthenaEmojiItemDefinition>("/Game/Athena/Items/Cosmetics/Dances/Emoji/Emoji_Comm.Emoji_Comm");

	if (EmojiComm)
	{
		PlayerController->ServerPlayEmoteItem(EmojiComm, 0);
	}
}

void ServerRemoveMapMarker(UAthenaMarkerComponent* Comp, FMarkerID MarkerID, ECancelMarkerReason CancelReason)
{
	AFortPlayerControllerAthena* PC = (AFortPlayerControllerAthena*)Comp->GetOwner();
	AFortPlayerStateAthena* PlayerState = (AFortPlayerStateAthena*)PC->PlayerState;

	for (size_t i = 0; i < PlayerState->PlayerTeam->TeamMembers.Num(); i++)
	{
		AFortPlayerControllerAthena* pc = (AFortPlayerControllerAthena*)PlayerState->PlayerTeam->TeamMembers[i];
		if (pc->IsA(AFortPlayerControllerAthena::StaticClass()) && PlayerState->PlayerTeam->TeamMembers[i] != PC)
		{
			pc->MarkerComponent->ClientCancelMarker(MarkerID);
		}
	}
}


namespace TeamsFromHooking
{
	void IntHooksForTeams2()
	{
		//MH_CreateHook((LPVOID)(ImageBase + 0x1583360), ApplyCost, (void**)&ApplyCostOG);
	//MH_CreateHook((LPVOID)(InSDKUtils::GetImageBase() + 0x20ca410), GetSquadIdForCurrentPlayer, nullptr); // proper!

	MH_CreateHook((LPVOID)(InSDKUtils::GetImageBase() + 0x197f6d0), ServerPlaySquadQuickChatMessage, nullptr); // SURE BUB!
	}
}