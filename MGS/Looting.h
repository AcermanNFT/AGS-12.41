#pragma once
#include "Utils.h"
#include "Lootpool.h"
#include <vector>
#include <map>
#include <algorithm>
#include <random>
#include <unordered_set>
#include <string>
#include <cstdlib>
#include <ctime>  


inline UFortItemDefinition* GetRandomItemByProbability(TArray<LootItemInfo>& Items) {
    int32 TotalWeight = 0;
    for (int32 i = 0; i < Items.Num(); i++) {
        TotalWeight += Items[i].Probability;
    }

    int32 RandomWeight = std::rand() % TotalWeight;
    int32 CumulativeWeight = 0;

    for (int32 i = 0; i < Items.Num(); i++) {
        CumulativeWeight += Items[i].Probability;
        if (RandomWeight < CumulativeWeight) {
            return Items[i].ItemDefinition;
        }
    }
}

inline char __fastcall SpawnllamaLoot(ABuildingContainer* Object)
{
    FVector Loc = Object->K2_GetActorLocation() + (Object->GetActorForwardVector() * Object->LootSpawnLocation_Athena.X) + (Object->GetActorRightVector() * Object->LootSpawnLocation_Athena.Y) + (Object->GetActorUpVector() * Object->LootSpawnLocation_Athena.Z);
    UFortItemDefinition* AmmoItemHeavy = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsHeavy.AthenaAmmoDataBulletsHeavy");
    if (AmmoItemHeavy) {
        NewSpawnPickup(Loc, AmmoItemHeavy, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, 90, 1);
    }
    UFortItemDefinition* AmmoItemAR = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium");
    if (AmmoItemAR) {
        NewSpawnPickup(Loc, AmmoItemAR, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, 150, 1);
    }
    UFortItemDefinition* AmmoItemSMG = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight");
    if (AmmoItemSMG) {
        NewSpawnPickup(Loc, AmmoItemSMG, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, 180, 1);
    }
    UFortItemDefinition* AmmoItemShotGun = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataShells.AthenaAmmoDataShells");
    if (AmmoItemShotGun) {
        NewSpawnPickup(Loc, AmmoItemShotGun, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, 30, 1);
    }
    static auto WoodDef = StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/WoodItemData.WoodItemData");
    static auto StoneDef = StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/StoneItemData.StoneItemData");
    static auto MetalDef = StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/MetalItemData.MetalItemData");
    NewSpawnPickup(Loc, WoodDef, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 350, 1);
    NewSpawnPickup(Loc, StoneDef, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 350, 1);
    NewSpawnPickup(Loc, MetalDef, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 350, 1);
    TArray<ConsumableInfo> GetConsumable = Consumable();
    for (int i = 0; i < 2; ++i) //we want to loop twice so 2 consumables spawn 
    {
        int32 ConsumableIndex = std::rand() % GetConsumable.Num();
        ConsumableInfo& RandomConsumable = GetConsumable[ConsumableIndex];
        UFortItemDefinition* RandomConsum = RandomConsumable.Definition;
        int Quantity = RandomConsumable.Quantity;
        NewSpawnPickup(Loc, RandomConsum, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, Quantity, 1);
    }
    return 1;
}

inline char __fastcall SpawnLoot(ABuildingContainer* Object)
{
    string ClassName = Object->Class->GetName();

    if (ClassName.contains("Tiered_Chest_Apollo_IceBox"))
    {
        if (Object->bAlreadySearched) return 0;

        FVector Loc = Object->K2_GetActorLocation() + (Object->GetActorForwardVector() * Object->LootSpawnLocation_Athena.X) + (Object->GetActorRightVector() * Object->LootSpawnLocation_Athena.Y) + (Object->GetActorUpVector() * Object->LootSpawnLocation_Athena.Z);


        TArray<LootItemInfo> GetIceBox = IceBox();
        UFortItemDefinition* Coal = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/Seasonal/WID_Athena_Bucket_Coal.WID_Athena_Bucket_Coal"); // i hate you vs
        UFortItemDefinition* SnowMan = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Gameplay/SneakySnowmanV2/AGID_SneakySnowmanV2.AGID_SneakySnowmanV2"); // i hate you vs

        for (int i = 0; i < 2; ++i) //we want to loop twice so 2 items spawn
        {
            UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetIceBox);
            if (RandomItem)
            {
                if (RandomItem == Coal || RandomItem == SnowMan)
                {
                    NewSpawnPickup(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Unset, 1, 5);
                }
                else
                {
                    NewSpawnPickup(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Unset, 1, 1);
                }
            }
        }

        Object->bAlreadySearched = true;
        Object->SearchBounceData.SearchAnimationCount++;
        Object->BounceContainer();
        Object->OnRep_bAlreadySearched();

        return 1;
    }

    else if (ClassName.contains("Tiered_Chest"))
    {
        if (Object->bAlreadySearched) return 0;

        FVector Loc = Object->K2_GetActorLocation() + (Object->GetActorForwardVector() * Object->LootSpawnLocation_Athena.X) + (Object->GetActorRightVector() * Object->LootSpawnLocation_Athena.Y) + (Object->GetActorUpVector() * Object->LootSpawnLocation_Athena.Z);

        TArray<LootItemInfo> GetAssaultRifle = AssaultRifleForChest();
        TArray<LootItemInfo> GetShotgun = ShotgunForChest();
        TArray<LootItemInfo> GetSMG = SMGForChest();
        TArray<LootItemInfo> GetPistol = PistolForChest();
        TArray<LootItemInfo> GetSniper = SniperForChest();
        TArray<LootItemInfo> GetRPG = RPGForChest();

        int32_t RandomCategory = std::rand() % 6 + 1; //really proper


        if (RandomCategory == 1) {
            UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetAssaultRifle);
            if (RandomItem) {
                NewSpawnPickup(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 30);
                UFortItemDefinition* AmmoItem = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium");
                if (AmmoItem) {
                    NewSpawnPickup(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 30, 30);
                }
                TArray<ConsumableInfo> GetConsumable = Consumable();
                TArray<MatsInfo> GetMats = Mats();

                int32 ConsumableIndex = std::rand() % GetConsumable.Num();
                ConsumableInfo& RandomConsumable = GetConsumable[ConsumableIndex];
                UFortItemDefinition* RandomConsum = RandomConsumable.Definition;
                int Quantity = RandomConsumable.Quantity;
                NewSpawnPickup(Loc, RandomConsum, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, Quantity, 1);

                int32 MatsIndex = std::rand() % GetMats.Num();
                MatsInfo& RandomMats = GetMats[MatsIndex];
                UFortItemDefinition* RandomMat = RandomMats.Definition;
                int QuantityM = RandomMats.Quantity;
                NewSpawnPickup(Loc, RandomMat, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, QuantityM, 1);
            }
        }
        else if (RandomCategory == 2) {
            UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetShotgun);
            if (RandomItem) {
                NewSpawnPickup(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 5);
                UFortItemDefinition* AmmoItem = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataShells.AthenaAmmoDataShells");
                if (AmmoItem) {
                    NewSpawnPickup(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 5, 5);
                }
                TArray<ConsumableInfo> GetConsumable = Consumable();
                TArray<MatsInfo> GetMats = Mats();

                int32 ConsumableIndex = std::rand() % GetConsumable.Num();
                ConsumableInfo& RandomConsumable = GetConsumable[ConsumableIndex];
                UFortItemDefinition* RandomConsum = RandomConsumable.Definition;
                int Quantity = RandomConsumable.Quantity;
                NewSpawnPickup(Loc, RandomConsum, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, Quantity, 1);

                int32 MatsIndex = std::rand() % GetMats.Num();
                MatsInfo& RandomMats = GetMats[MatsIndex];
                UFortItemDefinition* RandomMat = RandomMats.Definition;
                int QuantityM = RandomMats.Quantity;
                NewSpawnPickup(Loc, RandomMat, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, QuantityM, 1);
            }
        }
        else if (RandomCategory == 3) {
            UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetSMG);
            if (RandomItem) {
                NewSpawnPickup(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 28);
                UFortItemDefinition* AmmoItem = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight");
                if (AmmoItem) {
                    NewSpawnPickup(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 12, 18);
                }
                TArray<ConsumableInfo> GetConsumable = Consumable();
                TArray<MatsInfo> GetMats = Mats();

                int32 ConsumableIndex = std::rand() % GetConsumable.Num();
                ConsumableInfo& RandomConsumable = GetConsumable[ConsumableIndex];
                UFortItemDefinition* RandomConsum = RandomConsumable.Definition;
                int Quantity = RandomConsumable.Quantity;
                NewSpawnPickup(Loc, RandomConsum, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, Quantity, 1);

                int32 MatsIndex = std::rand() % GetMats.Num();
                MatsInfo& RandomMats = GetMats[MatsIndex];
                UFortItemDefinition* RandomMat = RandomMats.Definition;
                int QuantityM = RandomMats.Quantity;
                NewSpawnPickup(Loc, RandomMat, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, QuantityM, 1);
            }
        }
        else if (RandomCategory == 4) {
            UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetPistol);
            if (RandomItem) {
                NewSpawnPickup(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 28);
                UFortItemDefinition* AmmoItem = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight");
                if (AmmoItem) {
                    NewSpawnPickup(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 12, 18);
                }
                TArray<ConsumableInfo> GetConsumable = Consumable();
                TArray<MatsInfo> GetMats = Mats();

                int32 ConsumableIndex = std::rand() % GetConsumable.Num();
                ConsumableInfo& RandomConsumable = GetConsumable[ConsumableIndex];
                UFortItemDefinition* RandomConsum = RandomConsumable.Definition;
                int Quantity = RandomConsumable.Quantity;
                NewSpawnPickup(Loc, RandomConsum, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, Quantity, 1);

                int32 MatsIndex = std::rand() % GetMats.Num();
                MatsInfo& RandomMats = GetMats[MatsIndex];
                UFortItemDefinition* RandomMat = RandomMats.Definition;
                int QuantityM = RandomMats.Quantity;
                NewSpawnPickup(Loc, RandomMat, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, QuantityM, 1);
            }
        }
        else if (RandomCategory == 5) {
            UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetSniper);
            if (RandomItem) {
                NewSpawnPickup(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 1);
                UFortItemDefinition* AmmoItem = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsHeavy.AthenaAmmoDataBulletsHeavy");
                if (AmmoItem) {
                    NewSpawnPickup(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 12, 1);
                }
                TArray<ConsumableInfo> GetConsumable = Consumable();
                TArray<MatsInfo> GetMats = Mats();

                int32 ConsumableIndex = std::rand() % GetConsumable.Num();
                ConsumableInfo& RandomConsumable = GetConsumable[ConsumableIndex];
                UFortItemDefinition* RandomConsum = RandomConsumable.Definition;
                int Quantity = RandomConsumable.Quantity;
                NewSpawnPickup(Loc, RandomConsum, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, Quantity, 1);

                int32 MatsIndex = std::rand() % GetMats.Num();
                MatsInfo& RandomMats = GetMats[MatsIndex];
                UFortItemDefinition* RandomMat = RandomMats.Definition;
                int QuantityM = RandomMats.Quantity;
                NewSpawnPickup(Loc, RandomMat, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, QuantityM, 1);
            }
        }
        else if (RandomCategory == 6) {
            UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetRPG);
            if (RandomItem) {
                NewSpawnPickup(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 1);
                UFortItemDefinition* AmmoItem = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AmmoDataRockets.AmmoDataRockets");
                if (AmmoItem) {
                    NewSpawnPickup(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 9, 1);
                }
                TArray<ConsumableInfo> GetConsumable = Consumable();
                TArray<MatsInfo> GetMats = Mats();

                int32 ConsumableIndex = std::rand() % GetConsumable.Num();
                ConsumableInfo& RandomConsumable = GetConsumable[ConsumableIndex];
                UFortItemDefinition* RandomConsum = RandomConsumable.Definition;
                int Quantity = RandomConsumable.Quantity;
                NewSpawnPickup(Loc, RandomConsum, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, Quantity, 1);

                int32 MatsIndex = std::rand() % GetMats.Num();
                MatsInfo& RandomMats = GetMats[MatsIndex];
                UFortItemDefinition* RandomMat = RandomMats.Definition;
                int QuantityM = RandomMats.Quantity;
                NewSpawnPickup(Loc, RandomMat, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, QuantityM, 1);
            }
        }

        Object->bAlreadySearched = true;
        Object->SearchBounceData.SearchAnimationCount++;
        Object->BounceContainer();
        Object->OnRep_bAlreadySearched();

        return 1;
    }

    else if (ClassName.contains("Tiered_Ammo"))
    {
        if (Object->bAlreadySearched) return 0;

        FVector Loc = Object->K2_GetActorLocation() + (Object->GetActorForwardVector() * Object->LootSpawnLocation_Athena.X) + (Object->GetActorRightVector() * Object->LootSpawnLocation_Athena.Y) + (Object->GetActorUpVector() * Object->LootSpawnLocation_Athena.Z);


        TArray<AmoInfo> GetAmo = Amo();

        for (int i = 0; i < 2; ++i) //we want to loop twice so 2 types of amo spawn
        {
            int32 AmoIndex = std::rand() % GetAmo.Num();
            AmoInfo& RandomAmo = GetAmo[AmoIndex];
            UFortItemDefinition* RandomAmu = RandomAmo.Definition;
            int Quantity = RandomAmo.Quantity;
            NewSpawnPickup(Loc, RandomAmu, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::AmmoBox, Quantity, 1);
        }

        Object->bAlreadySearched = true;
        Object->SearchBounceData.SearchAnimationCount++;
        Object->BounceContainer();
        Object->OnRep_bAlreadySearched();

        return 1;
    }

    else  if (ClassName.contains("Barrel_FishingRod"))
    {
        if (Object->bAlreadySearched) return 0;

        FVector Loc = Object->K2_GetActorLocation() + (Object->GetActorForwardVector() * Object->LootSpawnLocation_Athena.X) + (Object->GetActorRightVector() * Object->LootSpawnLocation_Athena.Y) + (Object->GetActorUpVector() * Object->LootSpawnLocation_Athena.Z);


        TArray<LootItemInfo> GetBarrel = Barrel();
        UFortItemDefinition* Harpon = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Consumables/HappyGhost/WID_Athena_HappyGhost.WID_Athena_HappyGhost"); // i hate you vs

        for (int i = 0; i < 2; ++i) //we want to loop twice so 2 items spawn
        {
            UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetBarrel);
            if (RandomItem)
            {
                if (RandomItem == Harpon) // if i directly do smth like "WID_Athena_HappyGhost" it gives me Error C2446	'==': no conversion from 'const char [22] to 'SDK::UFortItemDefinition * but this workes some how
                {
                    NewSpawnPickup(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 10);
                }
                else
                {
                    NewSpawnPickup(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 9999); // idk what amo i should set it??? and nullptr doesnt work (this is for fishing rod)
                }
            }
        }

        Object->bAlreadySearched = true;
        Object->SearchBounceData.SearchAnimationCount++;
        Object->BounceContainer();
        Object->OnRep_bAlreadySearched();

        return 1;
    }

}

inline char __fastcall SpawnSupplyLoot(ABuildingContainer* Object)
{

    FVector Loc = Object->K2_GetActorLocation() + (Object->GetActorForwardVector() * Object->LootSpawnLocation_Athena.X) + (Object->GetActorRightVector() * Object->LootSpawnLocation_Athena.Y) + (Object->GetActorUpVector() * Object->LootSpawnLocation_Athena.Z);

    TArray<LootItemInfo> GetAssaultRifle = AssaultRifleForSupply();
    TArray<LootItemInfo> GetShotgun = ShotgunForSupply();
    TArray<LootItemInfo> GetSMG = SMGForSupply();
    TArray<LootItemInfo> GetPistol = PistolForSupply();
    TArray<LootItemInfo> GetSniper = SniperForSupply();
    TArray<LootItemInfo> GetRPG = RPGForSupply();

    int32_t RandomCategory = std::rand() % 6 + 1; //really proper


    if (RandomCategory == 1) {
        UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetAssaultRifle);
        if (RandomItem) {
            NewSpawnPickup(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, 1, 30);
            UFortItemDefinition* AmmoItem = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium");
            if (AmmoItem) {
                NewSpawnPickup(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, 20, 30);
            }
            TArray<ConsumableInfo> GetConsumable = Consumable();
            TArray<MatsInfo> GetMats = Mats();

            for (int i = 0; i < 2; ++i) //we want to loop twice so 2 consumables spawn and 2 mats
            {
                int32 ConsumableIndex = std::rand() % GetConsumable.Num();
                ConsumableInfo& RandomConsumable = GetConsumable[ConsumableIndex];
                UFortItemDefinition* RandomConsum = RandomConsumable.Definition;
                int Quantity = RandomConsumable.Quantity;
                NewSpawnPickup(Loc, RandomConsum, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, Quantity, 1);

                int32 MatsIndex = std::rand() % GetMats.Num();
                MatsInfo& RandomMats = GetMats[MatsIndex];
                UFortItemDefinition* RandomMat = RandomMats.Definition;
                int QuantityM = RandomMats.Quantity;
                NewSpawnPickup(Loc, RandomMat, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, QuantityM, 1);
            }
        }
    }
    else if (RandomCategory == 2) {
        UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetShotgun);
        if (RandomItem) {
            NewSpawnPickup(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, 1, 5);
            UFortItemDefinition* AmmoItem = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataShells.AthenaAmmoDataShells");
            if (AmmoItem) {
                NewSpawnPickup(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, 5, 5);
            }
            TArray<ConsumableInfo> GetConsumable = Consumable();
            TArray<MatsInfo> GetMats = Mats();

            for (int i = 0; i < 2; ++i) //we want to loop twice so 2 consumables spawn and 2 mats
            {
                int32 ConsumableIndex = std::rand() % GetConsumable.Num();
                ConsumableInfo& RandomConsumable = GetConsumable[ConsumableIndex];
                UFortItemDefinition* RandomConsum = RandomConsumable.Definition;
                int Quantity = RandomConsumable.Quantity;
                NewSpawnPickup(Loc, RandomConsum, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, Quantity, 1);

                int32 MatsIndex = std::rand() % GetMats.Num();
                MatsInfo& RandomMats = GetMats[MatsIndex];
                UFortItemDefinition* RandomMat = RandomMats.Definition;
                int QuantityM = RandomMats.Quantity;
                NewSpawnPickup(Loc, RandomMat, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, QuantityM, 1);
            }
        }
    }
    else if (RandomCategory == 3) {
        UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetSMG);
        if (RandomItem) {
            NewSpawnPickup(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, 1, 28);
            UFortItemDefinition* AmmoItem = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight");
            if (AmmoItem) {
                NewSpawnPickup(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, 5, 18);
            }
            TArray<ConsumableInfo> GetConsumable = Consumable();
            TArray<MatsInfo> GetMats = Mats();

            for (int i = 0; i < 2; ++i) //we want to loop twice so 2 consumables spawn and 2 mats
            {
                int32 ConsumableIndex = std::rand() % GetConsumable.Num();
                ConsumableInfo& RandomConsumable = GetConsumable[ConsumableIndex];
                UFortItemDefinition* RandomConsum = RandomConsumable.Definition;
                int Quantity = RandomConsumable.Quantity;
                NewSpawnPickup(Loc, RandomConsum, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, Quantity, 1);

                int32 MatsIndex = std::rand() % GetMats.Num();
                MatsInfo& RandomMats = GetMats[MatsIndex];
                UFortItemDefinition* RandomMat = RandomMats.Definition;
                int QuantityM = RandomMats.Quantity;
                NewSpawnPickup(Loc, RandomMat, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, QuantityM, 1);
            }
        }
    }
    else if (RandomCategory == 4) {
        UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetPistol);
        if (RandomItem) {
            NewSpawnPickup(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 28);
            UFortItemDefinition* AmmoItem = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight");
            if (AmmoItem) {
                NewSpawnPickup(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 5, 18);
            }
            TArray<ConsumableInfo> GetConsumable = Consumable();
            TArray<MatsInfo> GetMats = Mats();

            for (int i = 0; i < 2; ++i) //we want to loop twice so 2 consumables spawn and 2 mats
            {
                int32 ConsumableIndex = std::rand() % GetConsumable.Num();
                ConsumableInfo& RandomConsumable = GetConsumable[ConsumableIndex];
                UFortItemDefinition* RandomConsum = RandomConsumable.Definition;
                int Quantity = RandomConsumable.Quantity;
                NewSpawnPickup(Loc, RandomConsum, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, Quantity, 1);

                int32 MatsIndex = std::rand() % GetMats.Num();
                MatsInfo& RandomMats = GetMats[MatsIndex];
                UFortItemDefinition* RandomMat = RandomMats.Definition;
                int QuantityM = RandomMats.Quantity;
                NewSpawnPickup(Loc, RandomMat, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, QuantityM, 1);
            }
        }
    }
    else if (RandomCategory == 5) {
        UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetSniper);
        if (RandomItem) {
            NewSpawnPickup(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 1);
            UFortItemDefinition* AmmoItem = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsHeavy.AthenaAmmoDataBulletsHeavy");
            if (AmmoItem) {
                NewSpawnPickup(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 4, 1);
            }
            TArray<ConsumableInfo> GetConsumable = Consumable();
            TArray<MatsInfo> GetMats = Mats();

            for (int i = 0; i < 2; ++i) //we want to loop twice so 2 consumables spawn and 2 mats
            {
                int32 ConsumableIndex = std::rand() % GetConsumable.Num();
                ConsumableInfo& RandomConsumable = GetConsumable[ConsumableIndex];
                UFortItemDefinition* RandomConsum = RandomConsumable.Definition;
                int Quantity = RandomConsumable.Quantity;
                NewSpawnPickup(Loc, RandomConsum, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, Quantity, 1);

                int32 MatsIndex = std::rand() % GetMats.Num();
                MatsInfo& RandomMats = GetMats[MatsIndex];
                UFortItemDefinition* RandomMat = RandomMats.Definition;
                int QuantityM = RandomMats.Quantity;
                NewSpawnPickup(Loc, RandomMat, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, QuantityM, 1);
            }
        }
    }
    else if (RandomCategory == 6) {
        UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetRPG);
        if (RandomItem) {
            NewSpawnPickup(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 1);
            UFortItemDefinition* AmmoItem = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AmmoDataRockets.AmmoDataRockets");
            if (AmmoItem) {
                NewSpawnPickup(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 9, 1);
            }
            TArray<ConsumableInfo> GetConsumable = Consumable();
            TArray<MatsInfo> GetMats = Mats();

            for (int i = 0; i < 2; ++i) //we want to loop twice so 2 consumables spawn and 2 mats
            {
                int32 ConsumableIndex = std::rand() % GetConsumable.Num();
                ConsumableInfo& RandomConsumable = GetConsumable[ConsumableIndex];
                UFortItemDefinition* RandomConsum = RandomConsumable.Definition;
                int Quantity = RandomConsumable.Quantity;
                NewSpawnPickup(Loc, RandomConsum, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, Quantity, 1);

                int32 MatsIndex = std::rand() % GetMats.Num();
                MatsInfo& RandomMats = GetMats[MatsIndex];
                UFortItemDefinition* RandomMat = RandomMats.Definition;
                int QuantityM = RandomMats.Quantity;
                NewSpawnPickup(Loc, RandomMat, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, QuantityM, 1);
            };
        }
    }

    //these are handled by the game and i dont need to do these (it actaully crashes if i do)
    //Object->bAlreadySearched = true;
    //Object->SearchBounceData.SearchAnimationCount++;
    //Object->BounceContainer();
    //Object->OnRep_bAlreadySearched();

    return 1;
}

inline char __fastcall SpawnFloorLoot(ABuildingContainer* Object)
{
    if (Object->bAlreadySearched) return 0;

    FVector Loc = Object->K2_GetActorLocation() + (Object->GetActorForwardVector() * Object->LootSpawnLocation_Athena.X) + (Object->GetActorRightVector() * Object->LootSpawnLocation_Athena.Y) + (Object->GetActorUpVector() * Object->LootSpawnLocation_Athena.Z);

    TArray<LootItemInfo> GetAssaultRifle = AssaultRifleForFloor();
    TArray<LootItemInfo> GetShotgun = ShotgunForFloor();
    TArray<LootItemInfo> GetSMG = SMGForFloor();
    TArray<LootItemInfo> GetPistol = PistolForFloor();
    TArray<LootItemInfo> GetSniper = SniperForFloor();

    int32_t RandomCategory = std::rand() % 10 + 1; // Increased range to 10 for better control its cause for mats

    if (RandomCategory >= 1 && RandomCategory <= 2) {
        UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetAssaultRifle);
        if (RandomItem) {
            NewSpawnPickup(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 30);
            UFortItemDefinition* AmmoItem = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium");
            if (AmmoItem) {
                NewSpawnPickup(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 20, 30);
            }
        }
    }
    else if (RandomCategory == 3 || RandomCategory == 4) {
        UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetShotgun);
        if (RandomItem) {
            NewSpawnPickup(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 5);
            UFortItemDefinition* AmmoItem = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataShells.AthenaAmmoDataShells");
            if (AmmoItem) {
                NewSpawnPickup(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 5, 5);
            }
        }
    }
    else if (RandomCategory == 5 || RandomCategory == 6) {
        UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetSMG);
        if (RandomItem) {
            NewSpawnPickup(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 28);
            UFortItemDefinition* AmmoItem = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight");
            if (AmmoItem) {
                NewSpawnPickup(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 5, 18);
            }
        }
    }
    else if (RandomCategory == 7) {
        UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetPistol);
        if (RandomItem) {
            NewSpawnPickup(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 28);
            UFortItemDefinition* AmmoItem = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight");
            if (AmmoItem) {
                NewSpawnPickup(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 5, 18);
            }
        }
    }
    else if (RandomCategory == 8) {
        UFortItemDefinition* RandomItem = GetRandomItemByProbability(GetSniper);
        if (RandomItem) {
            NewSpawnPickup(Loc, RandomItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1, 1);
            UFortItemDefinition* AmmoItem = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsHeavy.AthenaAmmoDataBulletsHeavy");
            if (AmmoItem) {
                NewSpawnPickup(Loc, AmmoItem, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 4, 1);
            }
        }
    }
    else if (RandomCategory == 9)
    {
        TArray<MatsInfo> GetMats = Mats();

        int32 MatsIndex = std::rand() % GetMats.Num();
        MatsInfo& RandomMats = GetMats[MatsIndex];
        UFortItemDefinition* RandomMat = RandomMats.Definition;
        int QuantityM = RandomMats.Quantity;
        NewSpawnPickup(Loc, RandomMat, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, QuantityM, 1);
    }
    else if (RandomCategory == 10)
    {
        TArray<ConsumableInfo> GetConsumable = Consumable();

        int32 ConsumableIndex = std::rand() % GetConsumable.Num();
        ConsumableInfo& RandomConsumable = GetConsumable[ConsumableIndex];
        UFortItemDefinition* RandomConsum = RandomConsumable.Definition;
        int Quantity = RandomConsumable.Quantity;
        NewSpawnPickup(Loc, RandomConsum, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, Quantity, 1);
    }
    return 1;
}


