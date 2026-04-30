// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"

#include "NBCCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
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
	
	PelletCount =1;
	SpreadAngle = 0;
	FireRate = 0.15f;
	
	RecoilPitch = 2;
	RecoilYawMin = -0.5f;
	RecoilYawMax = 0.5f;
	RecoilKnockbackStrength =200;
	RecoilRecoverySpeed = 8;
	
	CurrentMagazineAmmo = 0;
	CurrentTotalAmmo = 0;
	bCanFire = true;
	CurrentRecoilRotation = FRotator::ZeroRotator;
	
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
	
	CurrentRecoilRotation = FMath::RInterpTo(
		CurrentRecoilRotation,
		FRotator::ZeroRotator,
		DeltaTime,
		RecoilRecoverySpeed
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
		TEXT("WeaponSocket")
	);
	
	if (Collision){
		Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AWeaponBase::Fire()
{
	if (!CanFire()) return;
	
	const FVector StartLocation = OwnerPlayer->GetActorLocation() + FVector(0,0,60);
	const FRotator FireRatation = OwnerPlayer->GetActorRotation() + CurrentRecoilRotation;
	const FVector BaseDirection = FireRatation.Vector();
	
	const float SpreadRadians = FMath::DegreesToRadians(SpreadAngle);
	
	for (int32 i = 0; i< PelletCount; ++i){
		FVector ShotDirection = BaseDirection;

		if (SpreadAngle > 0){
			ShotDirection = FMath::VRandCone(BaseDirection, SpreadRadians);
		}

		FireTrace(StartLocation, ShotDirection);
	}
	
	CurrentMagazineAmmo--;
	UE_LOG(LogTemp, Warning, TEXT("Ammo: %d / %d"), CurrentMagazineAmmo, CurrentTotalAmmo);
	
	ApplyReCoil();
	StartFireCooldown();
}

void AWeaponBase::Reload()
{
	if (CurrentTotalAmmo <= 0)return;
	if (CurrentMagazineAmmo >= MagazineSize) return;
	
	const int32 NeedAmmo = MagazineSize - CurrentMagazineAmmo;
	const int32 reloadAmmo = FMath::Min(NeedAmmo, CurrentTotalAmmo);
	
	CurrentMagazineAmmo += reloadAmmo;
	CurrentTotalAmmo -= reloadAmmo;
	
	UE_LOG(LogTemp, Warning, TEXT("Reload: Magazine = %d / Total = %d"), CurrentMagazineAmmo, CurrentTotalAmmo);
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
	
	OwnerController->GetPlayerViewPoint(OutLocation, OutRotation);
	return true;
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
	
	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit, Start, End, ECC_Visibility, Params);
	
	// 필요 없을 시 주석 처리
	DrawDebugLine(
		GetWorld(), 
		Start, 
		End, 
		bHit ? FColor::Red : FColor::Green, 
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
	
	const float RandomYaw = FMath::RandRange(RecoilYawMin, RecoilYawMax);
	
	CurrentRecoilRotation.Pitch += RecoilPitch;
	CurrentRecoilRotation.Yaw += RandomYaw;
	
	UCharacterMovementComponent* MovementComponent = OwnerPlayer->GetCharacterMovement();
	if (!MovementComponent) return;
	
	const FVector BackwardVector = -OwnerPlayer->GetActorForwardVector();
	const FVector recoilImpulse = BackwardVector * RecoilKnockbackStrength;
	
	MovementComponent->AddImpulse(recoilImpulse, true);
	
	OwnerPlayer->AddControllerPitchInput(-RecoilPitch);
	OwnerPlayer->AddControllerYawInput(RandomYaw);
}

void AWeaponBase::StartFireCooldown()
{
	bCanFire = false;
	
	if (FireRate <= 0){
		ResetFire();
		return;
	}
	
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

