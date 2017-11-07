// Fill out your copyright notice in the Description page of Project Settings.

#include "FGInputBranch.h"

USM_State* UFGInputBranch::TryBranch(const UObject *RefObject, const TArray<USM_InputAtom*> &DataSource, int32 DataIndex, int32 &OutDataIndex)
{
	// We deliberately do not call Super. We are 100% replacing the logic.

	OutDataIndex = DataIndex;

	if (RequiredButtons & ForbiddenButtons)
	{
		UE_LOG(LogTemp, Warning, TEXT("Impassable condition: Required button is also forbidden."));
		return nullptr;
	}

	// Make sure we have at least the minimum data here.
	if ((DataIndex + (int32)EFGInputButtons::Count) >= DataSource.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Error: Not enough data"));
		return nullptr;
	}

	// The first piece of data must be a DirectionalInput
	if (UFGDirectionalInputAtom* DirectionalInput = Cast<UFGDirectionalInputAtom>(DataSource[DataIndex]))
	{
		if (!(AcceptableDirections & (1 << (int32)DirectionalInput->InputDirection)))
		{
			return nullptr;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error: No directional input at index %i"), DataIndex);
		return nullptr;
	}
	++OutDataIndex;

	bool bRequiredButtonPressed = false;
	for (int32 i = 0; i < (int32)EFGInputButtons::Count; ++i, ++OutDataIndex)
	{
		if (UFGButtonInputAtom* ButtonAtom = Cast<UFGButtonInputAtom>(DataSource[OutDataIndex]))
		{
			if (RequiredButtons & (1 << i))
			{
				if (ButtonAtom->ButtonState == EFGButtonState::JustPressed)
				{
					bRequiredButtonPressed = true;
					continue;
				}
				else if (ButtonAtom->ButtonState == EFGButtonState::HeldDown)
				{
					continue;
				}
				return nullptr;
			}
			else if (ForbiddenButtons & (1 << i))
			{
				// Any state other than having just pressed the forbidden button is OK, even holding it down.
				if (ButtonAtom->ButtonState != EFGButtonState::JustPressed)
				{
					continue;
				}
				return nullptr;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error: Expected %i button inputs, only found %i."), (int32)EFGInputButtons::Count, i);
			return nullptr;
		}
	}
	// If there were any required buttons, make sure at least one of them was just pressed.
	if (RequiredButtons && !bRequiredButtonPressed)
	{
		return nullptr;
	}

	return DestinationState;
}


