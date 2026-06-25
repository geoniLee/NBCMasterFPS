// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NBCHealthManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHealthDeadSignature, AController*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FHealthDamagedSignature, float, NewHealth, float, MaxHealth, float, HealthChange);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NBCMASTER_API UNBCHealthManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNBCHealthManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(BlueprintAssignable)
	FHealthDeadSignature OnHealthDead;
	
	UPROPERTY(BlueprintAssignable)
	FHealthDamagedSignature OnHealthDamaged;
	
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxHealth;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CurrentHealth;
	
private:
	UFUNCTION()
	void DamageTake(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* Instigator, AActor* Causer);
};
