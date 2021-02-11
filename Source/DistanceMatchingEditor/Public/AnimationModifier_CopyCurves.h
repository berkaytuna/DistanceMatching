// Written by Berkay Tuna, August 2020

#pragma once

#include "AnimationModifier.h"
#include "AnimationBlueprintLibrary.h"
#include "DistanceMatchingComponent.h"
#include "AnimationModifier_CopyCurves.generated.h"

/*
USTRUCT()
struct FAnimType
{
	GENERATED_BODY()

	FString CurveType;
	FString AnimDir;
};
*/

UCLASS(Blueprintable, Experimental, config = Editor, defaultconfig, DisplayName = "Copy Curves")
class DISTANCEMATCHINGEDITOR_API UAnimationModifier_CopyCurves : public UAnimationModifier
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Animation Sequence")
	FString RootFolderPath;

	UPROPERTY(EditAnywhere, Category = "Animation Sequence")
	FString InPlaceSubString;

	UPROPERTY(EditAnywhere, Category = "Curve")
	FName DistanceCurveName;
	UPROPERTY(EditAnywhere, Category = "Curve")
	FName RotationCurveName;

	UPROPERTY(EditAnywhere, Category = "Compression")
	UAnimCurveCompressionSettings* CompressionSettings;

/*
	UPROPERTY(EditAnywhere, Category = "Animation Type")
	FString ForwardSubString;
	UPROPERTY(EditAnywhere, Category = "Animation Type")
	FString LeftSubString;
	UPROPERTY(EditAnywhere, Category = "Animation Type")
	FString RightSubString;
	UPROPERTY(EditAnywhere, Category = "Animation Type")
	FString BackwardsSubString;

	UPROPERTY(EditAnywhere, Category = "Animation Type")
	FString StartSubString;
	UPROPERTY(EditAnywhere, Category = "Animation Type")
	FString StopSubString;
	UPROPERTY(EditAnywhere, Category = "Animation Type")
	FString RotationSubString;
*/

	UAnimationModifier_CopyCurves();

	virtual void OnApply_Implementation(UAnimSequence* AnimSequence) override;
	virtual void OnRevert_Implementation(UAnimSequence* AnimSequence) override;

/*
private:

	FAnimType GetAnimType(FString AnimName);
*/

};