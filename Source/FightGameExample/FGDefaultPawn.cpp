// Fill out your copyright notice in the Description page of Project Settings.

#include "FGDefaultPawn.h"
#include "FGAtoms.h"
#include "FGMove.h"
#include "FGMoveLink.h"
#include "Kismet/KismetSystemLibrary.h"
#include "FightGameExampleGameModeBase.h"
#include "FGCameraActor.h"
#include "Engine/ActorChannel.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

AFGDefaultPawn::AFGDefaultPawn()
{
	// Needed because we're using DefaultPawn.
	bAddDefaultMovementBindings = false;
	// This needs to be tweaked for your game's feel, but it works.
	InputExpirationTime = 0.75f;

	//Directional Button Atoms
	static ConstructorHelpers::FObjectFinder<UFGDirectionalInputAtom> test(TEXT("FGDirectionalInputAtom'/Game/DirectionAtoms/1_DownBack.1_DownBack'"));
	DirectionDownBackAtom = test.Object;

	static ConstructorHelpers::FObjectFinder<UFGDirectionalInputAtom> test2(TEXT("FGDirectionalInputAtom'/Game/DirectionAtoms/2_Down.2_Down'"));
	DirectionDownAtom = test2.Object;

	static ConstructorHelpers::FObjectFinder<UFGDirectionalInputAtom> test3(TEXT("FGDirectionalInputAtom'/Game/DirectionAtoms/3_DownForward.3_DownForward'"));
	DirectionDownForwardAtom = test3.Object;

	static ConstructorHelpers::FObjectFinder<UFGDirectionalInputAtom> test4(TEXT("FGDirectionalInputAtom'/Game/DirectionAtoms/4_Back.4_Back'"));
	DirectionBackAtom = test4.Object;

	static ConstructorHelpers::FObjectFinder<UFGDirectionalInputAtom> test5(TEXT("FGDirectionalInputAtom'/Game/DirectionAtoms/5_Neutral.5_Neutral'"));
	DirectionNeutralAtom = test5.Object;

	static ConstructorHelpers::FObjectFinder<UFGDirectionalInputAtom> test6(TEXT("FGDirectionalInputAtom'/Game/DirectionAtoms/6_Forward.6_Forward'"));
	DirectionForwardAtom = test6.Object;

	static ConstructorHelpers::FObjectFinder<UFGDirectionalInputAtom> test7(TEXT("FGDirectionalInputAtom'/Game/DirectionAtoms/7_UpBack.7_UpBack'"));
	DirectionUpBackAtom = test7.Object;

	static ConstructorHelpers::FObjectFinder<UFGDirectionalInputAtom> test8(TEXT("FGDirectionalInputAtom'/Game/DirectionAtoms/8_Up.8_Up'"));
	DirectionUpAtom = test8.Object;

	static ConstructorHelpers::FObjectFinder<UFGDirectionalInputAtom> test9(TEXT("FGDirectionalInputAtom'/Game/DirectionAtoms/9_UpForward.9_UpForward'"));
	DirectionUpForwardAtom = test9.Object;

	//Button Atoms
	static ConstructorHelpers::FObjectFinder<UFGButtonInputAtom> test10(TEXT("FGButtonInputAtom'/Game/ButtonAtoms/HeldDown.HeldDown'"));
	ButtonAtoms.Add(test10.Object);

	static ConstructorHelpers::FObjectFinder<UFGButtonInputAtom> test11(TEXT("FGButtonInputAtom'/Game/ButtonAtoms/JustPressed.JustPressed'"));
	ButtonAtoms.Add(test11.Object);

	static ConstructorHelpers::FObjectFinder<UFGButtonInputAtom> test12(TEXT("FGButtonInputAtom'/Game/ButtonAtoms/Up.Up'"));
	ButtonAtoms.Add(test12.Object);

	// Start current move at idle
	static ConstructorHelpers::FObjectFinder<UFGMove> test13(TEXT("FGMove'/Game/Moves/Idle.Idle'"));
	CurrentMove = test13.Object;

	bReplicates = true;
	bAlwaysRelevant = true;
}

void AFGDefaultPawn::BeginPlay()
{
	Super::BeginPlay();

	if (UStaticMeshComponent* SMC = GetMeshComponent())
	{
		SMC->SetHiddenInGame(true);
	}

	if (!CurrentMove)
	{
		UE_LOG(LogTemp, Warning, TEXT("No initial move."));
		Destroy();
		return;
	}
	// Check that we have all the states we need.
	if (!DirectionDownBackAtom || !DirectionDownAtom || !DirectionDownForwardAtom
		|| !DirectionBackAtom || !DirectionNeutralAtom || !DirectionForwardAtom
		|| !DirectionUpBackAtom || !DirectionUpAtom || !DirectionUpForwardAtom)
	{
		UE_LOG(LogTemp, Warning, TEXT("Missing at least one directional atom."));
		Destroy();
		return;
	}
	int32 ButtonInputCount = (int32)EFGButtonState::Count;
	for (int32 i = 0; i < ButtonInputCount; ++i)
	{
		if (!ButtonAtoms.IsValidIndex(i) || !ButtonAtoms[i])
		{
			UE_LOG(LogTemp, Warning, TEXT("Not enough button input atoms, or a NULL entry was found in the list."));
			Destroy();
			return;
		}
	}

//	GetWorldTimerManager().SetTimerForNextTick(this, &AFGDefaultPawn::UseGameCamera);
}

void AFGDefaultPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Process input
	// Add one atom for stick direction
	const float DirectionThreshold = 0.5f;
	UFGDirectionalInputAtom* InputDirection = nullptr;
	if (DirectionInput.X < -DirectionThreshold)
	{
		if (DirectionInput.Y < -DirectionThreshold)
		{
			InputDirection = DirectionDownBackAtom;
		}
		else if (DirectionInput.Y < DirectionThreshold)
		{
			InputDirection = DirectionBackAtom;
		}
		else
		{
			InputDirection = DirectionUpBackAtom;
		}
	}
	else if (DirectionInput.X < DirectionThreshold)
	{
		if (DirectionInput.Y < -DirectionThreshold)
		{
			InputDirection = DirectionDownAtom;
		}
		else if (DirectionInput.Y < DirectionThreshold)
		{
			InputDirection = DirectionNeutralAtom;
		}
		else
		{
			InputDirection = DirectionUpAtom;
		}
	}
	else
	{
		if (DirectionInput.Y < -DirectionThreshold)
		{
			InputDirection = DirectionDownForwardAtom;
		}
		else if (DirectionInput.Y < DirectionThreshold)
		{
			InputDirection = DirectionForwardAtom;
		}
		else
		{
			InputDirection = DirectionUpForwardAtom;
		}
	}
	InputStream.Add(InputDirection);

	// Add one atom for each button's state.
	for (int32 i = 0; i < (int32)EFGInputButtons::Count; ++i)
	{
		if (ButtonsDown & (1 << i))
		{
			if (ButtonsDown_Old & (1 << i))
			{
				InputStream.Add(ButtonAtoms[(int32)EFGButtonState::HeldDown]);
			}
			else
			{
				InputStream.Add(ButtonAtoms[(int32)EFGButtonState::JustPressed]);
			}
		}
		else
		{
			InputStream.Add(ButtonAtoms[(int32)EFGButtonState::Up]);
		}
	}
	// Cache old button state so we can distinguish between held and just pressed.
	ButtonsDown_Old = ButtonsDown;

	// Always add an input time stamp to match the input sequence.
	float CurrentTime = UKismetSystemLibrary::GetGameTimeInSeconds(this);
	InputTimeStamps.Add(CurrentTime);

	// Prune old inputs. This would be better-suited to a ringbuffer than an array, but it's not much data.
	for (int32 i = 0; i < InputStream.Num(); ++i)
	{
		if ((InputTimeStamps[i] + InputExpirationTime) >= CurrentTime)
		{
			// Remove everything before this, then exit the loop.
			if (i > 0)
			{
				InputTimeStamps.RemoveAt(0, i, false);
				InputStream.RemoveAt(0, i * ((int32)EFGInputButtons::Count + 1), false);
			}
			break;
		}
	}

	FFGMoveLinkToFollow MoveLinkToFollow = CurrentMove->TryLinks(this, InputStream);
	if (MoveLinkToFollow.SMR.CompletionType == EStateMachineCompletionType::Accepted)
	{
		UE_LOG(LogTemp, Warning, TEXT("Switching to state %s"), *MoveLinkToFollow.Link->Move->MoveName.ToString());
		if (MoveLinkToFollow.Link->bClearInput || MoveLinkToFollow.Link->Move->bClearInputOnEntry || CurrentMove->bClearInputOnExit)
		{
			InputStream.Reset();
			InputTimeStamps.Reset();
		}
		else if (MoveLinkToFollow.SMR.DataIndex)
		{
			// Consume the input we used to get to this move.
			check((MoveLinkToFollow.SMR.DataIndex % (1 + (int32)EFGInputButtons::Count)) == 0);
			InputTimeStamps.RemoveAt(0, MoveLinkToFollow.SMR.DataIndex / (1 + (int32)EFGInputButtons::Count), false);
			InputStream.RemoveAt(0, MoveLinkToFollow.SMR.DataIndex, false);
		}

		// Set and start the new move.
		CurrentMove = MoveLinkToFollow.Link->Move;
		TimeInCurrentMove = 0.0f;
		if (HasAuthority())
			NetMulticast_DoMove(CurrentMove);
		else
			Server_DoMove(CurrentMove);
	}
	else
	{
		TimeInCurrentMove += DeltaSeconds;		// Modulate by move animation length
	}
}

void AFGDefaultPawn::Server_DoMove_Implementation(UFGMove *NewMove)
{
	NetMulticast_DoMove(NewMove);
}
bool AFGDefaultPawn::Server_DoMove_Validate(UFGMove *NewMove) { return true; }

void AFGDefaultPawn::NetMulticast_DoMove_Implementation(UFGMove *NewMove)
{
	DoMove(NewMove);
}
bool AFGDefaultPawn::NetMulticast_DoMove_Validate(UFGMove *NewMove) { return true; }

void AFGDefaultPawn::SetupPlayerInputComponent(UInputComponent* InInputComponent)
{
	Super::SetupPlayerInputComponent(InInputComponent);

	InInputComponent->BindAxis("XAxis", this, &AFGDefaultPawn::ReadXAxis);
	InInputComponent->BindAxis("YAxis", this, &AFGDefaultPawn::ReadYAxis);
	InInputComponent->BindAction("LeftButton", IE_Pressed, this, &AFGDefaultPawn::LeftButtonPressed);
	InInputComponent->BindAction("LeftButton", IE_Released, this, &AFGDefaultPawn::LeftButtonReleased);
	InInputComponent->BindAction("TopButton", IE_Pressed, this, &AFGDefaultPawn::TopButtonPressed);
	InInputComponent->BindAction("TopButton", IE_Released, this, &AFGDefaultPawn::TopButtonReleased);
}

void AFGDefaultPawn::ReadXAxis(float Value)
{
	// Don't care about clamping. We just need to know negative, zero, or positive.
	DirectionInput.X = Value;
}

void AFGDefaultPawn::ReadYAxis(float Value)
{
	// Don't care about clamping. We just need to know negative, zero, or positive.
	DirectionInput.Y = Value;
}

void AFGDefaultPawn::LeftButtonPressed()
{
	ButtonsDown |= (1 << (int32)EFGInputButtons::LeftFace);
}

void AFGDefaultPawn::LeftButtonReleased()
{
	ButtonsDown &= ~(1 << (int32)EFGInputButtons::LeftFace);
}

void AFGDefaultPawn::TopButtonPressed()
{
	ButtonsDown |= (1 << (int32)EFGInputButtons::TopFace);
}

void AFGDefaultPawn::TopButtonReleased()
{
	ButtonsDown &= ~(1 << (int32)EFGInputButtons::TopFace);
}
/*
void AFGDefaultPawn::UseGameCamera()
{
	if (AFightGameExampleGameModeBase* GM = Cast<AFightGameExampleGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			if (AFGCameraActor* Cam = Cast<AFGCameraActor>(GM->MainGameCamera))
			{
				if (UGameplayStatics::GetPlayerControllerID(PC) == 0)
				{
					UE_LOG(LogTemp, Warning, TEXT("Player %i registering with game camera (one)"), UGameplayStatics::GetPlayerControllerID(PC));
					Cam->PlayerOne = this;
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Player %i registering with game camera (two)"), UGameplayStatics::GetPlayerControllerID(PC));
					Cam->PlayerTwo = this;
				}
				PC->SetViewTarget(GM->MainGameCamera);
				return;
			}
		}
	}
	// Try again next frame. Currently, there's no limit to how many times we'll do this.
	GetWorldTimerManager().SetTimerForNextTick(this, &AFGDefaultPawn::UseGameCamera);
}
*/
void AFGDefaultPawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

//	DOREPLIFETIME(AFGDefaultPawn, CurrentMove);
}

bool AFGDefaultPawn::ReplicateSubobjects(UActorChannel *Channel, FOutBunch *Bunch, FReplicationFlags *RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

//	if (CurrentMove)
//	{
//		WroteSomething |= Channel->ReplicateSubobject(CurrentMove, *Bunch, *RepFlags);
//	}

	return WroteSomething;
}