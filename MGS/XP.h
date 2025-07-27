#pragma once
#include "Utils.h"
#include "CustomLogs.h"
#include "API.h"
// it fucking works somehow!

inline void GiveAccolade(AFortPlayerControllerAthena* PC, UFortAccoladeItemDefinition* Def)
{
    if (!Def)
        return;
    //for (size_t i = 0; i < PC->XPComponent->PlayerAccolades.Num(); i++)
    //{
    //    if (PC->XPComponent->PlayerAccolades[i].AccoladeDef == Def)
    //        return;
    //}
    FAthenaAccolades Accolade{};
    Accolade.AccoladeDef = Def;
    Accolade.Count = 1;
    string DefName = Def->GetName();
    Accolade.TemplateId = wstring(DefName.begin(), DefName.end()).c_str();

    auto ID = UKismetSystemLibrary::GetDefaultObj()->GetPrimaryAssetIdFromObject(Def);

    FXPEventInfo EventInfo{};
    EventInfo.Accolade = ID;
    EventInfo.EventName = Def->Name;
    EventInfo.EventXpValue = Def->GetAccoladeXpValue();
    EventInfo.Priority = Def->Priority;
    EventInfo.SimulatedText = Def->GetShortDescription();
    EventInfo.RestedValuePortion = EventInfo.EventXpValue;
    EventInfo.RestedXPRemaining = EventInfo.EventXpValue;
    EventInfo.SeasonBoostValuePortion = 20;
    EventInfo.TotalXpEarnedInMatch = EventInfo.EventXpValue + PC->XPComponent->TotalXpEarned;

    PC->XPComponent->MedalBonusXP += 12500;
    PC->XPComponent->MatchXp += EventInfo.EventXpValue;
    PC->XPComponent->TotalXpEarned += EventInfo.EventXpValue + 12500;

    PC->XPComponent->PlayerAccolades.Add(Accolade);
    PC->XPComponent->MedalsEarned.Add(Def);

    PC->XPComponent->ClientMedalsRecived(PC->XPComponent->PlayerAccolades);
    PC->XPComponent->OnXPEvent(EventInfo);
}

enum class EAccoladeEvent : uint8
{
	Kill,
	Search,
	MAX
};

inline UFortAccoladeItemDefinition* GetDefFromEvent(EAccoladeEvent Event, int Count, UObject* Object = nullptr)
{
	UFortAccoladeItemDefinition* Def = nullptr;

	switch (Event)
	{
	case EAccoladeEvent::Kill:
		if (Count == 1)
		{
			Def = StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_014_Elimination_Bronze.AccoladeId_014_Elimination_Bronze");
		}
		else if (Count == 4)
		{
			Def = StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_015_Elimination_Silver.AccoladeId_015_Elimination_Silver");
		}
		else if (Count == 8)
		{
			Def = StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_016_Elimination_Gold.AccoladeId_016_Elimination_Gold");
		}
		else
		{
			Def = StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_012_Elimination.AccoladeId_012_Elimination");
		}
		break;
	case EAccoladeEvent::Search:
		if (!Object || !Object->Class->GetName().contains("Ammo"))
		{
			if (Count == 3)
			{
				Def = StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_008_SearchChests_Bronze.AccoladeId_008_SearchChests_Bronze");
			}
			else if (Count == 7)
			{
				Def = StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_009_SearchChests_Silver.AccoladeId_009_SearchChests_Silver");
			}
			else if (Count == 12)
			{
				Def = StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_010_SearchChests_Gold.AccoladeId_010_SearchChests_Gold");
			}
			else
			{
				Def = StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_007_SearchChests.AccoladeId_007_SearchChests");
			}
		}
		else
		{
			Def = StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_011_SearchAmmoBox.AccoladeId_011_SearchAmmoBox");
		}
		break;
	case EAccoladeEvent::MAX:
		break;
	default:
		break;
	}

	return Def;
}


void ProgressQuest(AFortPlayerControllerAthena* PC, UFortQuestItemDefinition* QuestDef, FName BackendName)
{
	PC->GetQuestManager(ESubGame::Athena)->SelfCompletedUpdatedQuest(PC, QuestDef, BackendName, 1, 1, nullptr, true, false);
	AFortPlayerStateAthena* PlayerState = (AFortPlayerStateAthena*)PC->PlayerState;
	for (size_t i = 0; i < PlayerState->PlayerTeam->TeamMembers.Num(); i++)
	{
		auto pc = (AFortPlayerControllerAthena*)PlayerState->PlayerTeam->TeamMembers[i];
		if (pc && pc != PC)
		{
			pc->GetQuestManager(ESubGame::Athena)->SelfCompletedUpdatedQuest(PC, QuestDef, BackendName, 1, 1, PlayerState, true, false);
		}
	}
	auto QuestItem = PC->GetQuestManager(ESubGame::Athena)->GetQuestWithDefinition(QuestDef);

	FXPEventEntry test{};
	test.EventXpValue = 15000;//still skunked i will make it so it gets real xp value from datatable
	test.QuestDef = QuestDef;
	test.Time = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());
	PC->XPComponent->ChallengeXp += test.EventXpValue;
	PC->XPComponent->TotalXpEarned += test.EventXpValue;
	test.TotalXpEarnedInMatch = PC->XPComponent->TotalXpEarned;
	test.SimulatedXpEvent = QuestDef->GetSingleLineDescription();
	PC->XPComponent->RestXP += test.EventXpValue;
	PC->XPComponent->InMatchProfileVer++;
	PC->XPComponent->OnInMatchProfileUpdate(PC->XPComponent->InMatchProfileVer);
	PC->XPComponent->OnProfileUpdated();
	PC->XPComponent->OnXpUpdated(PC->XPComponent->CombatXp, PC->XPComponent->SurvivalXp, PC->XPComponent->MedalBonusXP, PC->XPComponent->ChallengeXp, PC->XPComponent->MatchXp, PC->XPComponent->TotalXpEarned);
	PC->XPComponent->WaitingQuestXp.Add(test);

	cout << PC->XPComponent->WaitingQuestXp.Num() << endl;
	PC->XPComponent->HighPrioXPEvent(test);

	auto bruh = QuestDef->GetRewardDescription();
	auto bruh2 = QuestDef->GetDescription();
	auto bruh3 = QuestDef->GetSingleLineDescription();
	auto bruh4 = QuestDef->GetShortDescription();
	Log(UKismetTextLibrary::GetDefaultObj()->Conv_TextToString(bruh).ToString());
	for (size_t i = 0; i < bruh2.Num(); i++)
	{
		Log(UKismetTextLibrary::GetDefaultObj()->Conv_TextToString(bruh2[i]).ToString());
	}
	Log(UKismetTextLibrary::GetDefaultObj()->Conv_TextToString(bruh3).ToString());
	Log(UKismetTextLibrary::GetDefaultObj()->Conv_TextToString(bruh4).ToString());
}
