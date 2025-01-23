#include "Test/Player/MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"

AMyCharacter::AMyCharacter()
{
    WalkSpeed = 900.f;
    SprintSpeed = 1200.f;
    CrouchSpeed = 600.f;
    ShootDistance = 10000.f;

    bIsShooting = false;
    bIsAiming = false;

    Player = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Player"));
    Player->SetupAttachment(RootComponent);

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 200.0f; 
    SpringArm->bUsePawnControlRotation = true; 

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Player Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->bUsePawnControlRotation = false; 

    GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // PLAYER MOVEMENT
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);

    PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMyCharacter::StartSprinting);
    PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMyCharacter::StopSprinting);

    PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMyCharacter::StartCrouching);
    PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AMyCharacter::StopCrouching);

    // PLAYER CAMERA
    PlayerInputComponent->BindAxis("TurnCamera", this, &AMyCharacter::TurnCamera);
    PlayerInputComponent->BindAxis("LookUp", this, &AMyCharacter::LookUp);

    // PLAYER GUN
    PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &AMyCharacter::Shoot);
    PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AMyCharacter::AimDownSights);
    PlayerInputComponent->BindAction("Aim", IE_Released, this, &AMyCharacter::StopAiming);
}

void AMyCharacter::MoveForward(float Input)
{
    FVector ForwardDirection = GetActorForwardVector();
    AddMovementInput(ForwardDirection, Input);
}

void AMyCharacter::MoveRight(float Input)
{
    FVector RightDirection = GetActorRightVector();
    AddMovementInput(RightDirection, Input);
}

void AMyCharacter::TurnCamera(float Input)
{
    AddControllerYawInput(Input);
}

void AMyCharacter::LookUp(float Input)
{
    AddControllerPitchInput(Input);
}

void AMyCharacter::StartSprinting()
{
    GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AMyCharacter::StopSprinting()
{
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AMyCharacter::StartCrouching()
{
    Crouch();
    GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
}

void AMyCharacter::StopCrouching()
{
    UnCrouch();
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AMyCharacter::Shoot()
{
    if (bIsShooting) return;

    bIsShooting = true;

    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AMyCharacter::ResetAttackState, 0.85f, false);

    FVector CameraLocation;
    FRotator CameraRotation;
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        PC->GetPlayerViewPoint(CameraLocation, CameraRotation);
    }

    FVector MuzzleLocation = Player->GetSocketLocation(FName("Muzzle_01"));
    FRotator MuzzleRotation = Player->GetSocketRotation(FName("Muzzle_01"));

    const FVector TraceStart = MuzzleLocation;
    const FVector TraceEnd = TraceStart + (CameraRotation.Vector() * ShootDistance);

    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    QueryParams.bTraceComplex = false;

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        TraceStart,
        TraceEnd,
        ECC_Visibility,
        QueryParams
    );

    if (bHit)
    {
        DrawDebugLine(
            GetWorld(),
            TraceStart,
            HitResult.ImpactPoint,
            FColor::Green,
            false,
            2.0f,
            0,
            1.0f
        );

        DrawDebugSphere(
            GetWorld(),
            HitResult.ImpactPoint,
            10.0f,
            12,
            FColor::Red,
            false,
            2.0f
        );

        if (AActor* HitActor = HitResult.GetActor())
        {
            UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s at location: %s"),
                *HitActor->GetName(),
                *HitResult.ImpactPoint.ToString());
        }
    }
}

void AMyCharacter::AimDownSights()
{
    bIsAiming = true;
    SpringArm->TargetArmLength = 150.0f;
    Camera->SetFieldOfView(70.0f);
}

void AMyCharacter::StopAiming()
{
    bIsAiming = false;
    SpringArm->TargetArmLength = 200.0f;
    Camera->SetFieldOfView(90.0f);
}

void AMyCharacter::ResetAttackState()
{
    bIsShooting = false;
    UE_LOG(LogTemp, Warning, TEXT("isAttacking set to false."));
}