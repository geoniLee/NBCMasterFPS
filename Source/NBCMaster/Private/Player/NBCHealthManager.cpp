// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/NBCHealthManager.h"

// Sets default values for this component's properties
UNBCHealthManager::UNBCHealthManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	MaxHealth = 100.0f;
}


// Called when the game starts
void UNBCHealthManager::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	
	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UNBCHealthManager::DamageTake);
}


// Called every frame
void UNBCHealthManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, FString::Printf(TEXT("Health: %f"), CurrentHealth));

}

void UNBCHealthManager::DamageTake(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* Instigator, AActor* Causer)
{
	float FinalDamage = FMath::Min(Damage, CurrentHealth);
	CurrentHealth -= FinalDamage;
	OnHealthDamaged.Broadcast(FinalDamage, MaxHealth, CurrentHealth);
	if (CurrentHealth <= 0.f){
		OnHealthDead.Broadcast(Instigator);
	}
}

