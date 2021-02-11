// Written by Berkay Tuna, August 2020

#include "AnimationModifier_DistanceCurve.h"
#include "DistanceMatching.h"
#include "Animation/AnimSequence.h"
#include "DistanceMatchingComponent.h"
#include "Animation/AnimCurveCompressionSettings.h"

UAnimationModifier_DistanceCurve::UAnimationModifier_DistanceCurve() 
{
	
}

ECurveType UAnimationModifier_DistanceCurve::GetCurveType(FName& CurveName, UAnimSequence* AnimSequence) {

	FString AnimName = AnimSequence->GetName();

	bool bIsStartAnim = AnimName.Contains(StartSubString, ESearchCase::CaseSensitive, ESearchDir::FromStart);
	bool bIsStopAnim = AnimName.Contains(StopSubString, ESearchCase::CaseSensitive, ESearchDir::FromStart);
	bool bIsRotationAnim = AnimName.Contains(RotationSubString, ESearchCase::CaseSensitive, ESearchDir::FromStart);

	if (bIsStartAnim) {

		CurveName = DistanceCurveName;
		return ECurveType::Start;
	}
	else if (bIsStopAnim) {

		CurveName = DistanceCurveName;
		return ECurveType::Stop;
	}
	else if (bIsRotationAnim) {

		CurveName = RotationCurveName;
		return ECurveType::Rotation;
	}
	else {

		return ECurveType::None;
	}
}

void UAnimationModifier_DistanceCurve::OnRevert_Implementation(UAnimSequence* AnimSequence)
{
	FName CurveName;
	ECurveType CurveType = GetCurveType(CurveName, AnimSequence);

	bool bCurveExist = UAnimationBlueprintLibrary::DoesCurveExist(AnimSequence, CurveName, ERawCurveTrackTypes::RCT_Float);

	if (bCurveExist) {

		UAnimationBlueprintLibrary::RemoveCurve(AnimSequence, CurveName, false);
	}

	UAnimationBlueprintLibrary::RemoveCurve(AnimSequence, CurveName, false);
}

void UAnimationModifier_DistanceCurve::OnApply_Implementation(UAnimSequence* AnimSequence)
{
	FName CurveName;
	ECurveType CurveType = GetCurveType(CurveName, AnimSequence);

	if ((CurveType == ECurveType::Start) || (CurveType == ECurveType::Stop)) {

		CurveName = DistanceCurveName;
	}
	else if (CurveType == ECurveType::Rotation) {

		CurveName = RotationCurveName;
	}

	bool bCurveExist = UAnimationBlueprintLibrary::DoesCurveExist(AnimSequence, CurveName, ERawCurveTrackTypes::RCT_Float);
	
	if (bCurveExist) {

		UAnimationBlueprintLibrary::RemoveCurve(AnimSequence, CurveName, false);
	}

	UAnimationBlueprintLibrary::AddCurve(AnimSequence, CurveName, ERawCurveTrackTypes::RCT_Float);

	int32 NumFrames = AnimSequence->GetNumberOfFrames();

	for (int i = 0; i < NumFrames; i++) {

		float Time;
		UAnimationBlueprintLibrary::GetTimeAtFrame(AnimSequence, i, Time);

		FTransform InitBoneTM;
		UAnimationBlueprintLibrary::GetBonePoseForFrame(AnimSequence, RootName, 0, false, InitBoneTM);
		FVector InitBoneLoc = InitBoneTM.GetLocation();
		FRotator InitBoneRot = InitBoneTM.GetRotation().Rotator();

		FTransform CurrentBoneTM;
		UAnimationBlueprintLibrary::GetBonePoseForFrame(AnimSequence, RootName, i, false, CurrentBoneTM);
		FVector CurrentBoneLoc = CurrentBoneTM.GetLocation();
		FRotator CurrentBoneRot = CurrentBoneTM.GetRotation().Rotator();

		float Value;

		if (CurveType == ECurveType::Start) {

			float Distance = (CurrentBoneLoc - InitBoneLoc).Size();

			Value = Distance;
		}
		else if (CurveType == ECurveType::Stop) {

			FTransform FinalBoneTM;
			UAnimationBlueprintLibrary::GetBonePoseForFrame(AnimSequence, RootName, NumFrames, false, FinalBoneTM);
			FVector FinalBoneLoc = FinalBoneTM.GetLocation();

			float Offset = (FinalBoneLoc - InitBoneLoc).Size();

			float Distance = (CurrentBoneLoc - InitBoneLoc).Size();

			Value = Distance - Offset;
		}
		else if (CurveType == ECurveType::Rotation) {

			FTransform FinalBoneTM;
			UAnimationBlueprintLibrary::GetBonePoseForFrame(AnimSequence, RootName, NumFrames, false, FinalBoneTM);
			FRotator FinalBoneRot = FinalBoneTM.GetRotation().Rotator();

			float Offset = (FinalBoneRot - InitBoneRot).Yaw;

			float Rotation = (CurrentBoneRot - InitBoneRot).Yaw;

			Value = Rotation - Offset;

			Value = Value < 0.0f ? Value : Value * (-1);

		//	UE_LOG(LogTemp, Warning, TEXT("%d: %f, %f, %f"), i, Offset, Rotation, Value);		
		}

		UAnimationBlueprintLibrary::AddFloatCurveKey(AnimSequence, CurveName, Time, Value);
	}

	if (CompressionSettings) {

		AnimSequence->CurveCompressionSettings = CompressionSettings;

		FRequestAnimCompressionParams Params(false);

		AnimSequence->RequestAnimCompression(Params);
	}

	UAnimationBlueprintLibrary::FinalizeBoneAnimation(AnimSequence);
}