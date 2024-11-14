#include "Rover.h"
#include "MarsRover.h"
#include "WheelRayComp.h"


ARover::ARover()
{
	PrimaryActorTick.bCanEverTick = true;
	
}

void ARover::BeginPlay()
{
	

	Super::BeginPlay();
	isRun = 0;
	currSpeed = 0;

	UChildActorComponent* childActor = Cast<UChildActorComponent>(GetComp("ChildVehicle"));
	if (childActor)
	{
		//MrUtils::DebugErr("Rover ChildActor Not Found!");
		vehicle = Cast<AVehicleCont>(childActor->GetChildActor());
		vehicle->SetMaxSpeed(MaxSpeed);
		vehicle->ListHideBoneNamesOnVr = ListHideBoneNamesOnVr;
		for (UWheelRayComp* wheelComp : vehicle->GetWheelComps())
		{
			wheelComp->TrailLifeTime = TrailLifeTime;
			wheelComp->TrailDecalMaterial = TrailDecalMaterial;
			
		}
	}

	if (SoundEngine)
	{
		SoundEngine->Stop();
		SoundEngine->SetVolumeMultiplier(0);
		//SoundEngine->repea
	}

}

#if WITH_EDITOR
void ARover::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	Debug(PropertyName.ToString());
	//if ((PropertyName == GET_MEMBER_NAME_CHECKED(UPaperTileLayer, LayerWidth)) || (PropertyName == GET_MEMBER_NAME_CHECKED(UPaperTileLayer, LayerHeight)))
	//{
	//}

	/*if ((PropertyName == GET_MEMBER_NAME_CHECKED(float, TrailLifeTime))
	{

	}
*/
}
#endif
// Called every frame
void ARover::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!vehicle) return;
	float newSpeed = FMath::FloorToFloat(vehicle->GetCurrSpeed());
	if (SoundEngine)
	{
		
		if (currSpeed == 0 && newSpeed > 1)
		{
			if (isRun == 0)
			{
				isRun = 1;
				SoundEngine->Play(0);
			
			}
			
		}

		if (currSpeed > 0 && newSpeed < 1)
		{
			if (isRun == 1)
			{
				isRun = 0;
				//SoundEngine->Stop();
			}

		}

		float vol = (newSpeed / 60) * 0.6;
		vol = FMath::Clamp<float>(vol, 0, 0.6);
		SoundEngine->SetVolumeMultiplier(vol);
		
		
		
	}
	currSpeed = newSpeed;

	//float GetInputAxisValue("LookRight");
}

