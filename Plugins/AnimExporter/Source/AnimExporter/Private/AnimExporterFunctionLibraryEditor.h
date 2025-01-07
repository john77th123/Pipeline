// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelSequence.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AnimExporterFunctionLibraryEditor.generated.h"

class UMovieSceneSequence;
class UMovieSceneTrack;
class UMovieScene;
struct FMovieSceneBindingProxy;
struct FAnimExportNode;
class UAnimExportSet;


/**
 * 
 */
UCLASS()
class ANIMEXPORTER_API UAnimExporterFunctionLibraryEditor : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="Animation Exporter")
	static TArray<FString> GetSkeletalMeshActors(const UMovieSceneSequence* LevelSequence);

	UFUNCTION(BlueprintPure, Category = "Animation Exporter")
	static TArray<FString> GetAdditiveControlRigLayers(const UMovieSceneSequence* LevelSequence, FAnimExportNode ExportNode);

	UFUNCTION(BlueprintCallable, Category = "Animation Exporter")
	static void MuteControlRigLayers(FAnimExportNode ExportNode);

	UFUNCTION(BlueprintCallable, Category = "Animation Exporter")
	static TArray<UAnimSequence*> ExportAnimationSet(UAnimExportSet* AnimExportSet);

	UFUNCTION(BlueprintCallable, Category = "Animation Exporter")
	static void ExportAnimationNode(FAnimExportNode AnimExportNode, UAnimSequence* AnimSequence);

	UFUNCTION(BlueprintCallable, Category = "Animation Exporter")
	static UAnimSequence* GetOrCreateAnimSequenceAsset(USkeleton* Skeleton, const FString&  AssetPath);

	UFUNCTION(BlueprintCallable, Category = "Animation Exporter")
	static FMovieSceneBindingProxy FindBindingByName(ULevelSequence* LevelSequence, FString Name);

	UFUNCTION(BlueprintCallable, Category = "Animation Exporter")
	static FString OpenDirectoryDialogue();

private:
	static bool IsLayeredControlRigTrack(const UMovieSceneTrack* Track);

	static bool ValidateLevelSequence(const UMovieSceneSequence* LevelSequence, UMovieScene*& OutMovieScene);
};
