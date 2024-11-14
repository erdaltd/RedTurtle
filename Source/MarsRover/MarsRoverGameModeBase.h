#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MarsRoverGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class MARSROVER_API AMarsRoverGameModeBase : public AGameModeBase
{
	GENERATED_BODY()


	
	void BeginPlay() override;
};
