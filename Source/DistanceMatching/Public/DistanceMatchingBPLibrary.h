// Written by Berkay Tuna, August 2020

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "DistanceMatchingBPLibrary.generated.h"

UENUM(BlueprintType)
enum EFourDirectionEnum
{
	N,
	W,
	E,
	S
};

UENUM(BlueprintType)
enum EDoubleDirectionEnum
{
	NW,
	SW,
	SE,
	NE
};

UCLASS()
class UDistanceMatchingBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, Category = "Math", meta = (ExpandEnumAsExecs = "Branches"))
	static void SwitchOnDirection(float Direction, TEnumAsByte<EFourDirectionEnum>& Branches);

	UFUNCTION(BlueprintCallable, Category = "Math", meta = (ExpandEnumAsExecs = "BranchesDouble"))
	static void SwitchOnDoubleDirection(float Direction, TEnumAsByte<EDoubleDirectionEnum>& BranchesDouble);
};
