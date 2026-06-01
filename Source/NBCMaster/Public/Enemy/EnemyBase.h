// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

class ANBCCharacter;

UCLASS()
class NBCMASTER_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	void Patrol();
	void DetectForwardAsync();
	void OnDetectTraceComplete(const FTraceHandle& TraceHandle, FTraceDatum& TraceDatum);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Patrol")
	float PatrolDistance;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Patrol")
	float PatrolAcceptanceRadius;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Detect")
	float DetectRange;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Detect")
	float DetectRadius;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Detect")
	float DetectEyeHeight;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Detect")
	float DetectInterval;
	
	FVector PatrolStartLocation;
	FVector PatrolTargetLocation;
	bool bMovingToForwardPoint;
	
	FTimerHandle DetectTimerHandle;
};
