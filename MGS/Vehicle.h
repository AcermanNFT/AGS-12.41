#pragma once
#include "Utils.h"
#include "Inventory.h"
#include "Abilities.h"


inline void GiveModifier(UFortGameplayModifierItemDefinition* Modifier, AFortPlayerControllerAthena* PC)
{
	static auto AbilityOffset = GetOffset(Modifier, "PersistentAbilitySets");
	static auto EffectsOffset = GetOffset(Modifier, "PersistentGameplayEffects");

	TArray<FFortAbilitySetDeliveryInfo>& Abilities = *(TArray<FFortAbilitySetDeliveryInfo>*)(__int64(Modifier) + AbilityOffset);
	TArray<FFortGameplayEffectDeliveryInfo>& Effects = *(TArray<FFortGameplayEffectDeliveryInfo>*)(__int64(Modifier) + EffectsOffset);

	for (size_t i = 0; i < Abilities.Num(); i++)
	{
		auto& idk = Abilities[i];
		if (idk.DeliveryRequirements.bApplyToPlayerPawns)
		{
			for (size_t j = 0; j < idk.AbilitySets.Num(); j++)
			{
				UFortAbilitySet* Set = StaticLoadObject<UFortAbilitySet>(Conv_NameToString(idk.AbilitySets[j].ObjectID.AssetPathName).ToString());
				Log(Set->GetName());
				GiveAbilitySet(PC, Set);
			}
		}
	}

	for (size_t i = 0; i < Effects.Num(); i++)
	{
		auto& idk = Effects[i];
		if (idk.DeliveryRequirements.bApplyToPlayerPawns)
		{
			for (size_t j = 0; j < idk.GameplayEffects.Num(); j++)
			{
				UClass* Effect = StaticLoadObject<UClass>(Conv_NameToString(idk.GameplayEffects[j].GameplayEffect.ObjectID.AssetPathName).ToString());
				Log(Effect->GetName());
				PC->MyFortPawn->AbilitySystemComponent->BP_ApplyGameplayEffectToSelf(Effect, idk.GameplayEffects[j].Level, FGameplayEffectContextHandle());
			}
		}
	}
}

//void GiveModifiers(AFortPlayerControllerAthena* PlayerController, UFortPlaylistAthena* Playlist) {
//	for (auto& Modifier : Playlist->ModifierList) {
//
//		for (auto& PersistentAbilitySet : Modifier->PersistentAbilitySets) {
//
//			if (Modifier->PersistentAbilitySets.Num() == 0) { std::cout << "No PersistentAbilitySets in Modifier" << std::endl; continue; }
//
//			if (PersistentAbilitySet.DeliveryRequirements.bApplyToPlayerPawns) {
//				for (auto& AbilitySet : PersistentAbilitySet.AbilitySets) {
//
//					if (AbilitySet.ObjectID.AssetPathName.GetRawString() == "None") continue;
//
//					UFortAbilitySet* AbilitySetClass = StaticLoadObject<UFortAbilitySet>(AbilitySet.ObjectID.AssetPathName.GetRawString());
//					if (!AbilitySetClass) { std::cout << "Failed to load AbilitySetClass" << std::endl; continue; }
//
//					for (auto& Ability : AbilitySetClass->GameplayAbilities) {
//
//						if (!Ability.Get()) { std::cout << "GameplayAbility is null, skipping" << std::endl; continue; }
//
//						auto DefaultAbility = Ability.Get()->DefaultObject;
//						if (!DefaultAbility) { std::cout << "DefaultObject for Ability is null" << std::endl; continue; }
//
//						std::cout << "Giving ability: " << DefaultAbility->GetName() << std::endl;
//						GiveAbility((AFortPlayerPawnAthena*)PlayerController->MyFortPawn, (UGameplayAbility*)DefaultAbility);
//					}
//
//					for (auto& GrantedGameplayEffect : AbilitySetClass->GrantedGameplayEffects) {
//						if (!GrantedGameplayEffect.GameplayEffect.Get()) { std::cout << "GrantedGameplayEffect is null" << std::endl; continue; }
//
//						std::cout << "Giving effect: " << GrantedGameplayEffect.GameplayEffect.Get()->GetName() << std::endl;
//
//						auto AbilitySystemComponent = PlayerController->MyFortPawn->AbilitySystemComponent;
//						if (!AbilitySystemComponent) { std::cout << "AbilitySystemComponent is null" << std::endl; continue; }
//
//						AbilitySystemComponent->BP_ApplyGameplayEffectToSelf(GrantedGameplayEffect.GameplayEffect.Get(), GrantedGameplayEffect.Level, FGameplayEffectContextHandle());
//					}
//				}
//			}
//
//			if (PersistentAbilitySet.DeliveryRequirements.bApplyToBuildingActors) {
//				for (auto& AbilitySet : PersistentAbilitySet.AbilitySets) {
//					break;
//				}
//			}
//		}
//
//		for (auto& PersistentGameplayEffect : Modifier->PersistentGameplayEffects) {
//			if (Modifier->PersistentGameplayEffects.Num() == 0) {
//				continue;
//			}
//
//			if (PersistentGameplayEffect.DeliveryRequirements.bApplyToPlayerPawns) {
//				for (auto& GameplayEffect : PersistentGameplayEffect.GameplayEffects) {
//					if (GameplayEffect.GameplayEffect.ObjectID.AssetPathName.GetRawString() == "None") continue;
//
//					std::cout << "Giving effect: " << GameplayEffect.GameplayEffect.ObjectID.AssetPathName.GetRawString() << std::endl;
//
//					UClass* GameplayEffectClass = StaticLoadObject<UClass>(GameplayEffect.GameplayEffect.ObjectID.AssetPathName.GetRawString());
//					if (!GameplayEffectClass) { std::cout << "Failed to load GameplayEffectClass" << std::endl; continue; }
//
//					auto AbilitySystemComponent = PlayerController->MyFortPawn->AbilitySystemComponent;
//					if (!AbilitySystemComponent) { std::cout << "AbilitySystemComponent is null" << std::endl; continue; }
//
//					AbilitySystemComponent->BP_ApplyGameplayEffectToSelf(GameplayEffectClass, GameplayEffect.Level, FGameplayEffectContextHandle());
//				}
//			}
//
//			if (PersistentGameplayEffect.DeliveryRequirements.bApplyToBuildingActors) {
//				for (auto& GameplayEffect : PersistentGameplayEffect.GameplayEffects) {
//					std::cout << std::format("Building actor GameplayEffect: {}", GameplayEffect.GameplayEffect.Get()->GetName()) << std::endl;
//				}
//			}
//		}
//	}
//}
void SpawnVehicle() {

	AFortGameModeAthena* GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;
	AFortGameStateAthena* GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;
	static bool bHasSpawnedSharkChopper = false;

	TArray<AActor*> Spawners;
	UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), AFortAthenaVehicleSpawner::StaticClass(), &Spawners);

	for (AActor* ActorSp : Spawners) {
		AFortAthenaVehicleSpawner* Spawner = Cast<AFortAthenaVehicleSpawner>(ActorSp);
		if (!Spawner) continue; // failed to cast/invaild actor

		std::string Name = Spawner->GetName();

		std::cout << "SPAWNING VEHICLE " << Name << std::endl;

		//if (!bAllVehicles) {
			// add vehicles you want to spawn
			if (!Name.starts_with("Apollo_Hoagie_Spawner") && !Name.starts_with("Athena_Meatball_L_Spawner")) continue;
		//}

		AActor* Vehicle = BuildingSpawnActor<AActor>(Spawner->K2_GetActorLocation(), Spawner->K2_GetActorRotation(), Spawner->GetVehicleClass());

		if (!bHasSpawnedSharkChopper && Name.starts_with("Apollo_Hoagie_Spawner")) {
			bHasSpawnedSharkChopper = true;

			AActor* Shark_Chopper = BuildingSpawnActor<AActor>(FVector(113665, -91120, -3065), {}, Spawner->GetVehicleClass());

			//Utils::HookVTableFunction(Shark_Chopper->VTable, 0xEE, ServerMove);
		}

		//Utils::HookVTableFunction(Vehicle->VTable, 0xEE, ServerMove);
	}

	// supposed to free this
	Spawners.Free();
}


inline void (*ServerMoveOG)(AFortPhysicsPawn* Pawn, FReplicatedPhysicsPawnState InState);
inline void ServerMove(AFortPhysicsPawn* Pawn, FReplicatedPhysicsPawnState InState)
{
	UPrimitiveComponent* Mesh = (UPrimitiveComponent*)Pawn->RootComponent;

	InState.Rotation.X -= 2.5;
	InState.Rotation.Y /= 0.3;
	InState.Rotation.Z -= -2.0;
	InState.Rotation.W /= -1.2;

	FTransform Transform{};
	Transform.Translation = InState.Translation;
	Transform.Rotation = InState.Rotation;
	Transform.Scale3D = FVector{ 1, 1, 1 };

	Mesh->K2_SetWorldTransform(Transform, false, nullptr, true);
	Mesh->bComponentToWorldUpdated = true;
	Mesh->SetPhysicsLinearVelocity(InState.LinearVelocity, 0, FName());
	Mesh->SetPhysicsAngularVelocity(InState.AngularVelocity, 0, FName());
}
inline void (*ServerAttemptExitVehicleOG)(AFortPlayerController* PlayerController);
inline void ServerAttemptExitVehicle(AFortPlayerControllerZone* PlayerController) {
	if (!PlayerController) { std::cout << "PlayerController is null" << std::endl; return; }

	ServerAttemptExitVehicleOG(PlayerController);

	uint8 SeatIdx = PlayerController->GetVehicleSeatTransitionTargetIndex();
	if (SeatIdx != 0) return;

	auto CurrentWeapon = PlayerController->MyFortPawn->CurrentWeapon;
	if (!CurrentWeapon) { std::cout << "CurrentWeapon is null" << std::endl; return; }

	auto VehicleWeaponDefinition = CurrentWeapon->WeaponData;
	if (!VehicleWeaponDefinition) { /*std::cout << "VehicleWeaponDefinition is null" << std::endl;*/ return; }

	FFortItemEntry* ItemEntry = FindItemEntry(PlayerController, VehicleWeaponDefinition);
	if (ItemEntry && ItemEntry->ItemDefinition->IsA(AFortWeaponRangedForVehicle::StaticClass()))
		RemoveItem(PlayerController, ItemEntry->ItemGuid, 1);

	UFortWorldItemDefinition* SwappingItemDefinition = ((AFortPlayerControllerAthena*)PlayerController)->SwappingItemDefinition;
	if (!SwappingItemDefinition) { /*std::cout << "SwappingItemDefinition is null" << std::endl;*/ return; }

	FFortItemEntry* SwappingItemEntry = FindItemEntry(PlayerController, SwappingItemDefinition);
	if (!SwappingItemEntry) { /*std::cout << "ItemEntry is null" << std::endl;*/ return; }

	PlayerController->MyFortPawn->EquipWeaponDefinition((UFortWeaponItemDefinition*)SwappingItemDefinition, SwappingItemEntry->ItemGuid);

	//if (PlayerController->MyFortPawn->AbilitySystemComponent)
		//GiveModifiers((AFortPlayerControllerAthena*)PlayerController, GetPlaylist());
}

void GiveVehicleWeapon(AFortPlayerControllerAthena* PlayerController, AFortAthenaVehicle* Vehicle) {
	if (!PlayerController) { std::cout << "PlayerController is null" << std::endl;  return; }
	if (!Vehicle) { std::cout << "Vehicle is null" << std::endl;  return; }

	uint8 SeatIdx = PlayerController->GetVehicleSeatTransitionTargetIndex();
	if (SeatIdx != 0 || !PlayerController->MyFortPawn->IsInVehicle()) { /*std::cout << "SeatIdx is not 0" << std::endl;*/ return; }

	PlayerController->SwappingItemDefinition = PlayerController->MyFortPawn->CurrentWeapon->WeaponData;

	UFortVehicleSeatWeaponComponent* WeaponComponent = Vehicle->GetSeatWeaponComponent(0); // scuffed
	if (!WeaponComponent) { /*std::cout << std::format("WeaponComponent is null for SeatIdx: {}", SeatIdx) << std::endl;*/ return; }

	UFortWeaponItemDefinition* VehicleWeaponDefinition = WeaponComponent->WeaponSeatDefinitions[0].VehicleWeapon; // scuffed
	if (!VehicleWeaponDefinition) { /*std::cout << std::format("VehicleWeaponDefinition is null for SeatIdx: {}", SeatIdx) << std::endl;*/ return; }

	GiveItem(PlayerController, VehicleWeaponDefinition, 1, 9999, false, false);

	auto ItemEntry = FindItemEntry(PlayerController, VehicleWeaponDefinition);
	if (!ItemEntry) { std::cout << "no item entry" << std::endl; return; }

	PlayerController->MyFortPawn->EquipWeaponDefinition(VehicleWeaponDefinition, ItemEntry->ItemGuid);

	AFortWeaponRangedForVehicle* VehicleWeapon = Cast<AFortWeaponRangedForVehicle>(PlayerController->MyFortPawn->CurrentWeapon);
	if (!VehicleWeapon) { std::cout << "VehicleWeapon is null" << std::endl; return; }

	FMountedWeaponInfo MountedWeaponInfo{};
	MountedWeaponInfo.bNeedsVehicleAttachment = true;
	MountedWeaponInfo.bTargetSourceFromVehicleMuzzle = true;

	FMountedWeaponInfoRepped MountedWeaponInfoRepped{};
	MountedWeaponInfoRepped.HostVehicleCachedActor = Vehicle;
	MountedWeaponInfoRepped.HostVehicleSeatIndexCached = SeatIdx;

	VehicleWeapon->MountedWeaponInfo = MountedWeaponInfo;
	VehicleWeapon->MountedWeaponInfoRepped = MountedWeaponInfoRepped;

	VehicleWeapon->OnRep_MountedWeaponInfoRepped();
	VehicleWeapon->OnHostVehicleSetup();
}




inline void (*ServerRequestSeatChangeOG)(AFortPlayerControllerAthena* PC, int TargetSeatIndex);
inline void ServerRequestSeatChange(AFortPlayerControllerAthena* PC, int TargetSeatIndex) {
	//LOG("Called!\n");
	/*auto Pawn = PC->MyFortPawn;

	if (!Pawn)
		return ServerRequestSeatChangeOG(PC, TargetSeatIndex);

	auto Vehicle = Pawn->GetVehicle();

	if (!Vehicle)
		return ServerRequestSeatChangeOG(PC, TargetSeatIndex);

	int lastSeats[] = {
		3,
		0,
		1,
		2
	};

	auto SeatIndex2 = Vehicle->FindSeatIndex(Pawn);
	auto SeatIndex = lastSeats[TargetSeatIndex];
	log_debug("%d %d\n", SeatIndex2, SeatIndex);
	auto OldVehicleWeaponDefinition = GetVehicleWeaponForSeat(Vehicle, SeatIndex);

	log_debug("OldVehicleWeaponDefinition: %s\n", OldVehicleWeaponDefinition ? OldVehicleWeaponDefinition->GetFullName().c_str() : "BadRead");

	if (!OldVehicleWeaponDefinition)
		return ServerRequestSeatChangeOG(PC, TargetSeatIndex);

	auto WorldInventory = PC->WorldInventory;

	if (!WorldInventory)
		return ServerRequestSeatChangeOG(PC, TargetSeatIndex);

	auto OldVehicleWeaponInstance = FindItemInstance(WorldInventory, OldVehicleWeaponDefinition);

	if (OldVehicleWeaponInstance)
	{
		bool bShouldUpdate = false;
		RemoveItem(WorldInventory, OldVehicleWeaponInstance->ItemEntry.ItemGuid, &bShouldUpdate, OldVehicleWeaponInstance->ItemEntry.Count, true);
		log_debug("removed\n");

		if (bShouldUpdate) {
			WorldInventory->HandleInventoryLocalUpdate();

			WorldInventory->Inventory.MarkArrayDirty();
			log_debug("updated\n");
		}
	}

	UFortWeaponItemDefinition* RequestingVehicleWeaponDefinition = GetVehicleWeaponForSeat(Vehicle, TargetSeatIndex);
	if (!RequestingVehicleWeaponDefinition)
	{
		auto PickaxeInstance = GetPickaxeInstance(WorldInventory);
		log_debug("pickaxe\n");


		if (!PickaxeInstance)
			return ServerRequestSeatChangeOG(PC, TargetSeatIndex);

		//ServerExecuteInventoryItem(PC, PickaxeInstance->ItemEntry.ItemGuid); // Bad, we should equip the last weapon.
		Pawn->EquipWeaponDefinition((UFortWeaponItemDefinition *) PickaxeInstance->ItemEntry.ItemDefinition, PickaxeInstance->ItemEntry.ItemGuid);
		log_debug("Equipped!\n");
		return ServerRequestSeatChangeOG(PC, TargetSeatIndex);
	}

	auto NewAndModifiedInstances = InvAddItem(WorldInventory, RequestingVehicleWeaponDefinition, nullptr);
	UFortWorldItem *RequestedVehicleInstance = (UFortWorldItem *) NewAndModifiedInstances.first[0];

	if (!RequestedVehicleInstance)
		return ServerRequestSeatChangeOG(PC, TargetSeatIndex);

	WorldInventory->HandleInventoryLocalUpdate();

	WorldInventory->Inventory.MarkArrayDirty();

	log_debug("equip htis idiot\n");
	auto RequestedVehicleWeapon = Pawn->EquipWeaponDefinition(RequestingVehicleWeaponDefinition, RequestedVehicleInstance->ItemEntry.ItemGuid);
	log_debug("ploosh is now mad\n");*/

	return ServerRequestSeatChangeOG(PC, TargetSeatIndex);
};