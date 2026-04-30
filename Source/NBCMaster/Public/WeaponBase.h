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
	// 무기 스탯
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Stat")
	float Damage;					// 공격력
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Stat")
	float Range;					// 범위
	
	// 무기 탄창
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 MagazineSize;				// 최대 총알 수 (현재 탄창)
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 MaxAmmo;					// 최대 총알수(예비 탄창)
	
	// 무기 사용
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Fire")
	int32 PelletCount;				// 한번에 발사하는 탄 수
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Fire")
	float SpreadAngle;				// 확산 각도
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Fire")
	float FireRate;					// 발사 간격
	
	// 무기 반동
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Recoil")
	float RecoilPitch;				// 반동 - Pitch
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Recoil")
	float RecoilYawMin;				// 반동 -Yaw 최솟갑
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Recoil")
	float RecoilYawMax;				// 반동 - Yaw 최댓값
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Recoil")
	float RecoilKnockbackStrength;	// 뒤로 밀리는 힘
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Recoil")
	float RecoilRecoverySpeed;		// 발사 반동 회복 속도
	
	// 런타임 변경 값
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|RunTime")
	int32 CurrentMagazineAmmo;		// 현재 남은 총알 수(현재 탄창)
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|RunTime")
	int32 CurrentTotalAmmo;			// 남은 총 총알 수 (예비 탄창)
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|RunTime")
	bool bCanFire;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|RunTime")
	FRotator CurrentRecoilRotation;
	
	UPROPERTY()
	ACharacter* OwnerPlayer;
	
protected:
	// 타이머 핸들
	FTimerHandle FireRateTimerHandle;
	
public:	
	// 생성자
	AWeaponBase();

protected:
	// Unreal LifeCycle
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaTime ) override;
	
public:
	// 무기 액션
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Equip(ACharacter* newOwnerPlayer);
	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Fire();
	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Reload();
	
protected:
	// 발사 관리
	bool CanFire() const;
	bool GetFireViewPoint(FVector& OutLocation, FRotator& OutRotation) const;
	
	void FireTrace(const FVector& Start, const FVector& Direction);
	void ApplyReCoil();
	
	void StartFireCooldown();
	void ResetFire();
	
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
