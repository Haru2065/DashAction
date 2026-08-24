#include "BreakableFloor.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"

ABreakableFloor::ABreakableFloor()
{
	PrimaryActorTick.bCanEverTick = true;

	// =========================================================
	// Root
	// =========================================================

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	// =========================================================
	// Floor Mesh
	// =========================================================

	FloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorMesh"));
	FloorMesh->SetupAttachment(SceneRoot);

	// 最初は物理シミュレーションOFF
	FloorMesh->SetSimulatePhysics(false);

	// =========================================================
	// Trigger
	// =========================================================

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(SceneRoot);

	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	TriggerBox->OnComponentBeginOverlap.AddDynamic(
		this,
		&ABreakableFloor::OnFloorBeginOverlap
	);
}

void ABreakableFloor::BeginPlay()
{
	Super::BeginPlay();

	InitialLocation = GetActorLocation();
}

void ABreakableFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// =========================================================
	// 落下前の振動
	// =========================================================

	if (bActivated && !bIsFalling)
	{
		ShakeTime += DeltaTime;

		const float ShakeX = FMath::Sin(ShakeTime * ShakeSpeed) * ShakeAmount;
		const float ShakeY = FMath::Cos(ShakeTime * ShakeSpeed * 1.3f) * ShakeAmount;

		FVector NewLocation = InitialLocation;
		NewLocation.X += ShakeX;
		NewLocation.Y += ShakeY;

		SetActorLocation(NewLocation);
	}

	// =========================================================
	// 落下後
	// =========================================================

	if (bIsFalling)
	{
		const float CurrentFallDistance =
			InitialLocation.Z - GetActorLocation().Z;

		if (CurrentFallDistance >= DestroyFallDistance)
		{
			Destroy();
		}
	}
}

void ABreakableFloor::OnFloorBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (bActivated)
	{
		return;
	}

	// Characterだけ反応
	ACharacter* Character = Cast<ACharacter>(OtherActor);

	if (!Character)
	{
		return;
	}

	bActivated = true;

	// 落下開始まで待つ
	GetWorldTimerManager().SetTimer(
		FallTimerHandle,
		this,
		&ABreakableFloor::StartFalling,
		FallDelay,
		false
	);
}

void ABreakableFloor::StartFalling()
{
	if (bIsFalling)
	{
		return;
	}

	bIsFalling = true;

	// 振動でずれた位置を一度戻す
	SetActorLocation(InitialLocation);

	// Triggerはもう不要
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// PhysicsをONにして落下
	FloorMesh->SetSimulatePhysics(true);
}