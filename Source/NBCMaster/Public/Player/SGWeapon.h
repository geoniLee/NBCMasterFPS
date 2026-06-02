// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/WeaponBase.h"
#include "SGWeapon.generated.h"

/**
 * 
 */
UCLASS()
class NBCMASTER_API ASGWeapon : public AWeaponBase
{
	GENERATED_BODY()
	
public:
	ASGWeapon();
	
protected:
	virtual void ApplyFire(const FVector& ViewLocation, const FVector& BaseDirection) override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Shotgun")
	int32 PelletCount;
};
