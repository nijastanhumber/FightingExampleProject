// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "FGState.h"
#include "FGDefaultPawn.generated.h"

class UFGDirectionalInputAtom;
class UFGButtonInputAtom;
class UFGMove;

/**
 * 
 */
UCLASS()
class FIGHTGAMEEXAMPLE_API AFGDefaultPawn : public ADefaultPawn
{
	GENERATED_BODY()
	
public:
	AFGDefaultPawn();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* InInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;
	virtual bool ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags) override;

	FORCEINLINE float GetTimeInMove() const { return TimeInCurrentMove; }

protected:
	void LeftButtonPressed();
	void LeftButtonReleased();
	void TopButtonPressed();
	void TopButtonReleased();
	void ReadXAxis(float Value);
	void ReadYAxis(float Value);

//	void UseGameCamera();

	FVector2D DirectionInput;
	uint32 ButtonsDown;
	uint32 ButtonsDown_Old;
	float TimeInCurrentMove;

	UPROPERTY(EditAnywhere)
		UFGMove* CurrentMove;

	// Input atoms are removed when they pass this age threshold. All moves must be executed in under this time.
	UPROPERTY(EditAnywhere)
		float InputExpirationTime;

	UPROPERTY(EditAnywhere, Category = "Input Atoms")
		UFGDirectionalInputAtom* DirectionDownBackAtom;

	UPROPERTY(EditAnywhere, Category = "Input Atoms")
		UFGDirectionalInputAtom* DirectionDownAtom;

	UPROPERTY(EditAnywhere, Category = "Input Atoms")
		UFGDirectionalInputAtom* DirectionDownForwardAtom;

	UPROPERTY(EditAnywhere, Category = "Input Atoms")
		UFGDirectionalInputAtom* DirectionBackAtom;

	UPROPERTY(EditAnywhere, Category = "Input Atoms")
		UFGDirectionalInputAtom* DirectionNeutralAtom;

	UPROPERTY(EditAnywhere, Category = "Input Atoms")
		UFGDirectionalInputAtom* DirectionForwardAtom;

	UPROPERTY(EditAnywhere, Category = "Input Atoms")
		UFGDirectionalInputAtom* DirectionUpBackAtom;

	UPROPERTY(EditAnywhere, Category = "Input Atoms")
		UFGDirectionalInputAtom* DirectionUpAtom;

	UPROPERTY(EditAnywhere, Category = "Input Atoms")
		UFGDirectionalInputAtom* DirectionUpForwardAtom;

	// Order in this array is the same as EFGButtonState: Up, JustPressed, Held
	UPROPERTY(EditAnywhere, Category = "Input Atoms")
		TArray<UFGButtonInputAtom*> ButtonAtoms;

	UFUNCTION(BlueprintImplementableEvent)
		void DoMove(UFGMove* NewMove);

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_DoMove(UFGMove *NewMove);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void NetMulticast_DoMove(UFGMove *NewMove);

private:
	//~ This array relates to InputStream. InputStream must not be updated without this stream being updated as well.
	UPROPERTY(VisibleInstanceOnly)
		TArray<float> InputTimeStamps;

	UPROPERTY(VisibleInstanceOnly)
		TArray<USM_InputAtom*> InputStream;
};
