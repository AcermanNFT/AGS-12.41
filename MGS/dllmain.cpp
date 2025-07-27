#include "minhook/MinHook.h"
#include "Utils.h"
#include "Hooking.h"
#include <thread>
#include "Gui.h"
#pragma comment(lib, "minhook/minhook.lib")

using namespace std;
using namespace SDK;

static void WaitForLogin() {
	Log("Waiting until we reach the login screen...");

	// Wait until we are at the login screen
	FName Frontend = UKismetStringLibrary::Conv_StringToName(L"Frontend");
	FName MatchState = UKismetStringLibrary::Conv_StringToName(L"InProgress");

	while (true) {
		UWorld* CurrentWorld = ((UWorld*)UWorld::GetWorld());
		if (CurrentWorld) {
			if (CurrentWorld->Name == Frontend) {
				auto GameMode = (AGameMode*)CurrentWorld->AuthorityGameMode;
				if (GameMode->GetMatchState() == MatchState) {
					break;
				}
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 1));
}

DWORD InitThread(LPVOID)
{
    AllocConsole();
    FILE* fptr;
    freopen_s(&fptr, "CONOUT$", "w+", stdout);

    MH_Initialize();
    Log("Initializing AGS");

	Log("AGS is starting now!");
	__int64 BaseAddr = __int64(GetModuleHandleW(0));
	std::cout << std::format("ModuleBase: 0x{:x}", BaseAddr);

	// Wait until the Engine is loaded!
	while (UEngine::GetEngine() == 0)
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	UFortEngine* Engine = Cast<UFortEngine>(UEngine::GetEngine());
	std::cout << std::format("FortEngine Object: 0x{:x}", __int64(UEngine::GetEngine()));


	WaitForLogin();
	Sleep(200);



    bool* GIsClient = (bool*)(__int64(GetModuleHandleA(0)) + 0x804B658);
    bool* GIsServer = (bool*)(__int64(GetModuleHandleA(0)) + 0x804B65A);

    *GIsClient = true;
    *GIsServer = true;

    CreateThread(0, 0, ImGuiThread, 0, 0, 0); // more proper

    Gamemode::InitHooks();
    Tick::InitHooks();
    PC::InitHooks();
    Misc::InitHooks();
    Abilities::InitHooks();
    Inventory::InitHooks();
    Building::InitHooks();
    ProcessEventHook2::InitHooks();
   VehicleSpanwerHook::InitHooks();
   LootingHook::InitHooks();
   TeamsFromHooking::IntHooksForTeams2();
    MH_EnableHook(MH_ALL_HOOKS);

    *(bool*)(__int64(GetModuleHandleW(0)) + 0x804B659) = false; //GIsClient
    *(bool*)(__int64(GetModuleHandleW(0)) + 0x804B660) = true; //GIsServer

	if (bLateGame) {
		log_info("Setting loadouts\n");
		UFortWeaponItemDefinition* ArDef = StaticLoadObject<UFortWeaponItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_Auto_Athena_R_Ore_T03.WID_Assault_Auto_Athena_R_Ore_T03");
		UFortWeaponItemDefinition* FlopperDef = StaticLoadObject<UFortWeaponItemDefinition>("/Game/Athena/Items/Consumables/Flopper/WID_Athena_Flopper.WID_Athena_Flopper");
		UFortWeaponItemDefinition* PumpDef = StaticLoadObject<UFortWeaponItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_SR_Ore_T03.WID_Shotgun_Standard_Athena_SR_Ore_T03");
		UFortWeaponItemDefinition* SmgDef = StaticLoadObject<UFortWeaponItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavyPDW_Athena_R_Ore_T03.WID_Pistol_AutoHeavyPDW_Athena_R_Ore_T03");
		UFortWeaponItemDefinition* DrumgunDef = StaticLoadObject<UFortWeaponItemDefinition>("/Game/Athena/Items/Weapons/Boss/WID_Boss_Midas.WID_Boss_Midas");
		UFortWeaponItemDefinition* SlurpFishDef = UObject::FindObject<UFortWeaponItemDefinition>("WID_Athena_Flopper_Effective.WID_Athena_Flopper_Effective");
		UFortWeaponItemDefinition* SmallShieldDef = UObject::FindObject<UFortWeaponItemDefinition>("Athena_ShieldSmall.Athena_ShieldSmall");

		Loadout Loadout1{};
		Loadout1.ItemDefinitions[PumpDef].push_back(1);
		Loadout1.ItemDefinitions[PumpDef].push_back(5);
		Loadout1.ItemDefinitions[PumpDef].push_back(15);
		Loadout1.ItemDefinitions[SmgDef].push_back(1);
		Loadout1.ItemDefinitions[SmgDef].push_back(30);
		Loadout1.ItemDefinitions[SmgDef].push_back(180);
		Loadout1.ItemDefinitions[ArDef].push_back(1);
		Loadout1.ItemDefinitions[ArDef].push_back(30);
		Loadout1.ItemDefinitions[ArDef].push_back(130);
		Loadout1.ItemDefinitions[SmallShieldDef].push_back(6);
		Loadout1.ItemDefinitions[SmallShieldDef].push_back(0);
		Loadout1.ItemDefinitions[SlurpFishDef].push_back(3);
		Loadout1.ItemDefinitions[SlurpFishDef].push_back(0);
		Loadout1.LoadoutType = ELoadoutType::LateGame;
		Loadouts.push_back(Loadout1);

		auto Loadout2 = Loadout{};
		Loadout2.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/Boss/WID_Boss_Adventure_AR.WID_Boss_Adventure_AR")].push_back(1);
		Loadout2.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/Boss/WID_Boss_Adventure_AR.WID_Boss_Adventure_AR")].push_back(30);
		Loadout2.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/Boss/WID_Boss_Adventure_AR.WID_Boss_Adventure_AR")].push_back(60);
		Loadout2.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_VR_Ore_T03.WID_Shotgun_Standard_Athena_VR_Ore_T03")].push_back(1);
		Loadout2.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_VR_Ore_T03.WID_Shotgun_Standard_Athena_VR_Ore_T03")].push_back(5);
		Loadout2.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_VR_Ore_T03.WID_Shotgun_Standard_Athena_VR_Ore_T03")].push_back(15);
		Loadout2.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavySuppressed_Athena_R_Ore_T03.WID_Pistol_AutoHeavySuppressed_Athena_R_Ore_T03")].push_back(1);
		Loadout2.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavySuppressed_Athena_R_Ore_T03.WID_Pistol_AutoHeavySuppressed_Athena_R_Ore_T03")].push_back(30);
		Loadout2.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavySuppressed_Athena_R_Ore_T03.WID_Pistol_AutoHeavySuppressed_Athena_R_Ore_T03")].push_back(120);
		Loadout2.ItemDefinitions[SlurpFishDef].push_back(3);
		Loadout2.ItemDefinitions[SlurpFishDef].push_back(0);
		Loadout2.ItemDefinitions[FlopperDef].push_back(3);
		Loadout2.ItemDefinitions[FlopperDef].push_back(0);
		Loadout2.LoadoutType = ELoadoutType::LateGame;
		Loadouts.push_back(Loadout2);

		auto Loadout3 = Loadout{};
		Loadout3.ItemDefinitions.clear();
		Loadout3.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_Auto_Athena_R_Ore_T03.WID_Assault_Auto_Athena_R_Ore_T03")].push_back(1);
		Loadout3.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_Auto_Athena_R_Ore_T03.WID_Assault_Auto_Athena_R_Ore_T03")].push_back(30);
		Loadout3.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_Auto_Athena_R_Ore_T03.WID_Assault_Auto_Athena_R_Ore_T03")].push_back(60);
		Loadout3.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_HighSemiAuto_Athena_VR_Ore_T03.WID_Shotgun_HighSemiAuto_Athena_VR_Ore_T03")].push_back(1);
		Loadout3.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_HighSemiAuto_Athena_VR_Ore_T03.WID_Shotgun_HighSemiAuto_Athena_VR_Ore_T03")].push_back(8);
		Loadout3.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_HighSemiAuto_Athena_VR_Ore_T03.WID_Shotgun_HighSemiAuto_Athena_VR_Ore_T03")].push_back(15);
		Loadout3.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavySuppressed_Athena_R_Ore_T03.WID_Pistol_AutoHeavySuppressed_Athena_R_Ore_T03")].push_back(1);
		Loadout3.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavySuppressed_Athena_R_Ore_T03.WID_Pistol_AutoHeavySuppressed_Athena_R_Ore_T03")].push_back(30);
		Loadout3.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavySuppressed_Athena_R_Ore_T03.WID_Pistol_AutoHeavySuppressed_Athena_R_Ore_T03")].push_back(120);
		Loadout3.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/Boss/WID_Boss_Adventure_GH.WID_Boss_Adventure_GH")].push_back(1);
		Loadout3.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/Boss/WID_Boss_Adventure_GH.WID_Boss_Adventure_GH")].push_back(999999);
		Loadout3.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/Boss/WID_Boss_Adventure_GH.WID_Boss_Adventure_GH")].push_back(0);
		Loadout3.ItemDefinitions[SlurpFishDef].push_back(3);
		Loadout3.ItemDefinitions[SlurpFishDef].push_back(0);
		Loadout3.LoadoutType = ELoadoutType::LateGame;
		Loadouts.push_back(Loadout3);

		auto Loadout4 = Loadout{};
		Loadout4.ItemDefinitions.clear();
		Loadout4.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/Boss/WID_Boss_Midas.WID_Boss_Midas")].push_back(1);
		Loadout4.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/Boss/WID_Boss_Midas.WID_Boss_Midas")].push_back(40);
		Loadout4.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/Boss/WID_Boss_Midas.WID_Boss_Midas")].push_back(120);
		Loadout4.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_SemiAuto_Athena_VR_Ore_T03.WID_Shotgun_SemiAuto_Athena_VR_Ore_T03")].push_back(1);
		Loadout4.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_SemiAuto_Athena_VR_Ore_T03.WID_Shotgun_SemiAuto_Athena_VR_Ore_T03")].push_back(8);
		Loadout4.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_SemiAuto_Athena_VR_Ore_T03.WID_Shotgun_SemiAuto_Athena_VR_Ore_T03")].push_back(15);
		Loadout4.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Sniper_Heavy_Athena_VR_Ore_T03.WID_Sniper_Heavy_Athena_VR_Ore_T03")].push_back(1);
		Loadout4.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Sniper_Heavy_Athena_VR_Ore_T03.WID_Sniper_Heavy_Athena_VR_Ore_T03")].push_back(1);
		Loadout4.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Sniper_Heavy_Athena_VR_Ore_T03.WID_Sniper_Heavy_Athena_VR_Ore_T03")].push_back(9);
		Loadout4.ItemDefinitions[SmallShieldDef].push_back(6);
		Loadout4.ItemDefinitions[SmallShieldDef].push_back(0);
		Loadout4.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Consumables/Shields/Athena_Shields.Athena_Shields")].push_back(3);
		Loadout4.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Consumables/Shields/Athena_Shields.Athena_Shields")].push_back(0);
		Loadout4.LoadoutType = ELoadoutType::LateGame;
		Loadouts.push_back(Loadout4);

		auto Loadout5 = Loadout{};
		Loadout5.ItemDefinitions.clear();
		Loadout5.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_AutoHigh_Athena_VR_Ore_T03.WID_Assault_AutoHigh_Athena_VR_Ore_T03")].push_back(1);
		Loadout5.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_AutoHigh_Athena_VR_Ore_T03.WID_Assault_AutoHigh_Athena_VR_Ore_T03")].push_back(30);
		Loadout5.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_AutoHigh_Athena_VR_Ore_T03.WID_Assault_AutoHigh_Athena_VR_Ore_T03")].push_back(60);
		Loadout5.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_UC_Ore_T03.WID_Shotgun_Standard_Athena_UC_Ore_T03")].push_back(1);
		Loadout5.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_UC_Ore_T03.WID_Shotgun_Standard_Athena_UC_Ore_T03")].push_back(5);
		Loadout5.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_UC_Ore_T03.WID_Shotgun_Standard_Athena_UC_Ore_T03")].push_back(15);   Loadout5.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavySuppressed_Athena_R_Ore_T03.WID_Pistol_AutoHeavySuppressed_Athena_R_Ore_T03")].push_back(1);
		Loadout5.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavySuppressed_Athena_R_Ore_T03.WID_Pistol_AutoHeavySuppressed_Athena_R_Ore_T03")].push_back(30);
		Loadout5.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavySuppressed_Athena_R_Ore_T03.WID_Pistol_AutoHeavySuppressed_Athena_R_Ore_T03")].push_back(120);
		Loadout5.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/Boss/WID_Boss_Adventure_GH.WID_Boss_Adventure_GH")].push_back(1);
		Loadout5.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/Boss/WID_Boss_Adventure_GH.WID_Boss_Adventure_GH")].push_back(999999);
		Loadout5.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/Boss/WID_Boss_Adventure_GH.WID_Boss_Adventure_GH")].push_back(0);
		Loadout5.ItemDefinitions[SlurpFishDef].push_back(3);
		Loadout5.ItemDefinitions[SlurpFishDef].push_back(0);
		Loadout5.LoadoutType = ELoadoutType::LateGame;
		Loadouts.push_back(Loadout5);

		Loadout5 = {};
		Loadout5.ItemDefinitions.clear();
		Loadout5.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/Boss/WID_Boss_Meow_HA.WID_Boss_Meow_HA")].push_back(1);
		Loadout5.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/Boss/WID_Boss_Meow_HA.WID_Boss_Meow_HA")].push_back(25);
		Loadout5.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/Boss/WID_Boss_Meow_HA.WID_Boss_Meow_HA")].push_back(75);
		Loadout5.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_SR_Ore_T03.WID_Shotgun_Standard_Athena_SR_Ore_T03")].push_back(1);
		Loadout5.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_SR_Ore_T03.WID_Shotgun_Standard_Athena_SR_Ore_T03")].push_back(5);
		Loadout5.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_SR_Ore_T03.WID_Shotgun_Standard_Athena_SR_Ore_T03")].push_back(15);   Loadout5.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavySuppressed_Athena_R_Ore_T03.WID_Pistol_AutoHeavySuppressed_Athena_R_Ore_T03")].push_back(1);
		Loadout5.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavySuppressed_Athena_R_Ore_T03.WID_Pistol_AutoHeavySuppressed_Athena_R_Ore_T03")].push_back(30);
		Loadout5.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavySuppressed_Athena_R_Ore_T03.WID_Pistol_AutoHeavySuppressed_Athena_R_Ore_T03")].push_back(120);
		Loadout5.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Consumables/Shields/Athena_Shields.Athena_Shields")].push_back(3);
		Loadout5.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Consumables/Shields/Athena_Shields.Athena_Shields")].push_back(0);
		Loadout5.ItemDefinitions[SlurpFishDef].push_back(3);
		Loadout5.ItemDefinitions[SlurpFishDef].push_back(0);
		Loadout5.LoadoutType = ELoadoutType::LateGame;
		Loadouts.push_back(Loadout5);

		auto Loadout6 = Loadout{};
		Loadout6.ItemDefinitions.clear();
		Loadout6.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_AutoHigh_Athena_SR_Ore_T03.WID_Assault_AutoHigh_Athena_SR_Ore_T03")].push_back(1);
		Loadout6.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_AutoHigh_Athena_SR_Ore_T03.WID_Assault_AutoHigh_Athena_SR_Ore_T03")].push_back(30);
		Loadout6.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Assault_AutoHigh_Athena_SR_Ore_T03.WID_Assault_AutoHigh_Athena_SR_Ore_T03")].push_back(60);  Loadout6.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_SemiAuto_Athena_VR_Ore_T03.WID_Shotgun_SemiAuto_Athena_VR_Ore_T03")].push_back(1);
		Loadout6.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_SemiAuto_Athena_VR_Ore_T03.WID_Shotgun_SemiAuto_Athena_VR_Ore_T03")].push_back(8);
		Loadout6.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_SemiAuto_Athena_VR_Ore_T03.WID_Shotgun_SemiAuto_Athena_VR_Ore_T03")].push_back(15);
		Loadout6.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/Boss/WID_Boss_Hos_MG.WID_Boss_Hos_MG")].push_back(1);
		Loadout6.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/Boss/WID_Boss_Hos_MG.WID_Boss_Hos_MG")].push_back(0);
		Loadout6.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/Boss/WID_Boss_Hos_MG.WID_Boss_Hos_MG")].push_back(120);
		Loadout6.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Consumables/Shields/Athena_Shields.Athena_Shields")].push_back(3);
		Loadout6.ItemDefinitions[StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Consumables/Shields/Athena_Shields.Athena_Shields")].push_back(0);
		Loadout6.ItemDefinitions[FlopperDef].push_back(3);
		Loadout6.ItemDefinitions[FlopperDef].push_back(0);
		Loadout6.LoadoutType = ELoadoutType::LateGame;
		Loadouts.push_back(Loadout6);
	}


    /**(bool*)(InSDKUtils::GetImageBase() + 0x804B659) = false;
    *(bool*)(InSDKUtils::GetImageBase() + 0x804B660) = true;*/


    UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"open Apollo_Terrain", nullptr);
    UWorld::GetWorld()->OwningGameInstance->LocalPlayers.Remove(0); 
   
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD Reason, LPVOID lpReserved)
{
    switch (Reason)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, InitThread, 0, 0, 0);
        break;
    default:
        break;
    }
    return TRUE;
}