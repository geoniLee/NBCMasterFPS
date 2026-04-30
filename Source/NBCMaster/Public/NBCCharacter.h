// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NBCCharacter.generated.h"

struct FInputActionValue;
class AWeaponBase;
class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class NBCMASTER_API ANBCCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANBCCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArm;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* Camera;
	
protected:
	// 입력
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* InteractAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* FireAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* ReloadAction;
	
protected:
	// 무기 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	AWeaponBase* OverlappedWeapon;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	AWeaponBase* EquippedWeapon;
	
public:
	// 무기 충돌
	void SetOverlappedWeapon(AWeaponBase* Weapon);
	void ClearOverlappedWeapon(AWeaponBase* Weapon);
	
protected:
	// 입력 함수
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	
	void Interact();
	void StartFire();
	void Reload();
	
protected:
	// 무기 기능 함수
	void EquipWeapon(AWeaponBase* Weapon);
	
};
