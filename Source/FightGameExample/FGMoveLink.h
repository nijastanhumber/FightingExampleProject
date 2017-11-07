// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FGState.h"
#include "FGMoveLink.generated.h"

class AFGDefaultPawn;
class UFGMove;

/**
 * 
 */
UCLASS()
class FIGHTGAMEEXAMPLE_API UFGMoveLink : public UDataAsset
{
	GENERATED_BODY()
	
public:
	FStateMachineResult TryLink(const AFGDefaultPawn *RefObject, const TArray<USM_InputAtom*> &DataSource, int32 DataIndex = 0, int32 RemainingSteps = -1);
	
	UPROPERTY(EditAnywhere)
		UFGMove *Move; // replicate subobject

	/** Set of windows (in seconds) when this link can be used. */
	UPROPERTY(EditAnywhere)
		TArray<FVector2D> CancelWindows;

	/** Clears input buffer when using this link. */
	UPROPERTY(EditAnywhere)
		uint32 bClearInput : 1;

	/** Sequence of inputs used to trigger this move. If not NULL, will override move's own input state machine. */
	UPROPERTY(EditAnywhere)
		UFGState *InputStateMachine;
};
