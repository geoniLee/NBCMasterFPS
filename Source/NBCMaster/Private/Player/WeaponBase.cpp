// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/WeaponBase.h"

#include "Player/NBCCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AWeaponBase::AWeaponBase() :OwnerPlayer(nullptr)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// 컴포넌트 설정
	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	SetRootComponent(Scene);
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetupAttachment(Scene);
	
	Collision = CreateDefaultSubobject<USphereComponent>("Collision");
	Collision->SetupAttachment(Scene);
	
	// Collision Overlap 관련 설정
	Collision->SetSphereRadius(100.0f);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// 무기 초깃값 설정
	Damage = 10;
	Range = 3000;
	
	MagazineSize = 30;
	MaxAmmo = 90;
	
	SpreadAngle = 0;
	FireRate = 0.15f;
	
	RecoilPitch = 2;
	RecoilMultiplier = 0.5f;
	RecoilYawMin = -0.5f;
	RecoilYawMax = 0.5f;
	
	SustainedFireSpreadPerShot = 0.25f;
	MaxSustainedFireSpread = 5.0f;
	SustainedFireRecoverySpeed = 3.0f;
	CurrentSustainedFireSpread = 0.0f;
	
	CurrentMagazineAmmo = 0;
	CurrentTotalAmmo = 0;
	bCanFire = true;
	bIsAiming = false;
	
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentMagazineAmmo = MagazineSize;
	CurrentTotalAmmo = MaxAmmo;
	
	// Overlap 이벤트 연결
	if (Collision){
		Collision->OnComponentBeginOverlap.AddDynamic(this, &AWeaponBase::OnWeaponOverlapBegin);
		
		Collision->OnComponentEndOverlap.AddDynamic(this, &AWeaponBase::OnWeaponOverlapEnd);
	}
}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// 총기 발사 가능할 때에만 연사로 인한 탄퍼짐 감소
	if (!bCanFire) return;
	
	CurrentSustainedFireSpread = FMath::FInterpTo(
		CurrentSustainedFireSpread,
		0.0f,
		DeltaTime,
		SustainedFireRecoverySpeed
	);
}

void AWeaponBase::Equip(ACharacter* newOwnerPlayer)
{
	if (!newOwnerPlayer) return;
	
	// Actor의 Owner 설정
	OwnerPlayer = newOwnerPlayer;
	SetOwner(newOwnerPlayer);
	
	// Character에 무기 부착
	AttachToComponent(
		newOwnerPlayer->GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		TEXT("weapon_r")
	);
	
	SetActorRelativeScale3D(FVector(0.5f));
	
	if (Collision){
		Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AWeaponBase::Fire()
{
	if (!CanFire()) return;
	
	// 카메라 기준 발사 위치
	FVector ViewLocation;
	FRotator ViewRotation;
	if (!GetFireViewPoint(ViewLocation, ViewRotation)) return;
	
	// 카메라 방향을 기준으로 발사 방향을 계산
	const FVector BaseDirection = ViewRotation.Vector();
	
	ApplyFire(ViewLocation, BaseDirection);
	FinishFire();
}

void AWeaponBase::Reload()
{
	if (CurrentTotalAmmo <= 0)return;
	if (CurrentMagazineAmmo >= MagazineSize) return;
	
	// 예비 탄창에서 가져올 수 있는 만큼만 가져옴
	const int32 NeedAmmo = MagazineSize - CurrentMagazineAmmo;
	const int32 reloadAmmo = FMath::Min(NeedAmmo, CurrentTotalAmmo);
	
	CurrentMagazineAmmo += reloadAmmo;
	CurrentTotalAmmo -= reloadAmmo;
	
	UE_LOG(LogTemp, Warning, TEXT("Reload: Magazine = %d / Total = %d"), CurrentMagazineAmmo, CurrentTotalAmmo);
}

void AWeaponBase::SetAiming(bool bNewAiming)
{
	bIsAiming = bNewAiming;
}

bool AWeaponBase::CanFire() const
{
	if (!OwnerPlayer || !bCanFire) return false;
	if (CurrentMagazineAmmo <= 0) return false;
	
	return true;
}

bool AWeaponBase::GetFireViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	if (!OwnerPlayer) return false;
	
	AController* OwnerController = OwnerPlayer->GetController();
	if (!OwnerController) return false;
	
	// 카메라 기준으로 위치와 회전값을 가져옴
	OwnerController->GetPlayerViewPoint(OutLocation, OutRotation);
	return true;
}

void AWeaponBase::ApplyFire(const FVector& ViewLocation, const FVector& BaseDirection)
{
	FVector ShotDirection = BaseDirection;
	const float CurrentSpreadAngle = SpreadAngle + CurrentSustainedFireSpread;
	
	// 탄 퍼짐, 명중 오차
	if (CurrentSpreadAngle > 0){
		ShotDirection = FMath::VRandCone(BaseDirection, FMath::DegreesToRadians(CurrentSpreadAngle));
	}
	
	FireTrace(ViewLocation, ShotDirection);
}

void AWeaponBase::FinishFire()
{
	CurrentMagazineAmmo--;
	UE_LOG(LogTemp, Warning, TEXT("Ammo: %d / %d"), CurrentMagazineAmmo, CurrentTotalAmmo);
	
	// 연사 시 탄 퍼짐 증가
	CurrentSustainedFireSpread = FMath::Min(
		CurrentSustainedFireSpread + SustainedFireSpreadPerShot, MaxSustainedFireSpread);
	
	ApplyReCoil();
	StartFireCooldown();
}

void AWeaponBase::FireTrace(const FVector& Start, const FVector& Direction)
{
	const FVector End = Start + Direction * Range;
	
	FHitResult Hit;
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	if (OwnerPlayer){
		Params.AddIgnoredActor(OwnerPlayer);
	}
	
	// 라인 트레이스 실행
	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit, Start, End, ECC_Visibility, Params);
	
	// 필요 없을 시 주석 처리
	DrawDebugLine(
		GetWorld(), 
		Start, 
		End, 
		bHit ? FColor::Green : FColor::Red, 
		false,
		1,
		0,
		1
	);
	
	if (!bHit) return;
	
	AActor* HitActor = Hit.GetActor();
	if (!HitActor) return;
	
	UGameplayStatics::ApplyPointDamage(
		HitActor, 
		Damage,
		Direction,
		Hit,
		OwnerPlayer ?
		OwnerPlayer->GetController(): nullptr,
		this,
		nullptr
	);
	
	UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitActor->GetName());
}

void AWeaponBase::ApplyReCoil()
{
	if (!OwnerPlayer) return;
	
	// 조준 여부를 확인하여 반동 값 계산
	const float AppliedRecoilPitch = bIsAiming ? RecoilPitch * RecoilMultiplier : RecoilPitch;
	const float RandomYaw = bIsAiming 
		? FMath::RandRange(RecoilYawMin, RecoilYawMax) * RecoilMultiplier 
		: FMath::RandRange(RecoilYawMin, RecoilYawMax);
	
	// 플레이어 입력에 반동을 적용해 시야를 흔듦
	OwnerPlayer->AddControllerPitchInput(-AppliedRecoilPitch);
	OwnerPlayer->AddControllerYawInput(RandomYaw);
}

void AWeaponBase::StartFireCooldown()
{
	bCanFire = false;
	
	if (FireRate <= 0){
		ResetFire();
		return;
	}
	
	// 발사 cooldown 적용
	GetWorldTimerManager().SetTimer(
		FireRateTimerHandle,
		this,
		&AWeaponBase::ResetFire,
		FireRate,
		false
	);
}

void AWeaponBase::ResetFire()
{
	bCanFire = true;
}

void AWeaponBase::OnWeaponOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ANBCCharacter* PlayerCharacter = Cast<ANBCCharacter>(OtherActor);
	if (!PlayerCharacter) return;
	
	// 플레이어 Overlap 객체 갱신
	PlayerCharacter->SetOverlappedWeapon(this);
	
	UE_LOG(LogTemp, Warning, TEXT("Weapon Overlap Begin"));
}

void AWeaponBase::OnWeaponOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	ANBCCharacter* PlayerCharacter = Cast<ANBCCharacter>(OtherActor);
	if (!PlayerCharacter) return;
	
	// 플레이어 Overlap 객체 초기화
	PlayerCharacter->ClearOverlappedWeapon(this);
	
	UE_LOG(LogTemp, Warning, TEXT("Weapon Overlap End"));
}

