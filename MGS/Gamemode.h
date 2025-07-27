#pragma once
#include "Utils.h"
#include "Vehicle.h"
#include "Llamas.h"
#include "API.h"

inline bool (*ReadyToStartMatchOG)(AFortGameModeAthena* GameMode);
inline bool ReadyToStartMatch(AFortGameModeAthena* GameMode)
{
    ReadyToStartMatchOG(GameMode);
    auto GameState = (AFortGameStateAthena*)GameMode->GameState;

    static bool Initialized = false;
    static bool Listening = false;

    if (!GameMode || !GameState)
    {
        return false;
    }

    if (!Initialized)
    {
        Initialized = true;
        PlooshFNAPI::SetupAPI();

      //  UFortPlaylistAthena* Playlist = UObject::FindObjectContain<UFortPlaylistAthena>("Playlist_DefaultSolo");
        UFortPlaylistAthena* Playlist = UObject::FindObject<UFortPlaylistAthena>(string(PlaylistBuffer) + "." + string(PlaylistBuffer));

        GameState->CurrentPlaylistInfo.BasePlaylist = Playlist;
        GameState->CurrentPlaylistInfo.OverridePlaylist = Playlist;
        GameState->CurrentPlaylistInfo.PlaylistReplicationKey++;
        GameState->CurrentPlaylistId = Playlist->PlaylistId;
        GameState->CurrentPlaylistInfo.MarkArrayDirty();
        //if (bBattleLab || bCreative)
        //{
        //    *(bool*)(__int64(GetGameState()) + GetOffset(GetGameState(), "bGameModeWillSkipAircraft")) = true; // sure bub
        //    bSpreadTeams = (bBattleLab || *(bool*)(__int64(Playlist) + GetOffset(Playlist, "bIsLargeTeamGame"))); // ever worst
        //}

      //  if (Playlist->bIsTournament) {
            GameState->EventTournamentRound = EEventTournamentRound::Open;
            GameState->EventTournamentRound = EEventTournamentRound::Arena;
            GameState->OnRep_EventTournamentRound();
       // }

        GetGameState()->EventTournamentRound = EEventTournamentRound::Arena;
        GetGameState()->OnRep_EventTournamentRound();
        
        AFortGameSessionDedicatedAthena* GameSession = SpawnActor<AFortGameSessionDedicatedAthena>();

        GameSession->SessionName = UKismetStringLibrary::Conv_StringToName(FString(L"MGS"));
        float TimeSeconds = GetStatics()->GetTimeSeconds(UWorld::GetWorld());

        GetGameState()->WarmupCountdownEndTime = TimeSeconds + Duration;
        GameMode->WarmupCountdownDuration = Duration;
        GetGameState()->WarmupCountdownStartTime = TimeSeconds;
        GameMode->WarmupEarlyCountdownDuration = Duration;
        GetGameState()->CurrentPlaylistInfo.BasePlaylist->MaxPlayers = *(int32*)(__int64(GameSession) + GetOffset(GameSession, "MaxPlayers"));// better than GameSession->MaxPlayers?
    //    GameSession->MaxPlayers = 100;
        GameMode->GameSession = GameSession;
        GameMode->FortGameSession = GameSession;
        GameMode->CurrentPlaylistId = Playlist->PlaylistId;

        if (bLateGame)
            *(int32*)(__int64(GameSession) + GetOffset(GameSession, "MaxPlayers")) = 40; // porper def not eon paste!

        GameMode->ServerBotManager = (UFortServerBotManagerAthena*)UGameplayStatics::SpawnObject(UFortServerBotManagerAthena::StaticClass(), GameMode);
        GameMode->ServerBotManager->CachedGameMode = GameMode;
        GameMode->ServerBotManager->CachedGameState = GameState;
        GameMode->ServerBotManager->CachedBotMutator = SpawnActor<AFortAthenaMutator_Bots>();
        GameMode->ServerBotManager->CachedBotMutator->CachedGameMode = GameMode;
        GameMode->ServerBotManager->CachedBotMutator->CachedGameState = GameState;

        for (auto& AdditionalLevel : GetPlaylist()->AdditionalLevels) {
            std::cout << std::format("Loading level: {}", AdditionalLevel.ObjectID.AssetPathName.ToString()) << std::endl;
            ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(UWorld::GetWorld(), AdditionalLevel, {}, {}, nullptr, FString());
            GameState->AdditionalPlaylistLevelsStreamed.Add(FAdditionalLevelStreamed{ AdditionalLevel.ObjectID.AssetPathName, false });
        }
        for (auto& AdditionalLevel : GetPlaylist()->AdditionalLevelsServerOnly) {
            std::cout << std::format("Loading server level: {}", AdditionalLevel.ObjectID.AssetPathName.ToString()) << std::endl;
            ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(UWorld::GetWorld(), AdditionalLevel, {}, {}, nullptr, FString());
            GameState->AdditionalPlaylistLevelsStreamed.Add(FAdditionalLevelStreamed{ AdditionalLevel.ObjectID.AssetPathName, true });
        }

        GameState->OnRep_AdditionalPlaylistLevelsStreamed();
        GameState->OnFinishedStreamingAdditionalPlaylistLevel();
    }

    static bool bShowFoundations = false;
    if (!bShowFoundations) {
        for (auto& BuildingFoundation : GetAllActorsOfClass<ABuildingFoundation>()) {

            if (BuildingFoundation->GetName().contains("Jerky") || BuildingFoundation->GetName().contains("LF_Athena_POI_19x19_2")) {
                std::cout << std::format("Showing foundation: {}", BuildingFoundation->GetName()) << std::endl;
                ShowFoundation(BuildingFoundation);
            }
        }
        GetAllActorsOfClass<ABuildingFoundation>().Free();
        bShowFoundations = true;
    }

  //  InitLooting();
   // SpawnFloorLoot();
   // SpawnVehicle();
    IsLoaded = true;

  //static bool bDestroyedVehicles = false;

  //if (!bDestroyedVehicles)
  //{
  //    TArray<AActor*> QuadcrasherArray;
  //    GetStatics()->GetAllActorsOfClass(UWorld::GetWorld(), UObject::FindObject<UClass>("BlueprintGeneratedClass AntelopeVehicle.AntelopeVehicle_C"), &QuadcrasherArray);

  //    for (AActor* Vehicle : QuadcrasherArray)
  //    {
  //        Vehicle->K2_DestroyActor();
  //    }

  //    TArray<AActor*> GolfCartsArray;
  //    GetStatics()->GetAllActorsOfClass(UWorld::GetWorld(), UObject::FindObject<UClass>("BlueprintGeneratedClass GolfCartVehicleSK.GolfCartVehicleSK_C"), &GolfCartsArray);

  //    for (AActor* Vehicle : GolfCartsArray)
  //    {
  //        Vehicle->K2_DestroyActor();
  //    }

  //    TArray<AActor*> ShoppingCartsArray;
  //    GetStatics()->GetAllActorsOfClass(UWorld::GetWorld(), UObject::FindObject<UClass>("BlueprintGeneratedClass ShoppingCartVehicleSK.ShoppingCartVehicleSK_C"), &ShoppingCartsArray);

  //    for (AActor* Vehicle : ShoppingCartsArray)
  //    {
  //        Vehicle->K2_DestroyActor();
  //    }

  //    bDestroyedVehicles = true;
  //}

  ////Log("Destroyed Shoppingcarts");
   // PlooshFNAPI::MarkServerOnline(Region, to_string(GameMode->GameSession->MaxPlayers), to_string(Port), SessionName, "Playlist_DADBRO_Squads");
        static auto OverrideBattleBusSkin = StaticFindObject<UAthenaBattleBusItemDefinition>("/Game/Athena/Items/Cosmetics/BattleBuses/BBID_DonutBus.BBID_DonutBus");
        GetGameState()->DefaultBattleBus = OverrideBattleBusSkin;
  if (bEnableLlamas) SpawnLlamas();

    if (!GameState->MapInfo)
    {
        return false;
    }

    if (!Listening)
    {
        Listening = true;

        if (Initialized)
        {
            GameState->OnRep_CurrentPlaylistId();
            GameState->OnRep_CurrentPlaylistInfo();
        }


        FName GameNetDriver = UKismetStringLibrary::Conv_StringToName(FString(L"GameNetDriver"));

        UNetDriver* Driver = CreateNetDriver(UEngine::GetEngine(), UWorld::GetWorld(), GameNetDriver);

        Driver->World = UWorld::GetWorld();
        Driver->NetDriverName = GameNetDriver;

        FString Error;
        FURL URL = FURL();
        URL.Port = 7777;

        InitListen(Driver, UWorld::GetWorld(), URL, false, Error);
        SetWorld(Driver, UWorld::GetWorld());

        UWorld::GetWorld()->NetDriver = Driver;
        UWorld::GetWorld()->LevelCollections[0].NetDriver = Driver;
        UWorld::GetWorld()->LevelCollections[1].NetDriver = Driver;

        GameMode->bWorldIsReady = true;
        GameMode->WarmupRequiredPlayerCount = 1;

        if (GetGameState()->PlayersLeft > 0)
        {
            return true;
        }
        else
        {

            float TimeSeconds = GetStatics()->GetTimeSeconds(UWorld::GetWorld());

            GetGameState()->WarmupCountdownEndTime = TimeSeconds + Duration;
            GameMode->WarmupCountdownDuration = Duration;
            GetGameState()->WarmupCountdownStartTime = TimeSeconds;
            GameMode->WarmupEarlyCountdownDuration = Duration;
        }

        SetConsoleTitleA("AGS 12.41 hosting on port 7777");
        Log("Listening on port 7777.");
    }

    return UWorld::GetWorld()->NetDriver->ClientConnections.Num() > 0;
}


inline APawn* SpawnDefaultPawnFor(AFortGameMode* GM, AFortPlayerControllerAthena* Player, AActor* StartSpot)
{
    FTransform Transform;

    if (Player->IsInAircraft())
    {
        Transform.Translation = StartSpot ? StartSpot->K2_GetActorLocation() : GetSpawnLoc();
    }
    else
    {
        Transform.Translation = GetSpawnLoc();
    }

    Transform.Scale3D = FVector(1.f, 1.f, 1.f);

    AFortPlayerPawnAthena* Ret = (AFortPlayerPawnAthena*)GM->SpawnDefaultPawnAtTransform(Player, Transform);
    return Ret;
}

__int64(__fastcall* MCP_DispatchRequest)(void* McpProfile, void* Context, int a3);
__int64 __fastcall MCP_DispatchRequestHook(void* McpProfile, void* Context, int a3)
{
    return MCP_DispatchRequest(McpProfile, Context, bMcp ? 3 : a3);
}

inline void (*DispatchRequestOG)(__int64 a1, unsigned __int64* a2, int a3);
inline void DispatchRequest(__int64 a1, unsigned __int64* a2, int a3)
{
    return DispatchRequestOG(a1, a2, 3);
}

__int64 StartAircraftPhaseHook(AFortGameModeAthena* GM, char a2)
{
    cout << to_string(a2) << endl;
    //auto ret = StartAircraftPhase(GM, a2);
    //if (BotsEnabled)
    //	((UKismetSystemLibrary*)UKismetSystemLibrary::StaticClass()->DefaultObject)->ExecuteConsoleCommand(GetWorld(), TEXT("startaircraft"), nullptr);//1:1
    //return ret;
    return StartAircraftPhase(GM, a2);
}