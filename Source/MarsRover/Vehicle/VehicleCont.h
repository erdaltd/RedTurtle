// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "MotionControllerComponent.h"
#include "Components/ArrowComponent.h"
#include "Core/CoreActor.h"
#include "VehicleCont.generated.h"

class UWheelRayComp;




UCLASS()
class MARSROVER_API AVehicleCont : public AWheeledVehicle
{
	GENERATED_BODY()
	
public:
	AVehicleCont();
	void Init();
	void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void SetActiveCam(FString _camId);
	UFUNCTION() void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	float GetCurrSpeed();
	void SetMaxSpeed(float _maxSpeed);
	UPROPERTY(EditAnywhere, Category = MarsRover) 	float MaxSpeed = 200;
	UPROPERTY(EditAnywhere, Category = MarsRover, meta = (ClampMin = 0, ClampMax=1)) 	float RotationAccelRatio;
	/* extra - power for downhill */
	UPROPERTY(EditAnywhere, Category = MarsRover, meta = (ClampMin = 0, ClampMax=1)) 	float RotationDecelRatio;
	UPROPERTY(EditAnywhere, Category = MarsRover) 	float RotationAccelMaxAngle;
	UPROPERTY(BlueprintReadWrite, Category = MARSROVER) UCameraComponent* CamVr;
	UPROPERTY(BlueprintReadWrite, Category = MARSROVER) UCameraComponent* CamOuter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MARSROVER)  USceneComponent* CompVrRoot;;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MARSROVER) UMotionControllerComponent* CompMotionCont;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MARSROVER) UChildActorComponent* CompDisplayVrHand;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MARSROVER)  UArrowComponent* compArrowRaw;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MARSROVER)  UArrowComponent* compArrowSteering;


	UPROPERTY(EditAnywhere, Category = MARSROVER) float TrailLifeTime;
	UPROPERTY(EditAnywhere, Category = MARSROVER) class UMaterialInterface*  TrailDecalMaterial;
	UPROPERTY(BlueprintReadWrite, Category = MARSROVER) UAudioComponent* SoundEngine;

	/* [vr] vr hand controller that controls of the vehicle steering */
	UPROPERTY(EditAnywhere, Category = "MARSROVER-VR CONFIG") EControllerHand VrSteerControllerHand;
	UPROPERTY(EditAnywhere, Category = "MARSROVER-VR CONFIG") TArray<FString> ListHideBoneNamesOnVr;
	/* [vr] limit of hmd position. (limit < 0)  hmd not lock to head, (limit == 0)  hmd lock and not limit, (limit > 0) hmd lock and applied limit  */
	UPROPERTY(EditAnywhere, Category = "MARSROVER-VR CONFIG") float LimitHMDPosition = 0;


	//UFUNCTION(BlueprintImplementableEvent, Category = MarsRover) 	void UpdateWheelPos(const FString& _wheelSocketName, float _zDelta);
	UFUNCTION(BlueprintImplementableEvent, Category = MarsRover) 	void UpdateWheelPos(const TArray<FVector>& listWheelRayResult);
	UFUNCTION(BlueprintImplementableEvent, Category = MarsRover) 	void EventVrInputAxisRight(float _input);
	UFUNCTION() 	float GetAccelFactor();
	UFUNCTION() TArray<UWheelRayComp*> GetWheelComps();
	bool IsOnGround;

private:
	int8 isRun;
	float currSpeed;
	float vehicleMaxSteer;
	UCameraComponent* activeCam;
	float accelFactor;
	TArray<UWheelRayComp*> listWheelRayComps;
	UWheeledVehicleMovementComponent4W* CompMove;
	USkeletalMeshComponent* CompMesh;
	ACoreActor* displayVrCont;
	/* her  tekerlegin on ray grubundaki max hit location'i hesaplanir, her teker icin bu degerlerden max'i alinir. bu deger bize bir tumsek uzerinde oldugumuzu gosterir. on ray grubuna bakmamizin sebebi tam olarak bu */
	float maxClimpValueOfAllWhells;
	void updateSounds();
	
	
	
	void updateVrController();
	void updateHMD();
	/* inputs */
	float axisLookUp = 0;
	float axisLookRight = 0;
	void bindInputs();
	void updateInputs(float deltatime);
	void inputAxisForward(float axisValue);
	void inputAxisRight(float axisValue);
	void inputAxisBrakeOn();
	void inputAxisBrakeOff();
	void inputAxisBrake(float axisValue);
	void inputLookUp(float axisValue);
	void inputLookRight(float axisValue);
	void inputToogleCam();
};
