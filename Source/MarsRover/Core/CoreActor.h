#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoreActor.generated.h"

UCLASS()
class MARSROVER_API ACoreActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ACoreActor();

protected:
	virtual void BeginPlay() override;
	
	TMap<FString, USceneComponent*> listAllComps;
		

public:	
	virtual void Tick(float DeltaTime) override;
	USceneComponent* GetComp(FString _name);
	
	
};
