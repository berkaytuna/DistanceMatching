// Written by Berkay Tuna, August 2020

#include "AnimNode_RotationMatching.h"
#include "Animation/AnimInstanceProxy.h"
#include "AnimationRuntime.h"
//#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"

FAnimNode_RotationMatching::FAnimNode_RotationMatching() :
	RotationInterpolationAlpha(0.04f),
	MaxHorizontalRotation(120.0f),
	DisableAlpha(0.1f),
	bEnabled(true)
{
}

void FAnimNode_RotationMatching::GatherDebugData(FNodeDebugData& DebugData)
{
	FString DebugLine = DebugData.GetNodeName(this);
	DebugData.AddDebugItem(DebugLine);
	ComponentPose.GatherDebugData(DebugData);
}

bool FAnimNode_RotationMatching::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
	if (!Root.IsValidToEvaluate(RequiredBones) || (bHeadFollowsCamera && !Head.IsValidToEvaluate(RequiredBones))) {

		return false;
	}
	else {

		return true;
	}
}

void FAnimNode_RotationMatching::InitializeBoneReferences(const FBoneContainer& RequiredBones)
{
	Root.Initialize(RequiredBones);

	if (bHeadFollowsCamera) {
		Head.Initialize(RequiredBones);
	}
}

void FAnimNode_RotationMatching::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("%f"), NewRootRotYaw));
	//UE_LOG(LogTemp, Warning, TEXT("%f"), NewRootRotYaw);

	if (!(!bEnabled && abs(NewRootRotYaw) < 1.0f))
	{

		// Note: if i directly get the Output as FComponentSpacePoseContext without prior conversion to FPoseContext
		// (i.e. from another Skeletal Control Node), i lose the functionality of this node. Just Evaluate Output 
		// won`t work. That is why i am converting to Local Pose and then again back to Component Space Pose.
		// Notice that in this node i just used Output and not OutBoneTransforms when manupulating Bones.

		ComponentPose.EvaluateComponentSpace(Output);

		FPoseContext LocalOutput(Output.AnimInstanceProxy);
		FCSPose<FCompactPose>::ConvertComponentPosesToLocalPoses(MoveTemp(Output.Pose), LocalOutput.Pose);
		LocalOutput.Curve = MoveTemp(Output.Curve);

		Output = LocalOutput.AnimInstanceProxy;
		Output.Pose.InitPose(MoveTemp(LocalOutput.Pose));
		Output.Curve = MoveTemp(LocalOutput.Curve);



		float NewHeadRotYaw;

		float ControlRotYawDiff = ControlRotation.Yaw - ControlRotationAtStop.Yaw;

		if (ControlRotYawDiff > 180.0f) {

			NewHeadRotYaw = ControlRotYawDiff - 360.0f;
			HeadRot.Yaw = (NewHeadRotYaw - HeadRot.Yaw) * RotationInterpolationAlpha + HeadRot.Yaw;
		}
		else {

			if (ControlRotYawDiff < -180.0f) {

				NewHeadRotYaw = ControlRotYawDiff + 360.0f;
				HeadRot.Yaw = (NewHeadRotYaw - HeadRot.Yaw) * RotationInterpolationAlpha + HeadRot.Yaw;
			}
			else {

				NewHeadRotYaw = ControlRotYawDiff;
				HeadRot.Yaw = (NewHeadRotYaw - HeadRot.Yaw) * RotationInterpolationAlpha + HeadRot.Yaw;
			}
		}

		// for example from Stop to Idle, if i change my head Rotation in the Stop State, it will try to interpolate
		// from 0, when i get to the node in the Idle State. So for the first time the Node begins functioning, the head 
		// rotation should begin at where it was in the previous State.

		if (bResetNode && !PrevbResetNode) {

			bCanDoOnce = true;
		}

		PrevbResetNode = bResetNode;

		if (bCanDoOnce) {

			HeadRot.Yaw = NewHeadRotYaw;

			bCanDoOnce = false;
		}

		FRotator HeadRotClamped(FRotator::ZeroRotator);

		HeadRotClamped.Roll = (-1) * ControlRotation.Pitch;

		float MaxHorizontalRotEach = MaxHorizontalRotation / 2;
		HeadRotClamped.Yaw = HeadRot.Yaw < -MaxHorizontalRotEach ? -MaxHorizontalRotEach : HeadRot.Yaw < MaxHorizontalRotEach ? HeadRot.Yaw : MaxHorizontalRotEach;
		HeadRotClamped.Yaw = (-1) * HeadRotClamped.Yaw;

		if (bEnabled)
		{
			NewRootRotYaw = ControlRotationAtStop.Yaw + (-1) * ControlRotation.Yaw;
		}
		else
		{
			NewRootRotYaw = (0.0f - NewRootRotYaw) * DisableAlpha + NewRootRotYaw;
		}

		float Offset;

		if (NewRootRotYaw < -180.0f)
		{
			Offset = 360.0f;
		}
		else if (NewRootRotYaw > 180.0f)
		{
			Offset = -360.0f;
		}
		else
		{
			Offset = 0.0f;
		}

		NewRootRotYaw += Offset;

		/*
			GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Red, FString::Printf(TEXT("%f, %f, %f"),
				ControlRotationAtStop.Yaw,
				ControlRotation.Yaw,
				NewRootRotYaw));
		*/

		FRotator NewRootRot = FRotator(0.0f, NewRootRotYaw, 0.0f);
		FRotator NewHeadRot = FRotator(0.0f, HeadRotClamped.Roll, HeadRotClamped.Yaw);

		const FBoneContainer& BoneContainer = Output.AnimInstanceProxy->GetRequiredBones();

		FCompactPoseBoneIndex RootIndex = Root.GetCompactPoseIndex(BoneContainer);
		FTransform NewRootTM = Output.Pose.GetComponentSpaceTransform(RootIndex);

		FTransform ComponentTransform = Output.AnimInstanceProxy->GetComponentTransform();

		FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTransform, Output.Pose, NewRootTM, RootIndex, BCS_ComponentSpace);

		NewRootTM.GetRotation().Rotator();

		const FQuat NewRootQuat(NewRootRot);
		NewRootTM.SetRotation(NewRootQuat * NewRootTM.GetRotation());

		FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTransform, Output.Pose, NewRootTM, RootIndex, BCS_ComponentSpace);

		Output.Pose.SetComponentSpaceTransform(RootIndex, NewRootTM);

		if (bHeadFollowsCamera) {

			FCompactPoseBoneIndex HeadIndex = Head.GetCompactPoseIndex(BoneContainer);
			FTransform NewHeadTM = Output.Pose.GetComponentSpaceTransform(HeadIndex);

			FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTransform, Output.Pose, NewHeadTM, HeadIndex, BCS_BoneSpace);

			const FQuat NewHeadQuat(NewHeadRot);
			NewHeadTM.SetRotation(NewHeadQuat * NewHeadTM.GetRotation());

			FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTransform, Output.Pose, NewHeadTM, HeadIndex, BCS_BoneSpace);

			Output.Pose.SetComponentSpaceTransform(HeadIndex, NewHeadTM);
		}
	}
}