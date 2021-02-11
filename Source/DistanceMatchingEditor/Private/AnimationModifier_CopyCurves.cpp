// Written by Berkay Tuna, August 2020

#include "AnimationModifier_CopyCurves.h"
#include "DistanceMatching.h"
#include "Animation/AnimSequence.h"
#include "HAL/FileManager.h"
#include "UObject/UObjectGlobals.h"
#include "Animation/AnimCurveCompressionCodec_UniformIndexable.h"

UAnimationModifier_CopyCurves::UAnimationModifier_CopyCurves()
{
	
}

/*
FAnimType UAnimationModifier_CopyCurves::GetAnimType(FString AnimName) 
{	
	bool bStartAnim = AnimName.Contains(StartSubString, ESearchCase::CaseSensitive, ESearchDir::FromStart);
	bool bStopAnim = AnimName.Contains(StopSubString, ESearchCase::CaseSensitive, ESearchDir::FromStart);
	bool bRotationAnim = AnimName.Contains(RotationSubString, ESearchCase::CaseSensitive, ESearchDir::FromStart);

	bool bFwdAnim = AnimName.Contains(ForwardSubString, ESearchCase::CaseSensitive, ESearchDir::FromStart);
	bool bLeftAnim = AnimName.Contains(LeftSubString, ESearchCase::CaseSensitive, ESearchDir::FromStart);
	bool bRightAnim = AnimName.Contains(RightSubString, ESearchCase::CaseSensitive, ESearchDir::FromStart);
	bool bBwdAnim = AnimName.Contains(BackwardsSubString, ESearchCase::CaseSensitive, ESearchDir::FromStart);

	FString CurveType;
	FString AnimDir;

	if (bStartAnim) {

		CurveType = StartSubString;
	}
	else if (bStopAnim) {

		CurveType = StopSubString;
	}
	else if (bStopAnim) {

		CurveType = RotationSubString;
	}

	if (bFwdAnim) {

		AnimDir = ForwardSubString;
	}
	else if (bLeftAnim) {

		AnimDir = LeftSubString;
	}
	else if (bRightAnim) {

		AnimDir = RightSubString;
	}
	else if (bBwdAnim) {

		AnimDir = BackwardsSubString;
	}

	FAnimType AnimType;

	AnimType.CurveType = CurveType;
	AnimType.AnimDir = AnimDir;

	return AnimType;
}
*/

void UAnimationModifier_CopyCurves::OnRevert_Implementation(UAnimSequence* AnimSequence)
{
	bool bDistanceCurveExist = UAnimationBlueprintLibrary::DoesCurveExist(AnimSequence, DistanceCurveName, ERawCurveTrackTypes::RCT_Float);
	bool bRotationCurveExist = UAnimationBlueprintLibrary::DoesCurveExist(AnimSequence, RotationCurveName, ERawCurveTrackTypes::RCT_Float);

	if (bDistanceCurveExist) {

		UAnimationBlueprintLibrary::RemoveCurve(AnimSequence, DistanceCurveName, false);
	}
	else if (bRotationCurveExist) {

		UAnimationBlueprintLibrary::RemoveCurve(AnimSequence, RotationCurveName, false);
	}
}

void UAnimationModifier_CopyCurves::OnApply_Implementation(UAnimSequence* AnimSequence)
{

/*
	FString AnimName = AnimSequence->GetName();
	FAnimType AnimType = GetAnimType(AnimName);
	FAnimType RootAnimType;

	TArray<FString> FileNames;
	IFileManager::Get().FindFiles(FileNames, *RootFolderPath);

	for (int i = 0; i < FileNames.Num(); i++) {

		RootAnimType = GetAnimType(FileNames[i]);

	//	UE_LOG(LogTemp, Warning, TEXT("Text"));

		if ((RootAnimType.CurveType == AnimType.CurveType) && (RootAnimType.AnimDir == AnimType.AnimDir)) {

			break;
		}
	}
*/

	UE_LOG(LogTemp, Warning, TEXT("%s"), *RootFolderPath);

	FString AnimName = AnimSequence->GetName(); UE_LOG(LogTemp, Warning, TEXT("%s"), *AnimName);

	int32 Index = AnimName.Find(InPlaceSubString, ESearchCase::CaseSensitive, ESearchDir::FromStart);
	int32 Count = InPlaceSubString.Len();
	AnimName.RemoveAt(Index, Count, ESearchCase::CaseSensitive); UE_LOG(LogTemp, Warning, TEXT("%s"), *RootFolderPath);

	int32 ContentIndex = RootFolderPath.Find(FString(TEXT("Content")), ESearchCase::CaseSensitive, ESearchDir::FromStart);
	int32 RemoveCount = ContentIndex + 7;
	RootFolderPath.RemoveAt(0, RemoveCount, ESearchCase::CaseSensitive);UE_LOG(LogTemp, Warning, TEXT("%s"), *RootFolderPath);

	FString First = RootFolderPath.Mid(0, 1); UE_LOG(LogTemp, Warning, TEXT("%s"), *First);

	FString Second = FString(TEXT("/")); UE_LOG(LogTemp, Warning, TEXT("%s"), *Second);
	RootFolderPath.ReplaceInline(*First, *Second, ESearchCase::CaseSensitive); UE_LOG(LogTemp, Warning, TEXT("%s"), *RootFolderPath);

	FString Prefix = FString(TEXT("AnimSequence'/Game")); UE_LOG(LogTemp, Warning, TEXT("%s"), *Prefix);

	Prefix += RootFolderPath; UE_LOG(LogTemp, Warning, TEXT("%s"), *Prefix);
	Prefix += Second; UE_LOG(LogTemp, Warning, TEXT("%s"), *Prefix);
	Prefix += AnimName; UE_LOG(LogTemp, Warning, TEXT("%s"), *Prefix);
	Prefix += FString(TEXT(".")); UE_LOG(LogTemp, Warning, TEXT("%s"), *Prefix);
	Prefix += AnimName; UE_LOG(LogTemp, Warning, TEXT("%s"), *Prefix);
	Prefix += FString(TEXT("'"));

	FString RootAssetReference = Prefix;  UE_LOG(LogTemp, Warning, TEXT("%s"), *RootAssetReference);

	UAnimSequence* RootAnimSequence = (UAnimSequence*)StaticLoadObject(UAnimSequence::StaticClass(), NULL, *RootAssetReference);

	FString RootAnimName;

	if (RootAnimSequence) {

		RootAnimName = RootAnimSequence->GetName();
	}

	UE_LOG(LogTemp, Warning, TEXT("%s"), RootAnimSequence ? *RootAnimName : *FString(TEXT("RootAnimSequence is nullptr")));

	bool bDistanceCurveExist = UAnimationBlueprintLibrary::DoesCurveExist(RootAnimSequence, DistanceCurveName, ERawCurveTrackTypes::RCT_Float);
	bool bRotationCurveExist = UAnimationBlueprintLibrary::DoesCurveExist(RootAnimSequence, RotationCurveName, ERawCurveTrackTypes::RCT_Float);

	TArray<float> Times;
	TArray<float> Values;

	if (bDistanceCurveExist) {

		UAnimationBlueprintLibrary::GetFloatKeys(RootAnimSequence, DistanceCurveName, Times, Values);

		UAnimationBlueprintLibrary::DoesCurveExist(AnimSequence, DistanceCurveName, ERawCurveTrackTypes::RCT_Float);
		UAnimationBlueprintLibrary::RemoveCurve(AnimSequence, DistanceCurveName, false);
		UAnimationBlueprintLibrary::AddCurve(AnimSequence, DistanceCurveName, ERawCurveTrackTypes::RCT_Float);
		UAnimationBlueprintLibrary::AddFloatCurveKeys(AnimSequence, DistanceCurveName, Times, Values);
	}
	else if (bRotationCurveExist) {

		UAnimationBlueprintLibrary::GetFloatKeys(RootAnimSequence, RotationCurveName, Times, Values);

		UAnimationBlueprintLibrary::DoesCurveExist(AnimSequence, RotationCurveName, ERawCurveTrackTypes::RCT_Float);
		UAnimationBlueprintLibrary::RemoveCurve(AnimSequence, RotationCurveName, false);
		UAnimationBlueprintLibrary::AddCurve(AnimSequence, RotationCurveName, ERawCurveTrackTypes::RCT_Float);
		UAnimationBlueprintLibrary::AddFloatCurveKeys(AnimSequence, RotationCurveName, Times, Values);
	}
	else {

		UE_LOG(LogTemp, Warning, TEXT("Curve Does Not Exist!"));
	}

	if (CompressionSettings) {

		AnimSequence->CurveCompressionSettings = CompressionSettings;

		FRequestAnimCompressionParams Params(false);

		AnimSequence->RequestAnimCompression(Params);
	}

	UAnimationBlueprintLibrary::FinalizeBoneAnimation(AnimSequence);
}