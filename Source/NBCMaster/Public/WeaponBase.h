// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

class USphereComponent;

UCLASS()
class NBCMASTER_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
protected:
	// 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USceneComponent* Scene;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* StaticMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USphereComponent* Collision;
	
protected:
	// 무기 정보
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Stat")
	float Damage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Stat")
	float Range;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 MagazineSize;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 MaxAmmo;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Fire")
	int32 PelletCount;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Fire")
	float SpreadAngle;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Fire")
	float FireRate;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Recoil")
	float RecoilPitch;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Recoil")
	float RecoilYawMin;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Recoil")
	float RecoilYawMax;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|RunTime")
	int32 CurrentMagazineAmmo;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|RunTime")
	int32 CurrentTotalAmmo;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|RunTime")
	bool bCanFire;
	
	UPROPERTY()
	ACharacter* OwnerPlayer;
	
public:	
	// 생성자
	AWeaponBase();

protected:
	// Unreal LifeCycle
	virtual void BeginPlay() override;
	
public:
	// 무기 액션
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Equip(ACharacter* newOwnerPlayer);
	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Fire();
	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Reload();
	
protected:
	// 충돌 이벤트
	UFUNCTION()
	void OnWeaponOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	
	UFUNCTION()
	void OnWeaponOverlapEnd(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex
	);
};
