#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnrealType.h"
#include "Core/CoreActor.h"
#include "VehicleCont.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Rover.generated.h"

class AVehicleCont;
UCLASS()
class MARSROVER_API ARover : public ACoreActor
{
	GENERATED_BODY()
public:	
	ARover();
	virtual void Tick(float DeltaTime) override;
	void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	UPROPERTY(EditAnywhere, Category=MARSROVER) float MaxSpeed;
	UPROPERTY(EditAnywhere, Category = MARSROVER) float TrailLifeTime;
	UPROPERTY(EditAnywhere, Category = MARSROVER) class UMaterialInterface*  TrailDecalMaterial;
	UPROPERTY(EditAnywhere, Category = MARSROVER) TArray<FString> ListHideBoneNamesOnVr;
	UPROPERTY(BlueprintReadWrite, Category = MARSROVER) UAudioComponent* SoundEngine;

protected:
	virtual void BeginPlay() override;
	UPROPERTY() AVehicleCont* vehicle;
	
private:
	int8 isRun;
	float currSpeed;
};