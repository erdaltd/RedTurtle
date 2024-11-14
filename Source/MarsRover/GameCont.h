// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "Vehicle/VehicleCont.h"
#include "GameCont.generated.h"

class AVehicleCont;

UCLASS()
class MARSROVER_API AGameCont : public AActor
{
	GENERATED_BODY()
	
public:	
	AGameCont();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	static AGameCont* INS;
	/* DebugMode : (0) no debug, (1) enable debug lines */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MARSROVER) int DebugMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MARSROVER) bool VrMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MARSROVER) AVehicleCont* Vehicle;


	
	
};
