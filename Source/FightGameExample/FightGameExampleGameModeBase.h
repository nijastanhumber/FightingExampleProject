// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "FightGameExampleGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTGAMEEXAMPLE_API AFightGameExampleGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	void SpawnSecondPlayer();

	// This will be spawned when the game starts.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ACameraActor* MainGameCamera;
	
	
};
