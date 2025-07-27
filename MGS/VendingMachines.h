#pragma once
#include "Utils.h"
#include "PC.h"
#include "CustomLogs.h"

struct LootDrop
{
    FFortItemEntry* ItemEntry;

    FFortItemEntry* operator->() {
        return ItemEntry;
    }

    ~LootDrop()
    {

    }
};

static inline float RandomFloatForLoot(float AllWeightsSum)
{
    return (rand() * 0.000030518509f) * AllWeightsSum;
}

template <typename KeyType, typename ValueType>
static ValueType PickWeightedElement(const std::map<KeyType, ValueType>& Elements,
    std::function<float(ValueType)> GetWeightFn,
    std::function<float(float)> RandomFloatGenerator = RandomFloatForLoot,
    float TotalWeightParam = -1, bool bCheckIfWeightIsZero = false, int RandMultiplier = 1, KeyType* OutName = nullptr, bool bPrint = false, bool bKeepGoingUntilWeGetValue = false)
{
    float TotalWeight = TotalWeightParam;

    if (TotalWeight == -1)
    {
        TotalWeight = std::accumulate(Elements.begin(), Elements.end(), 0.0f, [&](float acc, const std::pair<KeyType, ValueType>& p) {
            auto Weight = GetWeightFn(p.second);

            if (bPrint)
            {
                // if (Weight != 0)
                {
                    log_debug("Adding weight %f\n", Weight);
                }
            }

            return acc + Weight;
            });
    }

    float RandomNumber = // UKismetMathLibrary::RandomFloatInRange(0, TotalWeight);
        RandMultiplier * RandomFloatGenerator(TotalWeight);

    if (bPrint)
    {
        log_debug("RandomNumber: %f TotalWeight: %f Elements.size(): %zd\n", RandomNumber, TotalWeight, Elements.size());
    }

    for (auto& Element : Elements)
    {
        float Weight = GetWeightFn(Element.second);

        if (bCheckIfWeightIsZero && Weight == 0)
            continue;

        if (RandomNumber <= Weight)
        {
            if (OutName)
                *OutName = Element.first;

            return Element.second;
        }

        RandomNumber -= Weight;
    }

    if (bKeepGoingUntilWeGetValue)
        return PickWeightedElement<KeyType, ValueType>(Elements, GetWeightFn, RandomFloatGenerator, TotalWeightParam, bCheckIfWeightIsZero, RandMultiplier, OutName, bPrint, bKeepGoingUntilWeGetValue);

    return ValueType();
}



template <typename KeyType, typename ValueType>
static ValueType PickWeightedElement(const std::unordered_map<KeyType, ValueType>& Elements,
    std::function<float(ValueType)> GetWeightFn,
    std::function<float(float)> RandomFloatGenerator = RandomFloatForLoot,
    float TotalWeightParam = -1, bool bCheckIfWeightIsZero = false, int RandMultiplier = 1, KeyType* OutName = nullptr, bool bPrint = false, bool bKeepGoingUntilWeGetValue = false)
{
    float TotalWeight = TotalWeightParam;

    if (TotalWeight == -1)
    {
        TotalWeight = std::accumulate(Elements.begin(), Elements.end(), 0.0f, [&](float acc, const std::pair<KeyType, ValueType>& p) {
            auto Weight = GetWeightFn(p.second);

            //if (bPrint)
            {
                // if (Weight != 0)
                {
                    log_debug("Adding weight %f\n", Weight);
                }
            }

            return acc + Weight;
            });
    }

    float RandomNumber = // UKismetMathLibrary::RandomFloatInRange(0, TotalWeight);
        RandMultiplier * RandomFloatGenerator(TotalWeight);

    //if (bPrint)
    {
        log_debug("RandomNumber: %f TotalWeight: %f Elements.size(): %d\n", RandomNumber, TotalWeight, Elements.size());
    }

    for (auto& Element : Elements)
    {
        float Weight = GetWeightFn(Element.second);

        if (bCheckIfWeightIsZero && Weight == 0)
            continue;

        if (RandomNumber <= Weight)
        {
            if (OutName)
                *OutName = Element.first;

            return Element.second;
        }

        RandomNumber -= Weight;
    }

    if (bKeepGoingUntilWeGetValue)
        return PickWeightedElement<KeyType, ValueType>(Elements, GetWeightFn, RandomFloatGenerator, TotalWeightParam, bCheckIfWeightIsZero, RandMultiplier, OutName, bPrint, bKeepGoingUntilWeGetValue);

    return ValueType();
}


template <typename RowStructType = uint8>
void CollectDataTablesRows(const std::vector<UDataTable*>& DataTables, std::map<FName, RowStructType*>* OutMap/*std::function<void(FName, RowStructType*)> AddToMap = []() {}*/, std::function<bool(FName, RowStructType*)> Check = []() { return true; })
{
    std::vector<UDataTable*> DataTablesToIterate;

    //static auto CompositeDataTableClass = StaticLoadObject<UClass>("/Script/Engine.CompositeDataTable");

    for (UDataTable* DataTable : DataTables)
    {

        // if (auto CompositeDataTable = Cast<UCompositeDataTable>(DataTable))
        if (DataTable->IsA(UCompositeDataTable::StaticClass()))
        {
            auto CompositeDataTable = (UCompositeDataTable*)DataTable;

            //static auto ParentTablesOffset = GetOffset(CompositeDataTable, "ParentTables");
            //auto ParentTables = *(TArray<UDataTable*> *) ((int64_t)CompositeDataTable + ParentTablesOffset);
            auto ParentTables = CompositeDataTable->ParentTables;

            for (int i = 0; i < ParentTables.Num(); ++i)
            {
                DataTablesToIterate.push_back(ParentTables[i]);
            }
        }

        DataTablesToIterate.push_back(DataTable);
    }


    //std::map<FName, RowStructType*>& OM = *OutMap;
    for (auto CurrentDataTable : DataTablesToIterate)
    {
        auto RM = *(RTMap<FName, RowStructType*> *) (&CurrentDataTable->RowMap);
        for (RTPair<FName, RowStructType*> CurrentPair : RM)
        {
            if (Check(CurrentPair.Key(), (RowStructType*)CurrentPair.Value()))
            {
                // LOG_INFO(LogDev, "Setting key with {} comp {} num: {} then iterating through map!", CurrentPair.Key().ToString(), CurrentPair.Key().ComparisonIndex.Value, CurrentPair.Key().Number);
                (*OutMap)[CurrentPair.Key()] = CurrentPair.Value();
                //log_debug("Set %s to %p\n", CurrentPair.Key().ToString().c_str(), (void*) CurrentPair.Value());
                //AddToMap(CurrentPair.Key(), CurrentPair.Value());

                /* for (auto PairInOutMap : *OutMap)
                {
                    // LOG_INFO(LogDev, "Current Row Key {} comp {} num: {}!", PairInOutMap.first.ToString(), PairInOutMap.first.ComparisonIndex.Value, PairInOutMap.first.Number);
                } */
            }
        }
    }
}

/*float*/int GetAmountOfLootPackagesToDrop(FFortLootTierData* LootTierData, int OriginalNumberLootDrops)
{
    if (LootTierData->LootPackageCategoryMinArray.Num() != LootTierData->LootPackageCategoryWeightArray.Num()
        || LootTierData->LootPackageCategoryMinArray.Num() != LootTierData->LootPackageCategoryMaxArray.Num()
        )
        return 0;

    // return OriginalNumberLootDrops;

    int/*float*/ MinimumLootDrops = 0;

    if (LootTierData->LootPackageCategoryMinArray.Num() > 0)
    {
        for (int i = 0; i < LootTierData->LootPackageCategoryMinArray.Num(); ++i)
        {
            // Fortnite does more here, we need to figure it out.
            MinimumLootDrops += LootTierData->LootPackageCategoryMinArray[i];
        }
    }

    if (MinimumLootDrops > OriginalNumberLootDrops)
    {
        log_warn("Requested %d loot drops but minimum drops is %d for loot package %s\n", OriginalNumberLootDrops, MinimumLootDrops, LootTierData->LootPackage.ToString().c_str());
        // Fortnite doesn't return here?
    }

    int SumLootPackageCategoryWeightArray = 0;

    if (LootTierData->LootPackageCategoryWeightArray.Num() > 0)
    {
        for (int i = 0; i < LootTierData->LootPackageCategoryWeightArray.Num(); ++i)
        {
            // Fortnite does more here, we need to figure it out.

            if (LootTierData->LootPackageCategoryWeightArray[i] > 0)
            {
                auto LootPackageCategoryMaxArrayIt = LootTierData->LootPackageCategoryMaxArray[i];

                float IDK = 0; // TODO

                if (LootPackageCategoryMaxArrayIt < 0 || IDK < LootPackageCategoryMaxArrayIt)
                {
                    SumLootPackageCategoryWeightArray += LootTierData->LootPackageCategoryWeightArray[i];
                }
            }
        }
    }

    // if (MinimumLootDrops < OriginalNumberLootDrops) // real commeneted one to one
    {
        // IDK

        while (SumLootPackageCategoryWeightArray > 0)
        {
            // HONESTLY IDEK WHAT FORTNITE DOES HERE

            /*float v29 = (float)rand() * 0.000030518509f;

            float v35 = (int)(float)((float)((float)((float)SumLootPackageCategoryWeightArray * v29)
                + (float)((float)SumLootPackageCategoryWeightArray * v29))
                + 0.5f) >> 1;*/

                // OutLootTierInfo->Hello++;
            MinimumLootDrops++;

            if (MinimumLootDrops >= OriginalNumberLootDrops)
                return MinimumLootDrops;

            SumLootPackageCategoryWeightArray--;
        }

        /* if (MinimumLootDrops < OriginalNumberLootDrops)
        {
            log_warn("Requested %d loot drops but maximum drops is %f for loot package %s\n", OriginalNumberLootDrops, MinimumLootDrops, LootTierData->LootPackage.ToString().c_str());
        } */
    }

    return MinimumLootDrops;
}

/*struct UFortLootPackage
{
    int CurrentIdx = 0;
    std::vector<FFortItemEntry> ItemEntries;
}; */

FFortLootTierData* PickLootTierData(const std::vector<UDataTable*>& LTDTables, FName LootTierGroup, int ForcedLootTier = -1, FName* OutRowName = nullptr) // Fortnite returns the row name and then finds the tier data again, but I really don't see the point of this.
{
    // This like isn't right, at all.

    int/*float*/ LootTier = ForcedLootTier;

    if (LootTier == -1)
    {
        // LootTier = ??
    }
    else
    {
        // buncha code im too lazy to reverse
    }

    // if (fabs(LootTier) <= 0.0000000099999999f)
      //  return 0;

    int Multiplier = LootTier == -1 ? 1 : LootTier; // Idk i think we need to fill out the code above for this to work properly maybe

    std::map<FName, FFortLootTierData*> TierGroupLTDs;

    CollectDataTablesRows<FFortLootTierData>(LTDTables, &TierGroupLTDs, [&](FName RowName, FFortLootTierData* TierData) -> bool {
        //log_debug("TG: %s, TTG: %s\n", TierData->TierGroup.ToString().c_str(), LootTierGroup.ToString().c_str());
        if (LootTierGroup == TierData->TierGroup)
        {
            //log_debug("LootTier: %d\n", TierData->LootTier);
            if ((LootTier == -1 ? true : LootTier == TierData->LootTier))
            {
                //log_debug("Weight Collect %f\n", TierData->Weight);
                TierData->Weight = 0.5f + GetMath()->RandomFloatInRange(0.5, 1);
                return true;
            }
        }

        return false;
        });

    // LOG_INFO(LogDev, "TierGroupLTDs.size(): {}", TierGroupLTDs.size());

    FFortLootTierData* ChosenRowLootTierData = PickWeightedElement<FName, FFortLootTierData*>(TierGroupLTDs,
        [](FFortLootTierData* LootTierData) -> float { /*log_debug("[PVendor] Weight LootTierData %f\n", LootTierData->Weight);*/  return LootTierData->Weight; }, RandomFloatForLoot, -1,
        true, Multiplier, OutRowName);

    return ChosenRowLootTierData;
}

class UFortLootLevel
{
public:
    static int GetItemLevel(const FDataTableCategoryHandle& LootLevelData, int WorldLevel);
};

int PickLevel(UFortWorldItemDefinition* def, int PreferredLevel) // well min level and maxlevel is sometimes in ufortowrlditemdeifnit9 then on older versions ufortitemdefinitoj so idk wher tyo put this
{
    int PickedLevel = 0;

    if (PreferredLevel >= def->MinLevel)
        PickedLevel = PreferredLevel;

    if (def->MaxLevel >= 0)
    {
        if (PickedLevel <= def->MaxLevel)
            return PickedLevel;
        return def->MaxLevel;
    }

    return PickedLevel;
}
