// Written by Berkay Tuna, August 2020

#pragma once

#include "CoreMinimal.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "AnimNode_RotationMatching.generated.h"

USTRUCT(BlueprintInternalUseOnly)
struct DISTANCEMATCHING_API FAnimNode_RotationMatching : public FAnimNode_SkeletalControlBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Skeletal Control")
	FBoneReference Root;
	UPROPERTY(EditAnywhere, Category = "Skeletal Control", meta = (editcondition = "bHeadFollowsCamera"))
	FBoneReference Head;

	UPROPERTY(EditAnywhere, Category = "Skeletal Control", meta = (PinHiddenByDefault))
	bool bHeadFollowsCamera;

	UPROPERTY(EditAnywhere, Category = "Calculations", meta = (PinShownByDefault))
	FRotator ControlRotation;
	UPROPERTY(EditAnywhere, Category = "Calculations", meta = (PinShownByDefault))
	FRotator ControlRotationAtStop;
	UPROPERTY(EditAnywhere, Category = "Calculations", meta = (PinHiddenByDefault))
	bool bResetNode;
	//UPROPERTY(EditAnywhere, Category = "Calculations", meta = (PinHiddenByDefault, DisplayName = "InterpolateToDisable"))
	//bool bDisable;
	UPROPERTY(EditAnywhere, Category = "Calculations", meta = (PinHiddenByDefault))
	bool bEnabled;
	UPROPERTY(EditAnywhere, Category = "Calculations", meta = (PinHiddenByDefault, DisplayName = "Disable Interpolation Alpha"))
	float DisableAlpha;

	bool bCanDoOnce;
	bool PrevbResetNode;

	UPROPERTY(EditAnywhere, Category = "Head Rotation Clamp")
	float MaxHorizontalRotation;
	UPROPERTY(EditAnywhere, Category = "Head Rotation Clamp")
	float MaxVerticalRotation;

	UPROPERTY(EditAnywhere, Category = "Rotation Interpolation")
	float RotationInterpolationAlpha;

	FRotator HeadRot;

	FAnimNode_RotationMatching();

	// FAnimNode_Base
	virtual void GatherDebugData(FNodeDebugData& DebugData) override;

	// FAnimNode_SkeletalControlBase
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransfor) override;
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;			

private:

	float NewRootRotYaw;

	// FAnimNode_SkeletalControlBase
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;
};