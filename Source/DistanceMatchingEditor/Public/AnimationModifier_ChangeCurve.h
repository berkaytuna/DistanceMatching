// Written by Berkay Tuna, August 2020

#pragma once

#include "AnimationModifier.h"
#include "AnimationBlueprintLibrary.h"
#include "DistanceMatchingComponent.h"
#include "AnimationModifier_ChangeCurve.generated.h"

UCLASS(Blueprintable, Experimental, config = Editor, defaultconfig, DisplayName = "Change Curve")
class DISTANCEMATCHINGEDITOR_API UAnimationModifier_ChangeCurve : public UAnimationModifier
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Curve")
	FName CurveName;

	UPROPERTY(EditAnywhere, Category = "Curve")
	float Add;

	UPROPERTY(EditAnywhere, Category = "Curve")
	float Multiply;

	UPROPERTY(EditAnywhere, Category = "Curve")
	float BeginTime;

	UPROPERTY(EditAnywhere, Category = "Curve")
	float EndTime;

	UPROPERTY(EditAnywhere, Category = "Compression")
	UAnimCurveCompressionSettings* CompressionSettings;

	UAnimationModifier_ChangeCurve();

	virtual void OnApply_Implementation(UAnimSequence* AnimSequence) override;
	virtual void OnRevert_Implementation(UAnimSequence* AnimSequence) override;
};