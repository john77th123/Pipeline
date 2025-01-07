// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimExportSet.generated.h"

class ULevelSequence;
class USkeleton;
class UAnimSequence;


USTRUCT(BlueprintType)
struct FAnimExportNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation|Export")
	bool bEnable = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Export")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Export")
	FString Actor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Export")
	TArray<FString> Layers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Export")
	int StartFrame = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Export")
	int EndFrame = 100;

	bool IsValid() const;
};


UCLASS(BlueprintType)
class UAnimExportSet : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category= "Animation|Export")
	TArray<FAnimExportNode> ExportNodes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Export")
	TSoftObjectPtr<USkeleton> Skeleton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Export")
	TSoftObjectPtr<ULevelSequence> LevelSequence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Export")
	FString ExportDirectory;

	UFUNCTION(BlueprintCallable, Category = "Animation Export")
	TArray<FAnimExportNode> GetEnabledExportNodes() const;

	UFUNCTION(BlueprintCallable, Category = "Animation|Export")
	bool ValidateExportNodes() const;

	FString SanitizeExportDirectory() const;
};


