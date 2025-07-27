#pragma once
#include "Utils.h"
#include "Inventory.h"
#include "PC.h"
#include "API.h"
#include "Llamas.h"
#include "Vehicle.h"
bool bDropZone = false;
void (*ServerSetInAircraftOG)(AFortPlayerStateAthena* PlayerState, bool aircragft);
static void ServerSetInAircraft(AFortPlayerStateAthena* PlayerState, bool aircragft)
{
	AFortPlayerControllerAthena* PC = (AFortPlayerControllerAthena*)PlayerState->Owner;

	if (!PC || !PC->WorldInventory)
		return ServerSetInAircraftOG(PlayerState, aircragft);

	PlayersOnBus = GetGameState()->PlayersLeft;
	PC->WorldInventory->Inventory.ReplicatedEntries.Free();
	PC->WorldInventory->Inventory.ItemInstances.Free();
	BattleBus = GetGameState()->GetAircraft(0);
	for (size_t i = 0; i < GetGameMode()->StartingItems.Num(); i++)
	{
		if (GetGameMode()->StartingItems[i].Count <= 0)
			continue;
		EonGiveItem(PC, GetGameMode()->StartingItems[i].Item, GetGameMode()->StartingItems[i].Count);
	}

	if (!PC->CosmeticLoadoutPC.Pickaxe || !PC->CosmeticLoadoutPC.Pickaxe->WeaponDefinition)
	{
		PC->ClientReturnToMainMenu(TEXT("Invalid pickaxe, please report tyhis to a dev"));
		return;
	}
	EonGiveItem(PC, PC->CosmeticLoadoutPC.Pickaxe->WeaponDefinition);

	/*if (bTeamRumble)
	{
		static UFortWeaponItemDefinition* ArDef = StaticLoadObject<UFortWeaponItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_Auto_Athena_UC_Ore_T03.WID_Assault_Auto_Athena_UC_Ore_T03");
		static UFortWeaponItemDefinition* PumpDef = StaticLoadObject<UFortWeaponItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_C_Ore_T03.WID_Shotgun_Standard_Athena_C_Ore_T03");

		EonGiveItem(PC, PumpDef, 1, 5);
		EonGiveItem(PC, ArDef, 1, 30);
		EonGiveItem(PC, PumpDef->GetAmmoWorldItemDefinition_BP(), 15);
		EonGiveItem(PC, ArDef->GetAmmoWorldItemDefinition_BP(), 60);
	}*/

	if (bArsenal)
	{
		static UFortItemDefinition* WoodDef = StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/WoodItemData.WoodItemData");
		static UFortItemDefinition* StoneDef = StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/StoneItemData.StoneItemData");
		static UFortItemDefinition* MetalDef = StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/MetalItemData.MetalItemData");

		EonGiveItem(PC, WoodDef, 9999);
		EonGiveItem(PC, StoneDef, 9999);
		EonGiveItem(PC, MetalDef, 9999);

		AFortAthenaMutator_GG* Mutator = (AFortAthenaMutator_GG*)GetGameState()->GetMutatorByClass(GetGameMode(), AFortAthenaMutator_GG::StaticClass());

		if (Mutator && Mutator->WeaponEntries[0].Weapon)
		{
			//EonGiveItem(PC, Mutator->WeaponEntries[0].Weapon, 1, GetAmmoForDef(Mutator->WeaponEntries[0].Weapon));
		}
	}

	if (bStormKing)
	{
		static UFortItemDefinition* WoodDef = StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/WoodItemData.WoodItemData");
		static UFortItemDefinition* StoneDef = StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/StoneItemData.StoneItemData");
		static UFortItemDefinition* MetalDef = StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/MetalItemData.MetalItemData");
		static UFortWeaponItemDefinition* ScarDef = StaticLoadObject<UFortWeaponItemDefinition>("/Game/Athena/Items/Weapons/Boss/WID_Boss_Adventure_AR.WID_Boss_Adventure_AR");
		static UFortWeaponItemDefinition* DrumgunDef = StaticLoadObject<UFortWeaponItemDefinition>("/Game/Athena/Items/Weapons/Boss/WID_Boss_Midas.WID_Boss_Midas");
		static UFortWeaponItemDefinition* SlurpFishDef = UObject::FindObject<UFortWeaponItemDefinition>("WID_Athena_Flopper_Effective.WID_Athena_Flopper_Effective");
		static UFortWeaponItemDefinition* SmallShieldDef = UObject::FindObject<UFortWeaponItemDefinition>("Athena_ShieldSmall.Athena_ShieldSmall");

		static UFortWeaponItemDefinition* GraveDiggerDef = UObject::FindObject<UFortWeaponItemDefinition>("WID_Assault_Auto_Halloween_SR_Ore_T01.WID_Assault_Auto_Halloween_SR_Ore_T01");
		static UFortWeaponItemDefinition* NocturnoDef = UObject::FindObject<UFortWeaponItemDefinition>("WID_Assault_Auto_Founders_SR_Ore_T05.WID_Assault_Auto_Founders_SR_Ore_T05");
		static UFortWeaponItemDefinition* MeleeDef1 = UObject::FindObject<UFortWeaponItemDefinition>("WID_Edged_Sword_NeonGlow_SR_Ore_T05.WID_Edged_Sword_NeonGlow_SR_Ore_T05");
		static UFortWeaponItemDefinition* MeleeDef2 = UObject::FindObject<UFortWeaponItemDefinition>("WID_Edged_Sword_Medium_Laser_Founders_SR_Crystal_T05.WID_Edged_Sword_Medium_Laser_Founders_SR_Crystal_T05");

		EonGiveItem(PC, WoodDef, 9999);
		EonGiveItem(PC, StoneDef, 9999);
		EonGiveItem(PC, MetalDef, 9999);
		//GiveItem(PC, ScarDef, 1, 30);
		//GiveItem(PC, GraveDiggerDef, 1, 0, 144);
		//GiveItem(PC, NocturnoDef, 1, 0, 86);
		EonGiveItem(PC, SlurpFishDef, 3);
		EonGiveItem(PC, SmallShieldDef, 6);
		//GiveItem(PC, MeleeDef1, 1);
		//GiveItem(PC, MeleeDef2, 1);
		EonGiveItem(PC, ScarDef, 1, 30);
		EonGiveItem(PC, DrumgunDef, 1, 40);
		//GiveItem(PC, NocturnoDef->GetAmmoWorldItemDefinition_BP(), 9999);
		//GiveItem(PC, GraveDiggerDef->GetAmmoWorldItemDefinition_BP(), 9999);
		EonGiveItem(PC, ScarDef->GetAmmoWorldItemDefinition_BP(), 9999);

		auto Aircraft = GetGameState()->GetAircraft(0);
		auto PoiManager = GetGameState()->PoiManager;

		if (PoiManager)
		{
			Aircraft->FlightInfo.FlightSpeed = 1500.f;
			FVector Loc = (GetGameState()->PoiManager->AllPoiVolumes[0]->K2_GetActorLocation() - (GetGameState()->PoiManager->AllPoiVolumes[0]->GetActorForwardVector() * 5000));
			Loc.Z = 15000;
			Aircraft->FlightInfo.FlightStartLocation = (FVector_NetQuantize100)Loc;
			BusLocation = GetGameState()->PoiManager->AllPoiVolumes[0]->K2_GetActorLocation();
			BusLocation.Z = 0;
		}
	}

	//if (bTravis)
	//{
	//	//X: 62590.7 Y : -75501.8Z : 13982.4
	//	auto Aircraft = GetGameState()->GetAircraft(0);
	//	FVector Loc = FVector(62590, -75501, 13982);
	//	Aircraft->FlightInfo.FlightStartLocation = (FVector_NetQuantize100)Loc;
	//	Aircraft->FlightInfo.FlightSpeed = 1000;
	//	Aircraft->FlightInfo.TimeTillDropStart = 1;
	//	Aircraft->DropStartTime = GetStatics()->GetTimeSeconds(UWorld::GetWorld()) + 1;
	//	GetGameState()->bAircraftIsLocked = false;
	//	BusLocation = Loc;
	//}

	if (bLateGame)
	{
		static bool Done = false;
		auto Aircraft = GetGameState()->GetAircraft(0);
		auto PoiManager = GetGameState()->PoiManager;

		if (!Done)
		{
			Done = true;
			BattleBus = Aircraft;
			GetGameState()->GamePhase = EAthenaGamePhase::SafeZones;
			GetGameState()->GamePhaseStep = EAthenaGamePhaseStep::StormHolding;
			GetGameState()->OnRep_GamePhase(EAthenaGamePhase::Aircraft);
			GetGameState()->SafeZonesStartTime = GetStatics()->GetTimeSeconds(UWorld::GetWorld());

			if (PoiManager)
			{
				Aircraft->FlightInfo.FlightSpeed = 1000.f;

				TArray<AActor*> Test;
				GetStatics()->GetAllActorsOfClass(UWorld::GetWorld(), ABuildingContainer::StaticClass(), &Test);
				// FVector Proper = GetGameMode()->SafeZoneLocations[4]; // TEST
				int Idx = GetMath()->RandomIntegerInRange(0, Test.Num()); // BAD 
				FVector Loc = (Test[Idx]->K2_GetActorLocation() - (Test[Idx]->GetActorForwardVector() * 1000)); // bro what the fuck is wrong with u
				Loc.Z = 15000;
				Aircraft->FlightInfo.FlightStartLocation = (FVector_NetQuantize100)Loc;

				Aircraft->FlightInfo.TimeTillFlightEnd = 9;
				Aircraft->FlightInfo.TimeTillDropEnd = 9;
				Aircraft->FlightInfo.TimeTillDropStart = 1;
				Aircraft->DropStartTime = GetStatics()->GetTimeSeconds(UWorld::GetWorld()) + 1;
				Aircraft->DropEndTime = GetStatics()->GetTimeSeconds(UWorld::GetWorld()) + 9;
				GetGameState()->bAircraftIsLocked = false;
				BusLocation = Loc;
				BusLocation.Z = 0;
				Test.Free();
			}
		}

		static UFortItemDefinition* WoodDef = StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/WoodItemData.WoodItemData");
		static UFortItemDefinition* StoneDef = StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/StoneItemData.StoneItemData");
		static UFortItemDefinition* MetalDef = StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/MetalItemData.MetalItemData");
		//static UFortWeaponItemDefinition* ScarDef = StaticLoadObject<UFortWeaponItemDefinition>("/Game/Athena/Items/Weapons/Boss/WID_Boss_Adventure_AR.WID_Boss_Adventure_AR");
		//static UFortWeaponItemDefinition* DrumgunDef = StaticLoadObject<UFortWeaponItemDefinition>("/Game/Athena/Items/Weapons/Boss/WID_Boss_Midas.WID_Boss_Midas");
		//static UFortWeaponItemDefinition* CrashPadDef = StaticLoadObject<UFortWeaponItemDefinition>("/Game/Athena/Items/Consumables/AppleSun/WID_Athena_AppleSun_R.WID_Athena_AppleSun_R");
		//static UFortWeaponItemDefinition* PumpDef = StaticLoadObject<UFortWeaponItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_VR_Ore_T03.WID_Shotgun_Standard_Athena_VR_Ore_T03");
		//static UFortWeaponItemDefinition* SlurpFishDef = UObject::FindObject<UFortWeaponItemDefinition>("WID_Athena_Flopper_Effective.WID_Athena_Flopper_Effective");
		EonGiveItem(PC, WoodDef, 500);
		EonGiveItem(PC, StoneDef, 500);
		EonGiveItem(PC, MetalDef, 500);
		//EonGiveItem(PC, ScarDef, 1, 30);
		//EonGiveItem(PC, CrashPadDef, 12, 1);
		//EonGiveItem(PC, PumpDef, 1, 5);
		//EonGiveItem(PC, SlurpFishDef, 12);
		//EonGiveItem(PC, ScarDef->GetAmmoWorldItemDefinition_BP(), 9999);
		//EonGiveItem(PC, PumpDef->GetAmmoWorldItemDefinition_BP(), 9999);
		auto& LoadoutToGive = GetRandomLoadout();

		for (auto& Pair : LoadoutToGive.ItemDefinitions)
		{
			EonGiveItem(PC, Pair.first, Pair.second[0], Pair.second[1]);
			UFortItemDefinition* Def = ((UFortWeaponRangedItemDefinition*)Pair.first)->GetAmmoWorldItemDefinition_BP();
			if (Def && Def != Pair.first)
				EonGiveItem(PC, Def, Pair.second[2]);
		}
	}

	return ServerSetInAircraftOG(PlayerState, aircragft);
}


void ServerSendZiplineState(AFortPlayerPawnAthena* Pawn, FZiplinePawnState State)
{
	if (!Pawn)
		return;

	Pawn->ZiplineState = State;
	OnRep_ZiplineState(Pawn);

	if (State.bJumped)
	{
		Pawn->LaunchCharacter(FVector{ 0,0,1200 }, false, false);
	}
}

void (*ServerReadyToStartMatchOG)(AFortPlayerControllerAthena* PC);
void ServerReadyToStartMatch(AFortPlayerControllerAthena* PC)
{

	if (PC->IsA(AFortLiveBroadcastController::StaticClass()))
		return;

	if (bDropZone)
	{
		AFortAthenaMutator_SpyRumble* Mutator = (AFortAthenaMutator_SpyRumble*)GetGameState()->GetMutatorByClass(GetGameMode(), AFortAthenaMutator_SpyRumble::StaticClass());
		AFortAthenaMutator_PerkSystemMutator* Mutator2 = (AFortAthenaMutator_PerkSystemMutator*)GetGameState()->GetMutatorByClass(GetGameMode(), AFortAthenaMutator_PerkSystemMutator::StaticClass());

		cout << Mutator << endl;
		cout << Mutator2 << endl;

		UFortControllerComponent_PerkSystem* Comp = (UFortControllerComponent_PerkSystem*)PC->GetComponentByClass(UFortControllerComponent_PerkSystem::StaticClass());

		Comp->MutatorData = Mutator2->MutatorData;
		Comp->CachedFactionTag.TagName = Conv_StringToName(TEXT("Athena.Faction.Ego"));
		Log("PerkSelection num " + to_string(Comp->PerkSelection.Num()));
		Comp->RerollCount = 1;

		auto First = StaticLoadObject<UFortSpyTechItemDefinition>("/Game/Athena/Items/SpyTech/ItemDef/STID_PumpShotgun.STID_PumpShotgun");
		auto Second = StaticLoadObject<UFortSpyTechItemDefinition>("/Game/Athena/Items/SpyTech/ItemDef/STID_JetPack.STID_JetPack");

		Comp->SpyTechArray.Add(Comp->DefaultPerks[0]);
		Comp->SpyTechArray.Add(First);
		Comp->SpyTechArray.Add(Second);

		for (size_t i = 0; i < Comp->PerkSelection.Num(); i++)
		{
			Comp->PerkSelection[i].Items.Free();
			Comp->PerkSelection[i].Items.Add(Comp->SpyTechArray[i]);
			Comp->PerkSelection[i].Items.Add(Comp->SpyTechArray[i]);
		}

		Comp->RoundStartCache.bDataReady = true;
		Log("FuturePerks " + to_string(Comp->RoundStartCache.FuturePerks.Num()));
		Log("SpyTechArray  " + to_string(Comp->SpyTechArray.Num()));
		Log("AllSelectedPerks  " + to_string(Comp->AllSelectedPerks.Num()));
		Log("DefaultPerks  " + to_string(Comp->DefaultPerks.Num()));

		Comp->OnRep_PerkSelection();
		Comp->OnRep_SpyTechArray();
		Comp->OnRep_RerollCount();
	}

}

void (*ServerClientIsReadyToRespawnOG)(AFortPlayerControllerAthena* PC);
void ServerClientIsReadyToRespawn(AFortPlayerControllerAthena* PC)
{
	AFortPlayerStateAthena* PlayerState = (AFortPlayerStateAthena*)PC->PlayerState;

	FFortRespawnData& RespawnData = *(FFortRespawnData*)(__int64(PlayerState) + 0xD30);

	if (RespawnData.bRespawnDataAvailable && RespawnData.bServerIsReady)
	{
		RespawnData.bClientIsReady = true;

		FTransform Transform{};
		Transform.Scale3D = FVector{ 1,1,1 };
		Transform.Translation = RespawnData.RespawnLocation;

		AFortPlayerPawnAthena* NewPawn = (AFortPlayerPawnAthena*)GetGameMode()->SpawnDefaultPawnAtTransform(PC, Transform);
		PC->Possess(NewPawn);

		PC->MyFortPawn->SetMaxHealth(100);
		PC->MyFortPawn->SetMaxShield(100);
		PC->MyFortPawn->SetHealth(100);
		PC->MyFortPawn->SetShield(100);

		PC->MyFortPawn->bCanBeDamaged = true; // nah no need for this

		PC->RespawnPlayerAfterDeath(true);
	}

	return ServerClientIsReadyToRespawnOG(PC);
}
std::map<AFortPlayerController*, int> Shots{};
std::map<AFortPlayerController*, UFortWeaponItemDefinition*> Weapons{};
int LastTime = 0;
inline int64_t(*ApplyCostOG)(UGameplayAbility* arg1, int32_t arg2, void* arg3, void* arg4);
inline int64_t ApplyCost(UFortGameplayAbility* arg1, int32_t arg2, void* arg3, void* arg4) {
	if (arg1->GetName().starts_with("GA_Athena_AppleSun_Passive_C_")) {
		auto Def = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Consumables/AppleSun/WID_Athena_AppleSun.WID_Athena_AppleSun");
		auto ASC = arg1->GetActivatingAbilityComponent();
		AFortPlayerStateAthena* PS = (AFortPlayerStateAthena*)ASC->GetOwner();
		auto Pawn = PS->GetCurrentPawn();
		AFortPlayerController* PC = nullptr;
		PC = (AFortPlayerController*)Pawn->GetOwner();

		if (!PC->bInfiniteAmmo) {
			Remove(PC, Def);
		}
	}
	else if (arg1->GetName().starts_with("GA_Ranged_") || arg1->GetName().starts_with("GAB_Melee_ImpactCombo_Athena_")) {
		auto ASC = arg1->GetActivatingAbilityComponent();
		AFortPlayerStateAthena* PS = (AFortPlayerStateAthena*)ASC->GetOwner();
		auto Pawn = PS->GetCurrentPawn();
		AFortPlayerController* PC = nullptr;
		PC = (AFortPlayerController*)Pawn->GetOwner();
		if (PC->IsA(ABP_PhoebePlayerController_C::StaticClass())) {
			return ApplyCostOG(arg1, arg2, arg3, arg4);
		}
		auto t = std::floor(GetStatics()->GetTimeSeconds(UWorld::GetWorld()));
		if (LastTime != (int)t) {
			LastTime = (int)t;
			Shots[PC] = 0;
		}
		if (Weapons[PC] != Pawn->CurrentWeapon->WeaponData) {
			Shots[PC] = 0;
			Weapons[PC] = Pawn->CurrentWeapon->WeaponData;
		}

		auto WSH = Weapons[PC]->WeaponStatHandle;
	}
	return ApplyCostOG(arg1, arg2, arg3, arg4);
}



void ServerChangeName(AFortPlayerControllerAthena* PC, FString NewName) {
	string IP = PC->PlayerState->SavedNetworkAddress.ToString();
	//Log("Banned %s (%s).\n", PC->PlayerState->GetPlayerName().ToString().c_str(), IP.c_str());
	PC->MyFortPawn->BalloonActiveCount = 255;
	new thread(PlooshFNAPI::BanPlayer, GetAccountID(PC).ToString());
	PC->ClientReturnToMainMenu(L"You have been banned from AGS. Please go to https://discord.gg/StWVWn6a3B to appeal.");
}

//literally the most pasted thing ever
APlayerController* (*SpawnPlayActorOG)(UWorld* World, UPlayer* Player, ENetRole RemoteRole, FURL& InURL, void* UniqueID, FString& Error, uint8 InNetPlayerIndex);
APlayerController* SpawnPlayActor(UWorld* World, UPlayer* Player, ENetRole RemoteRole, FURL& InURL, void* UniqueID, FString& Error, uint8 InNetPlayerIndex)
{
	FString& JoinURL = *(FString*)(__int64(Player) + 0x1B8);
	if (JoinURL.ToString().contains("livespectator"))
	{
		GetGameMode()->PlayerControllerClass = GetGameMode()->LiveBroadcastPlayerControllerClass.Get();
	}
	else
	{
		GetGameMode()->PlayerControllerClass = AAthena_PlayerController_C::StaticClass();
	}
	auto Ret = SpawnPlayActorOG(World, Player, RemoteRole, InURL, UniqueID, Error, InNetPlayerIndex);

	if (GetGameMode()->PlayerControllerClass == AAthena_PlayerController_C::StaticClass() && GetGameState()->GamePhase >= EAthenaGamePhase::Aircraft)
	{
		//Ret->ClientReturnToMainMenu(TEXT(""));
		return Ret;
	}

	return Ret;
}
