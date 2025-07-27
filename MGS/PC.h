#pragma once
#include "Utils.h"
#include "Inventory.h"
#include "Abilities.h"
#include "Vehicle.h"
#include <map>
#include "Looting.h"
#include "XP.h"
UProperty* BackendNameProp = nullptr;
UClass* LettersClass = nullptr;
UProperty* QuestItem = nullptr;
void ServerReturnToMainMenu(AFortPlayerControllerAthena* PC)
{
	PC->ClientReturnToMainMenu(TEXT("u have ben kicked uh what are u doin"));
}
bool IsPrimaryQuickbar(UFortItemDefinition* Def)
{
	return Def->IsA(UFortConsumableItemDefinition::StaticClass()) || Def->IsA(UFortWeaponRangedItemDefinition::StaticClass()) || Def->IsA(UFortGadgetItemDefinition::StaticClass());
}

static bool IsPrimaryQuickbar2(UFortItemDefinition* ItemDefinition)
{
	/* if (ItemDefinition->IsA(UFortDecoItemDefinition::StaticClass()))
	{
		if (ItemDefinition->IsA(UFortTrapItemDefinition::StaticClass()))
			return false;
		else
			return true;
	}
	else if (ItemDefinition->IsA(UFortWeaponItemDefinition::StaticClass()))
		return true; */

	if (ItemDefinition->GetFullName().contains("UncleBrolly") || ItemDefinition->GetFullName().contains("D_Athena_Keycard")) return true;

	static auto FortWeaponMeleeItemDefinitionClass = StaticFindObject<UClass>("/Script/FortniteGame.FortWeaponMeleeItemDefinition");
	static auto FortEditToolItemDefinitionClass = StaticFindObject<UClass>("/Script/FortniteGame.FortEditToolItemDefinition");
	static auto FortBuildingItemDefinitionClass = StaticFindObject<UClass>("/Script/FortniteGame.FortBuildingItemDefinition");
	static auto FortAmmoItemDefinitionClass = StaticFindObject<UClass>("/Script/FortniteGame.FortAmmoItemDefinition");
	static auto FortResourceItemDefinitionClass = StaticFindObject<UClass>("/Script/FortniteGame.FortResourceItemDefinition");
	static auto FortTrapItemDefinitionClass = StaticFindObject<UClass>("/Script/FortniteGame.FortTrapItemDefinition");

	if (!ItemDefinition->IsA(FortWeaponMeleeItemDefinitionClass) && !ItemDefinition->IsA(FortEditToolItemDefinitionClass) &&
		!ItemDefinition->IsA(FortBuildingItemDefinitionClass) && !ItemDefinition->IsA(FortAmmoItemDefinitionClass)
		&& !ItemDefinition->IsA(FortResourceItemDefinitionClass) && !ItemDefinition->IsA(FortTrapItemDefinitionClass))
		return true;

	log_debug("NOT primqb %s\n", ItemDefinition->GetFullName().c_str());
	return false;
}

static inline int64(*CantBuild)(UObject*, UObject*, FVector, FRotator, char, TArray<ABuildingSMActor*>*, char*);
static inline ABuildingSMActor* (*BuildingSMActorReplaceBuilding)(ABuildingSMActor*, __int64, UClass*, int, int, uint8, AFortPlayerController*);

string SplitString(bool SecondString, string delim, string strtosplit)
{
	auto start = 0ULL;
	auto end = strtosplit.find(delim);
	if (SecondString)
	{
		while (end != std::string::npos)
		{
			start = end + delim.length();
			end = strtosplit.find(delim, start);
		}
	}

	return strtosplit.substr(start, end);
}
inline void (*ServerLoadingScreenDroppedOG)(AFortPlayerControllerAthena* PC);
inline void ServerLoadingScreenDropped(AFortPlayerControllerAthena* PC)
{
	auto Pawn = (AFortPlayerPawn*)PC->Pawn;
	AFortPlayerPawnAthena* PlayerPawn = Cast<AFortPlayerPawnAthena>(PC->Pawn);
	UFortAbilitySystemComponent* AbilitySystemComponent = PlayerPawn->AbilitySystemComponent;
	UFortAbilitySet* AbilitySet = StaticFindObject<UFortAbilitySet>("/Game/Abilities/Player/Generic/Traits/DefaultPlayer/GAS_AthenaPlayer.GAS_AthenaPlayer");
	AFortPlayerStateAthena* PlayerState = (AFortPlayerStateAthena*)PC->PlayerState;
	AFortGameStateAthena* GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;
	UFortPlaylistAthena* Playlist = GetPlaylist();

	//if (!PlayerState || !PlayerPawn || !Playlist || !AbilitySystemComponent || !AbilitySet)
		//return;
//	if (!AbilitySystemComponent || !AbilitySet)
	//	return;

	UFortKismetLibrary::UpdatePlayerCustomCharacterPartsVisualization(PlayerState);
	PlayerState->OnRep_CharacterData();

	if (bEnableTeams)
	{
		PlayerState->SquadId = PlayerState->TeamIndex - 2;

		PlayerState->OnRep_SquadId();
		PlayerState->OnRep_TeamIndex(0);
		PlayerState->OnRep_PlayerTeam();
		PlayerState->OnRep_PlayerTeamPrivate();

		FGameMemberInfo Info{ -1,-1,-1 };
		Info.MemberUniqueId = PlayerState->UniqueId;
		Info.SquadId = PlayerState->SquadId;
		Info.TeamIndex = PlayerState->TeamIndex;

		GameState->GameMemberInfoArray.Members.Add(Info);
		GameState->GameMemberInfoArray.MarkItemDirty(Info);
	}


	bool bHasStarterItems = false;
	for (auto& ItemEntry : PC->WorldInventory->Inventory.ReplicatedEntries) {
		if (ItemEntry.ItemDefinition->IsA(UFortWeaponMeleeItemDefinition::StaticClass())) {
			bHasStarterItems = true;
			std::cout << std::format("{} already has starter items", PlayerState->GetPlayerName().ToString()) << std::endl;
			break;
		}
	}

	if (!bHasStarterItems) {
		GiveItem(PC, PC->CosmeticLoadoutPC.Pickaxe->WeaponDefinition, 1, 0, false, false);
		for (FItemAndCount& Item : GetGameMode()->StartingItems) {
			if (Item.Item->IsA(UFortResourceItemDefinition::StaticClass())) continue;
			GiveItem(PC, Item.Item, Item.Count, 0, false, false);
			std::cout << std::format("{} ALR GIVE ITMES!", PlayerState->GetPlayerName().ToString()) << std::endl;
		}
	}

		if (!AbilitySet)
			return;

	if (!AbilitySet) { std::cout << "Failed to find AbilitySet" << std::endl; return; }

	for (auto& Ability : AbilitySet->GameplayAbilities) {

		if (!Ability.Get()) { std::cout << "Ability is null" << std::endl; continue; }
		GiveAbility((AFortPlayerPawnAthena*)PC->MyFortPawn, (UGameplayAbility*)Ability.Get()->DefaultObject);
	}

	for (auto& GrantedGameplayEffect : AbilitySet->GrantedGameplayEffects) {
		AbilitySystemComponent->BP_ApplyGameplayEffectToSelf(GrantedGameplayEffect.GameplayEffect.Get(), GrantedGameplayEffect.Level, FGameplayEffectContextHandle());
	}
	//auto PlayerState = (AFortPlayerStateAthena*)PC->PlayerState;
	auto CurrentPlaylist = GetGameState()->CurrentPlaylistInfo.BasePlaylist;
	auto& ModifierList = *(TArray<TSoftObjectPtr<UFortGameplayModifierItemDefinition>>*)(__int64(CurrentPlaylist) + 0x170);
	for (int i = 0; i < ModifierList.Num(); i++)
	{
		auto Current = StaticLoadObject<UFortGameplayModifierItemDefinition>(Conv_NameToString(ModifierList[i].ObjectID.AssetPathName).ToString());
		if (Current)
		{
			GiveModifier(Current, PC);
		}
	}

	PC->XPComponent->bRegisteredWithQuestManager = true;
	PC->XPComponent->OnRep_bRegisteredWithQuestManager();

	PlayerState->SeasonLevelUIDisplay = PC->XPComponent->CurrentLevel;
	PlayerState->OnRep_SeasonLevelUIDisplay();

	UFortPlayerControllerAthenaXPComponent* XPComponent = *(UFortPlayerControllerAthenaXPComponent**)(__int64(PC) + 0x3560);

	//static bool First = false;
	//if (!First)//1:1 fr
	//{
	//	First = true;
	//	LettersClass = StaticLoadObject<UClass>("/Game/Athena/Items/QuestInteractables/FortnightLetters/FortniteLettersBPs/Prop_QuestInteractable_Letters_Parent.Prop_QuestInteractable_Letters_Parent_C");
	//	QuestItem = StaticLoadObject<UProperty>("/Game/Athena/Items/QuestInteractables/Generic/Prop_QuestInteractable_Parent.Prop_QuestInteractable_Parent_C.QuestItem");
	//	BackendNameProp = StaticLoadObject<UProperty>("/Game/Athena/Items/QuestInteractables/Generic/Prop_QuestInteractable_Parent.Prop_QuestInteractable_Parent_C.QuestBackendObjectiveName");
	//}

	GetFortKismet()->UpdatePlayerCustomCharacterPartsVisualization((AFortPlayerState*)PC->PlayerState);
	((AFortPlayerStateAthena*)PC->PlayerState)->OnRep_CharacterData();

	SpawnVehicle();
	SpawnLlamas();

	PlayerState->ForceNetUpdate();
	Pawn->ForceNetUpdate();
	PC->ForceNetUpdate();

	return ServerLoadingScreenDroppedOG(PC);
}


void ServerCreateBuildingActor(AFortPlayerControllerAthena* PlayerController, FCreateBuildingActorData& CreateBuildingData) {
	if (!PlayerController || PlayerController->IsInAircraft())
		return;

	CantBuild = decltype(CantBuild)(InSDKUtils::GetImageBase() + 0x1E57790);

	auto BuildingClass = PlayerController->BroadcastRemoteClientInfo->RemoteBuildableClass.Get();
	if (!BuildingClass) { std::cout << "BuildingClass is null" << std::endl; return; }

	TArray<ABuildingSMActor*> ExistingBuildings;
	char BuildRestrictionFlag;
	if (CantBuild(UWorld::GetWorld(), BuildingClass, CreateBuildingData.BuildLoc, CreateBuildingData.BuildRot, CreateBuildingData.bMirrored, &ExistingBuildings, &BuildRestrictionFlag)) { return; }

	auto NewBuilding = BuildingSpawnActor<ABuildingSMActor>(CreateBuildingData.BuildLoc, CreateBuildingData.BuildRot, BuildingClass);
	if (!NewBuilding) { std::cout << "Failed to spawn NewBuilding" << std::endl; return; }

	auto ResDef = GetFortKismet()->K2_GetResourceItemDefinition(((ABuildingSMActor*)BuildingClass->DefaultObject)->ResourceType);
	Remove(PlayerController, ResDef, 10);

	NewBuilding->bPlayerPlaced = true;
	auto PlayerState = Cast<AFortPlayerStateAthena>(PlayerController->PlayerState);
	NewBuilding->TeamIndex = PlayerState->TeamIndex;
	NewBuilding->Team = static_cast<EFortTeam>(PlayerState->TeamIndex);
	NewBuilding->OnRep_Team();
	NewBuilding->InitializeKismetSpawnedBuildingActor(NewBuilding, PlayerController, true);

	for (auto& Building : ExistingBuildings) {
		Building->K2_DestroyActor();
	}
	ExistingBuildings.Free();

	//auto ItemDefinition = UFortKismetLibrary::K2_GetResourceItemDefinition(NewBuilding->ResourceType);
	//RemoveItem(PlayerController, FindItemEntry(PlayerController, ItemDefinition)->ItemGuid, 10);
}

void ServerBeginEditingBuildingActor(AFortPlayerControllerAthena* PC, ABuildingSMActor* BuildingActorToEdit)
{
	if (!PC || !PC->MyFortPawn || !BuildingActorToEdit)
		return;

	AFortPlayerStateAthena* PlayerState = (AFortPlayerStateAthena*)PC->PlayerState;
	if (!PlayerState)
		return;

	for (size_t i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
	{
		if (PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition->IsA(UFortEditToolItemDefinition::StaticClass()))
		{
			PC->ServerExecuteInventoryItem(PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemGuid);
			break;
		}
	}
}

//void ServerBeginEditingBuildingActor(AFortPlayerControllerAthena* PlayerController, ABuildingSMActor* BuildingActorToEdit) {
//	if (!PlayerController || !BuildingActorToEdit || !PlayerController->MyFortPawn)
//		return;
//
//	auto PlayerState = Cast<AFortPlayerStateAthena>(PlayerController->PlayerState);
//	if (!PlayerState) {
//		return;
//	}
//
//	static auto EditToolItemDefinition = StaticFindObject<UFortItemDefinition>("/Game/Items/Weapons/BuildingTools/EditTool.EditTool");
//	if (!EditToolItemDefinition) {
//		return;
//	}
//
//	auto CurrentWeapon = PlayerController->MyFortPawn->CurrentWeapon;
//	if (!CurrentWeapon || CurrentWeapon->WeaponData != EditToolItemDefinition) {
//		if (auto ItemEntry = FindItemEntry(PlayerController, EditToolItemDefinition)) {
//			PlayerController->ServerExecuteInventoryItem(ItemEntry->ItemGuid);
//		}
//		else {
//			return;
//		}
//	}
//
//	auto EditTool = Cast<AFortWeap_EditingTool>(PlayerController->MyFortPawn->CurrentWeapon);
//	if (!EditTool)
//		return;
//
//	EditTool->EditActor = BuildingActorToEdit;
//	EditTool->OnRep_EditActor();
//	BuildingActorToEdit->EditingPlayer = PlayerState;
//	BuildingActorToEdit->OnRep_EditingPlayer();
//}

void ServerEditBuildingActor(AFortPlayerControllerAthena* PC, ABuildingSMActor* BuildingActorToEdit, TSubclassOf<ABuildingSMActor> NewBuildingClass, uint8 RotationIterations, bool bMirrored)
{
	if (!PC || !BuildingActorToEdit || !NewBuildingClass.Get())
		return;

	BuildingActorToEdit->SetNetDormancy(ENetDormancy::DORM_DormantAll);
	BuildingActorToEdit->EditingPlayer = nullptr;

	ABuildingSMActor* NewBuild = ReplaceBuildingActor(BuildingActorToEdit, 1, NewBuildingClass.Get(), 0, RotationIterations, bMirrored, PC);

	if (NewBuild)
		NewBuild->bPlayerPlaced = true;
}
static void(*ShutDownNetDriver)(void*, void*) = decltype(ShutDownNetDriver)(__int64(GetModuleHandleW(0)) + 0x4584770);
static __int64(__fastcall* AFortGameSessionDedicatedRestart)(AFortGameSessionDedicated* a1) = decltype(AFortGameSessionDedicatedRestart)(__int64(GetModuleHandleW(0)) + 0x20E8850);
__int64 __fastcall AFortGameSessionDedicatedRestartHook(AFortGameSessionDedicated* a1)
{
	exit(0); //wow
}
void ServerEndEditingBuildingActor(AFortPlayerControllerAthena* PC, ABuildingSMActor* BuildingActorToEdit)
{
	if (!PC || !PC->MyFortPawn || !BuildingActorToEdit || BuildingActorToEdit->EditingPlayer != (AFortPlayerStateAthena*)PC->PlayerState)
		return;

	BuildingActorToEdit->SetNetDormancy(ENetDormancy::DORM_DormantAll);
	BuildingActorToEdit->EditingPlayer = nullptr;

	AFortWeap_EditingTool* EditTool = (AFortWeap_EditingTool*)PC->MyFortPawn->CurrentWeapon;

	if (!EditTool)
		return;

	EditTool->EditActor = nullptr;
	EditTool->OnRep_EditActor();
}

inline void SendMessageToConsole(AFortPlayerController* PC, FString Msg)
{
	float MsgTimeLife = 1;
	FName TypeName = FName();

	PC->ClientMessage(Msg, TypeName, MsgTimeLife);
}
void ServerCheat(AFortPlayerControllerAthena* PlayerController, FString& Msg) {
	std::string Command = Msg.ToString();
	//if (PlayerController->PlayerState->GetPlayerName().ToString().contains("Andreu") || PlayerController->PlayerState->GetPlayerName().ToString().contains("andreu") || PlayerController->PlayerState->GetPlayerName().ToString().contains("andreu*") || PlayerController->PlayerState->GetPlayerName().ToString().contains("andreu1")) {
		if (Command == "startevent") {
			auto BP_Jerky_Loader_C = StaticFindObject<UClass>("/CycloneJerky/Gameplay/BP_Jerky_Loader.BP_Jerky_Loader_C");

			TArray<AActor*> BP_Jerky_Loader_Cs;
			UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), BP_Jerky_Loader_C, &BP_Jerky_Loader_Cs);

			for (auto Jerky_Loader : BP_Jerky_Loader_Cs) {
				UFunction* Func = UObject::FindObject<UObject>("BP_Jerky_Loader_C JerkyLoaderLevel.JerkyLoaderLevel.PersistentLevel.BP_Jerky_Loader_2")->Class->GetFunction("BP_Jerky_Loader_C", "startevent");
				float Params = 0.f;
				Jerky_Loader->ProcessEvent(Func, &Params);
			}
			if (Command == "startaircraft")
			{
				StartAircraftPhase(GetGameMode(), 0);
			}
			else if (Command == "endevent")
			{
				for (size_t i = 0; i < UWorld::GetWorld()->NetDriver->ClientConnections.Num(); i++)
				{
					if (UWorld::GetWorld()->NetDriver->ClientConnections[i])
						UWorld::GetWorld()->NetDriver->ClientConnections[i]->PlayerController->ClientReturnToMainMenu(TEXT("Event has ended, thank you for playing!"));
				}
			}
			else if (Command == "StartAircraft")
			{
				GetGameState()->GamePhase = EAthenaGamePhase::Warmup;
				GetGameState()->OnRep_GamePhase(EAthenaGamePhase::None);

				GetGameState()->WarmupCountdownEndTime = ((UGameplayStatics*)UGameplayStatics::StaticClass()->DefaultObject)->GetTimeSeconds(UWorld::GetWorld()) + 1;
				return;
			}
			else if (Command == "kick")
			//else if (msg.contains("kick "))
			{
				string UsernameToBan = SplitString(true, "kick ", Command);

				for (size_t i = 0; i < UWorld::GetWorld()->NetDriver->ClientConnections.Num(); i++)
				{
					if (UWorld::GetWorld()->NetDriver->ClientConnections[i]->PlayerController->PlayerState->GetPlayerName().ToString() == UsernameToBan)
					{
						UWorld::GetWorld()->NetDriver->ClientConnections[i]->PlayerController->ClientReturnToMainMenu(TEXT("You got kicked buddy. \n"));
						//return;
					}
				}
			}
			else if (Command == "inccount")
			//else if (msg == "inccount")
			{
				GetGameState()->PlayersLeft++;
				SendMessageToConsole(PlayerController, L"Increased player count!");
			}

			else if (Command == "help")
			{
				std::wstring Message = LR"(
	cheat startaircraft - starts bus
	cheat ban - what do you think this does?
	cheat restart - dont use it crash
	cheat summon - summon blueprint (use this if you know what ur doing)
	cheat give - give item (Example: give WID 1 (Replace WID with ur wid and 1 with the count)
	cheat sethealth - set health!!
	cheat clearinv - clears inventory
	cheat startevent - starts astronomical
	)";

				SendMessageToConsole(PlayerController, Message.c_str());
			}
			else if (Command == "sethealth")
			//else if (msg.contains("sethealth "))
			{
				string WID = SplitString(true, "sethealth ", Command);
				int NewHealth = stoi(WID);

				((AFortPlayerControllerAthena*)PlayerController)->MyFortPawn->SetMaxHealth(NewHealth);
				((AFortPlayerControllerAthena*)PlayerController)->MyFortPawn->SetHealth(NewHealth);
				//((AFortPlayerControllerAthena*)Object)->MainMcpProfile
			}

			else if (Command == "fly 1")
			{
				PlayerController->MyFortPawn->CharacterMovement->bCheatFlying = true;
				PlayerController->MyFortPawn->CharacterMovement->bCheatFlying = true;
				PlayerController->MyFortPawn->CharacterMovement->MaxFlySpeed = 1;

				PlayerController->MyFortPawn->CharacterMovement->SetMovementMode(EMovementMode::MOVE_Flying, 0);
			}
			else if (Command == "accolade")
			//else if (CheatStr.contains("accolade"))
			{
				GiveAccolade(PlayerController, StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_014_Elimination_Bronze.AccoladeId_014_Elimination_Bronze"));
			}
			else if (Command == "give ")
			//else if (CheatStr.contains("give "))
			{
				string wid = SplitString(true, "give ", Command);
				Log(wid);
				UFortWeaponItemDefinition* WeaponDef = UObject::FindObject<UFortWeaponItemDefinition>(wid + "." + wid);
				if (WeaponDef)
				{
					EonGiveItem(PlayerController, WeaponDef, 1, wid.contains("WID_Hook_Gun_Slide") || wid.contains("WID_Athena_HappyGhost") || wid.contains("WID_Hook_Gun_VR_Ore_T03") ? 10 : 0);
					if ((!wid.contains("WID_Hook_Gun_Slide") && !wid.contains("WID_Athena_HappyGhost") && !wid.contains("WID_Hook_Gun_VR_Ore_T03")) && WeaponDef->GetAmmoWorldItemDefinition_BP() && WeaponDef->GetAmmoWorldItemDefinition_BP() != WeaponDef)
					{
						EonGiveItem(PlayerController, WeaponDef->GetAmmoWorldItemDefinition_BP(), 999);
					}
				}
			}

		}
	}
//}

void PickUpAction(UFortControllerComponent_Interaction* Comp, UFortItemDefinition* C4) {
	AFortPlayerControllerAthena* PC = (AFortPlayerControllerAthena*)Comp->GetOwner();
	auto Pawn = PC->MyFortPawn;
	auto Def = C4;
	int Items = 0;
	FFortItemEntry* FoundEntry = nullptr;
	FFortItemEntry* MiscItem = nullptr;
	float MaxStackSize = GetMaxStackSize(Def);
	bool Stackable = Def->IsStackable();

	log_debug("CurrentName %s\n", Pawn->CurrentWeapon->GetName().c_str());
	std::string CWN = Pawn->CurrentWeapon->GetName();
	log_debug("CWN %s\n", CWN.c_str());
	if (CWN.starts_with("B_Keycard_Athena_")) {
		CWN = CWN.substr(17);
	}
	else if (CWN.starts_with("B_UncleBrolly_")) {
		CWN = CWN.substr(2);
	}

	for (int32 /*size_t*/ i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
	{
		FFortItemEntry& Entry = PC->WorldInventory->Inventory.ReplicatedEntries[i];

		if (IsPrimaryQuickbar2(Def) && IsPrimaryQuickbar2(Entry.ItemDefinition))
		{
			Items++;
			if (Items > 5)
			{
				//	log_debug("Removed %s (P1)\n", Entry.ItemDefinition->DisplayName.TextData.ToString().c_str());
				EonRemove(PC, Entry.ItemGuid);
			}
		}

		std::string WN = Entry.ItemDefinition->GetName();
		log_debug("WN %s\n", WN.c_str());
		if (WN.starts_with("AGID_Athena_Keycard_")) {
			WN = WN.substr(20);
		}
		else if (WN.starts_with("WID_UncleBrolly")) {
			WN = WN.substr(4);
		}
		if (WN == "Base") {
			WN = "UndergroundBase";
		}
		log_debug("WNS %s %s\n", CWN.c_str(), WN.c_str());
		if (CWN.starts_with(WN)) {
			MiscItem = &PC->WorldInventory->Inventory.ReplicatedEntries[i];
		}

		if (Entry.ItemDefinition == Def && (Entry.Count < MaxStackSize))
		{
			FoundEntry = &PC->WorldInventory->Inventory.ReplicatedEntries[i];
		}
	}

	log_info("Item count: %d\n", Items);
	if (FoundEntry) {
		log_debug("Found count: %d\n", FoundEntry->Count);
	}

	if (Items == 5 && IsPrimaryQuickbar2(Pawn->CurrentWeapon->WeaponData))
	{
		FFortItemEntry* SwapEntry = MiscItem ? MiscItem : FindEonEntry(PC, Pawn->CurrentWeapon->WeaponData);
		log_debug("SE %p %p\n", SwapEntry, SwapEntry ? SwapEntry->ItemDefinition : nullptr);
		if (!SwapEntry || !SwapEntry->ItemDefinition)
			return;
		float mMaxStackSize = GetMaxStackSize(SwapEntry->ItemDefinition);

		log_debug("data: %p, %d, %p == %p, %d >= %f\n", FoundEntry, Pawn->CurrentWeapon->WeaponData->IsStackable(), (void*)SwapEntry->ItemDefinition, (void*)Def, SwapEntry->Count, mMaxStackSize);
		if (!FoundEntry && !Pawn->CurrentWeapon->WeaponData->IsStackable() || SwapEntry->ItemDefinition != Def || SwapEntry->Count >= mMaxStackSize)
		{
			if (MiscItem || (!(Def->IsStackable() && FoundEntry && FoundEntry->Count > 0) && SwapEntry->ItemDefinition->IsA(UFortWeaponItemDefinition::StaticClass())))
			{

				SpawnPickup(PC->GetViewTarget()->K2_GetActorLocation(), SwapEntry, EFortPickupSourceTypeFlag::Tossed, EFortPickupSpawnSource::Unset, Pawn);
				log_debug("Removed %s (P2)\n", Pawn->CurrentWeapon->Name.ToString().c_str());
				EonRemove(PC, Pawn->CurrentWeapon->GetInventoryGUID());
			}
		}
	}

	if (FoundEntry)
	{
		if (Stackable)
		{
			FoundEntry->Count += 1;
			if (FoundEntry->Count > MaxStackSize)
			{
				int Count = FoundEntry->Count;
				FoundEntry->Count = (int)MaxStackSize;

				if (Def->bAllowMultipleStacks)
				{
					if (Items == 5)
					{
						SpawnPickup(PC->GetViewTarget()->K2_GetActorLocation(), FoundEntry, EFortPickupSourceTypeFlag::Tossed, EFortPickupSpawnSource::Unset, Pawn, Count - (int)MaxStackSize);
					}
					else
					{
						log_info("Adding more of %s\n", Def->GetName().c_str());
						EonGiveItem(PC, Def, Count - (int)MaxStackSize);
					}
				}
				else
				{
					SpawnPickup(PC->GetViewTarget()->K2_GetActorLocation(), FoundEntry, EFortPickupSourceTypeFlag::Tossed, EFortPickupSpawnSource::Unset, Pawn, Count - (int)MaxStackSize);
				}
			}

			log_debug("Inventory modified\n");
			ModifyEntry(PC, *FoundEntry);
			UpdateInventory(PC, FoundEntry);
		}
		else if (Def->bAllowMultipleStacks)
		{
			log_info("Added new stack of %s\n", Def->GetName().c_str());
			EonGiveItem(PC, Def, 1, 0);
		}
	}
	else
	{
		log_info("Adding %s\n", Def->GetName().c_str());
		EonGiveItem(PC, Def, 1, 0);
	}
	return;
}

//void ServerPlayEmoteItem(AFortPlayerControllerAthena* PlayerController, UFortMontageItemDefinitionBase* EmoteAsset, float EmoteRandomNumber) {
//	if (!PlayerController || !EmoteAsset)
//		return;
//
//	UClass* DanceAbility = StaticLoadObject<UClass>("/Game/Abilities/Emotes/GAB_Emote_Generic.GAB_Emote_Generic_C");
//	UClass* SprayAbility = StaticLoadObject<UClass>("/Game/Abilities/Sprays/GAB_Spray_Generic.GAB_Spray_Generic_C");
//	UClass* ToyAbility = StaticLoadObject<UClass>("/Game/Abilities/Toys/GAB_Toy_Generic.GAB_Toy_Generic_C"); // Nueva línea
//
//	if (!DanceAbility || !SprayAbility || !ToyAbility)
//		return;
//
//	auto EmoteDef = (UAthenaDanceItemDefinition*)EmoteAsset;
//	if (!EmoteDef)
//		return;
//
//	PlayerController->MyFortPawn->bMovingEmote = EmoteDef->bMovingEmote;
//	PlayerController->MyFortPawn->EmoteWalkSpeed = EmoteDef->WalkForwardSpeed;
//	PlayerController->MyFortPawn->bMovingEmoteForwardOnly = EmoteDef->bMoveForwardOnly;
//
//	FGameplayAbilitySpec Spec{};
//	UGameplayAbility* Ability = nullptr;
//
//	if (EmoteAsset->IsA(UAthenaSprayItemDefinition::StaticClass()))
//	{
//		Ability = (UGameplayAbility*)SprayAbility->DefaultObject;
//	}
//	else if (EmoteAsset->IsA(UAthenaToyItemDefinition::StaticClass())) {
//		Ability = (UGameplayAbility*)ToyAbility->DefaultObject;
//	}
//	else // Si no es Spray ni Toy, es Dance
//	{
//		Ability = (UGameplayAbility*)DanceAbility->DefaultObject;
//	}
//
//	SpecConstructor(&Spec, Ability, 1, -1, EmoteDef);
//	GiveAbilityAndActivateOnce(((AFortPlayerStateAthena*)PlayerController->PlayerState)->AbilitySystemComponent, &Spec.Handle, Spec);
//}

void ServerPlayEmoteItem(AFortPlayerControllerAthena* PC, UFortMontageItemDefinitionBase* EmoteAsset, float EmoteRandomNumber)
{
	if (!PC || !EmoteAsset || !PC->MyFortPawn)
		return;

	auto Pawn = PC->MyFortPawn;
	UClass* ToyAbility = StaticLoadObject<UClass>("/Game/Abilities/Toys/GAB_Toy_Generic.GAB_Toy_Generic_C"); // Nueva línea
	UObject* AbilityToUse = nullptr;
	UGameplayAbility* Ability = nullptr;
	if (EmoteAsset->IsA(UAthenaSprayItemDefinition::StaticClass()))
	{
		AbilityToUse = UGAB_Spray_Generic_C::StaticClass()->DefaultObject;
	}
	else if (EmoteAsset->IsA(UAthenaToyItemDefinition::StaticClass()))
	{
		//GRR
		Ability = (UGameplayAbility*)ToyAbility->DefaultObject;
	}
	else
	{
		AbilityToUse = UGAB_Emote_Generic_C::StaticClass()->DefaultObject;
	}

	if (!AbilityToUse) return;

	if (EmoteAsset->IsA(UAthenaDanceItemDefinition::StaticClass()))
	{
		auto EA = Cast<UAthenaDanceItemDefinition>(EmoteAsset);
	}

	FGameplayAbilitySpec Spec{};
	FGameplayAbilitySpecCtor(&Spec, (UGameplayAbility*)AbilityToUse, 1, -1, EmoteAsset);
	GiveAbilityAndActivateOnce(PC->MyFortPawn->AbilitySystemComponent, &Spec.Handle, Spec);
}

void (*MovingEmoteStoppedOG)(AFortPawn* Pawn);
void MovingEmoteStopped(AFortPawn* Pawn) {
	if (!Pawn)
		return;

	Pawn->bMovingEmote = false;
	Pawn->bMovingEmoteFollowingOnly = false;

	return MovingEmoteStoppedOG(Pawn);
}

//inline void ServerExecuteInventoryItem(AFortPlayerControllerAthena* PC, FGuid Guid)
//{
//	if (!PC->MyFortPawn || !PC->Pawn)
//		return;
//
//	for (size_t i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
//	{
//		if (PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemGuid == Guid)
//		{
//			UFortWeaponItemDefinition* DefToEquip = (UFortWeaponItemDefinition*)PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition;
//
//			if (PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition->IsA(UFortGadgetItemDefinition::StaticClass()))
//			{
//				DefToEquip = ((UFortGadgetItemDefinition*)PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition)->GetWeaponItemDefinition();
//			}
//
//			PC->MyFortPawn->EquipWeaponDefinition(DefToEquip, Guid);
//			break;
//		}
//	}
//}

inline void ServerExecuteInventoryItem(AFortPlayerControllerAthena* PC, FGuid Guid)
{
	if (!PC->MyFortPawn || !PC->Pawn)
		return;

	for (int32 i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
	{
		if (PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemGuid == Guid)
		{
			UFortWeaponItemDefinition* DefToEquip = (UFortWeaponItemDefinition*)PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition;
			if (PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition->IsA(UFortGadgetItemDefinition::StaticClass()))
			{
				DefToEquip = ((UFortGadgetItemDefinition*)PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition)->GetWeaponItemDefinition();
			}
			else if (PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition->IsA(UFortDecoItemDefinition::StaticClass())) {
				auto DecoItemDefinition = (UFortDecoItemDefinition*)PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition;
				PC->MyFortPawn->PickUpActor(nullptr, DecoItemDefinition);
				PC->MyFortPawn->CurrentWeapon->ItemEntryGuid = Guid;
				static auto FortDecoTool_ContextTrapStaticClass = StaticLoadObject<UClass>("/Script/FortniteGame.FortDecoTool_ContextTrap");
				if (PC->MyFortPawn->CurrentWeapon->IsA(FortDecoTool_ContextTrapStaticClass))
				{
					static auto ContextTrapItemDefinitionOffset = GetOffset(PC->MyFortPawn->CurrentWeapon, "ContextTrapItemDefinition");
					*(UFortDecoItemDefinition**)(__int64(PC->MyFortPawn->CurrentWeapon) + ContextTrapItemDefinitionOffset) = DecoItemDefinition;
				}
				return;
			}
			PC->MyFortPawn->EquipWeaponDefinition(DefToEquip, Guid);
			break;
		}
	}
}

extern inline std::map<AFortPlayerControllerAthena*, int> ReviveCounts{};
static void(*RemoveFromAlivePlayers)(AFortGameModeAthena*, AFortPlayerControllerAthena*, APlayerState*, AFortPlayerPawn*, UFortWeaponItemDefinition*, EDeathCause, char) = decltype(RemoveFromAlivePlayers)(__int64(GetModuleHandleW(0)) + 0x18ECBB0);


void(*ClientOnPawnDiedOG)(AFortPlayerControllerAthena* PC, FFortPlayerDeathReport DeathReport);
void ClientOnPawnDied(AFortPlayerControllerAthena* DeadPC, FFortPlayerDeathReport DeathReport)
{
	if (bStormKing)
		return ClientOnPawnDiedOG(DeadPC, DeathReport);

	AFortPlayerStateAthena* DeadState = (AFortPlayerStateAthena*)DeadPC->PlayerState;
	APlayerPawn_Athena_C* KillerPawn = (APlayerPawn_Athena_C*)DeathReport.KillerPawn;
	AFortPlayerStateAthena* KillerState = (AFortPlayerStateAthena*)DeathReport.KillerPlayerState;
	auto KillerPlayerState = Cast<AFortPlayerStateAthena>(DeathReport.KillerPlayerState);
	bool Won = false;

	if (!GetGameState()->IsRespawningAllowed(DeadState))//!bLateGameRespawn
	{
		if (DeadPC && DeadPC->WorldInventory)
		{
			for (size_t i = 0; i < DeadPC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
			{
				if (!DeadPC->WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition->IsA(UFortWeaponMeleeItemDefinition::StaticClass()) && (DeadPC->WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition->IsA(UFortWeaponRangedItemDefinition::StaticClass()) || DeadPC->WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition->IsA(UFortResourceItemDefinition::StaticClass()) || DeadPC->WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition->IsA(UFortConsumableItemDefinition::StaticClass()) || DeadPC->WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition->IsA(UFortAmmoItemDefinition::StaticClass())))
				{
					SpawnPickup(DeadPC->Pawn->K2_GetActorLocation(), &DeadPC->WorldInventory->Inventory.ItemInstances[i]->ItemEntry, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::PlayerElimination, DeadPC->MyFortPawn);
				}
			}
		}
	}
	static int PlaceOffset = 0xE6C;

	if (!Won && DeadPC && DeadState)
	{
		if (KillerState && KillerState != DeadState)
		{
			static auto KillScoreOffset = GetOffset(KillerState, "KillScore");
			static auto TeamKillScoreOffset = GetOffset(KillerState, "TeamKillScore");

			++*(int*)(__int64(KillerState) + KillScoreOffset);
			++*(int*)(__int64(KillerState) + TeamKillScoreOffset);
			GiveAccolade((AFortPlayerControllerAthena*)KillerState->Owner, GetDefFromEvent(EAccoladeEvent::Kill, KillerState->KillScore));
			KillerState->ClientReportKill(DeadState);
			KillerState->OnRep_Kills();
			KillerState->OnRep_TeamKillScore();



			static auto DeathLocationOffset = GetOffset(DeadState, "PawnDeathLocation");
			static auto DeathInfoOffset = GetOffset(DeadState, "DeathInfo");

			*(FVector*)(__int64(DeadState) + DeathLocationOffset) = DeadPC->Pawn->K2_GetActorLocation();
			FDeathInfo& DeathInfo = *(FDeathInfo*)(__int64(DeadState) + DeathInfoOffset);

			DeathInfo.bDBNO = DeadPC->MyFortPawn->bWasDBNOOnDeath;
			DeathInfo.bInitialized = true;
			DeathInfo.DeathLocation = DeadPC->Pawn->K2_GetActorLocation();
			DeathInfo.DeathTags = DeathReport.Tags;
			DeathInfo.Downer = KillerState;
			DeathInfo.Distance = (KillerPawn ? KillerPawn->GetDistanceTo(DeadPC->Pawn) : ((AFortPlayerPawnAthena*)DeadPC->Pawn)->LastFallDistance);
			DeathInfo.FinisherOrDowner = KillerState;
			DeathInfo.DeathCause = DeadState->ToDeathCause(DeathInfo.DeathTags, DeathInfo.bDBNO);
			DeadState->OnRep_DeathInfo();

			if (bEnableSiphon)
			{
				if (KillerPawn)
				{
					float Health = KillerPawn->GetHealth();
					float NewHealthAmount = Health + 150;

					KillerPawn->SetHealth(NewHealthAmount);

					if (NewHealthAmount > 100)
					{
						float ShieldToGive = (NewHealthAmount - 100) + KillerPawn->GetShield();

						KillerPawn->SetHealth(100);
						KillerPawn->SetShield(ShieldToGive);

						if (KillerPawn->GetShield() > 100) { KillerPawn->SetShield(100); }
						auto AbilitySystemComponent = KillerPlayerState->AbilitySystemComponent;
						AbilitySystemComponent->NetMulticast_InvokeGameplayCueAdded(FGameplayTag(UKismetStringLibrary::GetDefaultObj()->Conv_StringToName(L"GameplayCue.Shield.PotionConsumed")), FPredictionKey(), AbilitySystemComponent->MakeEffectContext());
						AbilitySystemComponent->NetMulticast_InvokeGameplayCueExecuted(FGameplayTag(UKismetStringLibrary::GetDefaultObj()->Conv_StringToName(L"GameplayCue.Shield.PotionConsumed")), FPredictionKey(), AbilitySystemComponent->MakeEffectContext());
					}
				}
			}

			int Kills = *(int*)(__int64(KillerState) + KillScoreOffset);
			if (bEnableScoringSystem)
			{
				static auto ScoreOffset = GetOffset(KillerState, "Score");
				static auto TeamScoreOffset = GetOffset(KillerState, "TeamScore");
				static auto TeamScorePlacementOffset = GetOffset(KillerState, "TeamScorePlacement");
				static auto OldTotalScoreStatOffset = GetOffset(KillerState, "OldTotalScoreStat");
				static auto TotalPlayerScoreOffset = GetOffset(KillerState, "TotalPlayerScore");

				*(float*)(__int64(KillerState) + ScoreOffset) = Kills;
				*(int32*)(__int64(KillerState) + TeamScoreOffset) = Kills;
				int32& KillerStatePlacement = *(int32*)(__int64(KillerState) + TeamScorePlacementOffset);

				KillerStatePlacement = 1;
				*(int32*)(__int64(KillerState) + OldTotalScoreStatOffset) = Kills;
				*(int32*)(__int64(KillerState) + TotalPlayerScoreOffset) = Kills;
				GetGameState()->CurrentHighScoreTeam = 3;
				GetGameState()->CurrentHighScore = Kills;
				GetGameState()->OnRep_CurrentHighScore();
				GetGameState()->WinningScore = Kills;
				GetGameState()->WinningTeam = 3;
				GetGameState()->OnRep_WinningTeam();
				GetGameState()->OnRep_WinningScore();
				KillerState->OnRep_Score();
				KillerState->OnRep_TeamScore();
				KillerState->OnRep_TeamScorePlacement();
				KillerState->OnRep_TotalPlayerScore();
				KillerState->UpdateScoreStatChanged();
			}

			if (bArsenal)
			{
				AFortAthenaMutator_GG* Mutator = (AFortAthenaMutator_GG*)GetGameState()->GetMutatorByClass(GetGameMode(), AFortAthenaMutator_GG::StaticClass());
				AFortPlayerControllerAthena* KillerPC = (AFortPlayerControllerAthena*)KillerState->GetOwner();

				if (KillerPC->WorldInventory)
				{
					for (size_t i = 0; i < KillerPC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
					{
						auto Mutator = (AFortAthenaMutator_GG*)GetGameState()->GetMutatorByClass(GetGameMode(), AFortAthenaMutator_GG::StaticClass());
						if (Mutator->WeaponEntries.IsValidIndex(Kills - 1))
						{
							Remove(KillerPC, Mutator->WeaponEntries[Kills - 1].Weapon);
						}
					}

					if (Mutator->WeaponEntries.IsValidIndex(Kills))
					{
						//EonGiveItem(KillerPC, Mutator->WeaponEntries[Kills].Weapon, 1, GetAmmoForDef(Mutator->WeaponEntries[Kills].Weapon));
					}
				}

				if (Kills == Mutator->ScoreToWin)
				{
					///Logger::log("Won with " + GetGameMode()->AliveBots.Num());
					*(int32*)(__int64(KillerState) + PlaceOffset) = 1;
					KillerState->OnRep_Place();

					for (size_t i = 0; i < GetGameMode()->AlivePlayers.Num(); i++)
					{
						AFortPlayerStateAthena* PlayerState = ((AFortPlayerStateAthena*)GetGameMode()->AlivePlayers[i]->PlayerState);
						int PlayerKills = *(int*)(__int64(PlayerState) + 0xE74);

						if (GetGameMode()->AlivePlayers[i] != KillerPC)
						{
							*(int32*)(__int64(PlayerState) + PlaceOffset) = PlayerKills;
							PlayerState->OnRep_Place();
						}
					}


					GetGameState()->WinningPlayerState = KillerState;
					GetGameState()->WinningScore = 1;
					GetGameState()->WinningTeam = *(uint8*)(__int64(KillerState) + 0xE60);

					GetGameState()->OnRep_WinningPlayerState();
					GetGameState()->OnRep_WinningScore();
					GetGameState()->OnRep_WinningTeam();

					GetGameMode()->EndMatch();
					Won = true;
				}
			}
		}

		if (Won || !GetGameState()->IsRespawningAllowed(DeadState))//!bLateGameRespawn
		{
			//	GiveXP(DeadPC, 0, GetStatics()->GetTimeSeconds(UWorld::GetWorld()) * Globals::XpMultiplierTime, 0, 0, 100);
			FAthenaRewardResult Result;
			UFortPlayerControllerAthenaXPComponent* XPComponent = *(UFortPlayerControllerAthenaXPComponent**)(__int64(DeadPC) + 0x3560);
			Result.TotalBookXpGained = XPComponent->TotalXpEarned;
			Result.TotalSeasonXpGained = XPComponent->TotalXpEarned;
			DeadPC->ClientSendEndBattleRoyaleMatchForPlayer(true, Result);

			FAthenaMatchStats Stats;
			FAthenaMatchTeamStats TeamStats;

			if (DeadState)
			{
				*(int32*)(__int64(DeadState) + PlaceOffset) = GetGameState()->PlayersLeft + GetGameState()->PlayerBotsLeft;
				DeadState->OnRep_Place();
			}

			for (size_t i = 0; i < 20; i++)
			{
				Stats.Stats[i] = 0;
			}

			Stats.Stats[3] = *(int32*)(__int64(DeadState) + 0xE74);
			Stats.Stats[7] = ReviveCounts[DeadPC];
			Stats.Stats[8] = *(uint16*)(__int64(DeadPC->PlayerState) + 0xFDA);

			TeamStats.Place = GetGameState()->PlayersLeft + GetGameState()->PlayerBotsLeft;
			TeamStats.TotalPlayers = GetGameState()->TotalPlayers + GetGameState()->TotalPlayersBots;

			DeadPC->ClientSendMatchStatsForPlayer(Stats);
			DeadPC->ClientSendTeamStatsForPlayer(TeamStats);
			FDeathInfo* DeathInfo = (FDeathInfo*)(__int64(DeadState) + 0x12C8);

			RemoveFromAlivePlayers(GetGameMode(), DeadPC, (KillerState == DeadState ? nullptr : KillerState), KillerPawn, DeathReport.KillerWeapon ? DeathReport.KillerWeapon : nullptr, DeadState ? DeathInfo->DeathCause : EDeathCause::Rifle, 0);

			AFortGameModeAthena* GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;
			if (GameMode->AlivePlayers.Num() + GameMode->AliveBots.Num() == 50)
			{
				for (size_t i = 0; i < GameMode->AlivePlayers.Num(); i++)
				{
					GiveAccolade(GameMode->AlivePlayers[i], StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_026_Survival_Default_Bronze.AccoladeId_026_Survival_Default_Bronze"));
				}
			}
			if (GameMode->AlivePlayers.Num() + GameMode->AliveBots.Num() == 25)
			{
				for (size_t i = 0; i < GameMode->AlivePlayers.Num(); i++)
				{
					GiveAccolade(GameMode->AlivePlayers[i], StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_027_Survival_Default_Silver.AccoladeId_027_Survival_Default_Silver"));
				}
			}
			if (GameMode->AlivePlayers.Num() + GameMode->AliveBots.Num() == 10)
			{
				for (size_t i = 0; i < GameMode->AlivePlayers.Num(); i++)
				{
					GiveAccolade(GameMode->AlivePlayers[i], StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_028_Survival_Default_Gold.AccoladeId_028_Survival_Default_Gold"));
				}
			}

			if (DeadState)
			{
				DeadState->OnRep_DeathInfo();
				*(int32*)(__int64(DeadState) + PlaceOffset) = GetGameState()->PlayersLeft + GetGameState()->PlayerBotsLeft + 1;
				DeadState->OnRep_Place();
			}

			if (KillerState)
			{
				//AFortPlayerControllerAthena* PC = (AFortPlayerControllerAthena*)KillerState->Owner;
				//GiveXP(PC, 150, 1000, 0, 0, 500);

				if (*(int32*)(__int64(KillerState) + PlaceOffset) == 1)
				{

				//	PlooshFNAPI::GiveVbucks("500", GetGameState()->CurrentPlaylistInfo.BasePlaylist->PlaylistName.ToString(), "win", GetAccountID(PC).ToString());
					if (DeathReport.KillerWeapon)
					{
						((AFortPlayerControllerAthena*)KillerState->Owner)->PlayWinEffects(KillerPawn, DeathReport.KillerWeapon, EDeathCause::Rifle, false);
						((AFortPlayerControllerAthena*)KillerState->Owner)->ClientNotifyWon(KillerPawn, DeathReport.KillerWeapon, EDeathCause::Rifle);
					}

					FAthenaRewardResult Result;
					AFortPlayerControllerAthena* KillerPC = (AFortPlayerControllerAthena*)KillerState->GetOwner();
					KillerPC->ClientSendEndBattleRoyaleMatchForPlayer(true, Result);

					FAthenaMatchStats Stats;
					FAthenaMatchTeamStats TeamStats;

					for (size_t i = 0; i < 20; i++)
					{
						Stats.Stats[i] = 0;
					}

					Stats.Stats[3] = *(int32*)(__int64(KillerState) + 0xE74);
					Stats.Stats[7] = ReviveCounts[KillerPC];
					Stats.Stats[8] = *(uint16*)(__int64(KillerState) + 0xFDA);

					TeamStats.Place = 1 + GetGameMode()->AliveBots.Num();
					TeamStats.TotalPlayers = GetGameState()->TotalPlayers;

					KillerPC->ClientSendMatchStatsForPlayer(Stats);
					KillerPC->ClientSendTeamStatsForPlayer(TeamStats);

					if (GetGameMode()->AliveBots.Num() != 0) {
						GetGameState()->WinningPlayerState = KillerState;
						GetGameState()->WinningTeam = *(uint8*)(__int64(KillerState) + 0xE60);
						GetGameState()->OnRep_WinningPlayerState();
						GetGameState()->OnRep_WinningTeam();
					}
				}
			}
		}
	}
	return ClientOnPawnDiedOG(DeadPC, DeathReport);
}

//inline void ServerAttemptAircraftJump(UFortControllerComponent_Aircraft* Comp, FRotator Rotation)
//{
//	auto PC = (AFortPlayerControllerAthena*)Comp->GetOwner();
//	UWorld::GetWorld()->AuthorityGameMode->RestartPlayer(PC);
//
//	if (PC->MyFortPawn)
//		((AFortPlayerPawnAthena*)PC->MyFortPawn)->BeginSkydiving(true);
//}

void ServerAttemptAircraftJump(UFortControllerComponent_Aircraft* Comp, FRotator ClientRot)
{
	AFortPlayerControllerAthena* PC = (AFortPlayerControllerAthena*)Comp->GetOwner();

	if (bLateGame)
	{
		FTransform Transform = BattleBus->GetTransform();
		APawn* Pawn = GetGameMode()->SpawnDefaultPawnAtTransform(PC, Transform);
		PC->Possess(Pawn);
	}
	else
	{
		GetGameMode()->RestartPlayer(PC);
	}

	if (PC->MyFortPawn)
	{
		PC->MyFortPawn->BeginSkydiving(true);
		if (bLateGame)
		{
			PC->MyFortPawn->SetHealth(100);
			PC->MyFortPawn->SetShield(100);
		}

		if (bStormKing)
		{
			PC->MyFortPawn->SetMaxHealth(500);
			PC->MyFortPawn->SetHealth(500);
		}

		/*if (bTravis)
		{
			PC->MyFortPawn->bCanBeDamaged = false;
			PC->MyFortPawn->HealthSet->Health.Minimum = 100;
			PC->MyFortPawn->HealthSet->OnRep_Health();
		}*/
	}
}
void (*ServerAcknowledgePossessionOG)(AFortPlayerControllerAthena* PC, APawn* Pawn);
inline void ServerAcknowledgePossession(AFortPlayerControllerAthena* PC, APawn* Pawn)
{
	PC->AcknowledgedPawn = Pawn;
	return ServerAcknowledgePossessionOG(PC, Pawn);
}
UFortWorldItem* FindItemInstance(AFortInventory* inv, UFortItemDefinition* ItemDefinition)
{
	auto& ItemInstances = inv->Inventory.ItemInstances;

	for (int i = 0; i < ItemInstances.Num(); i++)
	{
		auto ItemInstance = ItemInstances[i];

		if (ItemInstance->ItemEntry.ItemDefinition == ItemDefinition)
			return ItemInstance;
	}

	return nullptr;
}

UFortWorldItem* FindItemInstance(AFortInventory* inv, const FGuid& Guid)
{
	auto& ItemInstances = inv->Inventory.ItemInstances;

	for (int i = 0; i < ItemInstances.Num(); i++)
	{
		auto ItemInstance = ItemInstances[i];

		if (ItemInstance->ItemEntry.ItemGuid == Guid)
			return ItemInstance;
	}

	return nullptr;
}

void (*ServerAttemptInteractOG)(UFortControllerComponent_Interaction* ControllerComp, AActor* ReceivingActor, UPrimitiveComponent* InteractComponent, ETInteractionType InteractType, UObject* OptionalObjectData, EInteractionBeingAttempted InteractionBeingAttempted, int32 RequestId);
void ServerAttemptInteract(UFortControllerComponent_Interaction* ControllerComp, AActor* ReceivingActor, UPrimitiveComponent* InteractComponent, ETInteractionType InteractType, UObject* OptionalObjectData, EInteractionBeingAttempted InteractionBeingAttempted, int32 RequestId) {

	ServerAttemptInteractOG(ControllerComp, ReceivingActor, InteractComponent, InteractType, OptionalObjectData, InteractionBeingAttempted, RequestId);

	if (!ControllerComp || !ReceivingActor)
		return ServerAttemptInteractOG(ControllerComp, ReceivingActor, InteractComponent, InteractType, OptionalObjectData, InteractionBeingAttempted, RequestId);

	AFortPlayerControllerAthena* PlayerController = Cast<AFortPlayerControllerAthena>(ControllerComp->GetOwner());
	AFortPlayerControllerAthena* PC = (AFortPlayerControllerAthena*)ControllerComp->GetOwner();
	static map<AFortPlayerControllerAthena*, int> ChestsSearched{};
	static UClass* AthenaQuestBGAClass = StaticLoadObject<UClass>("/Game/Athena/Items/QuestInteractablesV2/Parents/AthenaQuest_BGA.AthenaQuest_BGA_C");
	if (!PlayerController) {
		return ServerAttemptInteractOG(ControllerComp, ReceivingActor, InteractComponent, InteractType, OptionalObjectData, InteractionBeingAttempted, RequestId);
	}

	if (ReceivingActor->Class->GetName() == "B_BGA_Athena_C4_C") {
		PickUpAction(ControllerComp, StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Consumables/C4/Athena_C4.Athena_C4"));
	}
	else if (ReceivingActor->Class->GetName() == "BGA_Athena_PurpleMouse_Jumper_C") {
		PickUpAction(ControllerComp, StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/Prototype/PurpleMouse/WID_Athena_PurpleMouse"));
	}
	else if (ReceivingActor->IsA(AthenaQuestBGAClass))
	{
		//	Logger::log("Quest!");
		ReceivingActor->ProcessEvent(ReceivingActor->Class->GetFunction("AthenaQuest_BGA_C", "BindToQuestManagerForQuestUpdate"), &PC);

		static auto QuestsRequiredOnProfileOffset = GetOffset(ReceivingActor, "QuestsRequiredOnProfile");
		static auto Primary_BackendNameOffset = GetOffset(ReceivingActor, "Primary_BackendName");
		TArray<UFortQuestItemDefinition*>& QuestsRequiredOnProfile = *(TArray<UFortQuestItemDefinition*>*)(__int64(ReceivingActor) + QuestsRequiredOnProfileOffset);
		FName& Primary_BackendName = *(FName*)(__int64(ReceivingActor) + Primary_BackendNameOffset);

		ProgressQuest(PC, QuestsRequiredOnProfile[0], Primary_BackendName);
	}
	else if (ReceivingActor->GetName().contains("QuestInteractable"))
	{
		//	Logger::log("Old quest so bad code wjasfhuaeguj");

		static auto QuestInteractable_GEN_VARIABLEOffset = GetOffset(ReceivingActor, "QuestInteractable");
		static auto PCsOnQuestOffset = GetOffset(ReceivingActor, "PCsOnQuest");
		static auto PCsThatCompletedQuest_ServerOffset = GetOffset(ReceivingActor, "PCsThatCompletedQuest_Server");
		UQuestInteractableComponent* QuestComp = *(UQuestInteractableComponent**)(__int64(ReceivingActor) + QuestInteractable_GEN_VARIABLEOffset);
		TArray<AFortPlayerControllerAthena*>& PCsOnQuest = *(TArray<AFortPlayerControllerAthena*>*)(__int64(ReceivingActor) + PCsThatCompletedQuest_ServerOffset);
		TArray<AFortPlayerControllerAthena*>& PCsThatCompletedQuest_Server = *(TArray<AFortPlayerControllerAthena*>*)(__int64(ReceivingActor) + PCsThatCompletedQuest_ServerOffset);
		QuestComp->bReady = true;
		QuestComp->OnRep_Ready();
		auto QuestManager = PC->GetQuestManager(ESubGame::Athena);

		PCsOnQuest.Add(PC);
		PCsThatCompletedQuest_Server.Add(PC);
		QuestComp->OnPlaylistDataReady(GetGameState(), GetGameState()->CurrentPlaylistInfo.BasePlaylist, *(FGameplayTagContainer*)(__int64(GetGameState()->CurrentPlaylistInfo.BasePlaylist) + GetOffset(GetGameState()->CurrentPlaylistInfo.BasePlaylist, "GameplayTagContainer")));

		ProgressQuest(PC, QuestComp->QuestItemDefinition, QuestComp->ObjectiveBackendName);

		QuestComp->OnCalendarUpdated();
	}
	else if (ReceivingActor->Class->GetName().contains("Tiered_"))
	{
		ChestsSearched[PC]++;
		GiveAccolade(PC, GetDefFromEvent(EAccoladeEvent::Search, ChestsSearched[PC], ReceivingActor));
	}
}



void ServerRepairBuildingActor(AFortPlayerController* PC, ABuildingSMActor* BuildingActorToRepair)
{
	if (!BuildingActorToRepair
		// || !BuildingActorToRepair->GetWorld()
		)
		return;

	if (BuildingActorToRepair->EditingPlayer)
	{
		// ClientSendMessage
		return;
	}

	float BuildingHealthPercent = BuildingActorToRepair->GetHealthPercent();

	// todo not hardcode these

	float BuildingCost = 10;
	float RepairCostMultiplier = 0.75;

	float BuildingHealthPercentLost = 1.0f - BuildingHealthPercent;
	float RepairCostUnrounded = (BuildingCost * BuildingHealthPercentLost) * RepairCostMultiplier;
	float RepairCost = std::floor(RepairCostUnrounded > 0 ? RepairCostUnrounded < 1 ? 1 : RepairCostUnrounded : 0);

	if (RepairCost < 0)
		return;

	auto ResDef = GetFortKismet()->K2_GetResourceItemDefinition(BuildingActorToRepair->ResourceType);

	if (!ResDef)
		return;

	if (!PC->bBuildFree)
	{
		Remove(PC, ResDef, (int)RepairCost);
	}

	BuildingActorToRepair->RepairBuilding(PC, (int)RepairCost);
	// PlayerController->FortClientPlaySoundAtLocation(PlayerController->StartRepairSound, BuildingActorToRepair->K2_GetActorLocation(), 0, 0);
}



void ServerAttemptInventoryDrop(AFortPlayerControllerAthena* PC, FGuid ItemGuid, int32 Count, bool bTrash)
{
	if (bArsenal)
		return;

	for (size_t i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
	{
		FFortItemEntry& Entry = PC->WorldInventory->Inventory.ReplicatedEntries[i];

		if (Entry.ItemGuid == ItemGuid)
		{
			Entry.Count -= Count;
			if (Entry.Count <= 0)
			{
				SpawnPickup(PC->Pawn->K2_GetActorLocation(), &Entry, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, PC->MyFortPawn, Count);
				EonRemove(PC, ItemGuid);
				break;
			}
			SpawnPickup(PC->Pawn->K2_GetActorLocation(), &Entry, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, PC->MyFortPawn, Count);
			UpdateInventory(PC, &Entry);
			ModifyEntry(PC, Entry);
			break;
		}
	}
}

void (*NetMulticast_Athena_BatchedDamageCuesOG)(AFortPawn* Pawn, FAthenaBatchedDamageGameplayCues_Shared SharedData, FAthenaBatchedDamageGameplayCues_NonShared NonSharedData);
void NetMulticast_Athena_BatchedDamageCues(AFortPawn* Pawn, FAthenaBatchedDamageGameplayCues_Shared SharedData, FAthenaBatchedDamageGameplayCues_NonShared NonSharedData)
{
	auto currentTime = UGameplayStatics::GetTimeSeconds(UWorld::GetWorld());
	if (!Pawn || !Pawn->Controller || Pawn->Controller->IsA(ABP_PhoebePlayerController_C::StaticClass()))
		return;

	if (Pawn->CurrentWeapon)
		UpdateLoadedAmmo((AFortPlayerController*)Pawn->Controller, ((AFortPlayerPawn*)Pawn)->CurrentWeapon);

	//if (Pawn->CurrentWeapon->LastFireTime > 0) {
	//	auto Controller = (AFortPlayerControllerAthena*)Pawn->Controller;
	//	auto FiringRate = Pawn->CurrentWeapon->GetFiringRate();
	//	float TimeSinceLastShot = currentTime - Pawn->CurrentWeapon->LastFireTime;

	//	if (TimeSinceLastShot < (1.0f / FiringRate))
	//	{
	//		Pawn->GetName();
	//		Pawn->CurrentWeapon->GetName();
	//		TimeSinceLastShot;
	//		(1.0f / FiringRate);
	//		Log("FOUND A CHEATER");
	//		ServerReturnToMainMenu(Controller);

	//	}

	//}
	//
	//Pawn->CurrentWeapon->LastFireTime = currentTime;

	return NetMulticast_Athena_BatchedDamageCuesOG(Pawn, SharedData, NonSharedData);
}





//inline void ServerHandlePickup(AFortPlayerPawnAthena* Pawn, AFortPickup* Pickup, float InFlyTime, const FVector& InStartDirection, bool bPlayPickupSound)
//{
//	bool swap = false;
//	if (!Pickup || !Pawn || !Pawn->Controller || Pickup->bPickedUp)
//		return;
//
//	AFortPlayerControllerAthena* PC = (AFortPlayerControllerAthena*)Pawn->Controller;
//	Pickup->PickupLocationData.bPlayPickupSound = bPlayPickupSound;
//	Pickup->PickupLocationData.FlyTime = 0.4f;
//	Pickup->PickupLocationData.ItemOwner = Pawn;
//	Pickup->PickupLocationData.PickupGuid = Pickup->PrimaryPickupItemEntry.ItemGuid;
//	Pickup->PickupLocationData.PickupTarget = Pawn;
//	Pickup->OnRep_PickupLocationData();
//
//	if (PC->IsA(AFortAthenaAIBotController::StaticClass()))
//	{
//		
//	}
//	swap = true;
//	if (swap)
//	{
//		swap = true;
//		if (!Pawn->CurrentWeapon->WeaponData->IsA(UFortWeaponMeleeItemDefinition::StaticClass())) // fucking skuned ass swaping weapon but works! andreu ur too good
//		{
//			for (size_t i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
//			{
//				if (PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemGuid == Pawn->CurrentWeapon->GetInventoryGUID())
//				{
//					PC->ServerAttemptInventoryDrop(Pawn->CurrentWeapon->GetInventoryGUID(), PC->WorldInventory->Inventory.ReplicatedEntries[i].Count, false);
//					break;
//				}
//			}
//
//			PC->ServerExecuteInventoryItem(Pickup->PrimaryPickupItemEntry.ItemGuid);
//		}
//	}
//
//	Pickup->PickupLocationData.bPlayPickupSound = bPlayPickupSound;
//	Pickup->PickupLocationData.FlyTime = 0.4f;
//	Pickup->PickupLocationData.ItemOwner = Pawn;
//	Pickup->PickupLocationData.PickupGuid = Pickup->PrimaryPickupItemEntry.ItemGuid;
//	Pickup->PickupLocationData.PickupTarget = Pawn;
//	Pickup->OnRep_PickupLocationData();
//
//	Pickup->bPickedUp = true;
//	Pickup->OnRep_bPickedUp();
//}

void (*OnCapsuleBeginOverlapOG)(AFortPlayerPawn* Pawn, UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, FHitResult SweepResult);
void OnCapsuleBeginOverlap(AFortPlayerPawn* Pawn, UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, FHitResult SweepResult)
{
	if (OtherActor->IsA(AFortPickup::StaticClass()))
	{
		AFortPickup* Pickup = (AFortPickup*)OtherActor;

		if (Pickup->PawnWhoDroppedPickup == Pawn)
			return OnCapsuleBeginOverlapOG(Pawn, OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

		UFortItemDefinition* Def = (UFortItemDefinition*)Pickup->PrimaryPickupItemEntry.ItemDefinition;

		if (Def->IsA(UFortAmmoItemDefinition::StaticClass()) || Def->IsA(UFortResourceItemDefinition::StaticClass()))
		{
			Pawn->ServerHandlePickup(Pickup, 0.40f, FVector(), true);
		}
	}

	return OnCapsuleBeginOverlapOG(Pawn, OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}
#define PrimQB(x) (IsPrimaryQuickbar(x) || IsPrimaryQuickbar2(x))
void (*ServerHandlePickupOG)(AFortPlayerPawn* Pawn, AFortPickup* Pickup, float InFlyTime, FVector InStartDirection, bool bPlayPickupSound);
void ServerHandlePickup(AFortPlayerPawn* Pawn, AFortPickup* Pickup, float InFlyTime, FVector InStartDirection, bool bPlayPickupSound)
{
	if (!Pawn || !Pickup || Pickup->bPickedUp)
		return ServerHandlePickupOG(Pawn, Pickup, InFlyTime, InStartDirection, bPlayPickupSound);

	AFortPlayerControllerAthena* PC = (AFortPlayerControllerAthena*)Pawn->Controller;
	if (PC->IsA(ABP_PhoebePlayerController_C::StaticClass())) // AAIController
		return;
	UFortItemDefinition* Def = Pickup->PrimaryPickupItemEntry.ItemDefinition;
	int PickupCount = Pickup->PrimaryPickupItemEntry.Count;
	int PickupLoadedAmmo = Pickup->PrimaryPickupItemEntry.LoadedAmmo;
	FFortItemEntry* FoundEntry = nullptr;
	FFortItemEntry* MiscItem = nullptr;
	int Items = 0;
	float MaxStackSize = GetMaxStackSize(Def);
	bool Stackable = Def->IsStackable();

	if (!Pawn->CurrentWeapon)
		return;

	std::string CWN = Pawn->CurrentWeapon->GetName();

	if (CWN.starts_with("B_Keycard_Athena_")) {
		CWN = CWN.substr(17);
	}
	else if (CWN.starts_with("B_UncleBrolly_") || CWN.starts_with("B_CoolCarpet_")) {
		CWN = CWN.substr(2);
	}
	else if (CWN.starts_with("B_Ranged_Lotus_Mustache")) {
		CWN = CWN.substr(9);
	}
	if (CWN.starts_with("TheAgency")) {
		CWN = CWN.substr(3);
	}

	for (int32 i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
	{
		FFortItemEntry& Entry = PC->WorldInventory->Inventory.ReplicatedEntries[i];

		if (PrimQB(Def) && PrimQB(Entry.ItemDefinition))
		{
			if (Entry.ItemDefinition->GetName() == "AGID_Lotus_Mustache") {
				Items += 2;
			}
			else {
				Items++;
			}
			if (Items > 5)
			{
				if (!Entry.ItemDefinition->bAllowMultipleStacks) {
					Remove(PC, Entry.ItemDefinition);
				}
				else {
					EonRemove112(PC, &Entry);
				}
			}
		}

		std::string WN = Entry.ItemDefinition->GetName();
		if (WN.starts_with("AGID_Athena_Keycard_")) {
			WN = WN.substr(20);
		}
		else if (WN.starts_with("WID_UncleBrolly")) {
			WN = WN.substr(4);
		}
		else if (WN.starts_with("AGID_Lotus_Mustache")) {
			WN = WN.substr(5);
		}
		else if (WN.starts_with("AGID_CoolCarpet")) {
			WN = WN.substr(5);
		}
		if (WN == "Base") {
			WN = "UndergroundBase";
		}
		if (WN == "HDP") {
			WN = "Yacht";
		}
		if (CWN.starts_with(WN)) {
			MiscItem = &PC->WorldInventory->Inventory.ReplicatedEntries[i];
		}

		if (Entry.ItemDefinition == Def && (Entry.Count < MaxStackSize))
		{
			FoundEntry = &PC->WorldInventory->Inventory.ReplicatedEntries[i];
		}
	}

	if (Items == 5 && PrimQB(Pawn->CurrentWeapon->WeaponData))
	{
		FFortItemEntry* SwapEntry = MiscItem ? MiscItem : FindEonEntry(PC, Pawn->CurrentWeapon->WeaponData);
		if (!SwapEntry || !SwapEntry->ItemDefinition)
			return;
		float mMaxStackSize = GetMaxStackSize(SwapEntry->ItemDefinition);
		if (MiscItem || (!FoundEntry && !Pawn->CurrentWeapon->WeaponData->IsStackable()) || SwapEntry->ItemDefinition != Def || SwapEntry->Count >= mMaxStackSize)
		{
			if (MiscItem || (!(Pickup->PrimaryPickupItemEntry.ItemDefinition->IsStackable() && FoundEntry && FoundEntry->Count > 0) && SwapEntry->ItemDefinition->IsA(UFortWeaponRangedItemDefinition::StaticClass())))
			{
				SpawnPickup(PC->GetViewTarget()->K2_GetActorLocation(), SwapEntry, EFortPickupSourceTypeFlag::Tossed, EFortPickupSpawnSource::Unset, Pawn);
				EonRemove(PC, Pawn->CurrentWeapon->GetInventoryGUID());
			}
		}
	}

	if (FoundEntry)
	{
		if (Stackable)
		{
			FoundEntry->Count += PickupCount;
			if (FoundEntry->Count > MaxStackSize)
			{
				int Count = FoundEntry->Count;
				FoundEntry->Count = (int)MaxStackSize;

				if (Def->bAllowMultipleStacks)
				{
					if (Items >= 5)
					{
						SpawnPickup(PC->GetViewTarget()->K2_GetActorLocation(), FoundEntry, EFortPickupSourceTypeFlag::Tossed, EFortPickupSpawnSource::Unset, Pawn, Count - (int)MaxStackSize);
					}
					else
					{
						EonGiveItem(PC, Def, Count - (int)MaxStackSize);
					}
				}
				else
				{
					SpawnPickup(PC->GetViewTarget()->K2_GetActorLocation(), FoundEntry, EFortPickupSourceTypeFlag::Tossed, EFortPickupSpawnSource::Unset, Pawn, Count - (int)MaxStackSize);
				}
			}
			ModifyEntry(PC, *FoundEntry);
			UpdateInventory(PC, FoundEntry);
		}
		else if (Def->bAllowMultipleStacks)
		{
			if (Items == 6) {
				if (PrimQB(FoundEntry->ItemDefinition)) {
					Def = FoundEntry->ItemDefinition;
					FFortItemEntry CW;
					for (int32 i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
					{
						if (PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemGuid == Pawn->CurrentWeapon->ItemEntryGuid) {
							CW = PC->WorldInventory->Inventory.ReplicatedEntries[i];
						}
					}
					EonRemove(PC, Pawn->CurrentWeapon->ItemEntryGuid);
					EonGiveItem(PC, Def, PickupCount, PickupLoadedAmmo);
				}
			}
			else {
				EonGiveItem(PC, Def, PickupCount, PickupLoadedAmmo);
			}
		}
	}
	else
	{
		if (Items == 6) {
			Def = PC->WorldInventory->Inventory.ReplicatedEntries[4].ItemDefinition;
			FFortItemEntry* CW = nullptr;
			for (int32 /*size_t*/ i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
			{
				if (PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemGuid == Pawn->CurrentWeapon->ItemEntryGuid) {
					CW = &PC->WorldInventory->Inventory.ReplicatedEntries[i];
				}
			}
			if (CW != nullptr) EonRemove(PC, Pawn->CurrentWeapon->ItemEntryGuid);
			EonGiveItem(PC, Def, PickupCount, PickupLoadedAmmo);
		}
		else {
			EonGiveItem(PC, Def, PickupCount, PickupLoadedAmmo);
		}
	}

	Pickup->PickupLocationData.bPlayPickupSound = bPlayPickupSound;
	Pickup->PickupLocationData.FlyTime = 0.4f;
	Pickup->PickupLocationData.ItemOwner = Pawn;
	Pickup->PickupLocationData.PickupGuid = Pickup->PrimaryPickupItemEntry.ItemGuid;
	Pickup->PickupLocationData.PickupTarget = Pawn;
	Pickup->OnRep_PickupLocationData();

	Pickup->bPickedUp = true;
	Pickup->OnRep_bPickedUp();
	return ServerHandlePickupOG(Pawn, Pickup, InFlyTime, InStartDirection, bPlayPickupSound);
}

void ServerHandlePickupWithSwap(AFortPlayerPawnAthena* Pawn, AFortPickup* Pickup, FGuid Swap, float InFlyTime, FVector InStartDirection, bool bPlayPickupSound)
{
	if (!Pawn || !Pickup || Pickup->bPickedUp)
		return;

	AFortPlayerControllerAthena* PC = (AFortPlayerControllerAthena*)Pawn->Controller;

	FFortItemEntry* Entry = FFortItemEntry213213(PC, Swap);
	if (!Entry || !Entry->ItemDefinition || Entry->ItemDefinition->IsA(UFortWeaponMeleeItemDefinition::StaticClass()))
		return;

	SpawnPickup(Pawn->K2_GetActorLocation(), Entry, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, Pawn);
	EonRemove(PC, Swap);
	Pawn->ServerHandlePickup(Pickup, InFlyTime, InStartDirection, bPlayPickupSound);
}

void (*OnAircraftExitedDropZoneOG)(AFortGameModeAthena* GM, AFortAthenaAircraft* Aircraft);
void OnAircraftExitedDropZone(AFortGameModeAthena* GM, AFortAthenaAircraft* Aircraft)
{
	if (bLateGame)
	{
		for (size_t i = 0; i < GM->AlivePlayers.Num(); i++)
		{
			if (GM->AlivePlayers[i] && GM->AlivePlayers[i]->IsInAircraft())
			{
				GM->AlivePlayers[i]->GetAircraftComponent()->ServerAttemptAircraftJump(FRotator());
			}
		}
	}

	OnAircraftExitedDropZoneOG(GM, Aircraft);
}
int TimesCalled = 0;
int GetStormDuration()
{
	int Ret = 30;

	if (bStormKing)
	{
		switch (TimesCalled)
		{
		case 0:
			Ret = 30;
			break;

		case 1:
			Ret = 180;
			break;

		case 2:
			Ret = 300;
			break;

		case 3:
			Ret = 360;
			break;

		default:
			break;
		}
	}
	else
	{
		switch (TimesCalled)
		{
		case 0:
			Ret = 0.f;
			break;

		case 1:
			Ret = 60;
			break;

		case 2:
			Ret = 30;
			break;

		case 3:
			Ret = 15;
			break;

		default:
			break;
		}
	}

	return Ret;
}

float GetZoneStartDuration()
{
	if (bLateGame)
	{
		switch (TimesCalled)
		{
		case 0:
		{
			return 0.f;
		}

		case 1:
		{
			return 60;
		}

		case 2:
			return 60;

		case 3:
			return 60;

		default:
			return 0.f;
		}
	}
}

void(__fastcall* stormstuffOG)(__int64 a1, int a2);
void __fastcall stormstuff(__int64 a1, int a2)
{
	/*if (bTravis && !StartedEvent)
		return;*/
	auto GM = ((AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode);
	stormstuffOG(a1, a2);

	if (bLateGame)
	{
		GetGameState()->SafeZoneIndicator->NextCenter = (FVector_NetQuantize100)BusLocation;
		GetGameState()->SafeZoneIndicator->NextNextCenter = (FVector_NetQuantize100)BusLocation;

		switch (TimesCalled)
		{
		case 0:
		{
			GetGameState()->SafeZoneIndicator->NextRadius = 10000;
			break;
		}

		case 1:
		{
			GetGameState()->SafeZoneIndicator->NextRadius = 7500;
			break;
		}

		case 2:
			GetGameState()->SafeZoneIndicator->NextRadius = 5000;
			break;

		case 3:
			GetGameState()->SafeZoneIndicator->NextRadius = 3500;
			break;

		default:
			GetGameState()->SafeZoneIndicator->NextRadius = 0;
			break;
		}

		FVector Center = GetGameState()->SafeZoneIndicator->GetSafeZoneCenter();
		FVector NextCenter = GetGameState()->SafeZoneIndicator->GetSafeZoneCenter();

		if (TimesCalled)
		{
			int Amount = GetMath()->RandomIntegerInRange(-25000, 25000);
			NextCenter.X += Amount;
			NextCenter.Y += Amount;
			GetGameState()->SafeZoneIndicator->NextCenter = (FVector_NetQuantize100)NextCenter;
		}

		int StormDuration = GetStormDuration();
		float Distance = GetMath()->Vector_Distance2D(Center, NextCenter);

		if (Distance >= 10000)
			StormDuration += 85;

		cout << "Distance: " << Distance << endl;

		GetGameState()->SafeZoneIndicator->SafeZoneStartShrinkTime = GetStatics()->GetTimeSeconds(UWorld::GetWorld()) + GetZoneStartDuration();
		GetGameState()->SafeZoneIndicator->SafeZoneFinishShrinkTime = GetGameState()->SafeZoneIndicator->SafeZoneStartShrinkTime + StormDuration;
		GetGameState()->SafeZonePhase = 5;
		GetGameMode()->SafeZonePhase = 5;
		GetGameState()->OnRep_SafeZonePhase();
		for (size_t i = 0; i < GM->AlivePlayers.Num(); i++)
		{
			GiveAccolade(GM->AlivePlayers[i], StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeID_SurviveStormCircle.AccoladeID_SurviveStormCircle"));
		}
		TimesCalled++;
	}

	if (bStormKing && GetGameState()->GamePhase > EAthenaGamePhase::Aircraft)
	{
		AFortAthenaMutator_DadBro* Mutator = (AFortAthenaMutator_DadBro*)GetGameState()->GetMutatorByClass(GetGameMode(), AFortAthenaMutator_DadBro::StaticClass());
		GetGameState()->SafeZoneIndicator->NextCenter = (FVector_NetQuantize100)Mutator->DadBroSpawnLocation;
		GetGameState()->SafeZoneIndicator->NextNextCenter = (FVector_NetQuantize100)Mutator->DadBroSpawnLocation;

		switch (TimesCalled)
		{
		case 0:
			GetGameState()->SafeZoneIndicator->NextRadius = 10000;
			break;

		case 1:
			GetGameState()->SafeZoneIndicator->NextRadius = 7500;
			break;

		case 2:
			GetGameState()->SafeZoneIndicator->NextRadius = 5000;
			break;

		case 3:
			GetGameState()->SafeZoneIndicator->NextRadius = 4750;
			break;

		default:
			GetGameState()->SafeZoneIndicator->NextRadius = 4350;
			break;
		}

		GetGameState()->SafeZoneIndicator->SafeZoneStartShrinkTime = GetStatics()->GetTimeSeconds(UWorld::GetWorld());
		GetGameState()->SafeZoneIndicator->SafeZoneFinishShrinkTime = GetStatics()->GetTimeSeconds(UWorld::GetWorld()) + GetStormDuration();
		GetGameState()->SafeZonePhase = 5;
		GetGameMode()->SafeZonePhase = 5;
		GetGameState()->OnRep_SafeZonePhase();

		for (size_t i = 0; i < GM->AlivePlayers.Num(); i++)
		{
			GiveAccolade(GM->AlivePlayers[i], StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeID_SurviveStormCircle.AccoladeID_SurviveStormCircle"));
		}
		TimesCalled++;
	}
}


//void ServerHandlePickupWithSwap(AFortPlayerPawnAthena* Pawn, AFortPickup* Pickup, FGuid Swap, float InFlyTime, FVector InStartDirection, bool bPlayPickupSound)
//{
//	if (!Pawn || !Pickup || Pickup->bPickedUp)
//		return;
//
//	AFortPlayerControllerAthena* PC = (AFortPlayerControllerAthena*)Pawn->Controller;
//
//	FFortItemEntry* Entry = FindEonEntry(PC, Swap);
//	if (!Entry || !Entry->ItemDefinition || Entry->ItemDefinition->IsA(UFortWeaponMeleeItemDefinition::StaticClass()))
//		return;
//
//	SpawnPickup(Pawn->K2_GetActorLocation(), Entry, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, Pawn);
//	EonRemove(PC, Swap);
//	Pawn->ServerHandlePickup(Pickup, InFlyTime, InStartDirection, bPlayPickupSound);
//}

float anticheatConst = -1.45130635f;
void (*ServerMoveNoBaseOG)(ACharacter* character, float Timestamp, FVector_NetQuantize10 InAccel, FVector_NetQuantize100 ClientLoc, uint8 CompressedMoveFlags, uint8 ClientRoll, uint32 View, uint8 ClientMovementMode);
void ServerMoveNoBase(ACharacter* character, float Timestamp, FVector_NetQuantize10 InAccel, FVector_NetQuantize100 ClientLoc, uint8 CompressedMoveFlags, uint8 ClientRoll, uint32 View, uint8 ClientMovementMode) {
	printf("called for %p\n", character);
	APlayerPawn_Athena_C* Pawn = Cast<APlayerPawn_Athena_C>(character);
	if (Pawn && Timestamp != anticheatConst) {
		return ((AFortPlayerController*)Pawn->Controller)->ClientReturnToMainMenu(L"UHUHUHUHHUHUHUHUHUHUHUUH UR WEIRD UR CHEATING?");
	}

	return ServerMoveNoBaseOG(character, Timestamp, InAccel, ClientLoc, CompressedMoveFlags, ClientRoll, View, ClientMovementMode);
}

__int64 WorldNetMode(UWorld* World)
{
	return 1;
}

__int64 ActorNetMode(AActor* Actor)
{
	return 1;
}
FName SpecName;
void (*GetPlayerViewPointOG)(AFortPlayerController* PC, FVector& out_Location, FRotator& out_Rotation);
void GetPlayerViewPoint(AFortPlayerController* PC, FVector& out_Location, FRotator& out_Rotation)
{
	if (PC->IsA(AFortLiveBroadcastController::StaticClass()))
		return GetPlayerViewPointOG(PC, out_Location, out_Rotation);

	if (PC->StateName.ComparisonIndex == SpecName.ComparisonIndex)
	{
		out_Location = PC->LastSpectatorSyncLocation;
		out_Rotation = PC->LastSpectatorSyncRotation;
	}
	else if (PC->GetViewTarget())
	{
		out_Location = PC->GetViewTarget()->K2_GetActorLocation();
		out_Rotation = PC->GetControlRotation();
	}
	else
	{
		return GetPlayerViewPointOG(PC, out_Location, out_Rotation);
	}
}


__int64(__fastcall* test3OG)(AActor* a1, __int64 a2);
__int64 __fastcall test3(AActor* a1, __int64 a2)
{
	AFortInventory* NewInv = SpawnActor<AFortInventory>({});
	NewInv->SetOwner(a1);

	*(unsigned long long*)(__int64(a1) + 1352) = __int64(NewInv);
	return test3OG(a1, a2);
}

__int64 (*OnExplodedOG)(AB_Prj_Athena_Consumable_Thrown_C* Consumable, TArray<class AActor*>& HitActors, TArray<struct FHitResult>& HitResults) = decltype(OnExplodedOG)(__int64(GetModuleHandleA(0)) + 0x3EA2740);
__int64 OnExploded(AB_Prj_Athena_Consumable_Thrown_C* Consumable, TArray<class AActor*>& HitActors, TArray<struct FHitResult>& HitResults)
{
	if (!Consumable)
		return OnExplodedOG(Consumable, HitActors, HitResults);
	else if (Consumable->GetName() == "B_Prj_Athena_Bucket_Old_C" || Consumable->GetName() == "B_Prj_Athena_Bucket_Nice_C") {
		auto PC = Consumable->GetOwnerPlayerController();
		auto Pawn = PC->MyFortPawn;
		auto Def = *(UFortItemDefinition**)(__int64(Consumable) + 0x888);
		if (!Def)
			return OnExplodedOG(Consumable, HitActors, HitResults);

		AFortPickupAthena* NewPickup = SpawnActor<AFortPickupAthena>(Consumable->K2_GetActorLocation());
		NewPickup->bRandomRotation = true;
		NewPickup->PrimaryPickupItemEntry.ItemDefinition = Def;
		NewPickup->PrimaryPickupItemEntry.Count = 1;
		NewPickup->PrimaryPickupItemEntry.LoadedAmmo = 1;
		NewPickup->OnRep_PrimaryPickupItemEntry();
		NewPickup->PawnWhoDroppedPickup = Pawn;
		Pawn->ServerHandlePickup(NewPickup, 0.40f, FVector(), false);
	}
	if (!Consumable->GetName().starts_with("B_Prj_Athena_Consumable_Thrown_")) {
		return OnExplodedOG(Consumable, HitActors, HitResults);
	}
	auto Def = *(UFortItemDefinition**)(__int64(Consumable) + 0x888);
	if (!Def)
		return OnExplodedOG(Consumable, HitActors, HitResults);
	SpawnPickup(Consumable->K2_GetActorLocation(), Def, 1, 0, EFortPickupSourceTypeFlag::Tossed, EFortPickupSpawnSource::Unset, nullptr, false);
	Consumable->K2_DestroyActor();
	return OnExplodedOG(Consumable, HitActors, HitResults);
}

__int64 GetAccID(__int64 Smth, __int64 a2)
{
	static  __int64 (*sub_7FF7C019DC30)(__int64, __int64) = decltype(sub_7FF7C019DC30)(__int64(GetModuleHandleW(0)) + 0x2C2DC30);
	__int64 v4 = 0; // rcx
	__int64 v5 = 0; // r14
	__int64 v6 = 0; // rax
	wchar_t* v7 = 0; // rbx
	wchar_t* v8 = 0; // rsi
	__int64 v9 = 0; // rax
	__int64 v10 = 0; // rax
	__int64* v11 = 0; // rcx
	char v12 = 0; // bl
	int v13 = 0; // eax
	__int64 v15 = 0; // [rsp+20h] [rbp-58h] BYREF
	__int64 v16 = 0; // [rsp+28h] [rbp-50h]
	__int64 v17[2]; // [rsp+30h] [rbp-48h] BYREF
	__int64 v18 = 0; // [rsp+40h] [rbp-38h] BYREF
	__int64 v19 = 0; // [rsp+50h] [rbp-28h] BYREF
	int v20 = 0; // [rsp+80h] [rbp+8h] BYREF

	v20 = 0;
	v4 = *(_QWORD*)(Smth + 8);
	if (v4 && (*(unsigned __int8(__fastcall**)(__int64))(*(_QWORD*)v4 + 40i64))(v4))
	{
		v5 = *(_QWORD*)(Smth + 8);
		v6 = (*(__int64(__fastcall**)(__int64, __int64*))(*(_QWORD*)v5 + 56i64))(v5, &v19);
		cout << __int64(*(_QWORD*)v5 + 56i64) - __int64(GetModuleHandleW(0)) << endl;
		wchar_t* TestChar = *(wchar_t**)(v5 + 24);
		printf("testchar %ls \n", TestChar);
		if (*(DWORD*)(v6 + 8))
			v8 = *(wchar_t**)v6;
		v9 = (*(__int64(__fastcall**)(__int64, int*))(*(_QWORD*)v5 + 16i64))(v5, &v20);
		v10 = sub_7FF7C019DC30(v9, (__int64)&v18);
		if (*(DWORD*)(v10 + 8))
			v7 = *(wchar_t**)v10;
		printf("test1 %ls\n", v8);
		printf("test2 %ls\n", v7);
	}

	*(_QWORD*)a2 = *v11;
	*v11 = 0i64;
	*(DWORD*)(a2 + 8) = *((DWORD*)v11 + 2);
	v13 = *((DWORD*)v11 + 3);
	v11[1] = 0i64;
	*(DWORD*)(a2 + 12) = v13;

	return a2;
}

void(__fastcall* PreloginOG)(AFortGameModeAthena* GameMode, FString& Options, FString& Address, FUniqueNetIdRepl& UniqueId, FString& ErrorMessage);
void __fastcall Prelogin(AFortGameModeAthena* GameMode, FString& Options, FString& Address, FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	return;
	if (bBattleLab)
		return;

	return PreloginOG(GameMode, Options, Address, UniqueId, ErrorMessage);
}

void SetMatchPlacementHook(AFortPlayerStateAthena* PlayerState, int Placement)
{
	return SetMatchPlacement(PlayerState, Placement);
}


__int64 AActorGetNetMode(AActor* a1)
{
	return 1;
}

__int64 WorldGetNetMode(UWorld* a1)
{
	return 1;
}
void (*StormXpProOG)(__int64, int);
void __fastcall StormXpPro(__int64 a1, int a2)
{
	auto GM = ((AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode);

	for (size_t i = 0; i < GM->AlivePlayers.Num(); i++)
	{
		GiveAccolade(GM->AlivePlayers[i], StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeID_SurviveStormCircle.AccoladeID_SurviveStormCircle"));
	}

	return StormXpProOG(a1, a2);
}


