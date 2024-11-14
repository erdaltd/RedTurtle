// Fill out your copyright notice in the Description page of Project Settings.

#include "WheelRayComp.h"
#include "MarsRover.h"
#include "DrawDebugHelpers.h"
#include "VehicleCont.h"
#include "CollisionQueryParams.h"
#include "Utils/MrUtils.h"
#include "Core/CoreActor.h"
#include "Components/DecalComponent.h"


// Sets default values for this component's properties
UWheelRayComp::UWheelRayComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


void UWheelRayComp::BeginPlay()
{
	Super::BeginPlay();

	AVehicleCont* vehicleCont = Cast<AVehicleCont>(GetOwner());
	if (vehicleCont)
	{
		wheelBoneInitLoc = vehicleCont->GetMesh()->GetSocketLocation(FName(*SocketName)) - vehicleCont->GetActorLocation();
		wheelBottomPoint = wheelBoneInitLoc.Z - WheelRadius;
	}
	
	




	for (int8 i = 0; i < rowCount*columnCount; i++)
	{

		//float x1 = -(h / 2) + (FMath::Floor(i / row) *  (h / (column - 1)));
		//float y1 = -(w / 2) + ((i % row) * (w / (row - 1)));

		FWheelRay ray = FWheelRay(rayLength, FMath::Floor(i / rowCount), (i % rowCount) , i);
		//ray.defaultLoc = FVector(x1, y1, 0);
		//ray.start = FVector(x1, y1, rayLength);
		//ray.end = FVector(x1, x1, -rayLength);

		listRay.Add(ray);
	}


}


// Called every frame
void UWheelRayComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!IsEnable) return;

	//AVehicleCont* vehicleCont = Cast<AVehicleCont>(GetOwner());
	//FVector wheelLoc = vehicleCont->GetMesh()->GetSocketLocation(FName(*SocketName));
	//FVector wheelBottom = wheelLoc - FVector(0, 0, WheelRadius);



	FVector compLoc = GetComponentLocation();
	float compZ = compLoc.Z;
	float hitLocationZ = compZ -( WheelRadius );
	bool isAnyHit = false;


	updateRayLoc();

	for (int8 i = 0; i < listRay.Num(); i++)
	{
		
		FWheelRay ray = listRay[i];


		FVector rotatedOrigin = FRotator(0, wheelYaw, 0).RotateVector(ray.origin);
		
		FVector _start = compLoc + FVector(rotatedOrigin.X, rotatedOrigin.Y, rayLength);
		FVector _end = compLoc + FVector(rotatedOrigin.X, rotatedOrigin.Y, -rayLength);
		//FVector _end = compLoc + ray.end;

		FHitResult hit = rayTestCenterWheels(_start, _end, IsDebugLine,FColor::Green);
		
		

		if (hit.IsValidBlockingHit())
		{

			////if (ListGroundCollNames.Contains(hit.GetComponent()->GetCollisionProfileName().ToString()))
			//{
				isAnyHit = true;
				float newHitZ = hit.Location.Z;
				if (ray.rowIndex != 1) newHitZ -= WheelRadius;
				hitLocationZ = FMath::Max(hitLocationZ, newHitZ);
				FColor c = (ray.rowIndex == 2) ? FColor::Black : FColor::Blue;
				UMrUtils::DrawPoint(hit.Location, c);
			//}
	
		}

		ray.isHit = isAnyHit;
		listRay[i] = ray;
	}
	if (isAnyHit)
	{
		float  toZ = hitLocationZ - compZ;
		maxZHit = toZ;
		/*if (GetName() == "whComp3")
		{
			Debug("hitLocationZ ", hitLocationZ);
			Debug("maxZHit ", maxZHit);
		}*/
		
		//maxZHit = FMath::Lerp<float>(maxZHit, toZ, 0.9);
		//if(toZ < 20 && toZ > -5) 

		//deltaZ = FMath::FInterpTo(deltaZ, toZ, DeltaTime, 10);
		//UpdateWheelPos(3, _distWheel3 - wheelRadius);
		
	}
	

	if (TrailClass && isAnyHit && TrailDecalMaterial)
	{
		FVector _locDecal = GetComponentLocation();
		_locDecal.Z += maxZHit;
		float decalYaw = wheelYaw;
		FRotator _rotDecal = FRotator(0, decalYaw, 0);
		FActorSpawnParameters SpawnInfo;
		if ((_locDecal - lastDecalTrailLoc).Size() >= TrailMinDistanceToOthers)
		{
			ACoreActor* decActor = Cast<ACoreActor>( GetWorld()->SpawnActor<AActor>(TrailClass, _locDecal, _rotDecal, SpawnInfo));
			UDecalComponent* decComp = Cast<UDecalComponent>(decActor->GetComp("Decal"));
			 decComp->SetMaterial(0,TrailDecalMaterial);
			//decComp->FadeDuration = TrailLifeTime;
			decComp->SetFadeOut(0, TrailLifeTime, true);
			Debug("decComp->FadeDuration ", decComp->FadeDuration);
			lastDecalTrailLoc = decActor->GetActorLocation();
		}
		
	}


	


	

	
}

void UWheelRayComp::updateRayLoc()
{
	float xStart = -(wheelXLength / 2);
	float yStart = -(wheelYLength / 2);
	float xSize = (wheelXLength / (rowCount - 1));
	float ySize = (wheelYLength / (columnCount - 1));
	for (int8 i = 0; i < listRay.Num(); i++)
	{

		FWheelRay ray = listRay[i];



		float x = xStart + (ray.rowIndex *  xSize);
		float y = yStart + (ray.columnIndex * ySize);

		ray.origin = FVector(x, y, 0);
		//ray.start = FVector(x, y, (rayLength));
		//ray.end = FVector(x, y, -(rayLength));

		listRay[i] = ray;
	}
}
float UWheelRayComp::GetMaxZHit()
{
	return maxZHit;
}


float UWheelRayComp::GetClimpValue()
{
	float result = 0;
	int8 centerOfCenterRayIndex = 1 + (3 * 1);
	FWheelRay rayCenterOfCenter = listRay[centerOfCenterRayIndex];
	if (!rayCenterOfCenter.isHit) return result;

	FVector maxZFrontRows = rayCenterOfCenter.hitLoc;
	FVector maxZCenterRows = maxZFrontRows;
	for (FWheelRay ray : listRay)
	{
		if (ray.rowIndex == 2) //front
			if (ray.hitLoc.Z > maxZFrontRows.Z)
				maxZFrontRows = ray.hitLoc;
	}

	if (maxZFrontRows.Z > maxZCenterRows.Z)
	{

		
		/*DrawDebugCircle(
			GetWorld(),
			maxZFrontRows,
			5,
			0,
			FColor::Magenta,
			false

		);*/
		result = (maxZFrontRows.Z - maxZCenterRows.Z);
	}
		


	return result;

}


TPair<FVector, FVector> UWheelRayComp::GetExtraImpulses()
{
	TPair<FVector, FVector> result;
	FVector impulse = FVector::ZeroVector;
	FVector impulseAngular = FVector::ZeroVector;
	if (EnableExtraImpulse)
	{
		if (maxZHit > ExtraImpulseMinWheelZ && maxZHit < ExtraImpulseMaxWheelZ)
		{
			float impZ = ExtraImpulseScale * maxZHit;
			float angularImpZ = ExtraAngularImpulseScale * maxZHit;
			//angularImpZ = FMath::Clamp<float>(angularImpZ, ExtraAngularImpulseMin, ExtraAngularImpulseMax);
			//Debug("angularImpZ : ", angularImpZ);
			impulse = FVector(0, 0, impZ);
			impulseAngular = FVector(angularImpZ, 0, 0);
		}
	}
	result.Key = impulse;
	result.Value = impulseAngular;
	return result;

}

void UWheelRayComp::UpdateWheelYawAngle(float _yaw)
{
	wheelYaw = _yaw;
}


FHitResult UWheelRayComp::rayTestCenterWheels(FVector _start, FVector _end, bool _debugDraw, FColor _color)
{
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, GetOwner());
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;

	
	FHitResult RV_Hit;

	FVector startLine = _start;
	//FVector endLine = startLine + FVector(0, 0, _zLength);
	FVector endLine = _end;



	GetWorld()->LineTraceSingleByChannel(
		RV_Hit,        //result
		startLine,    //start
		endLine, //end
		ECC_WorldStatic, //collision channel
		RV_TraceParams
	);


	if (_debugDraw)
	{
		UMrUtils::DrawLine(startLine, endLine, _color, false);
		
	}
	

	return RV_Hit;
}
