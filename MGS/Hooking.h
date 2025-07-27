#pragma once
#include "Utils.h"
#include "Gamemode.h"
#include "PC.h"
#include "Tick.h"
#include "minhook/MinHook.h"
#include "Abilities.h"
#include "building.h"
#include "ProcessEvent.h"
#include "Vehicle.h"
#include "Looting.h"
#include "WeaponFixes.h"
#include "PlayerState.h"
#include "Teams.h"
#include "Bosses.h"
#pragma comment(lib, "minhook/minhook.lib")

static auto ImageBase = InSDKUtils::GetImageBase();

int True() {
	return 1;
}

int False() {
	return 0;
}


namespace Gamemode {

	inline void InitHooks()
	{
		Log("Hooking Gamemode");

		MH_CreateHook((LPVOID)(ImageBase + 0x4640A30), ReadyToStartMatch, (LPVOID*)&ReadyToStartMatchOG);
		MH_CreateHook((LPVOID)(ImageBase + 0x18F6250), SpawnDefaultPawnFor, nullptr);
		MH_CreateHook(LPVOID(ImageBase + 0x18E6B20), PickTeam, nullptr);
		MH_CreateHook((LPVOID)(ImageBase + 0x29B5C80), ClientOnPawnDied, (LPVOID*)&ClientOnPawnDiedOG);

		MH_CreateHook((LPVOID)(ImageBase + 0x22A08C0), OnCapsuleBeginOverlap, (LPVOID*)&OnCapsuleBeginOverlapOG);

		MH_CreateHook((LPVOID)(ImageBase + 0x18E07D0), OnAircraftExitedDropZone, (LPVOID*)&OnAircraftExitedDropZoneOG);

		MH_CreateHook((LPVOID)(ImageBase + 0x18FD350), StormXpPro, (LPVOID*)&StormXpProOG);//SetZoneToIndex

		MH_CreateHook((LPVOID)(InSDKUtils::GetImageBase() + 0x1583360), ApplyCost, (void**)&ApplyCostOG); // fixed crash pads!


		HookVTable(AFortPlayerStateAthena::GetDefaultObj(), 0xFF, ServerSetInAircraft, (void**)&ServerSetInAircraftOG);

		HookVTable(AFortPlayerPawnAthena::GetDefaultObj(), 0x119, NetMulticast_Athena_BatchedDamageCues, (LPVOID*)&NetMulticast_Athena_BatchedDamageCuesOG);
		HookVTable(AAthena_PlayerController_C::GetDefaultObj(), 0x21D, ServerAttemptInventoryDrop, nullptr);
		HookVTable(AAthena_PlayerController_C::GetDefaultObj(), 0x265, ServerReturnToMainMenu, nullptr);
		HookVTable(AAthena_PlayerController_C::GetDefaultObj(), 0x109, ServerChangeName, nullptr);
		HookVTable(AAthena_PlayerController_C::GetDefaultObj(), 0x22C, ServerRepairBuildingActor, nullptr);

		HookVTable(AAthena_PlayerController_C::GetDefaultObj(), 0x4F0, ServerClientIsReadyToRespawn, (void**)&ServerClientIsReadyToRespawnOG);

		MH_CreateHook((LPVOID)(ImageBase + 0x45C9D90), WorldNetMode, nullptr);

		MH_CreateHook((LPVOID)(ImageBase + 0x3EB6780), ActorNetMode, nullptr);

		MH_CreateHook((LPVOID)(ImageBase + 0x22E0D50), GetPlayerViewPoint, (void**)&GetPlayerViewPointOG);


		HookVTable(AAthena_PlayerController_C::GetDefaultObj(), 0x40b, ServerRequestSeatChange, (void**)&ServerRequestSeatChangeOG);
		MH_CreateHook((LPVOID)(ImageBase + 0x18F9BB0), StartAircraftPhaseHook, (void**)&StartAircraftPhase);

	}
}

namespace LootingHook {

	inline void InitHooks()
	{
		Log("Hooking Gamemode");

		MH_CreateHook((LPVOID)(ImageBase + 0x1B46D00), SpawnLoot, nullptr);

		HookVTable(APlayerPawn_Athena_C::GetDefaultObj(), 0x1EA, ServerHandlePickup, (void**)&ServerHandlePickupOG);

		HookVTable(APlayerPawn_Athena_C::GetDefaultObj(), 0x1E6, ServerHandlePickupWithSwap, nullptr);
		HookVTable(APlayerPawn_Athena_C::GetDefaultObj(), 0x1E8, ServerHandlePickupWithSwap, nullptr);

		HookVTable(APlayerPawn_Athena_C::GetDefaultObj(), 0x1F5, ServerSendZiplineState, nullptr);

		HookVTable(APlayerPawn_Athena_C::GetDefaultObj(), 0x1D2, ServerReviveFromDBNO, nullptr);

		//HookVTable(AFortPlayerControllerAthena::GetDefaultObj(), 0x1EA, ServerHandlePickup, nullptr);

	}
}

namespace Misc {

	void InitHooks()
	{
		Log("Hooking Misc");
		MH_CreateHook((LPVOID)(ImageBase + 0x2D95E00), True, nullptr); // collect garbage
		MH_CreateHook((LPVOID)(ImageBase + 0x4155600), False, nullptr);// validation / kick
		MH_CreateHook((LPVOID)(ImageBase + 0x1E23840), False, nullptr);// change gamesession id
		MH_CreateHook((LPVOID)(ImageBase + 0x108D740), DispatchRequest, (LPVOID*)&DispatchRequestOG);

		MH_CreateHook((LPVOID)(ImageBase + 0x108D740), MCP_DispatchRequestHook, (LPVOID*)&MCP_DispatchRequest);

		MH_CreateHook((LPVOID)(InSDKUtils::GetImageBase() + 0x6853B0), MovingEmoteStopped, (LPVOID*)&MovingEmoteStoppedOG);
		HookVTable(AFortPlayerControllerAthena::GetDefaultObj(), 0x1C7, ServerPlayEmoteItem, nullptr);
	//	MH_CreateHook((LPVOID)(InSDKUtils::GetImageBase() + 0x22E0D50), GetPlayerViewPoint, nullptr);
		HookVTable(UFortControllerComponent_Interaction::GetDefaultObj(), 0x8B, ServerAttemptInteract, (PVOID*)&ServerAttemptInteractOG);
		MH_CreateHook((LPVOID)(ImageBase + 0x260C490), OnReload, (LPVOID*)&OnReloadOG);

		for (size_t i = 0; i < UObject::GObjects->Num(); i++)
		{
			UObject* Obj = UObject::GObjects->GetByIndex(i);
			if (Obj && Obj->IsA(AB_Prj_Athena_Consumable_Thrown_C::StaticClass()))
			{
				HookVTable(Obj->Class->DefaultObject, 0x44, OnExploded, nullptr);
			}
		}

	}
}

namespace VehicleSpanwerHook
{
	void InitHooks()
	{
		HookVTable(AFortPlayerControllerAthena::GetDefaultObj(), 0x41E, ServerAttemptExitVehicle, (PVOID*)&ServerAttemptExitVehicleOG);

		for (size_t i = 0; i < UObject::GObjects->Num(); i++) {
			auto Object = UObject::GObjects->GetByIndex(i);
			if (!Object)
				continue;

			if (Object->IsA(AFortPhysicsPawn::StaticClass())) {
				HookVTable(Object->Class->DefaultObject, 0xEE, ServerMove, (LPVOID*)&ServerMoveOG);
			}
		}
	}
}

namespace Building {

	void InitHooks()
	{
		Log("Hooking Building");
		MH_CreateHook((LPVOID)(InSDKUtils::GetImageBase() + 0x2683F80), OnDamageServer, (LPVOID*)&OnDamageServerOG);
		HookVTable(AFortPlayerControllerAthena::GetDefaultObj(), 0x1C5, ServerCheat, nullptr);
		HookVTable(AFortPlayerControllerAthena::GetDefaultObj(), 0x230, ServerCreateBuildingActor, nullptr);
		HookVTable(AFortPlayerControllerAthena::GetDefaultObj(), 0x237, ServerBeginEditingBuildingActor, nullptr);
		HookVTable(AFortPlayerControllerAthena::GetDefaultObj(), 0x232, ServerEditBuildingActor, nullptr);
		HookVTable(AFortPlayerControllerAthena::GetDefaultObj(), 0x235, ServerEndEditingBuildingActor, nullptr);
	}
}


namespace ProcessEventHook2{

	void InitHooks()
	{
		Log("Hooking ProcessEvent");
		MH_CreateHook((LPVOID)(InSDKUtils::GetImageBase() + 0x2B98BE0), sub_2B98BE0, nullptr);
		MH_CreateHook((LPVOID)(InSDKUtils::GetImageBase() + Offsets::ProcessEvent), ProcessEvent, (PVOID*)&ProcessEventOG);

		MH_CreateHook((LPVOID)(InSDKUtils::GetImageBase() + 0x1637F50), test3, (LPVOID*)&test3OG);
	}
}

namespace Abilities {

	void InitHooks()
	{
		Log("Hooking Abilities");
		HookVTable(UAbilitySystemComponent::GetDefaultObj(), 0xFA, InternalServerTryActivateAbility, nullptr);
		HookVTable(UFortAbilitySystemComponent::GetDefaultObj(), 0xFA, InternalServerTryActivateAbility, nullptr);
		HookVTable(UFortAbilitySystemComponentAthena::GetDefaultObj(), 0xFA, InternalServerTryActivateAbility, nullptr);
	}
}

namespace PC {

	void InitHooks()
	{
		Log("Hooking PC");

		HookVTable(AFortPlayerControllerAthena::GetDefaultObj(), 0x26B, ServerLoadingScreenDropped, (LPVOID*)&ServerLoadingScreenDroppedOG);

		HookVTable(UFortControllerComponent_Aircraft::GetDefaultObj(), 0x89, ServerAttemptAircraftJump, nullptr);

		HookVTable(AFortPlayerControllerAthena::GetDefaultObj(), 0x10D, ServerAcknowledgePossession, (LPVOID*)&ServerAcknowledgePossessionOG);
	}
}

namespace Inventory {

	void InitHooks()
	{
		Log("Hooking Inventory");
		HookVTable(AFortPlayerControllerAthena::GetDefaultObj(), 0x20D, ServerExecuteInventoryItem, nullptr);

	}
}

namespace Tick {

	void InitHooks()
	{
		Log("Hooking Tick");

		MH_CreateHook((LPVOID)(ImageBase + 0x42C3ED0), TickFlushHook, (LPVOID*)&TickFlushOG);

		MH_CreateHook((LPVOID)(ImageBase + 0x4576310), GetMaxTickRate, nullptr);
	}
}
