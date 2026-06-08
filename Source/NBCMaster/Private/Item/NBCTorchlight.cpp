// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/NBCTorchlight.h"

#include "Item/NBCInterface.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ANBCTorchlight::ANBCTorchlight()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ANBCTorchlight::BeginPlay()
{
	Super::BeginPlay();
	
	for (const TWeakObjectPtr<AActor>& Item : Items){
		// 해당 클래스에 인터페이스가 구현되어 있는가
		if (UKismetSystemLibrary::DoesImplementInterface(Item.Get(), UNBCInterface::StaticClass())){
			INBCInterface::Execute_OnFireDetected(Item.Get(), 100.f, FVector::ZeroVector);
		}
		
	}
}

// Called every frame
void ANBCTorchlight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

