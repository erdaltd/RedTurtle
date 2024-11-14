#include "CoreActor.h"
#include "MarsRover.h"


// Sets default values
ACoreActor::ACoreActor()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ACoreActor::BeginPlay()
{
	Super::BeginPlay();


	TArray<USceneComponent*> _listComps;
	GetComponents(_listComps,true);
	for (USceneComponent* c : _listComps)
	{
		listAllComps.Add(c->GetName(), c);
	}
	
	

	
}

USceneComponent* ACoreActor::GetComp(FString _name)
{
	USceneComponent* c = 0;
	if (listAllComps.Contains(_name))
		c = listAllComps[_name];
	return c;
}

// Called every frame
void ACoreActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

