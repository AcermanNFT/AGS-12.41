#pragma once // this is basicly my framework file 
#include <string>
#include <iostream>
#include "SDK/SDK.hpp"
#include <map>
using namespace std;
using namespace SDK;
inline bool bLateGame = false;
inline AActor* BattleBus = nullptr;
inline static UNetDriver* (*CreateNetDriver)(void*, void*, FName) = decltype(CreateNetDriver)(InSDKUtils::GetImageBase() + 0x4573990);
inline static bool (*InitListen)(void*, void*, FURL&, bool, FString&) = decltype(InitListen)(InSDKUtils::GetImageBase() + 0xd44c40);
inline static void (*SetWorld)(void*, void*) = decltype(SetWorld)(InSDKUtils::GetImageBase() + 0x42c2b20);
inline static __int64 (*GetAccID1)(__int64, __int64) = decltype(GetAccID1)(__int64(GetModuleHandleA(0)) + 0x22DC390);
inline static __int64 (*GetAccID2)(__int64, __int64) = decltype(GetAccID2)(__int64(GetModuleHandleA(0)) + 0xAE5CB0);
inline static void(*OnRep_ZiplineState)(AFortPlayerPawn*) = decltype(OnRep_ZiplineState)(__int64(GetModuleHandleA(0)) + 0x2298F10);
inline static void (*XPCompOnProfileReady)(void*, void*) = decltype(XPCompOnProfileReady)(__int64(GetModuleHandleA(0)) + 0x19A9980);
inline static void (*SetMatchPlacement)(AFortPlayerStateAthena* PlayerState, int PlacementARg) = nullptr;
inline static void (*BotManagerSetupStuffIdk)(__int64 BotManaager, __int64 Pawn, __int64 BehaviorTree, __int64 a4, DWORD* SkillLevel, __int64 idk, __int64 StartupInventory, __int64 BotNameSettings, __int64 idk_1, BYTE* CanRespawnOnDeath, unsigned __int8 BitFieldDataThing, BYTE* CustomSquadId, FFortAthenaAIBotRunTimeCustomizationData InRuntimeBotData) = decltype(BotManagerSetupStuffIdk)(__int64(GetModuleHandleW(0)) + 0x19D93F0);
inline static ABuildingSMActor* (*ReplaceBuildingActor)(ABuildingSMActor* BuildingSMActor, unsigned int a2, UObject* a3, unsigned int a4, int a5, bool bMirrored, AFortPlayerControllerAthena* PC) = decltype(ReplaceBuildingActor)(__int64(GetModuleHandleW(0)) + 0x1B951B0);
inline float XpMultiplierTime = 10.f;
typedef __int64 _QWORD;
FVector BusLocation;
inline static float Duration = 120.f;
inline bool B0ssesEnabled = true;
inline bool bllamas = false;
inline bool bStormKing = false;
inline bool bArsenal = false;
inline bool bEnableSiphon = true;
inline bool bEnableLooting = true;
inline bool bEnableScoringSystem = false;
inline bool BotsEnabled = true;
inline bool bEnableLlamas = false;
inline bool bBattleLab = false;
inline bool bCreative = false;
inline bool IsLoaded = false;
inline bool GotLoc = false;
//inline ali::saved::mirror::vector BannedPlayers = ali::saved::mirror::vector("BannedPlayers");
inline TArray<AActor*> SpawnLocs;
inline static bool bSpreadTeams = false;
inline bool bEnableTeams = true;
inline static char PlaylistBuffer[1024] = "Playlist_DefaultSolo";//Playlist_DefaultSolo
inline std::vector<UAthenaCharacterItemDefinition*> Skin;
inline std::vector<UAthenaBackpackItemDefinition*> BackBling;
inline std::vector<UAthenaDanceItemDefinition*> Emotes;
inline std::vector<UAthenaGliderItemDefinition*> Gliders;
inline std::vector<UAthenaPickaxeItemDefinition*> Pickaxes;
extern inline AFortAthenaMutator_Bots* BotMutator = nullptr;
extern inline UObject* JerkyLoader = nullptr;
extern inline UCompositeDataTable* LootTierData = nullptr;
extern inline UCompositeDataTable* LootPackages = nullptr;
extern inline UCurveTable* GameData = nullptr;
inline bool bAllVehicles = true;

// Boss Spawn Location
inline FVector MidasLoc = FVector{ 7856.0, 7384.2, -1296.0 };
inline FVector BrutusLoc = FVector{ -15497, 109581, -3155 };
inline FVector TnTinaLoc = FVector{ -75742.914062, -84465.250000, -3142.823242 };
inline FVector SkyeLoc = FVector{ 107005.640625, -82960.484375, -3530.085449 };
inline FVector DeadPoolLoc = FVector{ 116316.351562, 113970.992188, -2377.677002 };

inline UFortItemDefinition* WoodDefBot;
inline UFortItemDefinition* StoneDefBot;
inline UFortItemDefinition* MetalDefBot;

// BOT NEEDS THIS
struct FLocationDistance
{
	FVector Location;
	float Distance;
};

enum EBotState : uint8
{
	Warmup,
	InBus,
	SkydivingFromBus,
	GlidingFromBus,
	Landed,
	Looting,
	MovingToZone,
	LookingForPlayers,
	Stuck,
	MAX
};

enum EBotStuckState : uint8
{
	NONE,
	Actor,
	FullStuck
};

enum class ELoadoutType : uint8
{
	LateGame = 0,
	CustomLTM = 1,
	MAX = 2
};

struct Loadout
{
	map<UFortItemDefinition*, vector<int>> ItemDefinitions{};
	ELoadoutType LoadoutType;
};

inline vector<Loadout> Loadouts{};
inline static __int64 (*StartAircraftPhase)(AFortGameModeAthena* GameMode, char a2) = nullptr;
inline void Log(string msg)
{
	cout << "[AGS GS]: " << msg << endl;
}

inline void HookVTable(void* Base, int Idx, void* Detour, void** OG)
{
	DWORD oldProtection;

	void** VTable = *(void***)Base;

	if (OG)
	{
		*OG = VTable[Idx];
	}

	VirtualProtect(&VTable[Idx], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtection);

	VTable[Idx] = Detour;

	VirtualProtect(&VTable[Idx], sizeof(void*), oldProtection, NULL);
}

inline FQuat RotatorToQuat(FRotator Rotation) 
{ 
	FQuat Quat;
	const float DEG_TO_RAD = 3.14159f / 180.0f;
	const float DIVIDE_BY_2 = DEG_TO_RAD / 2.0f;

	float SP = sin(Rotation.Pitch * DIVIDE_BY_2);
	float CP = cos(Rotation.Pitch * DIVIDE_BY_2);
	float SY = sin(Rotation.Yaw * DIVIDE_BY_2);
	float CY = cos(Rotation.Yaw * DIVIDE_BY_2);
	float SR = sin(Rotation.Roll * DIVIDE_BY_2);
	float CR = cos(Rotation.Roll * DIVIDE_BY_2);

	Quat.X = CR * SP * SY - SR * CP * CY;
	Quat.Y = -CR * SP * CY - SR * CP * SY;
	Quat.Z = CR * CP * SY - SR * SP * CY;
	Quat.W = CR * CP * CY + SR * SP * SY;

	return Quat;
}

template<typename T>
static inline T* SpawnActor(FVector Loc = { 0,0,0 }, FRotator Rot = { 0,0,0 }, AActor* Owner = nullptr)
{
	FTransform Transform{};
	Transform.Scale3D = { 1,1,1 };
	Transform.Translation = Loc;
	Transform.Rotation = RotatorToQuat(Rot);

	AActor* NewActor = UGameplayStatics::BeginDeferredActorSpawnFromClass(UWorld::GetWorld(), T::StaticClass(), Transform, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn, Owner);
	return (T*)UGameplayStatics::FinishSpawningActor(NewActor, Transform);
}

template <class T>
T* BuildingSpawnActor(FVector Location, FRotator Rotation = FRotator{ 0, 0, 0 }, UClass* Class = T::StaticClass(), FVector Scale3D = { 1,1,1 })
{
	FTransform Transform{};
	Transform.Rotation = UKismetMathLibrary::Conv_RotatorToTransform(Rotation).Rotation;
	Transform.Scale3D = Scale3D;
	Transform.Translation = Location;

	auto Actor = UGameplayStatics::GetDefaultObj()->BeginSpawningActorFromClass(UWorld::GetWorld(), Class, Transform, false, nullptr);
	if (Actor)
		UGameplayStatics::GetDefaultObj()->FinishSpawningActor(Actor, Transform);
	return (T*)Actor;
}


template <typename T>
inline T* Cast(UObject* Object) {
	if (Object && Object->IsA(T::StaticClass()))
	{
		return (T*)Object;
	}

	return nullptr;
}

inline AFortGameModeAthena* GetGameMode() {
	return Cast<AFortGameModeAthena>(UWorld::GetWorld()->AuthorityGameMode);
}

inline SDK::UObject* (*StaticLoadObject_)(UClass* Class, SDK::UObject* InOuter, const TCHAR* Name, const TCHAR* Filename, uint32_t LoadFlags, SDK::UObject* Sandbox, bool bAllowObjectReconciliation, void*) = decltype(StaticLoadObject_)(uintptr_t(GetModuleHandleW(0)) + 0x2E1D7A0);

template<typename T = SDK::UObject>
inline T* StaticLoadObject(const std::string& ObjectPath, UClass* Class = T::StaticClass()) {
	std::wstring WideObjectPath = std::wstring(ObjectPath.begin(), ObjectPath.end());
	const TCHAR* TCHARObjectPath = WideObjectPath.c_str();
	return (T*)StaticLoadObject_(Class, nullptr, TCHARObjectPath, nullptr, 0, nullptr, false, nullptr);
}

inline UFortPlaylistAthena* GetPlaylist() {
	auto Playlist = Cast<AFortGameStateAthena>(UWorld::GetWorld()->GameState)->CurrentPlaylistInfo.BasePlaylist;

	if (Playlist)
		return Playlist;

	return nullptr;
}

static UObject* (*StaticFindObjectOG)(UClass*, UObject* Package, const wchar_t* OrigInName, bool ExactClass) = decltype(StaticFindObjectOG)(__int64(GetModuleHandleW(0)) + 0x2E1C4B0);
template <typename T>
static T* StaticFindObject(string ObjectName)
{
	auto NameWStr = wstring(ObjectName.begin(), ObjectName.end()).c_str();

	return (T*)StaticFindObjectOG(T::StaticClass(), nullptr, NameWStr, false);
}

template <class T>
inline TArray<T*> GetAllActorsOfClass() {
	TArray<T*> ResultActors;

	if (UWorld* World = UWorld::GetWorld()) {
		TArray<AActor*> OutActors;
		UGameplayStatics::GetAllActorsOfClass(World, T::StaticClass(), &OutActors);

		for (AActor* Actor : OutActors) {
			if (T* CastedActor = Cast<T>(Actor)) {
				ResultActors.Add(CastedActor);
			}
		}
	}
	return ResultActors;
}

inline void ShowFoundation(ABuildingFoundation* BuildingFoundation) {
	if (!BuildingFoundation)
		return;

	BuildingFoundation->bServerStreamedInLevel = true;
	BuildingFoundation->DynamicFoundationType = EDynamicFoundationType::Static;
	BuildingFoundation->OnRep_ServerStreamedInLevel();

	BuildingFoundation->FoundationEnabledState = EDynamicFoundationEnabledState::Enabled;
	BuildingFoundation->DynamicFoundationRepData.EnabledState = EDynamicFoundationEnabledState::Enabled;
	BuildingFoundation->DynamicFoundationTransform = BuildingFoundation->GetTransform();
	BuildingFoundation->OnRep_DynamicFoundationRepData();
}

inline FName Sigma(std::wstring Name) {
	return UKismetStringLibrary::Conv_StringToName(Name.c_str());
}

inline UGameplayStatics* GetStatics()
{
	return (UGameplayStatics*)UGameplayStatics::StaticClass()->DefaultObject;
}

template<typename T>
T* VehiclesSpawnActor(UClass* Class, FVector Loc, FRotator Rot = {}, AActor* Owner = nullptr)
{
	static UGameplayStatics* statics = (UGameplayStatics*)UGameplayStatics::StaticClass()->DefaultObject;

	FTransform Transform{};
	Transform.Scale3D = FVector(1, 1, 1);
	Transform.Translation = Loc;
	Transform.Rotation = RotatorToQuat(Rot);

	return (T*)statics->FinishSpawningActor(statics->BeginDeferredActorSpawnFromClass(UWorld::GetWorld(), Class, Transform, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn, Owner), Transform);
}


static inline AFortGameStateAthena* GetGameState()
{
	return (AFortGameStateAthena*)UWorld::GetWorld()->GameState;
}


static inline int GetOffset(UObject* Object, std::string Name)
{
	for (UObject* ObjClass = Object->Class; ObjClass; ObjClass = *(UObject**)(__int64(ObjClass) + 0x40))
	{
		void* Property = *(void**)(__int64(ObjClass) + 0x50);

		if (Property)
		{
			FName* PropFName = (FName*)(__int64(Property) + 0x28);
			string PropName = PropFName ? PropFName->ToString() : "";

			while (Property)
			{
				if (PropName == Name)
				{
					return *(int*)(__int64(Property) + 0x4C);
				}

				Property = *(void**)(__int64(Property) + 0x20);
				PropFName = (FName*)(__int64(Property) + 0x28);
				PropName = Property ? (PropFName ? PropFName->ToString() : "None") : "None";
			}
		}
	}

	return 0;
}


static inline AFortPickupAthena* SpawnPickup(FVector Loc, UFortItemDefinition* Def, int Count, int LoadedAmmo, EFortPickupSourceTypeFlag SourceTypeFlag = EFortPickupSourceTypeFlag::Tossed, EFortPickupSpawnSource SpawnSource = EFortPickupSpawnSource::Unset, AFortPlayerPawn* Pawn = nullptr, bool Toss = true)
{
	AFortPickupAthena* NewPickup = SpawnActor<AFortPickupAthena>(Loc);
	NewPickup->bRandomRotation = true;
	NewPickup->PrimaryPickupItemEntry.ItemDefinition = Def;
	NewPickup->PrimaryPickupItemEntry.Count = Count;
	NewPickup->PrimaryPickupItemEntry.LoadedAmmo = LoadedAmmo;
	NewPickup->OnRep_PrimaryPickupItemEntry();
	NewPickup->PawnWhoDroppedPickup = Pawn;

	NewPickup->TossPickup(Loc, Pawn, -1, Toss, false, SourceTypeFlag, SpawnSource);

	if (SpawnSource == EFortPickupSpawnSource::Chest || SpawnSource == EFortPickupSpawnSource::AmmoBox)
	{
		NewPickup->bTossedFromContainer = true;
		NewPickup->OnRep_TossedFromContainer();
	}

	return NewPickup;
}

static inline AFortPickupAthena* SpawnPickup(FVector Loc, FFortItemEntry* Entry, EFortPickupSourceTypeFlag SourceTypeFlag = EFortPickupSourceTypeFlag::Tossed, EFortPickupSpawnSource SpawnSource = EFortPickupSpawnSource::Unset, AFortPlayerPawn* Pawn = nullptr, int OverrideCount = 0) //test
{
	if (!Entry)
		return nullptr;

	AFortPickupAthena* NewPickup = SpawnActor<AFortPickupAthena>(Loc);
	NewPickup->bRandomRotation = true;
	//NewPickup->PrimaryPickupItemEntry.Count = OverrideCount != 0 ? OverrideCount : Entry->Count;
	//NewPickup->PrimaryPickupItemEntry.LoadedAmmo = Entry->LoadedAmmo;
	//NewPickup->PrimaryPickupItemEntry.ItemDefinition = Entry->ItemDefinition;
	//NewPickup->PrimaryPickupItemEntry.ItemGuid = Entry->ItemGuid;
	NewPickup->PrimaryPickupItemEntry = *Entry;
	NewPickup->PrimaryPickupItemEntry.Count = OverrideCount != 0 ? OverrideCount : Entry->Count;
	NewPickup->OnRep_PrimaryPickupItemEntry();
	NewPickup->PawnWhoDroppedPickup = Pawn;

	NewPickup->TossPickup(Loc, Pawn, -1, true, false, SourceTypeFlag, SpawnSource);

	if (SpawnSource == EFortPickupSpawnSource::Chest || SpawnSource == EFortPickupSpawnSource::AmmoBox)
	{
		NewPickup->bTossedFromContainer = true;
		NewPickup->OnRep_TossedFromContainer();
	}

	return NewPickup;
}


inline void NewSpawnPickup(FVector Loc, UFortItemDefinition* Def, EFortPickupSourceTypeFlag Flag, EFortPickupSpawnSource SpawnSource, int Count = 1, int LoadedAmmo = 0, AFortPawn* Owner = nullptr);
inline void NewSpawnPickup(FVector Loc, UFortItemDefinition* Def, EFortPickupSourceTypeFlag Flag, EFortPickupSpawnSource SpawnSource, int Count, int LoadedAmmo, AFortPawn* Owner)
{
	AFortPickupAthena* Pickup = SpawnActor<AFortPickupAthena>(Loc);
	Pickup->bRandomRotation = true;
	Pickup->PrimaryPickupItemEntry.ItemDefinition = Def;
	Pickup->PrimaryPickupItemEntry.Count = Count;
	Pickup->PrimaryPickupItemEntry.LoadedAmmo = LoadedAmmo;
	Pickup->OnRep_PrimaryPickupItemEntry();

	if (Flag == EFortPickupSourceTypeFlag::Container)
	{
		Pickup->bTossedFromContainer = true;
		Pickup->OnRep_TossedFromContainer();
	}

	Pickup->TossPickup(Loc, nullptr, -1, true, false, Flag, SpawnSource);
}

inline UKismetMathLibrary* GetMath()
{
	return (UKismetMathLibrary*)UKismetMathLibrary::StaticClass()->DefaultObject;
}

inline FString Conv_NameToString(FName name)
{
	return ((UKismetStringLibrary*)UKismetStringLibrary::StaticClass()->DefaultObject)->Conv_NameToString(name);
}

inline void SwapVTable(void* base, int Idx, void* Detour, void** OG = nullptr)
{
	if (!base)
		return;

	void** VTable = *(void***)base;
	if (!VTable || !VTable[Idx])
		return;
	if (OG)
	{
		*OG = VTable[Idx];
	}

	//log_debug("Addr: 0x%llx\n", (__int64(VTable) + (Idx * 8)) - __int64(GetModuleHandleA(0)));

	DWORD oldProtection;

	VirtualProtect(&VTable[Idx], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtection);

	VTable[Idx] = Detour;

	VirtualProtect(&VTable[Idx], sizeof(void*), oldProtection, NULL);
}

inline UFortKismetLibrary* GetFortKismet()
{
	return (UFortKismetLibrary*)UFortKismetLibrary::StaticClass()->DefaultObject;
}

inline FName Conv_StringToName(FString str)
{
	return ((UKismetStringLibrary*)UKismetStringLibrary::StaticClass()->DefaultObject)->Conv_StringToName(str);
}

inline int GetAmmoForDef(UFortWeaponItemDefinition* Def)
{
	return 0;
}


inline UC::FString GetAccountID(AFortPlayerControllerAthena* PC)
{
	__int64 Smth = 0;
	__int64 Smth4 = 0;
	__int64 Smth2 = GetAccID1(__int64(PC), (__int64)&Smth);
	__int64 Smth3 = GetAccID2(Smth2, (__int64)&Smth4);
	__int64 v5 = *(_QWORD*)(Smth2 + 8);
	return *(wchar_t**)(v5 + 24);
}

class FChunkedFixedUObjectArray
{
	// enum { NumElementsPerChunk = 64 * 1024, };

	FUObjectItem** Objects;
	FUObjectItem* PreAllocatedObjects;
	int32 MaxElements;
	int32 NumElements;
	int32 MaxChunks;
	int32 NumChunks;
public:
	int32 Num() const { return NumElements; }
	int32 Capacity() const { return MaxElements; }
	bool IsValidIndex(int32 Index) const { return Index < Num() && Index >= 0; }

	FUObjectItem* GetItemByIndex(int32 Index)
	{
		if (!IsValidIndex(Index)) return nullptr;

		const int32 ChunkIndex = Index / 0x10000;
		const int32 WithinChunkIndex = Index % 0x10000;

		// checkf(ChunkIndex < NumChunks, TEXT("ChunkIndex (%d) < NumChunks (%d)"), ChunkIndex, NumChunks);
		// checkf(Index < MaxElements, TEXT("Index (%d) < MaxElements (%d)"), Index, MaxElements);
		FUObjectItem* Chunk = Objects[ChunkIndex];

		if (!Chunk)
			return nullptr;

		return Chunk + WithinChunkIndex;
	}

	bool IsValid(UObject* Object)
	{
		int32 Index = Object->Index;
		if (Index == -1)
		{
			// UE_LOG(LogUObjectArray, Warning, TEXT("Object is not in global object array"));
			return false;
		}
		if (!IsValidIndex(Index))
		{
			// UE_LOG(LogUObjectArray, Warning, TEXT("Invalid object index %i"), Index);
			return false;
		}

		FUObjectItem* Slot = GetItemByIndex(Index);
		if (!Slot || Slot->Object == nullptr)
		{
			// UE_LOG(LogUObjectArray, Warning, TEXT("Empty slot"));
			return false;
		}
		if (Slot->Object != Object)
		{
			// UE_LOG(LogUObjectArray, Warning, TEXT("Other object in slot"));
			return false;
		}
		return true;
	}

	UObject* GetObjectByIndex(int32 Index)
	{
		if (auto Item = GetItemByIndex(Index))
			return Item->Object;

		return nullptr;
	}
};




template<typename T>
T* BOtSpawnActor(FVector Loc, FRotator Rot = {}, AActor* Owner = nullptr)
{
	static UGameplayStatics* statics = (UGameplayStatics*)UGameplayStatics::StaticClass()->DefaultObject;

	FTransform Transform{};
	Transform.Scale3D = FVector(1, 1, 1);
	Transform.Translation = Loc;

	return (T*)statics->FinishSpawningActor(statics->BeginDeferredActorSpawnFromClass(UWorld::GetWorld(), T::StaticClass(), Transform, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn, Owner), Transform);
}

template<typename T>
T* BOtSpawnActor(UClass* Class, FVector Loc, FRotator Rot = {}, AActor* Owner = nullptr)
{
	static UGameplayStatics* statics = (UGameplayStatics*)UGameplayStatics::StaticClass()->DefaultObject;

	FTransform Transform{};
	Transform.Scale3D = FVector(1, 1, 1);
	Transform.Translation = Loc;
	Transform.Rotation = RotatorToQuat(Rot);

	return (T*)statics->FinishSpawningActor(statics->BeginDeferredActorSpawnFromClass(UWorld::GetWorld(), Class, Transform, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn, Owner), Transform);
}

template<typename T>
static inline T* SpawnActorAI(SDK::FVector Loc, SDK::FRotator Rot = SDK::FRotator(), SDK::AActor* Owner = nullptr, SDK::UClass* Class = T::StaticClass(), ESpawnActorCollisionHandlingMethod Handle = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn)
{
	SDK::FTransform Transform{};
	Transform.Scale3D = SDK::FVector{ 1,1,1 };
	Transform.Translation = Loc;
	Transform.Rotation = RotatorToQuat(Rot);
	return (T*)SDK::UGameplayStatics::GetDefaultObj()->FinishSpawningActor(SDK::UGameplayStatics::GetDefaultObj()->BeginDeferredActorSpawnFromClass(SDK::UWorld::GetWorld(), Class, Transform, Handle, Owner), Transform);
}


inline Loadout& GetRandomLoadout()
{
	static int Wow = 0;
	static bool Once = false;
	if (!Once)
	{
		Once = true;
		Wow = GetMath()->RandomIntegerInRange(0, Loadouts.size() - 1);
	}
	Loadout& loadout = Loadouts[Wow];
	Wow++;
	if (Wow == Loadouts.size())
		Wow = 0;
	return loadout;
}

inline FVector GetSpawnLoc()
{
	if (!GotLoc)
	{
		SpawnLocs;
		GotLoc = true;

		((UGameplayStatics*)UGameplayStatics::StaticClass()->DefaultObject)->GetAllActorsOfClass(UWorld::GetWorld(), ((!bBattleLab && !bCreative) ? AFortPlayerStartWarmup::StaticClass() : AFortPlayerStartCreative::StaticClass()), &SpawnLocs);
	}

	return SpawnLocs[rand() % (SpawnLocs.Num() - 1)]->K2_GetActorLocation();

	//return FVector(0,0,9999);
}


template<typename T>
T* BossesSpawnActor2(FVector Loc, FRotator Rot = {}, AActor* Owner = nullptr)
{
	static UGameplayStatics* statics = (UGameplayStatics*)UGameplayStatics::StaticClass()->DefaultObject;

	FTransform Transform{};
	Transform.Scale3D = FVector(1, 1, 1);
	Transform.Translation = Loc;

	return (T*)statics->FinishSpawningActor(statics->BeginDeferredActorSpawnFromClass(UWorld::GetWorld(), T::StaticClass(), Transform, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn, Owner), Transform);
}

template<typename T>
T* BossesSpawnActor3(FVector Loc, FRotator Rot = {}, AActor* Owner = nullptr)
{
	static UGameplayStatics* statics = (UGameplayStatics*)UGameplayStatics::StaticClass()->DefaultObject;

	FTransform Transform{};
	Transform.Scale3D = FVector(1, 1, 1);
	Transform.Translation = Loc;

	return (T*)statics->FinishSpawningActor(statics->BeginDeferredActorSpawnFromClass(UWorld::GetWorld(), T::StaticClass(), Transform, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn, Owner), Transform);
}