#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

UCLASS()
class TEST_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent* Player;

	UPROPERTY(EditAnywhere, Category = "Gun")
	class UStaticMesh* MuzzleFlashMesh;

	UPROPERTY(EditAnywhere, Category = "Gun")
	class UAnimSequence* AimLoopAnim;


	UPROPERTY(EditAnywhere, Category = "Gun")
	class UAnimSequence* AimStartAnim;


	UPROPERTY(EditAnywhere, Category = "Gun")
	class UAnimMontage* ShootAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CrouchSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun")
	float ShootDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsShooting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsAiming;


public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	// PLAYER MOVEMENT

	void MoveForward(float Input);
	void MoveRight(float Input);

	void StartSprinting();
	void StopSprinting();


	void StartCrouching();
	void StopCrouching();

	// PLAYER CAMERA

	void TurnCamera(float Input);
	void LookUp(float Input);

	void AimDownSights();
	void StopAiming();

	// Player Gun

	void Shoot();

	void ResetAttackState();

};
