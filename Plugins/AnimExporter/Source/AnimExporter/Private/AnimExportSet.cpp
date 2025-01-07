// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimExportSet.h"

bool FAnimExportNode::IsValid() const
{
	return !Name.IsEmpty() && !Actor.IsEmpty() && StartFrame <= EndFrame;
}

TArray<FAnimExportNode> UAnimExportSet::GetEnabledExportNodes() const
{
    TArray<FAnimExportNode> EnabledNodes;
    for (const FAnimExportNode& Node : ExportNodes)
    {
        if (Node.bEnable)
        {
            EnabledNodes.Add(Node);
        }
    }
    return EnabledNodes;
}

bool UAnimExportSet::ValidateExportNodes() const
{
    for (const FAnimExportNode& Node : ExportNodes)
    {
        if (!Node.IsValid())
        {
            UE_LOG(LogTemp, Warning, TEXT("Invalid Export Node: %s"), *Node.Name);
            return false;
        }
    }
    return true;
}

FString UAnimExportSet::SanitizeExportDirectory() const
{
    return FPaths::ConvertRelativePathToFull(ExportDirectory);
}