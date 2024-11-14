// Fill out your copyright notice in the Description page of Project Settings.

#include "TestActor.h"
#include "MarsRover.h"

// Sets default values
ATestActor::ATestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATestActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!p1 || !p2) return;
	FVector v1 = p1->GetComponentLocation();
	FVector v2 = p2->GetComponentLocation();
	FVector v = (v2 - v1);
	float sign = (v2.Y < v1.Y) ? -1.0f : 1.0f;
	FRotator r;
	//r = FRotationMatrix::MakeFromX(v.GetSafeNormal()).Rotator() * sign;
	r = FRotationMatrix::MakeFromXZ(FVector(v.X, 0, 0), FVector(0, 0, v.Z)).Rotator();	
	Debug("r : " + r.ToString());



}

