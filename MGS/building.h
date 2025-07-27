#pragma once
#include "Utils.h"
#include "Inventory.h"
#include "XP.h"
static inline void (*OnDamageServerOG)(ABuildingSMActor*, float, FGameplayTagContainer&, FVector&, FHitResult&, AFortPlayerControllerAthena*, AActor*, FGameplayEffectContextHandle&);
void OnDamageServer(ABuildingSMActor* ABuildingActor, float Damage, FGameplayTagContainer& DamageTags, FVector& Momentum, FHitResult& HitInfo, AFortPlayerControllerAthena* InstigatedBy, AActor* DamageCauser, FGameplayEffectContextHandle& EffectContext) {
    if (!ABuildingActor || !InstigatedBy || !DamageCauser)
        return OnDamageServerOG(ABuildingActor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);

    if (!InstigatedBy->IsA(AFortPlayerControllerAthena::StaticClass()) ||
        !DamageCauser->IsA(AFortWeapon::StaticClass()) ||
        !((AFortWeapon*)DamageCauser)->WeaponData->IsA(UFortWeaponMeleeItemDefinition::StaticClass()) ||
        ABuildingActor->bPlayerPlaced) {
        return OnDamageServerOG(ABuildingActor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);
    }

    UFortResourceItemDefinition* ResourceItemDefinition = UFortKismetLibrary::K2_GetResourceItemDefinition(ABuildingActor->ResourceType);
    if (!ResourceItemDefinition)
        return OnDamageServerOG(ABuildingActor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);

    float Average = 0.0f;
    UFortKismetLibrary::EvaluateCurveTableRow(ABuildingActor->BuildingResourceAmountOverride, 0.f, &Average, FString());
    float ResourceAmount = round(Average / (ABuildingActor->GetMaxHealth() / Damage));
    if (ResourceAmount <= 0)
        return OnDamageServerOG(ABuildingActor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);

    if (ResourceAmount > 0)
    {
        if (Damage == 100.f)
        {
            UFortAccoladeItemDefinition* AccoladeDef = StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_066_WeakSpotsInARow.AccoladeId_066_WeakSpotsInARow");
            if (AccoladeDef)
            {
                GiveAccolade(InstigatedBy, AccoladeDef);
            }
        }
    }

    InstigatedBy->ClientReportDamagedResourceBuilding(ABuildingActor, ABuildingActor->ResourceType, ResourceAmount, false, Damage == 100.f);
    GiveItem(InstigatedBy, ResourceItemDefinition, ResourceAmount, 0, true, false);

    return OnDamageServerOG(ABuildingActor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);
}
//__int64 (*OnDamageServerOG)(ABuildingSMActor* Actor, float Damage, FGameplayTagContainer DamageTags, FVector Momentum, FHitResult HitInfo, AFortPlayerControllerAthena* InstigatedBy, AActor* DamageCauser, FGameplayEffectContextHandle EffectContext);
//__int64 OnDamageServer(ABuildingSMActor* Actor, float Damage, FGameplayTagContainer DamageTags, FVector Momentum, FHitResult HitInfo, AFortPlayerControllerAthena* InstigatedBy, AActor* DamageCauser, FGameplayEffectContextHandle EffectContext)
//{
//	if (!Actor || !InstigatedBy || !InstigatedBy->IsA(AFortPlayerControllerAthena::StaticClass()) || !DamageCauser->IsA(AFortWeapon::StaticClass()) || !((AFortWeapon*)DamageCauser)->WeaponData->IsA(UFortWeaponMeleeItemDefinition::StaticClass()) || Actor->bPlayerPlaced)
//		return OnDamageServerOG(Actor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);
//
//	auto Def = UFortKismetLibrary::GetDefaultObj()->K2_GetResourceItemDefinition(Actor->ResourceType);
//
//	if (Def)
//	{
//		auto& BuildingResourceAmountOverride = Actor->BuildingResourceAmountOverride;
//		auto GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;
//
//		FString CurveTableAssetPath = UKismetStringLibrary::GetDefaultObj()->Conv_NameToString(GameState->CurrentPlaylistInfo.BasePlaylist->ResourceRates.ObjectID.AssetPathName);
//		static auto CurveTable = StaticLoadObject<UCurveTable>(CurveTableAssetPath.ToString()) ? StaticLoadObject<UCurveTable>(CurveTableAssetPath.ToString()) : StaticFindObject<UCurveTable>("/Game/Athena/Balance/DataTables/AthenaResourceRates.AthenaResourceRates");
//
//		float Average;
//		EEvaluateCurveTableResult OutCurveTable{};
//
//		reinterpret_cast<UDataTableFunctionLibrary*>(UDataTableFunctionLibrary::StaticClass()->DefaultObject)->EvaluateCurveTableRow(CurveTable, BuildingResourceAmountOverride.RowName, 0.f, &OutCurveTable, &Average, FString());
//		float FinalResourceCount = round(Average / (Actor->GetMaxHealth() / Damage));
//
//		if (FinalResourceCount > 0)
//		{
//			if (Damage == 100.f)
//			{
//				GiveAccolade(InstigatedBy, StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_066_WeakSpotsInARow.AccoladeId_066_WeakSpotsInARow"));
//			}
//			InstigatedBy->ClientReportDamagedResourceBuilding(Actor, Actor->ResourceType, FinalResourceCount, false, Damage == 100.f);
//			EonGiveItem(InstigatedBy, Def, FinalResourceCount, 0, true);
//		}
//	}
//
//	return OnDamageServerOG(Actor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);
//}


//FName PickaxeTagName;
//void (*OnDamageServerOG)(ABuildingSMActor*, float, FGameplayTagContainer, FVector, FHitResult, AFortPlayerControllerAthena*, AActor*, FGameplayEffectContextHandle);
//void OnDamageServer(ABuildingSMActor* Actor, float Damage, FGameplayTagContainer DamageTags, FVector Momentum, FHitResult HitInfo, AFortPlayerControllerAthena* InstigatedBy, AActor* DamageCauser, FGameplayEffectContextHandle EffectContext)
//{
//    bool FoundTag = false;
//    if (!InstigatedBy || Actor->bPlayerPlaced || Actor->GetHealth() == 1)
//        return OnDamageServerOG(Actor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);
//
//    for (size_t i = 0; i < DamageTags.GameplayTags.Num(); i++)
//    {
//        if (DamageTags.GameplayTags[i].TagName.ComparisonIndex == PickaxeTagName.ComparisonIndex)
//        {
//            FoundTag = true;
//            break;
//        }
//    }
//
//    if (!FoundTag)
//        return OnDamageServerOG(Actor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);
//
//    auto ResourceItemDef = GetFortKismet()->K2_GetResourceItemDefinition(Actor->ResourceType);
//
//    if (!ResourceItemDef)
//        return OnDamageServerOG(Actor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);
//    bool Found = false;
//
//    bool Weakspot = Damage == 100.f;
//    int ResCount = GetMath()->RandomIntegerInRange(3, 5);
//
//    if (Weakspot)
//    {
//        ResCount = GetMath()->RandomIntegerInRange(7, 12);
//    }
//
//    for (size_t i = 0; i < InstigatedBy->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
//    {
//        if (InstigatedBy->WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition == ResourceItemDef)
//        {
//            Found = true;
//            InstigatedBy->WorldInventory->Inventory.ReplicatedEntries[i].Count += ResCount;
//            if (InstigatedBy->WorldInventory->Inventory.ReplicatedEntries[i].Count > 999)
//            {
//                SpawnPickup(InstigatedBy->Pawn->K2_GetActorLocation(), InstigatedBy->WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition, InstigatedBy->WorldInventory->Inventory.ReplicatedEntries[i].Count - 999, 0, EFortPickupSourceTypeFlag::Tossed, EFortPickupSpawnSource::Unset, InstigatedBy->MyFortPawn);
//                InstigatedBy->WorldInventory->Inventory.ReplicatedEntries[i].Count = 999;
//            }
//            ModifyEntry(InstigatedBy, InstigatedBy->WorldInventory->Inventory.ReplicatedEntries[i]);
//            UpdateInventory(InstigatedBy, &InstigatedBy->WorldInventory->Inventory.ReplicatedEntries[i]);
//            break;
//        }
//    }
//
//    if (!Found)
//        EonGiveItem(InstigatedBy, ResourceItemDef, ResCount);
//
//    InstigatedBy->ClientReportDamagedResourceBuilding(Actor, Actor->ResourceType, ResCount, false, Weakspot);
//
//    return OnDamageServerOG(Actor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);
//}