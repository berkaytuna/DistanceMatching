// Written by Berkay Tuna, August 2020

#pragma once

#include "AnimationModifier.h"
#include "AnimationBlueprintLibrary.h"
#include "DistanceMatchingComponent.h"
#include "AnimationModifier_DistanceCurve.generated.h"

UCLASS(Blueprintable, Experimental, config = Editor, defaultconfig, DisplayName = "Distance Curve")
class DISTANCEMATCHINGEDITOR_API UAnimationModifier_DistanceCurve : public UAnimationModifier
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Bone")
	FName RootName;

	UPROPERTY(EditAnywhere, Category = "Animation Type")
	FString StartSubString;
	UPROPERTY(EditAnywhere, Category = "Animation Type")
	FString StopSubString;
	UPROPERTY(EditAnywhere, Category = "Animation Type")
	FString RotationSubString;

	UPROPERTY(EditAnywhere, Category = "Curve")
	FName DistanceCurveName;
	UPROPERTY(EditAnywhere, Category = "Curve")
	FName RotationCurveName;

	UPROPERTY(EditAnywhere, Category = "Compression")
	UAnimCurveCompressionSettings* CompressionSettings;

	UAnimationModifier_DistanceCurve();

	virtual void OnApply_Implementation(UAnimSequence* AnimSequence) override;
	virtual void OnRevert_Implementation(UAnimSequence* AnimSequence) override;

private:

	ECurveType GetCurveType(FName& CurveName, UAnimSequence* AnimSequence);
};