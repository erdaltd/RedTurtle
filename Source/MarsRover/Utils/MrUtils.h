// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MrUtils.generated.h"

/**
 * 
 */
UCLASS()
class MARSROVER_API UMrUtils : public UObject
{
	GENERATED_BODY()
	
	
	
public:

	static void DrawLine(FVector _from, FVector _to, FColor _color, bool _persistance=false);
	static void DrawPoint(FVector _loc, FColor _color, float _size=5, bool _persistance=false);
};
