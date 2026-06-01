// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyBase.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Player/NBCCharacter.h"

// Sets default values
AEnemyBase::AEnemyBase() :
	PatrolDistance(500.0f),
	PatrolAcceptanceRadius(50.0f),
	DetectRange(1200.0f),
	DetectRadius(250.0f),
	DetectEyeHeight(60.0f),
	DetectInterval(0.2f),
	PatrolStartLocation(FVector::ZeroVector),
	PatrolTargetLocation(FVector::ZeroVector),
	bMovingToForwardPoint(true)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->MaxWalkSpeed = 200.0f;
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
	// Patrol 범위 지정
	PatrolStartLocation = GetActorLocation();
	PatrolTargetLocation = PatrolStartLocation + GetActorForwardVector() * PatrolDistance;
		
	// 탐지 시작
	GetWorldTimerManager().SetTimer(
		DetectTimerHandle,
		this,
		&AEnemyBase::DetectForwardAsync,
		DetectInterval,
		true
	);
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Patrol();
}

void AEnemyBase::Patrol()
{
	// 방향 계산
	const FVector ToTarget = PatrolTargetLocation - GetActorLocation();
	const FVector MoveDirection = ToTarget.GetSafeNormal2D();
	
	// 도착
	if (ToTarget.Size2D() < PatrolAcceptanceRadius){
		bMovingToForwardPoint = !bMovingToForwardPoint;
		
		// 반대편으로 목표 지점 변경
		PatrolTargetLocation = bMovingToForwardPoint 
			? PatrolStartLocation + GetActorForwardVector() * PatrolDistance
			: PatrolStartLocation - GetActorForwardVector() * PatrolDistance;
		
		return;
	}
	
	// 이동 방향에 맞춰 회전
	SetActorRotation(MoveDirection.Rotation());
	AddMovementInput(MoveDirection);
}

void AEnemyBase::DetectForwardAsync()
{	
	// 탐지 범위
	const FVector Start = GetActorLocation() + FVector(0.0f, 0.0f, DetectEyeHeight);
	const FVector End = Start + GetActorForwardVector() * DetectRange;
		
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	// delegate에 OnDetectTraceComplete 연결
	FTraceDelegate DetectTraceDelegate;
	DetectTraceDelegate.BindUObject(this, &AEnemyBase::OnDetectTraceComplete);
		
	// DetectRadius의 크기만큼 Start부터 End까지 탐색
	GetWorld()->AsyncSweepByChannel(
		EAsyncTraceType::Multi,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(DetectRadius),
		Params,
		FCollisionResponseParams::DefaultResponseParam,
		&DetectTraceDelegate
	);
	
	// 탐색 범위 표시
	DrawDebugCapsule(
		GetWorld(), 
		(Start+End) *  0.5f,
		DetectRange * 0.5f + DetectRadius,
		DetectRadius,
		FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(),
		FColor::Yellow,
		false, 
		DetectInterval
	);
}

void AEnemyBase::OnDetectTraceComplete(const FTraceHandle& TraceHandle, FTraceDatum& TraceDatum)
{
	if (TraceDatum.OutHits.Num() <= 0) return;
	
	// Player가 탐지 되었는지 확인
	for (const FHitResult& Hit : TraceDatum.OutHits){
		AActor* HitActor = Hit.GetActor();
		if (!HitActor) continue;
		
		ANBCCharacter* PlayerCharacter = Cast<ANBCCharacter>(HitActor);
		if (!PlayerCharacter) continue;
		
		GEngine->AddOnScreenDebugMessage(
			0,
			0.2f,
			FColor::Green,
			TEXT("플레이어 탐색 성공")
		);
		return;
	}
	GEngine->AddOnScreenDebugMessage(
		0,
		0.2f,
		FColor::Green,
		TEXT("플레이어 탐색 실패")
	);
}