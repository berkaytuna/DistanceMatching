// Written by Berkay Tuna, August 2020

#include "AnimGraphNode_RotationMatching.h"
#include "DistanceMatchingEditor.h"
#include "AnimNodeEditModes.h"

#define LOCTEXT_NAMESPACE "A3Nodes"

UAnimGraphNode_RotationMatching::UAnimGraphNode_RotationMatching()
{

}

FText UAnimGraphNode_RotationMatching::GetTooltipText() const
{
    return LOCTEXT("RotationMatching", "RotationMatching");
}

FText UAnimGraphNode_RotationMatching::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return LOCTEXT("RotationMatching", "RotationMatching");
}

FString UAnimGraphNode_RotationMatching::GetNodeCategory() const
{
    return TEXT("RotationMatching");
}

#undef LOCTEXT_NAMESPACE