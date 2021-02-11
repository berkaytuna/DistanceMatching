// Written by Berkay Tuna, August 2020

#include "DistanceMatchingComponent.h"
#include "DistanceMatching.h"
#include "Engine/World.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimCurveCompressionCodec_UniformIndexable.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
//#include "DrawDebugHelpers.h"
//#include "Engine/Engine.h"

UDistanceMatchingComponent::UDistanceMatchingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	StopTolerance = 0.005f;
	RotationSpeed = 120.0f;
	MaxRotation = 180.0f;
}

void UDistanceMatchingComponent::TurnToControlRotation(bool& bOverRotationThreshold, float& ControlRotDiff,
	UAnimSequence*& RotationSequence, FRotator ControlRot, FRotator ControlRotAtStop, 
	UAnimSequence* LeftSequence, UAnimSequence* RightSequence, float RotThreshold)
{
	ControlRotDiff = ControlRot.Yaw - ControlRotAtStop.Yaw;
	float Offset;

	if (ControlRotDiff < -180.0f)
	{
		Offset = 360.0f;
	}
	else if (ControlRotDiff > 180.0f)
	{
		Offset = -360.0f;
	}
	else
	{
		Offset = 0.0f;
	}

	ControlRotDiff += Offset;

	if (abs(ControlRotDiff) > RotThreshold)
	{
		bOverRotationThreshold = true;
	}
	else
	{
		bOverRotationThreshold = false;
	}

	if (bOverRotationThreshold)
	{
		if (ControlRotDiff > 0)
		{
			RotationSequence = RightSequence;
		}
		else
		{
			RotationSequence = LeftSequence;
		}
	}
}

void UDistanceMatchingComponent::RotationMatching(FRotator& Output, FRotator Input, bool bIsRotating_DM)
{
	if (bIsRotating_DM)
	{
		if (bCanDoOnce_RM)
		{
			bCanDoOnce_RM = false;

			ControlRotYawAtRotateStart = Input.Yaw;
		}		

		Output = Input;
		Output.Yaw = ControlRotYawAtRotateStart + CurrentRotationYaw;
	}
	else
	{
		bCanDoOnce_RM = true;

		Output = Input;
	}
}

FVector UDistanceMatchingComponent::CalculateStopPosition()
{
	UWorld* World = GetWorld();
	float DeltaTime = World->DeltaTimeSeconds;

	AActor* Actor = GetOwner();
	ACharacter* Char = Cast<ACharacter>(Actor);
	UCharacterMovementComponent* CharMov = Char->GetCharacterMovement();

	float Acceleration = CharMov->GetCurrentAcceleration().Size();
	FVector Velocity = CharMov->Velocity;
	float Speed = Velocity.Size();

	FVector ActorLocation = Actor->GetActorLocation();

	float BrakingDecelerationWalking = CharMov->BrakingDecelerationWalking;
	float GroundFriction = CharMov->GroundFriction;
	float BrakingFrictionFactor = CharMov->BrakingFrictionFactor;

	float f = GroundFriction * BrakingFrictionFactor;
	float df = f * DeltaTime;
	float dB = BrakingDecelerationWalking * DeltaTime;

	float A = ((0.0f * f) + BrakingDecelerationWalking) / ((Speed * f) + BrakingDecelerationWalking);
	float B = 1 - df;

	float n = FMath::Loge(A) / FMath::Loge(B);

	float C = Speed * pow(B, (n + 2));
	float D = (-1.0f) * (Speed + dB) * pow(B, (n + 1));
	float E = ((dB * (n + 1)) - Speed) * B;
	float F = Speed - (dB * n);

	StopDistance = (C + D + E + F) / pow((1 - B), 2.0f) * DeltaTime;

	StopPosition = Velocity * (StopDistance / Speed) + ActorLocation;

	return StopPosition;
}

float UDistanceMatchingComponent::GetCurveTime(UAnimSequence* AnimSequence, float CurveValue, ECurveType CurveType)
{
	if (AnimSequence) {

		FName CurveName;

		if ((CurveType == ECurveType::Start) || (CurveType == ECurveType::Stop)) {

			CurveName = DistanceCurveName;
		}
		else if (CurveType == ECurveType::Rotation) {

			CurveName = RotationCurveName;
		}
	
	/*
		GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Red, FString::Printf(TEXT("%d, %s, %s"),
			CurveType,
			*DistanceCurveName.ToString(),
			*CurveName.ToString()));
	*/

		FSmartName CurveSmartName;
		AnimSequence->GetSkeleton()->GetSmartNameByName(USkeleton::AnimCurveMappingName, CurveName, CurveSmartName);

		FAnimCurveBufferAccess CurveBuffer = FAnimCurveBufferAccess(AnimSequence, CurveSmartName.UID);

		int32 NumSamples = CurveBuffer.GetNumSamples();
		int32 SampleIndex = 0;

		float CurveTime;
	
		if (CurveType == ECurveType::Start) {

			for (int i = 0; i < NumSamples; i++) {

				float CurveBufferValue = CurveBuffer.GetValue(i);

				if (abs(CurveValue) < abs(CurveBufferValue)) {

					SampleIndex = i;

					break;
				}
			}

			if ((SampleIndex > 0) && (SampleIndex < NumSamples)) {

				CurveTime = CurveBuffer.GetTime(SampleIndex);

				float n = (CurveValue - CurveBuffer.GetValue(SampleIndex - 1)) / (CurveBuffer.GetValue(SampleIndex) - CurveBuffer.GetValue(SampleIndex - 1));
				CurveTime = CurveBuffer.GetTime(SampleIndex - 1) + n * (CurveBuffer.GetTime(SampleIndex) - CurveBuffer.GetTime(SampleIndex - 1));
			}
		}
		else if (CurveType == ECurveType::Stop) {

			CurveValue = (-1) * CurveValue;

			for (int i = 0; i < NumSamples; i++) {

				float CurveBufferValue = CurveBuffer.GetValue(i);

				if (abs(CurveValue) > abs(CurveBufferValue)) {

					SampleIndex = i;

					break;
				}
			}

			if ((SampleIndex > 0) && (SampleIndex < NumSamples)) {

				CurveTime = CurveBuffer.GetTime(SampleIndex);

				float n = (CurveValue - CurveBuffer.GetValue(SampleIndex - 1)) / (CurveBuffer.GetValue(SampleIndex) - CurveBuffer.GetValue(SampleIndex - 1));
				CurveTime = CurveBuffer.GetTime(SampleIndex - 1) + n * (CurveBuffer.GetTime(SampleIndex) - CurveBuffer.GetTime(SampleIndex - 1));
			}
		}	
		else if (CurveType == ECurveType::Rotation) {

			for (int i = 0; i < NumSamples; i++) {

				float CurveBufferValue = CurveBuffer.GetValue(i);

				if (abs(CurveValue) > abs(CurveBufferValue)) {

					SampleIndex = i;

					break;
				}
			}

			if ((SampleIndex > 0) && (SampleIndex < NumSamples)) {

				CurveTime = CurveBuffer.GetTime(SampleIndex);

				float n = (CurveValue - CurveBuffer.GetValue(SampleIndex - 1)) / (CurveBuffer.GetValue(SampleIndex) - CurveBuffer.GetValue(SampleIndex - 1));
				CurveTime = CurveBuffer.GetTime(SampleIndex - 1) + n * (CurveBuffer.GetTime(SampleIndex) - CurveBuffer.GetTime(SampleIndex - 1));
			}
		}

		// UE_LOG(LogTemp, Warning, TEXT("Text, %d %f %s"), intVar, floatVar, *fstringVar);
		// UE_LOG(LogTemp, Warning, TEXT("Text, %d; %f; %f; %f; %f"), SampleIndex, CurveValue, n, CurveBuffer.GetTime(SampleIndex), CurveTime);

		return CurveTime < 0.0f ? 0.0f : CurveTime;
	}
	else {

		return 0.0f;
	}
}

void UDistanceMatchingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{

	AActor* Actor = GetOwner();
	ACharacter* Char = Cast<ACharacter>(Actor);
	UCharacterMovementComponent* CharMov = Char->GetCharacterMovement();

	float Acceleration = CharMov->GetCurrentAcceleration().Size();

	



	if (StartAnimations.Num() != 0)
	{
		bCalculateOnce_Start = true;

		bIsAccelerating = true;

		StartAnimations_Internal = StartAnimations;
		StartAnimations.Empty();

		StartCurveTimes.Empty(StartAnimations_Internal.Num());
		StartCurveTimes.AddZeroed(StartAnimations_Internal.Num());
	}

	if (StopAnimations.Num() != 0)
	{
		bCalculateOnce_Stop = true;

		bIsAccelerating = false;

		StopAnimations_Internal = StopAnimations;
		StopAnimations.Empty();

		StopCurveTimes.Empty(StopAnimations_Internal.Num());
		StopCurveTimes.AddZeroed(StopAnimations_Internal.Num());

		PrevStopCurveTimes.Empty(StopAnimations_Internal.Num());
		PrevStopCurveTimes.AddZeroed(StopAnimations_Internal.Num());

		StopControlCurveTimes.Empty(StopAnimations_Internal.Num());
		StopControlCurveTimes.AddZeroed(StopAnimations_Internal.Num());
	}

	if (RotationAnimations.Num() != 0)
	{
		bCalculateOnce_Rotation = true;
		bIsRotating = true;

		RotationAnimations_Internal = RotationAnimations;
		RotationAnimations.Empty();

		RotationCurveTimes.Empty(RotationAnimations_Internal.Num());
		RotationCurveTimes.AddZeroed(RotationAnimations_Internal.Num());
	}





	for (int i = 0; i < StopAnimations_Internal.Num(); i++)
	{
		if (StopAnimations_Internal[i])
		{
			if (bCalculateOnce_Stop)
			{
				bCalculateOnce_Stop = false;

				StopPosition = CalculateStopPosition();
			}

			float DistanceToStop = (StopPosition - ActorLoc).Size();

			StopControlCurveTimes[i] = GetCurveTime(StopAnimations_Internal[i], DistanceToStop, ECurveType::Stop);

			if (bIsAccelerating)
			{
				StopCurveTimes[i] += DeltaTime;
			}
			else if ((StopControlCurveTimes[i] != 0.0f) && (abs(StopControlCurveTimes[i] - PrevStopCurveTimes[i]) < StopTolerance))
			{
				StopCurveTimes[i] += DeltaTime;
			}
			else
			{
				StopCurveTimes[i] = StopControlCurveTimes[i];
			}

			float SequenceLength = StopAnimations_Internal[i]->SequenceLength;
			StopCurveTimes[i] = FMath::Clamp(StopCurveTimes[i], 0.0f, SequenceLength + 1.0f);

			//UE_LOG(LogTemp, Warning, TEXT("Text, %f"), StopCurveTimes[i]);
		}
	}

	PrevStopCurveTimes = StopControlCurveTimes;

	int32 StopCount = 0;

	for (int i = 0; i < StopAnimations_Internal.Num(); i++)
	{
		float SequenceLength = StopAnimations_Internal[i]->SequenceLength;

		if (StopCurveTimes[i] == SequenceLength + 1.0f)
		{
			StopCount++;
		}
	}

	if (StopCount == StopAnimations_Internal.Num())
	{
		StopCurveTimes.Empty(StopAnimations_Internal.Num());
		StopCurveTimes.AddZeroed(StopAnimations_Internal.Num());

		StopAnimations_Internal.Empty();
	}





	for (int i = 0; i < StartAnimations_Internal.Num(); i++)
	{
		if (bCalculateOnce_Start)
		{
			bCalculateOnce_Start = false;

			StartPosition = ActorLoc;
		}

		if (StartAnimations_Internal[i])
		{
			float DistanceToStart = (ActorLoc - StartPosition).Size();

			float SequenceLength = StartAnimations_Internal[i]->SequenceLength;

			float ControlStartCurveTime = GetCurveTime(StartAnimations_Internal[i], DistanceToStart, ECurveType::Start);

			if (!bIsAccelerating)
			{
				StartCurveTimes[i] += DeltaTime;
			}
			else if (ControlStartCurveTime < SequenceLength)
			{
				StartCurveTimes[i] = ControlStartCurveTime;
			}
			else
			{
				StartCurveTimes[i] += DeltaTime;
			}

			// SequenceLength + 1.0f, incase blending Start->Jog long

			StartCurveTimes[i] = FMath::Clamp(StartCurveTimes[i], 0.0f, SequenceLength + 1.0f);

			/*
			if (StartCurveTimes[i] > SequenceLength)
			{
				StartAnimations[i] = nullptr;

				StartCurveTimes[i] = 0.0f;
			}
			*/
		}
	}

	ActorLoc = Actor->GetActorLocation();

	int32 StartCount = 0;

	for (int i = 0; i < StartAnimations_Internal.Num(); i++)
	{
		float SequenceLength = StartAnimations_Internal[i]->SequenceLength;

		if (StartCurveTimes[i] == SequenceLength + 1.0f)
		{
			StartCount++;
		}
	}

	if (StartCount == StartAnimations_Internal.Num())
	{
		StartCurveTimes.Empty(StartAnimations_Internal.Num());
		StartCurveTimes.AddZeroed(StartAnimations_Internal.Num());

		StartAnimations_Internal.Empty();
	}





	for (int i = 0; i < RotationAnimations_Internal.Num(); i++)
	{
		if (RotationAnimations_Internal[i])
		{		
			if (bCalculateOnce_Rotation)
			{
				bCalculateOnce_Rotation = false;

				TotalRotation = RotationYawToStop;
			}

			if (RotationYawToStop > 0.0f)
			{
				RotationYawToStop -= RotationSpeed * DeltaTime;
			}
			else if (RotationYawToStop < 0.0f)
			{
				RotationYawToStop += RotationSpeed * DeltaTime;
			}

			if (RotationYawToStop * PrevRotationYawToStop < 0.0f)
			{
				RotationYawToStop = 0.0f;
			}

			CurrentRotationYaw = TotalRotation - RotationYawToStop;

			if (RotationYawToStop == 0.0f)
			{
				bIsRotating = false;
			}

			float RotationCurveValue;

			if (bStartFromBegin)
			{
				RotationCurveValue = -(MaxRotation - abs(CurrentRotationYaw));
			}
			else
			{
				RotationCurveValue = -abs(RotationYawToStop);
			}

			//float MaxCurveValue = -(MaxRotation - TotalRotation);
			//RotationCurveValue = FMath::Clamp(RotationCurveValue, -MaxRotation, MaxCurveValue);

			if (RotationYawToStop == 0.0f && !bStartFromBegin)
			{
				RotationCurveTimes[i] += DeltaTime;
			}
			else
			{
				RotationCurveTimes[i] = GetCurveTime(RotationAnimations_Internal[i], RotationCurveValue, ECurveType::Rotation);
			}
		}
	}

	PrevRotationAnimations = RotationAnimations_Internal;
	PrevRotationYawToStop = RotationYawToStop;





}
