// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FGState.h"
#include "Animation/AnimMontage.h"
#include "FGMoveLink.h"
#include "FGMove.generated.h"

class AFGDefaultPawn;

USTRUCT(BlueprintType)
struct FFGMoveLinkToFollow
{
	GENERATED_BODY()

public:
	FFGMoveLinkToFollow()
	{
		Link = nullptr;
	}

	UPROPERTY()
		UFGMoveLink *Link;

	UPROPERTY()
		FStateMachineResult SMR;
};

/**
 * 
 */
UCLASS()
class FIGHTGAMEEXAMPLE_API UFGMove : public UDataAsset
{
	GENERATED_BODY()
	
public:
	virtual bool IsSupportedForNetworking() const override
	{
		return true;
	}

	/** Tries all links, returns the best one to follow and the result of the state machines that got there. */
	FFGMoveLinkToFollow TryLinks(const AFGDefaultPawn *RefObject, const TArray<USM_InputAtom*> &DataSource, int32 DataIndex = 0, int32 RemainingSteps = -1);

	/** Animation to play while doing this move. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FName MoveName;

	/** Animation to play while doing this move. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UAnimMontage *Animation;

	/** Links from this move to others. */
	UPROPERTY(EditAnywhere)
		TArray<UFGMoveLink*> LinkedMoves; // replicate subobjects

	/** Clears input buffer when entering this state. */
	UPROPERTY(EditAnywhere)
		uint32 bClearInputOnEntry : 1;

	/** Clears input buffer when exiting this state. */
	UPROPERTY(EditAnywhere)
		uint32 bClearInputOnExit : 1;	

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;
};
