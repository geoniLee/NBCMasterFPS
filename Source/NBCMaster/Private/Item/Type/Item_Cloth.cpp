// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Type/Item_Cloth.h"

#include <iostream>
#include <ostream>

#include "Kismet/GameplayStatics.h"

void AItem_Cloth::OnFireDetected_Implementation(float Temperature, FVector HitLocation)
{
	if (FireEffect){
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), FireEffect, GetActorLocation(), GetActorRotation(), FVector(1.0f));
	}
}
