#include "GameCont.h"
#include "MarsRover.h"

AGameCont* AGameCont::INS = NULL;

AGameCont::AGameCont()
{
	PrimaryActorTick.bCanEverTick = true;
	AGameCont::INS = this;
}

void AGameCont::BeginPlay()
{
	if(!GEngine->HMDDevice.IsValid()) AGameCont::INS->VrMode = false;
	Super::BeginPlay();
	if (Vehicle) 	Vehicle->Init();


	//GetWorld()->Exec(GetWorld(), TEXT("stat fps"));
	 //Debug("run!!!", DebugMode);
	
}

void AGameCont::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

