#include "VehicleCont.h"
#include "MarsRover.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "WheelRayComp.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "IHeadMountedDisplay.h"
#include "GameCont.h"


//#include "../Plugins/Runtime/PhysXVehicles/Source/PhysXVehicles/Public/WheeledVehicleMovementComponent4W.h"




AVehicleCont::AVehicleCont()
{
	RotationAccelRatio = 0.2f;
	RotationDecelRatio = 0.3f;
	RotationAccelMaxAngle = 30;

	CompVrRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VrRoot"));
	CompVrRoot->SetRelativeLocation(FVector(190,0,0));
	CompVrRoot->SetupAttachment(RootComponent);

	CompMotionCont = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionCont"));
	CompMotionCont->SetupAttachment(CompVrRoot);

	

	compArrowSteering = CreateDefaultSubobject<UArrowComponent>(TEXT("arrowSteering"));
	compArrowSteering->SetupAttachment(CompVrRoot);

	compArrowRaw = CreateDefaultSubobject<UArrowComponent>(TEXT("arrowRaw"));
	compArrowRaw->SetupAttachment(CompVrRoot);


	CompDisplayVrHand = CreateDefaultSubobject<UChildActorComponent>(TEXT("DisplayVrHand"));
	CompDisplayVrHand->SetupAttachment(CompMotionCont);


	
}

void AVehicleCont::Init()
{
}
void AVehicleCont::BeginPlay()
{
	APlayerController* Controller = Cast<APlayerController>(GetController());
	Controller->SetViewTargetWithBlend(this);


	Super::BeginPlay();


	if (SoundEngine)
	{
		SoundEngine->Stop();
		SoundEngine->SetVolumeMultiplier(0);
		//SoundEngine->repea
	}



	CompMesh = GetMesh();
	CompMove = Cast<UWheeledVehicleMovementComponent4W>(GetMovementComponent());
	//CompMove->OnComponentHit.AddDynamic(this, &AVehicleCont::OnHit);

	accelFactor = 0;
	listWheelRayComps.Init(0, 6);
	TArray<USceneComponent*> listAllComps;
	GetComponents(listAllComps);
	for (USceneComponent* sc : listAllComps)
	{
		UWheelRayComp* rayComp = Cast<UWheelRayComp>(sc);
		if (rayComp)
		{
			listWheelRayComps[rayComp->Index] = rayComp;
			rayComp->TrailLifeTime = TrailLifeTime;
			rayComp->TrailDecalMaterial = TrailDecalMaterial;
		}
	}

	

	RotationAccelRatio = FMath::Clamp<float>(RotationAccelRatio, 0, 1);


	

	/* set vehicle max steer */
	{
		//CompMove->WheelSetups[0].
		UVehicleWheel* frontWheel = CompMove->Wheels[0];
		FWheelSetup frontWheelSetup = CompMove->WheelSetups[0];
		vehicleMaxSteer = frontWheel->SteerAngle;

	}

	/* vr motion cont component */

	if (AGameCont::INS->VrMode)
	{
		compArrowRaw->SetArrowColor(FLinearColor::Gray);
		compArrowRaw->SetHiddenInGame(false);
		compArrowSteering->SetArrowColor(FLinearColor::Red);
		compArrowSteering->SetHiddenInGame(false);

		CompMotionCont->Hand = VrSteerControllerHand;
	}





	 if (CamVr) 	CamVr->bLockToHmd = false;
	 SetActiveCam(AGameCont::INS->VrMode ? "VR" : "Outer");

	 SetMaxSpeed(MaxSpeed);
	 bindInputs();

}
void AVehicleCont::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	float v = GetVelocity().Size();
	//Debug("v : ",v);
	float rearWheelSteering = FMath::Clamp<float>((140 - v)/2, 0, 70);
	//Debug("rearWheelSteering : ", rearWheelSteering);
		
	
	FVehicleEngineData vEngineData;

	//CompMove->UpdateMaxSteer(2, rearWheelSteering);

	TArray<FVector> listResult;
	maxClimpValueOfAllWhells = 0;
	FVector impulseLoc = FVector::ZeroVector;
	FVector impulseRot = FVector::ZeroVector;
	for (UWheelRayComp* wheelRayComp : listWheelRayComps)
	{
		if (!wheelRayComp) return;
		FString refWheelCont = "wheel_control_" + FString::FromInt(wheelRayComp->Index + 1);
		FTransform socketTrans = GetMesh()->GetSocketTransform(FName(*refWheelCont) , ERelativeTransformSpace::RTS_Component);
		float yaw = socketTrans.GetRotation().Rotator().Yaw + GetActorRotation().Yaw;
		wheelRayComp->UpdateWheelYawAngle(yaw);
		//UpdateWheelPos(wheelRayComp->SocketName, wheelRayComp->GetDeltaZ());
		listResult.Add(FVector(0, 0, wheelRayComp->GetMaxZHit()));
		/* climp */
		maxClimpValueOfAllWhells = FMath::Max(wheelRayComp->GetClimpValue(), maxClimpValueOfAllWhells);
		/* extra impulse */
		if (wheelRayComp->EnableExtraImpulse)
		{
			TPair<FVector, FVector> impulses = wheelRayComp->GetExtraImpulses();
			//GetMesh()->AddImpulse(impulses.Key, NAME_None, true);
			//GetMesh()->AddAngularImpulse(impulses.Value, NAME_None, true);
			impulseLoc = impulses.Key.Z > impulseLoc.Z ? impulses.Key : impulseLoc;
			impulseRot = impulses.Value.Z > impulseRot.Z ? impulses.Value : impulseRot;
		}

	}
	/* apply extra impluse */
	CompMesh->AddImpulse(impulseLoc, NAME_None, true);
	CompMesh->AddAngularImpulse(impulseRot, NAME_None, true);


	//Debug("maxClimpValueOfAllWhells ", maxClimpValueOfAllWhells);
	UpdateWheelPos(listResult);
	//wvMoveComp->AddRadialForce()

	/* cam movement when not vr mode */
	if (CamVr)
	{
		/*if (!AGameCont::INS->VrMode && CamVr->IsActive())
		{
			FRotator relRotCamVr = CamVr->GetRelativeTransform().GetRotation().Rotator();
			float yaw = relRotCamVr.Yaw + GetInputAxisValue("LookRight");
			yaw = FMath::ClampAngle(yaw, -179, 179);
			float pitch = relRotCamVr.Pitch + GetInputAxisValue("LookUp");
			pitch = FMath::ClampAngle(pitch, -60, 60);
			relRotCamVr.Pitch = pitch;
			relRotCamVr.Yaw = yaw;
			CamVr->SetRelativeRotation(relRotCamVr);
		}*/

		/* hide some rigs when vr is active */
		for (FString boneName : ListHideBoneNamesOnVr)
		{
			if (boneName.Len() > 0)
			{
				if (CamVr->IsActive())
				{
					CompMesh->HideBoneByName(FName(*boneName), PBO_None);
				}

				else
				{
					CompMesh->UnHideBoneByName(FName(*boneName));
				}

			}
		}
	}
	

	
	

	updateSounds();

	if(AGameCont::INS->VrMode) updateVrController();
	
	updateHMD();
	updateInputs(DeltaTime);

}

void AVehicleCont::SetActiveCam(FString _camId)
{
	FString id = _camId.ToLower();
	if (activeCam)
	{
		activeCam->SetActive(false);
	}
	if (id == "vr")
		activeCam = CamVr;
	else
		activeCam = CamOuter;

	activeCam->SetActive(true);

}

void AVehicleCont::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != this)
	{
		IsOnGround = true;
	}
	else
	{
		IsOnGround = false;
	}
}


float AVehicleCont::GetCurrSpeed()
{
	return CompMesh->GetPhysicsLinearVelocity().Size();
}
/* params */
void AVehicleCont::SetMaxSpeed(float _maxSpeed)
{
	FRichCurve curve;
	curve.AddKey(0, _maxSpeed);
	curve.AddKey(1000, _maxSpeed);

	FVehicleEngineData engineData = CompMove->EngineSetup;
	FRuntimeFloatCurve runtFloatCurve = engineData.TorqueCurve;
	runtFloatCurve.EditorCurveData = curve;
	CompMove->EngineSetup = engineData;
	
	FKeyHandle keyhand;
	//Debug("SetMaxSpeed.getValue -> ",CompMove->EngineSetup.TorqueCurve.EditorCurveData.GetKeyValue(keyhand));
	Debug("SetMaxSpeed.GetDefaultValue -> ",CompMove->EngineSetup.TorqueCurve.EditorCurveData.GetDefaultValue());
}

float AVehicleCont::GetAccelFactor()
{
	float pitch = GetActorRotation().Pitch;
	float ratioRot = (pitch > 0) ? RotationAccelRatio : RotationDecelRatio;
	float wheelRayFactor = 0.05;
	float baseFactor = 1 - ratioRot ;
	
	pitch = FMath::Clamp<float>(pitch, -RotationAccelMaxAngle*2, RotationAccelMaxAngle);
	float pitchFactor = pitch / RotationAccelMaxAngle;
	pitchFactor = FMath::Clamp<float>(pitchFactor, -1,1);
	accelFactor = 0;
	float addRatioFromMaxRayWheelHit = (maxClimpValueOfAllWhells / 24) * wheelRayFactor;
	//accelFactor = baseFactor + (pitchFactor * RotationAccelRatio) ;

	//Debug("GetAccelFactor -> pitch / pitchFactor / final  " + FString::SanitizeFloat(pitch) + " / " + FString::SanitizeFloat(pitchFactor) + " / " + FString::SanitizeFloat(accelFactor));

	float currSpeed = GetCurrSpeed();
	float speedRatio =  currSpeed / MaxSpeed;
	speedRatio = FMath::Clamp<float>(speedRatio, 0, 1);
	float newAccel = 1 - speedRatio;
		
	
		if (pitchFactor < -5)
		{
			accelFactor = 0.4;
		
		}
		else
		{
			accelFactor = newAccel;
			accelFactor = FMath::Clamp<float>(newAccel, 0, 1);
		}
		
		
		Debug("GetAccelFactor -> pitchFactor : " , pitchFactor);
		Debug("GetAccelFactor -> speed / final : " + FString::SanitizeFloat(currSpeed) +  " / " + FString::SanitizeFloat(accelFactor));
	return accelFactor;
}

TArray<UWheelRayComp*> AVehicleCont::GetWheelComps()
{
	return listWheelRayComps;
}

void AVehicleCont::updateSounds()
{
	float newSpeed = FMath::FloorToFloat(GetCurrSpeed());
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

void AVehicleCont::updateVrController()
{
	if (CompMotionCont)
	{
		//FRotator motionRot = GetComponentRotation() - CompMotionCont->GetComponentRotation();
		FRotator motionRot = CompMotionCont->GetRelativeTransform().GetRotation().Rotator();
		float yaw = motionRot.Yaw;
		Debug(" yaw : ", yaw);

		FRotator rotRaw = FRotator(0, yaw, 0);
		compArrowRaw->SetRelativeRotation(rotRaw);

		float yawSteer = FMath::Clamp<float>(yaw, -vehicleMaxSteer, vehicleMaxSteer);
		FRotator rotStering = FRotator(0, yawSteer, 0);
		compArrowSteering->SetRelativeRotation(rotStering);

		float steeringFinal = yawSteer / vehicleMaxSteer;

		CompMove->SetSteeringInput(steeringFinal);

		EventVrInputAxisRight(steeringFinal);

			
	}
}

void AVehicleCont::updateHMD()
{

	if (AGameCont::INS->VrMode)
	{
		if (LimitHMDPosition >= 0)
		{
			FQuat Orientation;
			FVector Position;
			if (GEngine->HMDDevice->UpdatePlayerCamera(Orientation, Position))
			{
				FVector loc = Position.ClampSize(-LimitHMDPosition, LimitHMDPosition);

				CamVr->SetRelativeTransform(FTransform(Orientation, loc));
			}
			else
			{
				CamVr->ResetRelativeTransform();
			}
		}
		
	}
}

void AVehicleCont::updateInputs(float DeltaTime)
{
	if (!AGameCont::INS->VrMode)
	{
		FRotator relativeRot = activeCam->GetRelativeTransform().GetRotation().Rotator();
		float yaw = relativeRot.Yaw + axisLookRight;
		yaw = FMath::ClampAngle(yaw, -179, 179);
		float pitch = relativeRot.Pitch + axisLookUp;
		pitch = FMath::ClampAngle(pitch, -89, 89);
		relativeRot.Pitch = pitch;
		relativeRot.Yaw = yaw;
		activeCam->SetRelativeRotation(relativeRot);
		Debug("relativeRot "+ relativeRot.ToString());
	}


}

void AVehicleCont::bindInputs()
{
	InputComponent->BindAction("ToogleCam", IE_Pressed, this, &AVehicleCont::inputToogleCam);
	InputComponent->BindAction("Brake", IE_Pressed, this, &AVehicleCont::inputAxisBrakeOn);
	InputComponent->BindAction("Brake", IE_Released, this, &AVehicleCont::inputAxisBrakeOff);

	InputComponent->BindAxis("Forward ", this, &AVehicleCont::inputAxisForward);
	InputComponent->BindAxis("Right", this, &AVehicleCont::inputAxisRight);
	InputComponent->BindAxis("Brake2", this, &AVehicleCont::inputAxisBrake);
	InputComponent->BindAxis("LookRight", this, &AVehicleCont::inputLookRight);
	InputComponent->BindAxis("LookUp", this, &AVehicleCont::inputLookUp);
}


void AVehicleCont::inputAxisForward(float axisValue)
{
	float accel = axisValue * GetAccelFactor();
	CompMove->SetThrottleInput(accel);
	Debug("accel : ", accel);
}
void AVehicleCont::inputAxisRight(float axisValue)
{
	CompMove->SetSteeringInput(axisValue);
}
void AVehicleCont::inputAxisBrake(float axisValue)
{
	CompMove->SetBrakeInput(axisValue);
}
void AVehicleCont::inputAxisBrakeOn()
{
	CompMove->SetHandbrakeInput(true);
}
void AVehicleCont::inputAxisBrakeOff()
{
	CompMove->SetHandbrakeInput(false);
}

void AVehicleCont::inputLookUp(float axisValue)
{
	axisLookUp = axisValue;
}

void AVehicleCont::inputLookRight(float axisValue)
{
	axisLookRight = axisValue;
}

void AVehicleCont::inputToogleCam()
{
	SetActiveCam((activeCam == CamVr) ? "Outer" : "Vr");
}
