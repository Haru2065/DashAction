// Copyright Epic Games, Inc. All Rights Reserved.

#include "DashActionCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "DashAction.h"

ADashActionCharacter::ADashActionCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 180.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ADashActionCharacter::BeginPlay()
{
	Super::BeginPlay();

	//プレイヤーのパラメータデータアセットが設定されていればそこから初期移動速度を読み込む
	if (playerData)
	{
		//最初から最大速度でスタート
		CurrentSpeed = playerData->InitializeSpeed;

		//キャラクタームーブメントコンポーネントにも更新
		GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed;
	}
}

void ADashActionCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!playerData) return;

	//目標速度(初期値は現在速度)
	float TargetSpeed = CurrentSpeed;

	//加速度と減速度の値(0のままなら速度変化なし)
	float RateToUse = 0.f;

	//加速状態になったとき最高速度に向かって数値を加算
	if (bIsAccelerating)
	{
		//Wキーを押している間最高速度に向かって加算する
		TargetSpeed = playerData->MaxWalkSpeed;
		RateToUse = playerData->Acceleration;
	}

	//加速状態になったとき最低速度に向かって数値を加算
	else if (bIsDecelerating)
	{
		// Sキー押下中：最低速度に向かって、Decelerationの速さで減速する
		TargetSpeed = playerData->MinWalkSpeed;
		RateToUse = playerData->Deceleration;
	}

	// どちらも押されていない場合：TargetSpeed = CurrentSpeedのままなので変化しない
	if (RateToUse > 0.f)
	{
		//現在速度を目標速度に向かって、RateToUseの速さで一定速度に変化させる
		CurrentSpeed = FMath::FInterpConstantTo(CurrentSpeed, TargetSpeed, DeltaTime, RateToUse);

		GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed;
	}

	///デバック表示(ビルド時には表示されない)
	///現在の移動速度・加減速状態を画面上にオーバーレイ表示する
#if !UE_BUILD_SHIPPING
	if (GEngine)
	{
		// 現在の加減速状態によって表示色を切り替える
		// 色分けすることで、数値を読まなくても状態が一瞬で判別できるようにする

		//白 = ニュートラル（現状維持）
		FColor DebugColor = FColor::White;

		FString DebugState = TEXT("Neutral");

		//緑 = 加速中
		if (bIsAccelerating)
		{
			DebugColor = FColor::Green;
			DebugState = TEXT("Accelerating");
		}

		//赤 = 減速中
		else if(bIsDecelerating)
		{
			DebugColor = FColor::Red;
			DebugState = TEXT("Decelerating");
		}

		//表示する文字列の組み立て
		//CurrentSpeed:今の実速度
		//State:加速/減速/ニュートラルのどれか
		//Min/Max:PlayerDataで設定されている速度の範囲
		const FString DebugMessage = FString::Printf(
			TEXT("CurrentSpeed: %.1f | State: %s | Min: %.1f | Max: %.1f"),
			CurrentSpeed, *DebugState, playerData->MinWalkSpeed, playerData->MaxWalkSpeed
		);

		//Key(=1)を固定することで、Tickの度に行が増えず1行だけを毎フレーム上書きする
		GEngine->AddOnScreenDebugMessage(1, 0.0f, DebugColor, DebugMessage);
	}
#endif
}

//// <summary>
/// Wキーでの入力状態をセットするメソッド
/// </summary>
/// <param name="bNewValue">true: Wキーが押されている（加速する） / false: Wキーが離された（加速をやめる）</param>
void ADashActionCharacter::SetAccelerating(bool bNewValue)
{
	bIsAccelerating = bNewValue;
}

/// <summary>
/// Sキーでの入力状態をセットするメソッド
/// </summary>
/// <param name="bNewValue">true: Sキーが押されている（減速する） / false: Sキーが離された（減速をやめる）</param>
void ADashActionCharacter::SetDecelerating(bool bNewValue)
{
	bIsDecelerating = bNewValue;
}

void ADashActionCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADashActionCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ADashActionCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADashActionCharacter::Look);
	}
	else
	{
		UE_LOG(LogDashAction, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ADashActionCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	const FVector2D MovementVector = Value.Get<FVector2D>();
	
	// 体の向き(Actorの回転)を基準に前方向・右方向を取得
	// ※コントローラーやカメラの向きは使わない → 体は移動しても回転せず固定される
	const FVector ForwardDirection = GetActorForwardVector();
	const FVector RightDirection = GetActorRightVector();

	// route the input
	//DoMove(MovementVector.X, MovementVector.Y);

	//前後
	AddMovementInput(ForwardDirection, MovementVector.Y);

	//左右スライド。体は回転しない
	AddMovementInput(RightDirection, MovementVector.X);
}

void ADashActionCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ADashActionCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void ADashActionCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ADashActionCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void ADashActionCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}
