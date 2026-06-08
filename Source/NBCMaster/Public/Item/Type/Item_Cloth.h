// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemBase.h"
#include "Item/NBCInterface.h"
#include "Item_Cloth.generated.h"

/**
 * 
 */
UCLASS()
class NBCMASTER_API AItem_Cloth : public AItemBase, public INBCInterface
{
	GENERATED_BODY()
	
public:
	virtual void OnFireDetected_Implementation(float Temperature, FVector HitLocation) override;
	
protected:
	UPROPERTY(EditAnywhere, Category = "Effects")
	class UParticleSystem* FireEffect;
};
