// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"

#include "NBCCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"

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
	PelletCount  = 1;
	SpreadAngle = 0;
	FireRate = 0.15f;
	RecoilPitch = 2;
	RecoilYawMin = -0.5f;
	RecoilYawMax = 0.5f;
	CurrentMagazineAmmo = 0;
	CurrentTotalAmmo = 0;
	bCanFire = true;
	
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
	UE_LOG(LogTemp, Warning, TEXT("Weapon Fire"));
}

void AWeaponBase::Reload()
{
	UE_LOG(LogTemp, Warning, TEXT("Weapon Reload"));
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

