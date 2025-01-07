// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimExporterFunctionLibraryEditor.h"
#include "AnimationEditorUtils.h"
#include "AnimExportSet.h"
#include "DesktopPlatformModule.h"
#include "LevelEditorSequencerIntegration.h"
#include "LevelSequence.h"
#include "LevelSequenceEditorBlueprintLibrary.h"
#include "MovieSceneBindingProxy.h"
#include "PackageTools.h"
#include "Animation/SkeletalMeshActor.h"
#include "Exporters/AnimSeqExportOption.h"
#include "MVVM/ViewModels/TrackModel.h"
#include "MVVM/ViewModels/TrackRowModel.h"
#include "MVVM/ViewModels/SequenceModel.h"
#include "MVVM/ViewModels/SpawnableModel.h"
#include "MVVM/ViewModels/SequencerEditorViewModel.h"
#include "Sequencer/MovieSceneControlRigParameterTrack.h"
#include "SequencerTools.h"


TArray<FString> UAnimExporterFunctionLibraryEditor::GetSkeletalMeshActors(const UMovieSceneSequence* LevelSequence)
{
	TArray<FString> Actors;

	UMovieScene* MovieScene = nullptr;
	if (!ValidateLevelSequence(LevelSequence, MovieScene))
	{
		return Actors;
	}

	TArray<FMovieSceneBinding> Bindings = MovieScene->GetBindings();
	for (const FMovieSceneBinding Binding : Bindings)
	{
		if (FMovieSceneSpawnable* Spawnable = MovieScene->FindSpawnable(Binding.GetObjectGuid()))
		{
			if (Spawnable->GetObjectTemplate() && Spawnable->GetObjectTemplate()->IsA<ASkeletalMeshActor>())
			{
				Actors.Add(Spawnable->GetName());
			}
		}
	}

	return Actors;
}

TArray<FString> UAnimExporterFunctionLibraryEditor::GetAdditiveControlRigLayers(const UMovieSceneSequence* LevelSequence, FAnimExportNode ExportNode)
{
	TArray<FString> Layers;

	// Validate input
	UMovieScene* MovieScene = nullptr;
	if (!ValidateLevelSequence(LevelSequence, MovieScene))
	{
		return Layers;
	}

	TArray<FMovieSceneBinding> Bindings = MovieScene->GetBindings();
	for (const FMovieSceneBinding Binding : Bindings)
	{
		FMovieSceneSpawnable* Spawnable = MovieScene->FindSpawnable(Binding.GetObjectGuid());
		if (!Spawnable || ExportNode.Actor != Spawnable->GetName())
		{
			continue;
		}

		for (const UMovieSceneTrack* Track : Binding.GetTracks())
		{
			if (Track->IsA<UMovieSceneControlRigParameterTrack>())
			{
				bool bIsLayeredControlRig = IsLayeredControlRigTrack(Track);
				int32 MaxRowIndex = Track->GetMaxRowIndex();

				// add default layered control rig track
				if(MaxRowIndex ==0 && bIsLayeredControlRig)
				{
					Layers.Add(Track->GetDisplayName().ToString());
					continue;
				}

				for (const UMovieSceneSection* Section : Track->GetAllSections())
				{
					if (Cast<UMovieSceneControlRigParameterSection>(Section) && Section->GetBlendType() == EMovieSceneBlendType::Additive || bIsLayeredControlRig)
					{
						FString RowDisplayName = Track->GetTrackRowDisplayName(Section->GetRowIndex()).ToString();
						Layers.Add(RowDisplayName);
					}
				}
			}
		}
	}

	return Layers;
}

void UAnimExporterFunctionLibraryEditor::MuteControlRigLayers(FAnimExportNode ExportNode)
{
	using namespace UE::Sequencer;
	{
		// get sequencer
		FLevelEditorSequencerIntegration& SequencerIntegration = FLevelEditorSequencerIntegration::Get();
		const TArray<TWeakPtr<ISequencer>> Sequencers = SequencerIntegration.GetSequencers();
		if (Sequencers.IsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("LevelSequence is null."));
			return;
		}
		TSharedPtr<ISequencer> PinnedSequencer = Sequencers[0].Pin();

		// view model
		TSharedPtr<FSequencerEditorViewModel> ViewModel = PinnedSequencer->GetViewModel();
		TViewModelPtr<FSequenceModel> RootModel = ViewModel->GetRootSequenceModel();
		UMovieScene* MovieScene = RootModel->GetMovieScene();
		if (!MovieScene)
		{
			UE_LOG(LogTemp, Warning, TEXT("MovieScene is null for the provided LevelSequence."));
			return;
		}

		// iterate for children models for skeletal mesh actor bindings
		for (const TViewModelPtr<FObjectBindingModel>& BindingModel : RootModel->GetDescendantsOfType<FObjectBindingModel>())
		{
			if (BindingModel.IsValid())
			{
				// filter by passed in actor
				FGuid ObjectGuid = BindingModel->GetObjectGuid();
				FMovieSceneSpawnable* Actor = MovieScene->FindSpawnable(ObjectGuid);
				if(!Actor)
				{
					UE_LOG(LogTemp, Warning, TEXT("Can't find actor with binding null."));
					continue;
				}

				if (ExportNode.Actor != Actor->GetName())
				{
					continue;
				}

				for (const TViewModelPtr<FTrackModel>& TrackModel : BindingModel->GetDescendantsOfType<FTrackModel>())
				{
					if (TrackModel.IsValid())
					{
						UMovieSceneTrack* Track = TrackModel->GetTrack();

						// filter by control rig track
						if (!Track->IsA<UMovieSceneControlRigParameterTrack>())
						{
							continue;
						}

						// if the track is layered control rig track and has only one track, don't process for the rest
						bool bIsLayeredControlRig = IsLayeredControlRigTrack(Track);
						int32 MaxRowIndex = Track->GetMaxRowIndex();
						if(bIsLayeredControlRig && MaxRowIndex == 0)
						{
							bool bContainsThisSection = ExportNode.Layers.Contains(Track->GetDisplayName().ToString());
							TrackModel->SetIsMuted(!bContainsThisSection);
							Track->SetEvalDisabled(!bContainsThisSection);
							Track->Modify();
							continue;
						}

						for (const TViewModelPtr<FTrackRowModel>& TrackRowModel : TrackModel->GetDescendantsOfType<FTrackRowModel>())
						{
							if (TrackRowModel.IsValid())
							{
								TArray<UMovieSceneSection*> Sections = TrackRowModel->GetSections();
								if(Sections.IsEmpty())
								{
									continue;
								}

								// filter out by blend type, we only want to affect additive control rig
								if(Sections[0]->GetBlendType() == EMovieSceneBlendType::Absolute && !bIsLayeredControlRig)
								{
									continue;
								}

								int32 RowIndex = TrackRowModel->GetRowIndex();
								FString RowDisplayName = Track->GetTrackRowDisplayName(RowIndex).ToString();
								bool bContainsThisSection = ExportNode.Layers.Contains(RowDisplayName);
								TrackRowModel->SetIsMuted(!bContainsThisSection);
								UMovieSceneTrack* TrackRow = TrackRowModel->GetTrack();
								TrackRow->SetRowEvalDisabled(!bContainsThisSection, RowIndex);
								TrackRow->Modify();
							}
						}
					}
				}
			}
		}
	}
}

TArray<UAnimSequence*> UAnimExporterFunctionLibraryEditor::ExportAnimationSet(UAnimExportSet* AnimExportSet)
{
	TArray<UAnimSequence*> ReturnArray;
	// get movie scene
	ULevelSequence* LevelSequence = ULevelSequenceEditorBlueprintLibrary::GetCurrentLevelSequence();
	UMovieScene* MovieScene = nullptr;
	if (!ValidateLevelSequence(LevelSequence, MovieScene))
	{
		return ReturnArray;
	}

	// cache out initial mute nodes so that we can set it back its original state after
	TArray<FString> InitialMuteNodes = MovieScene->GetMuteNodes();
	TRange<FFrameNumber> InitialPlaybackRange = MovieScene->GetPlaybackRange();
	USkeleton* LoadedSkeleton = AnimExportSet->Skeleton.LoadSynchronous();

	int32 TotalSteps = AnimExportSet->ExportNodes.Num();
	FScopedSlowTask SlowTask(TotalSteps, FText::FromString(TEXT("Exporting Animations...")));
	SlowTask.MakeDialog();

	for(const FAnimExportNode ExportNode : AnimExportSet->ExportNodes)
	{
		if(!ExportNode.bEnable)
		{
			continue;
		}

		FString AssetPath = FString::Printf(TEXT("%s/%s"), *AnimExportSet->ExportDirectory, *ExportNode.Name);
		bool IsValidPackagePath = FPackageName::IsValidLongPackageName(AssetPath);
		if(!IsValidPackagePath)
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid package path: %s"), *AssetPath);
			continue;
		}

		UAnimSequence* AnimSequence = LoadObject<UAnimSequence>(nullptr, *AssetPath);
		if (!AnimSequence)
		{
			AnimSequence = GetOrCreateAnimSequenceAsset(LoadedSkeleton, AssetPath);
		}
		
		ExportAnimationNode(ExportNode, AnimSequence);
		bool IsDirty = AnimSequence->MarkPackageDirty();
		ReturnArray.Add(AnimSequence);
		SlowTask.EnterProgressFrame(1);
		if (SlowTask.ShouldCancel())
		{
			break;
		}
	}

	// set its initial state
	MovieScene->GetMuteNodes() = InitialMuteNodes;
	MovieScene->SetPlaybackRange(InitialPlaybackRange);

	// browse to created anim sequence assets
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	TArray<UObject*> ObjectArray;

	for (UAnimSequence* AnimSequence : ReturnArray)
	{
		if (AnimSequence)
		{
			ObjectArray.Add(AnimSequence);
		}
	}
	ContentBrowserModule.Get().SyncBrowserToAssets(ObjectArray);

	return ReturnArray;
}

void UAnimExporterFunctionLibraryEditor::ExportAnimationNode(FAnimExportNode AnimExportNode, UAnimSequence* AnimSequence)
{
	ULevelSequence* LevelSequence = ULevelSequenceEditorBlueprintLibrary::GetCurrentLevelSequence();
	UMovieScene* MovieScene = nullptr;
	if(!ValidateLevelSequence(LevelSequence, MovieScene))
	{
		return;
	}

	UWorld* World = GEditor->GetEditorWorldContext().World();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't get editor world."));
		return;
	}

	// update mute state
	MuteControlRigLayers(AnimExportNode);

	// create export option
	UAnimSeqExportOption* ExportOptions = NewObject<UAnimSeqExportOption>(GetTransientPackage(), NAME_None);
	ensure(ExportOptions);

	ExportOptions->bExportTransforms = true;
	ExportOptions->bExportMorphTargets = true;
	ExportOptions->WarmUpFrames = 5;
	FFrameRate DisplayRate = MovieScene->GetDisplayRate();
	TRange<FFrameNumber> NewRange = MovieScene->GetPlaybackRange();
	NewRange.SetLowerBoundValue(ConvertFrameTime(AnimExportNode.StartFrame, DisplayRate, MovieScene->GetTickResolution()).FrameNumber);
	NewRange.SetUpperBoundValue(ConvertFrameTime(AnimExportNode.EndFrame, DisplayRate, MovieScene->GetTickResolution()).FrameNumber);
	MovieScene->SetPlaybackRange(NewRange);

	// get binding from actor name
	FMovieSceneBindingProxy Binding = FindBindingByName(LevelSequence, AnimExportNode.Actor);

	// export anim sequence using sequencer tool library
	USequencerToolsFunctionLibrary::ExportAnimSequence(World, LevelSequence, AnimSequence, ExportOptions, Binding, false);
}

UAnimSequence* UAnimExporterFunctionLibraryEditor::GetOrCreateAnimSequenceAsset(USkeleton* Skeleton, const FString& AssetPath)
{
	if (!Skeleton)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Skeleton!"));
		return nullptr;
	}

	FString PackagePath = UPackageTools::SanitizePackageName(AssetPath);
	UPackage* Package = CreatePackage(*PackagePath);

	UAnimSequence* AnimSequence = nullptr;

	if(UAnimSequence* ExistingSequence = LoadObject<UAnimSequence>(nullptr, *PackagePath))
	{
		AnimSequence = ExistingSequence;
		AnimSequence->ResetAnimation();
	}else
	{
		AnimSequence = AnimationEditorUtils::CreateAnimationAsset<UAnimSequence>(Skeleton, AssetPath, "");
	}

	if (!AnimSequence)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create AnimSequence: %s"), *AssetPath);
		return nullptr;
	}

	AnimSequence->GetController().InitializeModel();
	bool IsDirty = AnimSequence->MarkPackageDirty();
	return AnimSequence;
}

FMovieSceneBindingProxy UAnimExporterFunctionLibraryEditor::FindBindingByName(ULevelSequence* LevelSequence, FString Name)
{
	UMovieScene* MovieScene = LevelSequence->GetMovieScene();
	if (!ensure(MovieScene))
	{
		return FMovieSceneBindingProxy();
	}

	for (FMovieSceneBinding Binding : MovieScene->GetBindings())
	{
		if (FMovieSceneSpawnable* Spawnable = MovieScene->FindSpawnable(Binding.GetObjectGuid()))
		{
			if (Spawnable->GetObjectTemplate() && Spawnable->GetObjectTemplate()->IsA<ASkeletalMeshActor>())
			{
				if (Spawnable->GetName() == Name)
				{
					return FMovieSceneBindingProxy(Binding.GetObjectGuid(), LevelSequence);
				}
			}
		}
	}

	return FMovieSceneBindingProxy();
}

FString UAnimExporterFunctionLibraryEditor::OpenDirectoryDialogue()
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get(); // Get the desktop platform interface
	FString SelectedPath = FString(); // To store the selected path

	if (DesktopPlatform)
	{
		const void* ParentWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr); // Parent window for dialog
		DesktopPlatform->OpenDirectoryDialog(ParentWindowHandle, TEXT("Select Directory"), TEXT(""), SelectedPath);
	}

	return SelectedPath;
}

bool UAnimExporterFunctionLibraryEditor::IsLayeredControlRigTrack(const UMovieSceneTrack* Track)
{
	// currently FControlRigParameterTrackEditor is not exposed to API, rely on naming convention to check if it's layered control rig track or not
	// TODO: use bIsLayeredControlRig property from FControlRigParameterTrackEditor or other proper API method when available in the future
	return Track->GetDisplayName().ToString().Contains("Layered");
}

bool UAnimExporterFunctionLibraryEditor::ValidateLevelSequence(const UMovieSceneSequence* LevelSequence,
	UMovieScene*& OutMovieScene)
{
	if (!LevelSequence)
	{
		UE_LOG(LogTemp, Warning, TEXT("LevelSequence is null."));
		return false;
	}

	OutMovieScene = LevelSequence->GetMovieScene();
	if (!OutMovieScene)
	{
		UE_LOG(LogTemp, Warning, TEXT("MovieScene is null for the provided LevelSequence."));
		return false;
	}

	return true;
}
