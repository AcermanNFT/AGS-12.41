#pragma once
#include "Utils.h"

//void (*ProcessEventOG)(UObject* Object, UFunction* Function, void** Params);
//void ProcessEvent(UObject* Object, UFunction* Function, void** Params) {
//
//	auto FunctionName = Function->GetFullName();
//
//	//if (FunctionName.contains("")) {
//	//	std::cout << std::format("ProcessEvent: {}", FunctionName) << std::endl;
//	//}
//
//	return ProcessEventOG(Object, Function, Params);
//}
static LPVOID(*ProcessEvent)(void*, void*, void*) = nullptr;
__int64 sub_2B98BE0(__int64 a1, unsigned int a2, __int64 a3, unsigned __int8 a4, wchar_t* Format, ...) {

	if (wcsstr(Format, L"error")) {
		printf("%ls\n", Format);
		return 0;
	}

	return -1;
}

void (*ProcessEventOG)(void*, void*, void*);
void ProcessEventHook(UObject* Obj, UFunction* Func, void* Params)
{
    if (!Obj->GetName().contains("Vehicle") && Func->GetName().contains("Overlap")/* || Func->GetName().contains("Hit")*/)
    {
        cout << Func->GetName() << endl;
        cout << Obj->GetName() << endl;
        cout << __int64(*(void**)(__int64(Func) + 0xF0)) - __int64(GetModuleHandleW(0)) << endl;
        cout << __int64((*(void***)Obj->Class->DefaultObject)) - __int64(GetModuleHandleW(0)) << endl;
    }

    if (Func->GetName() == "ServerSpawnDeco") {
        ProcessEventOG(Obj, Func, Params);
        auto TT = (AFortTrapTool*)Obj;

        auto Def = TT->ItemDefinition;
        auto Pawn = (AFortPlayerPawn*)TT->GetOwner();
        auto PC = (AFortPlayerController*)Pawn->GetOwner();

        if (!PC->bInfiniteAmmo) {
            Remove(PC, Def);
        }

        return;
    }




    /*else if (Func->GetName() == "ServerCreateBuildingAndSpawnDeco")
    {
        struct ServerSpawnDeco_Params {
            FVector Location;
            FRotator Rotation;
            ABuildingSMActor* AttachedActor;
        };

        FVector TrapLocation;
        FRotator TrapRotation{};
        FVector BuildLocation;
        FRotator BuildRotation{};
        ABuildingSMActor* NewBuilding = nullptr;

        struct parms {
            FVector BuildingLocation;
            FRotator BuildingRotation;
            FVector Location;
            FRotator Rotation;
        };

        parms* Param = (parms*)Params;

        TrapLocation = Param->Location;
        TrapRotation = Param->Rotation;
        BuildLocation = Param->BuildingLocation;
        BuildRotation = Param->BuildingRotation;

        UClass* BuildingClass = APBWA_S1_Floor_C::StaticClass();
        char a7;
        TArray<AActor*> BuildingsToRemove;

        if (!CantBuild(UWorld::GetWorld(), BuildingClass, BuildLocation, BuildRotation, false, &BuildingsToRemove, &a7)) {
            auto PC = (AFortPlayerControllerAthena*)Obj;
            if (!PC->bBuildFree) {
                auto ResDef = GetFortKismet()->K2_GetResourceItemDefinition(((ABuildingSMActor*)BuildingClass->DefaultObject)->ResourceType);
                Remove(PC, ResDef, 10);
            }

            NewBuilding = VehiclesSpawnActor<ABuildingSMActor>(BuildingClass, BuildLocation, BuildRotation, PC);
            uint8 Team = *(uint8*)(__int64(PC->PlayerState) + 0xE60);

            NewBuilding->bPlayerPlaced = true;
            NewBuilding->InitializeKismetSpawnedBuildingActor(NewBuilding, PC, true);
            NewBuilding->TeamIndex = Team;
            NewBuilding->Team = EFortTeam(Team);

            for (int32 i = 0; i < BuildingsToRemove.Num(); i++) {
                BuildingsToRemove[i]->K2_DestroyActor();
            }
            BuildingsToRemove.Free();
        }

        ServerSpawnDeco_Params ServerSpawnDeco_params = {
            TrapLocation,
            TrapRotation,
            NewBuilding
        };

        auto SSD = AFortDecoTool::StaticClass()->DefaultObject->Class->GetFunction("FortDecoTool", "ServerSpawnDeco");
        ProcessEvent(Obj, SSD, &ServerSpawnDeco_params);
        return;
    }*/
    return ProcessEventOG(Obj, Func, Params);
}