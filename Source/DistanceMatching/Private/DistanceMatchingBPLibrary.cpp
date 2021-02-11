// Written by Berkay Tuna, August 2020

#include "DistanceMatchingBPLibrary.h"
#include "DistanceMatching.h"

UDistanceMatchingBPLibrary::UDistanceMatchingBPLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UDistanceMatchingBPLibrary::SwitchOnDirection(float Direction, TEnumAsByte<EFourDirectionEnum>& Branches)
{
	if (abs(Direction) <= 45.0f)
	{
		Branches = EFourDirectionEnum::N;
	}
	else if ((Direction < -45.0f) && (Direction > -135.0f))
	{
		Branches = EFourDirectionEnum::W;
	}
	else if ((Direction > 45.0f) && (Direction < 135.0f))
	{
		Branches = EFourDirectionEnum::E;
	}
	else if (abs(Direction) >= 135.0f)
	{
		Branches = EFourDirectionEnum::S;
	}
}

void UDistanceMatchingBPLibrary::SwitchOnDoubleDirection(float Direction, TEnumAsByte<EDoubleDirectionEnum>& BranchesDouble)
{
	if ((Direction < 0.0f) && (Direction >= -90.0f))
	{
		BranchesDouble = EDoubleDirectionEnum::NW;
	}
	else if ((Direction < -90.0f) && (Direction >= -180.0f))
	{
		BranchesDouble = EDoubleDirectionEnum::SW;
	}
	else if ((Direction >= 90.0f) && (Direction <= 180.0f))
	{
		BranchesDouble = EDoubleDirectionEnum::SE;
	}
	else if ((Direction < 90.0f) && (Direction >= 0.0f))
	{
		BranchesDouble = EDoubleDirectionEnum::NE;
	}
}
