#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Engine/DecalActor.h"
#include "Core/CoreActor.h"
#include "WheelRayComp.generated.h"


USTRUCT()
struct MARSROVER_API FWheelRay
{
	GENERATED_USTRUCT_BODY()
		bool isHit;
	FVector defaultLoc;
	FVector origin;
	//FVector start;
	//FVector end;
	float length;
	int8 index;
	int8 rowIndex;
	int8 columnIndex;
	/* hit world lod */
	FVector hitLoc;
	/* hit relative loc ratio from length */
	float zRatio;

	FWheelRay() {};
	FWheelRay(float  _length, int8 _rowIndex, int8 _columnIndex, int8 _index)
	{
		length = _length;
		rowIndex = _rowIndex;
		columnIndex = _columnIndex;
		index = _index;
		isHit = false;
		//start = FVector(0, 0, length);
		//end =  FVector(0, 0, -length);
	};


};


UCLASS( ClassGroup=(MarsRover), Blueprintable, meta=(BlueprintSpawnableComponent) )
class MARSROVER_API UWheelRayComp : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWheelRayComp();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	/* all rays start & end loc must updated because of wheel steering */
	void updateRayLoc();
	UPROPERTY(EditAnywhere) int Index = 0;
	UPROPERTY(EditAnywhere) FString SocketName;
	//UPROPERTY(EditAnywhere) float RayLength = 100;
	UPROPERTY(EditAnywhere) bool IsEnable = false;
	UPROPERTY(EditAnywhere) bool IsDebugLine = false;
	UPROPERTY(EditAnywhere) float WheelRadius = 32;
	/* the collision names of all surfaces which the vehicle can run */
	UPROPERTY(EditAnywhere) TArray<FString> ListGroundCollNames;
	UPROPERTY(EditAnywhere) float TrailLifeTime;
	UPROPERTY(EditAnywhere) TSubclassOf<ACoreActor> TrailClass;
	UPROPERTY() class UMaterialInterface*  TrailDecalMaterial;
	/* for avoid overlap decals */
	UPROPERTY(EditAnywhere) float TrailMinDistanceToOthers = 0;
	UPROPERTY(EditAnywhere, Category = "ExtraImpulse") bool  EnableExtraImpulse = false;
	UPROPERTY(EditAnywhere, Category = "ExtraImpulse") float  ExtraImpulseMinWheelZ = 0;
	UPROPERTY(EditAnywhere, Category = "ExtraImpulse") float  ExtraImpulseMaxWheelZ = 15;
	UPROPERTY(EditAnywhere, Category = "ExtraImpulse") float ExtraImpulseScale = 1;
	UPROPERTY(EditAnywhere, Category = "ExtraImpulse") float ExtraAngularImpulseScale = -0.01;
	UFUNCTION() float GetMaxZHit();

	/* GetClimpValue() :  front ray hit - center ray hit*/
	float GetClimpValue();
	/* get extra impulse vector when wheel is on the rock. first vector  in pair = "impulse ", second vector = "angular impulse "  */
	TPair<FVector, FVector> GetExtraImpulses();
	void UpdateWheelYawAngle(float _yaw);
	float GetForwardAngle();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	private:
		float maxZHit = 0;
		FVector wheelBoneInitLoc; //bone (soket) relative loc
		float wheelBottomPoint; //wheel bottom z
		FHitResult rayTestCenterWheels(FVector _start, FVector _zLength, bool _debugDraw=false, FColor _color=FColor::Blue);

private:
	TArray<FWheelRay> listRay;
	float wheelYaw;
	/* const params */
	float wheelXLength = 50;
	float wheelYLength = 60;
	int8 rowCount = 3; // y axis
	int8 columnCount = 3; //x axis
	float rayLength = 100; //x axis
	FVector lastDecalTrailLoc;
	
	
};
