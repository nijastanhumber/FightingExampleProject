// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SM_State.h"
#include "FGAtoms.generated.h"


UENUM(Blueprintable, Meta = (Bitflags))
enum class EFGInputDirections : uint8
{
	DownBack, // 1
	Down, // 2
	DownForward, // 3
	Back, // 4
	Neutral, // 5
	Forward, // 6
	UpBack, // 7
	Up, // 8
	UpForward // 9
};

UENUM()
enum class EFGButtonState : uint8
{
	Up,
	JustPressed,
	HeldDown,
	Count UMETA(Hidden)
};

UENUM(Blueprintable, Meta = (Bitflags))
enum class EFGInputButtons : uint8
{
	LeftFace, // x
	TopFace, // y
	Count UMETA(Hidden)
};

/**
*
*/
UCLASS()
class FIGHTGAMEEXAMPLE_API UFGDirectionalInputAtom : public USM_InputAtom
{
	GENERATED_BODY()

public:
	/** Which direction we're describing with this atom. */
	UPROPERTY(EditAnywhere)
		EFGInputDirections InputDirection;
};

/**
*
*/
UCLASS()
class FIGHTGAMEEXAMPLE_API UFGButtonInputAtom : public USM_InputAtom
{
	GENERATED_BODY()

public:
	/** State of the button being described. */
	UPROPERTY(EditAnywhere)
		EFGButtonState ButtonState;
};