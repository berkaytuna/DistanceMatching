// Written by Berkay Tuna, August 2020

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DistanceMatchingComponent.generated.h"

class UAnimSequence;

UENUM(BlueprintType)
enum class ECurveType : uint8
{		
	None,
	Start,			
	Stop,
	Rotation		
};

UCLASS(ClassGroup = (DistanceMatching), meta = (BlueprintSpawnableComponent, DisplayName = "Distance Matching"))
class DISTANCEMATCHING_API UDistanceMatchingComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	FVector ActorLoc;

	UPROPERTY(EditAnywhere, Category = "Curve Name", meta = (DisplayName = "Distance Curve"))
	FName DistanceCurveName;
	UPROPERTY(EditAnywhere, Category = "Curve Name", meta = (DisplayName = "Rotation Curve"))
	FName RotationCurveName;

	UPROPERTY(BlueprintReadWrite, Category = "Animations")
	TArray<UAnimSequence*> StopAnimations;

	UPROPERTY(BlueprintReadWrite, Category = "Animations")
	TArray<UAnimSequence*> StartAnimations;

	UPROPERTY(BlueprintReadWrite, Category = "Animations")
	TArray<UAnimSequence*> RotationAnimations;
	TArray<UAnimSequence*> PrevRotationAnimations;

	TArray<UAnimSequence*> StartAnimations_Internal;
	TArray<UAnimSequence*> StopAnimations_Internal;
	TArray<UAnimSequence*> RotationAnimations_Internal;



	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool bIsAccelerating;
	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool bIsRotating;

	bool bCalculateOnce_Start;
	bool bCalculateOnce_Stop;
	bool bCalculateOnce_Rotation;

	UPROPERTY(BlueprintReadWrite, Category = "Curve Time")
	TArray<float> StopCurveTimes;
	TArray<float> PrevStopCurveTimes;
	TArray<float> StopControlCurveTimes;

	UPROPERTY(BlueprintReadWrite, Category = "Curve Time")
	TArray<float> StartCurveTimes;

	UPROPERTY(BlueprintReadWrite, Category = "Curve Time")
	TArray<float> RotationCurveTimes;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	float RotationSpeed;
	UPROPERTY(EditANywhere, BlueprintReadWrite, Category = "Rotation")
	float MaxRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	bool bStartFromBegin;

	UPROPERTY(BlueprintReadWrite, Category = "Rotation")
	float RotationYawToStop;
	float PrevRotationYawToStop;

	UPROPERTY(BlueprintReadOnly, Category = "Rotation")
	float CurrentRotationYaw;

	float TotalRotation;

	UPROPERTY(BlueprintReadOnly, Category = "Start")
	FVector StartPosition;
	float StopDistance;
	UPROPERTY(BlueprintReadOnly, Category = "Stop")
	FVector StopPosition;

	UPROPERTY(EditAnywhere, Category = "Stop")
	float StopTolerance;

	UPROPERTY(BlueprintReadOnly, Category = "Debug")
	bool bCanDoOnce;
	bool bCanDoOnce_RM;

	float ControlRotYawAtRotateStart;

	UDistanceMatchingComponent();

	UFUNCTION(BlueprintCallable, Category = "Rotation")
	void TurnToControlRotation(bool& bOverRotationThreshold, float& ControlRotDiff,
		UAnimSequence*& RotationSequence, FRotator ControlRotation, FRotator ControlRotationAtStop,
		UAnimSequence* LeftSequence, UAnimSequence* RightSequence, float RotationThreshold = 120.0f);

	UFUNCTION(BlueprintCallable, Category = "Rotation")
	void RotationMatching(FRotator& Output, FRotator Input, bool bIsRotating_DM);

	UFUNCTION(BlueprintCallable, Category = "Stop")
	FVector CalculateStopPosition();

private:

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	float GetCurveTime(UAnimSequence* AnimSequence, float CurveValue, ECurveType CurveType);
};