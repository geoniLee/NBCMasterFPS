// Fill out your copyright notice in the Description page of Project Settings.


#include "NBCCharacter.h"

#include "WeaponBase.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ANBCCharacter::ANBCCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// 카메라 회전 동기화 하지 않음
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	// 플레이어 이동 방향과 Character 방향 동기화
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	
	// 스프링 암 설정
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.0f;
	SpringArm->bUsePawnControlRotation = true;
	
	// 카메라 설정
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;
	
	// 무기 상태
	OverlappedWeapon = nullptr;
	EquippedWeapon = nullptr;
	
}

// Called when the game starts or when spawned
void ANBCCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController) return;
	
	ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!LocalPlayer) return;
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!Subsystem) return;
	
	// InputMappingContext을 등록
	if (DefaultMappingContext) {
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

// Called every frame
void ANBCCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANBCCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInputComponent) return;
	
	// IA_Move
	if (MoveAction){
		EnhancedInputComponent->BindAction(
			MoveAction,
			ETriggerEvent::Triggered,
			this,
			&ANBCCharacter::Move
		);
	}
	
	// IA_Look
	if (LookAction){
		EnhancedInputComponent->BindAction(
			LookAction,
			ETriggerEvent::Triggered,
			this,
			&ANBCCharacter::Look
		);
	}
	
	// IA_Jump
	if (JumpAction){
		EnhancedInputComponent->BindAction(
			JumpAction,
			ETriggerEvent::Triggered,
			this,
			&ANBCCharacter::Jump
		);
		
		EnhancedInputComponent->BindAction(
			JumpAction,
			ETriggerEvent::Completed,
			this,
			&ANBCCharacter::StopJumping
		);
	}
	
	// IA_Interact
	if (InteractAction){
		EnhancedInputComponent->BindAction(
			InteractAction,
			ETriggerEvent::Triggered,
			this,
			&ANBCCharacter::Interact
		);
	}
	
	// IA_Fire
	if (FireAction){
		EnhancedInputComponent->BindAction(
			FireAction,
			ETriggerEvent::Triggered,
			this,
			&ANBCCharacter::StartFire
		);
	}
	
	// IA_Reload
	if (ReloadAction){
		EnhancedInputComponent->BindAction(
			ReloadAction,
			ETriggerEvent::Triggered,
			this,
			&ANBCCharacter::Reload
		);
	}
}

void ANBCCharacter::SetOverlappedWeapon(AWeaponBase* Weapon)
{
	OverlappedWeapon = Weapon;
}

void ANBCCharacter::ClearOverlappedWeapon(AWeaponBase* Weapon)
{
	OverlappedWeapon = nullptr;
}

void ANBCCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller) return;
	
	const FVector2D MoveInput = Value.Get<FVector2D>();
	
	// Yaw만 사용
	const FRotator ControllerRotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, ControllerRotation.Yaw, 0);
	
	// Controller Yaw 기준 방향
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	AddMovementInput(ForwardDirection, MoveInput.Y);
	AddMovementInput(RightDirection, MoveInput.X);
}

void ANBCCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookInput = Value.Get<FVector2D>();
	
	// 좌우 회전
	AddControllerYawInput(LookInput.X);
	// 상하 회전
	AddControllerPitchInput(-LookInput.Y);
}

void ANBCCharacter::Interact()
{
	if (!OverlappedWeapon) return;
	
	EquipWeapon(OverlappedWeapon);
}

void ANBCCharacter::StartFire()
{
	if (!EquippedWeapon) return;
	
	EquippedWeapon->Fire();
}

void ANBCCharacter::Reload()
{
	if (!EquippedWeapon) return;
	
	EquippedWeapon->Reload();
}

void ANBCCharacter::EquipWeapon(AWeaponBase* Weapon)
{
	if (!Weapon || EquippedWeapon == Weapon) return;
		
	// 장착 중인 무기가 있을 경우
	if (EquippedWeapon) EquippedWeapon->Destroy();
	
	// 무기 장착
	EquippedWeapon = Weapon;
	EquippedWeapon->Equip(this);
}