// Written by Berkay Tuna, September 2020

#include "AnimationModifier_ChangeCurve.h"
#include "DistanceMatching.h"
#include "Animation/AnimSequence.h"
#include "DistanceMatchingComponent.h"
#include "Animation/AnimCurveCompressionSettings.h"

UAnimationModifier_ChangeCurve::UAnimationModifier_ChangeCurve() 
{
	Add = 0.0f;
	Multiply = 1.0f;
	EndTime = 10.0f;
}

void UAnimationModifier_ChangeCurve::OnRevert_Implementation(UAnimSequence* AnimSequence)
{

	bool bCurveExist = UAnimationBlueprintLibrary::DoesCurveExist(AnimSequence, CurveName, ERawCurveTrackTypes::RCT_Float);

	if (bCurveExist) {

		UAnimationBlueprintLibrary::RemoveCurve(AnimSequence, CurveName, false);
	}

	UAnimationBlueprintLibrary::RemoveCurve(AnimSequence, CurveName, false);

}

void UAnimationModifier_ChangeCurve::OnApply_Implementation(UAnimSequence* AnimSequence)
{

	int32 NumFrames = AnimSequence->GetNumberOfFrames();

	TArray<float> Times;
	TArray<float> Values;

	bool bCurveExist = UAnimationBlueprintLibrary::DoesCurveExist(AnimSequence, CurveName, ERawCurveTrackTypes::RCT_Float);

	if (bCurveExist) 
	{
		UAnimationBlueprintLibrary::GetFloatKeys(AnimSequence, CurveName, Times, Values);

		for (int i = 0; i < NumFrames; i++)
		{
			if (Times[i] > BeginTime && Times[i] < EndTime)
			{
				Values[i] += Add;
				Values[i] *= Multiply;
			}	
		}

		UAnimationBlueprintLibrary::RemoveCurve(AnimSequence, CurveName, false);
		UAnimationBlueprintLibrary::AddCurve(AnimSequence, CurveName, ERawCurveTrackTypes::RCT_Float);
		UAnimationBlueprintLibrary::AddFloatCurveKeys(AnimSequence, CurveName, Times, Values);
	}

	if (CompressionSettings) {

		AnimSequence->CurveCompressionSettings = CompressionSettings;

		FRequestAnimCompressionParams Params(false);

		AnimSequence->RequestAnimCompression(Params);
	}

	UAnimationBlueprintLibrary::FinalizeBoneAnimation(AnimSequence);

}