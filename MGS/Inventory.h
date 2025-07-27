#pragma once
#include "Utils.h"

inline float GetMaxStackSize(UFortItemDefinition* Def)
{
	if (!Def->MaxStackSize.Curve.CurveTable)
		return Def->MaxStackSize.Value;
	EEvaluateCurveTableResult Result;
	float Ret;
	((UDataTableFunctionLibrary*)UDataTableFunctionLibrary::StaticClass()->DefaultObject)->EvaluateCurveTableRow(Def->MaxStackSize.Curve.CurveTable, Def->MaxStackSize.Curve.RowName, 0, &Result, &Ret, FString());
	return Ret;
}

inline void GiveItem(AFortPlayerController* PlayerController, UFortItemDefinition* ItemDefintion, int Count, int LoadedAmmo, bool bStack, bool bShowToast) {
	auto Item = Cast<UFortWorldItem>(ItemDefintion->CreateTemporaryItemInstanceBP(Count, 1));
	if (!bStack) {
		Item->OwnerInventory = PlayerController->WorldInventory;
		Item->SetOwningControllerForTemporaryItem(PlayerController);

		Item->ItemEntry.LoadedAmmo = LoadedAmmo;
		Item->ItemEntry.Count = Count;
		Item->ItemEntry.ItemDefinition = ItemDefintion;

		if (bShowToast) {
			FFortItemEntryStateValue StateValue{};
			StateValue.IntValue = 1;
			StateValue.StateType = EFortItemEntryState::ShouldShowItemToast;
			Item->ItemEntry.StateValues.Add(StateValue);
		}

		PlayerController->WorldInventory->Inventory.ItemInstances.Add(Item);
		PlayerController->WorldInventory->Inventory.ReplicatedEntries.Add(Item->ItemEntry);
		PlayerController->WorldInventory->Inventory.MarkItemDirty(Item->ItemEntry);
		PlayerController->WorldInventory->HandleInventoryLocalUpdate();
	}
	else {
		bool bFound = false;

		for (auto& ItemEntry : PlayerController->WorldInventory->Inventory.ReplicatedEntries) {
			if (ItemEntry.ItemDefinition == ItemDefintion) {
				bFound = true;

				ItemEntry.Count += Count;

				if (bShowToast) {
					FFortItemEntryStateValue StateValue{};
					StateValue.IntValue = 1;
					StateValue.StateType = EFortItemEntryState::ShouldShowItemToast;
					Item->ItemEntry.StateValues.Add(StateValue);
				}

				for (int i = 0; i < PlayerController->WorldInventory->Inventory.ReplicatedEntries.Num(); i++) {
					PlayerController->WorldInventory->Inventory.ReplicatedEntries[i].LoadedAmmo = ItemEntry.LoadedAmmo;
					PlayerController->WorldInventory->Inventory.ReplicatedEntries[i].Count = ItemEntry.Count;
					break;
				}

				PlayerController->WorldInventory->Inventory.MarkItemDirty(ItemEntry);
				PlayerController->WorldInventory->HandleInventoryLocalUpdate();
			}
		}

		if (!bFound) {
			GiveItem(PlayerController, ItemDefintion, Count, LoadedAmmo, false, bShowToast);
		}
	}
}

inline FFortItemEntry* FindItemEntry(AFortPlayerController* PlayerController, UFortItemDefinition* ItemDefinition) {
	for (auto& ItemEntry : PlayerController->WorldInventory->Inventory.ReplicatedEntries) {
		if (ItemEntry.ItemDefinition == ItemDefinition) {
			return &ItemEntry;
		}
	}

	return nullptr;
}

inline FFortItemEntry* FindItemEntry(AFortPlayerController* PlayerController, FGuid* Guid) {
	for (auto& ItemEntry : PlayerController->WorldInventory->Inventory.ReplicatedEntries) {
		if (ItemEntry.ItemGuid == *Guid) {
			return &ItemEntry;
		}
	}

	return nullptr;
}
inline void ModifyEntry(AFortPlayerControllerAthena* PC, FFortItemEntry& Entry)
{
	for (int32 i = 0; i < PC->WorldInventory->Inventory.ItemInstances.Num(); i++)
	{
		if (PC->WorldInventory->Inventory.ItemInstances[i]->ItemEntry.ItemGuid == Entry.ItemGuid)
		{
			PC->WorldInventory->Inventory.ItemInstances[i]->ItemEntry = Entry;
			break;
		}
	}
}

inline void UpdateInventory(AFortPlayerControllerAthena* PC, FFortItemEntry* Entry)
{	
	PC->WorldInventory->bRequiresLocalUpdate = true;
	PC->WorldInventory->HandleInventoryLocalUpdate();

	if (Entry)
	{
		PC->WorldInventory->Inventory.MarkItemDirty(*Entry);
	}
	else
	{
		PC->WorldInventory->Inventory.MarkArrayDirty();
	}
}

inline void RemoveItem(AFortPlayerController* PlayerController, FGuid Guid, int Count) {
	if (!PlayerController || !PlayerController->WorldInventory) return;

	auto& Inventory = PlayerController->WorldInventory->Inventory;
	auto& ReplicatedEntries = Inventory.ReplicatedEntries;

	bool ItemRemoved = false;

	for (int32 i = 0; i < ReplicatedEntries.Num(); ++i) {
		auto& Entry = ReplicatedEntries[i];

		if (Entry.ItemGuid == Guid) {
			Entry.Count = (Entry.Count > Count) ? (Entry.Count - Count) : 0;

			Inventory.MarkItemDirty(Entry);
			PlayerController->WorldInventory->HandleInventoryLocalUpdate();

			if (Entry.Count == 0) {
				ReplicatedEntries.Remove(i);
				ItemRemoved = true;
			}
			else {
				for (auto& OtherEntry : ReplicatedEntries) {
					OtherEntry.LoadedAmmo = Entry.LoadedAmmo;
				}
			}
			break;
		}
	}

	if (ItemRemoved) {
		for (int32 i = 0; i < Inventory.ItemInstances.Num(); ++i) {
			if (Inventory.ItemInstances[i]->GetItemGuid() == Guid) {
				Inventory.ItemInstances.Remove(i);
				break;
			}
		}

		Inventory.MarkArrayDirty();
	}

	PlayerController->WorldInventory->bRequiresLocalUpdate = true;
	PlayerController->WorldInventory->HandleInventoryLocalUpdate();
}

inline void Remove(AFortPlayerController* PC, UFortItemDefinition* Def, int Count = 1)
{
	for (int32 i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
	{
		if (PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition == Def)
		{
			PC->WorldInventory->Inventory.ReplicatedEntries[i].Count -= Count;
			if (PC->WorldInventory->Inventory.ReplicatedEntries[i].Count <= 0)
			{
				//Remove(PC, PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemGuid);
				break;
			}
			ModifyEntry((AFortPlayerControllerAthena*)PC, PC->WorldInventory->Inventory.ReplicatedEntries[i]);
			UpdateInventory((AFortPlayerControllerAthena*)PC, &PC->WorldInventory->Inventory.ReplicatedEntries[i]);
			break;
		}
	}
}

inline void UpdateLoadedAmmo(AFortPlayerController* PC, AFortWeapon* Weapon)
{
	for (int32 i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
	{
		if (PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemGuid == Weapon->ItemEntryGuid)
		{
			PC->WorldInventory->Inventory.ReplicatedEntries[i].LoadedAmmo = Weapon->AmmoCount;
			ModifyEntry((AFortPlayerControllerAthena*)PC, PC->WorldInventory->Inventory.ReplicatedEntries[i]);
			UpdateInventory((AFortPlayerControllerAthena*)PC, &PC->WorldInventory->Inventory.ReplicatedEntries[i]);
			break;
		}
	}
}

inline void UpdateLoadedAmmo(AFortPlayerController* PC, AFortWeapon* Weapon, int AmountToAdd)
{
	for (int32 i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
	{
		if (PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemGuid == Weapon->ItemEntryGuid)
		{
			PC->WorldInventory->Inventory.ReplicatedEntries[i].LoadedAmmo += AmountToAdd;
			ModifyEntry((AFortPlayerControllerAthena*)PC, PC->WorldInventory->Inventory.ReplicatedEntries[i]);
			UpdateInventory((AFortPlayerControllerAthena*)PC, &PC->WorldInventory->Inventory.ReplicatedEntries[i]);
			break;
		}
	}
}

inline EFortQuickBars GetQuickBars(UFortItemDefinition* ItemDefinition)
{
	if (!ItemDefinition->IsA(UFortWeaponMeleeItemDefinition::StaticClass()) && !ItemDefinition->IsA(UFortEditToolItemDefinition::StaticClass()) &&
		!ItemDefinition->IsA(UFortBuildingItemDefinition::StaticClass()) && !ItemDefinition->IsA(UFortAmmoItemDefinition::StaticClass()) && !ItemDefinition->IsA(UFortResourceItemDefinition::StaticClass()) && !ItemDefinition->IsA(UFortTrapItemDefinition::StaticClass()))
		return EFortQuickBars::Primary;

	return EFortQuickBars::Secondary;
}

inline void UpdateEntry(AFortPlayerController* PC, FFortItemEntry& Entry)
{
	for (size_t i = 0; i < PC->WorldInventory->Inventory.ItemInstances.Num(); i++)
	{
		if (PC->WorldInventory->Inventory.ItemInstances[i]->ItemEntry.ItemGuid == Entry.ItemGuid)
		{
			if (PC->WorldInventory->Inventory.ItemInstances[i]->ItemEntry.Count != Entry.Count)
			{
				PC->WorldInventory->Inventory.ItemInstances[i]->ItemEntry.Count = Entry.Count;
			}

			if (PC->WorldInventory->Inventory.ItemInstances[i]->ItemEntry.LoadedAmmo != Entry.LoadedAmmo)
			{
				PC->WorldInventory->Inventory.ItemInstances[i]->ItemEntry.LoadedAmmo = Entry.LoadedAmmo;
			}

			break;
		}
	}
}

inline void EonGiveItem(AFortPlayerControllerAthena* PC, UFortItemDefinition* Def, int Count = 1, int LoadedAmmo = 0, int Level = 0)
{
	UFortWorldItem* Item = (UFortWorldItem*)Def->CreateTemporaryItemInstanceBP(Count, Level);
	Item->SetOwningControllerForTemporaryItem(PC);
	Item->ItemEntry.LoadedAmmo = LoadedAmmo;

	PC->WorldInventory->Inventory.ReplicatedEntries.Add(Item->ItemEntry);
	PC->WorldInventory->Inventory.ItemInstances.Add(Item);

	UpdateInventory(PC, &Item->ItemEntry);
}


inline bool IsInventoryFull(AFortPlayerController* PC)
{
	int aaaaaa = 0;
	auto InstancesPtr = &PC->WorldInventory->Inventory.ItemInstances;
	for (int i = 0; i < InstancesPtr->Num(); i++)
	{
		if (InstancesPtr->operator[](i))
		{
			if (GetQuickBars(InstancesPtr->operator[](i)->ItemEntry.ItemDefinition) == EFortQuickBars::Primary)
			{
				aaaaaa++;

				if (aaaaaa >= 5)
				{
					break;
				}
			}
		}
	}

	return aaaaaa >= 5;
}

inline FFortItemEntry* FindEonEntry(AFortPlayerController* PC, UFortItemDefinition* Def)
{
	for (size_t i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
	{
		if (PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition == Def)
			return &PC->WorldInventory->Inventory.ReplicatedEntries[i];
	}
	return nullptr;
}

inline void EonRemove(AFortPlayerController* PC, FGuid Guid)
{
	for (size_t i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
	{
		if (PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemGuid == Guid)
		{
			PC->WorldInventory->Inventory.ReplicatedEntries.RemoveSingle(i);
			break;
		}
	}

	for (size_t i = 0; i < PC->WorldInventory->Inventory.ItemInstances.Num(); i++)
	{
		if (!PC->WorldInventory->Inventory.ItemInstances[i])
			continue;
		if (PC->WorldInventory->Inventory.ItemInstances[i]->ItemEntry.ItemGuid == Guid)
		{
			PC->WorldInventory->Inventory.ItemInstances.RemoveSingle(i);
			break;
		}
	}

	UpdateInventory((AFortPlayerControllerAthena*)PC, nullptr);
}

inline void EonRemove112(AFortPlayerController* PC, FFortItemEntry* Entry, int Count = 1)
{
	for (int32 i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
	{
		if (&PC->WorldInventory->Inventory.ReplicatedEntries[i] == Entry)
		{
			PC->WorldInventory->Inventory.ReplicatedEntries[i].Count -= Count;
			if (PC->WorldInventory->Inventory.ReplicatedEntries[i].Count <= 0)
			{
				EonRemove(PC, PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemGuid);
				break;
			}
			ModifyEntry((AFortPlayerControllerAthena*)PC, PC->WorldInventory->Inventory.ReplicatedEntries[i]);
			UpdateInventory((AFortPlayerControllerAthena*)PC, &PC->WorldInventory->Inventory.ReplicatedEntries[i]);
			break;
		}
	}
}


inline UFortWorldItem* EonPRoFindItemInstance(AFortInventory* inv, UFortItemDefinition* ItemDefinition)
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

inline UFortWorldItem* EonPRoFindItemInstance(AFortInventory* inv, const FGuid& Guid)
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


inline void GiveItem2Bot(AFortPlayerControllerAthena* PC, UFortWeaponItemDefinition* Weapon)
{
	EonGiveItem(PC, Weapon, 1, GetAmmoForDef(Weapon));
}


inline void RemoveAllDroppableItems(AFortPlayerControllerAthena* PlayerController) {
	if (!PlayerController)
		return;

	for (size_t i = 0; i < PlayerController->WorldInventory->Inventory.ItemInstances.Num(); i++) {
		if (PlayerController->WorldInventory->Inventory.ItemInstances[i]->CanBeDropped()) {
			PlayerController->WorldInventory->Inventory.ItemInstances.Remove(i);
			PlayerController->WorldInventory->Inventory.ReplicatedEntries.Remove(i);
			PlayerController->WorldInventory->Inventory.MarkArrayDirty();
		}
	}
}

inline FFortItemEntry* FFortItemEntry213213(AFortPlayerController* PC, FGuid& Guid)
{
	for (int32 i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
	{
		if (PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemGuid == Guid)
			return &PC->WorldInventory->Inventory.ReplicatedEntries[i];
	}
	return nullptr;
}