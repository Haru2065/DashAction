#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BreakableFloor.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class DASHACTION_API ABreakableFloor : public AActor
{
	GENERATED_BODY()

public:
	ABreakableFloor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:

	// プレイヤーが床に乗った時の判定
	UFUNCTION()
	void OnFloorBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	// 落下開始
	void StartFalling();

protected:

	// =========================================================
	// Components
	// =========================================================

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* FloorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* TriggerBox;


	// =========================================================
	// Settings
	// =========================================================

	/**
	 * プレイヤーが乗ってから落下するまでの時間
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breakable Floor|Timing",
		meta = (ClampMin = "0.0"))
	float FallDelay = 2.0f;

	/**
	 * どれくらい落下したら床を削除するか
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breakable Floor|Falling",
		meta = (ClampMin = "0.0"))
	float DestroyFallDistance = 1500.0f;

	/**
	 * 振動の大きさ
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breakable Floor|Shake",
		meta = (ClampMin = "0.0"))
	float ShakeAmount = 3.0f;

	/**
	 * 振動速度
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breakable Floor|Shake",
		meta = (ClampMin = "0.0"))
	float ShakeSpeed = 30.0f;


private:

	// 初期位置
	FVector InitialLocation;

	// 起動したか
	bool bActivated = false;

	// 落下中か
	bool bIsFalling = false;

	// 振動用時間
	float ShakeTime = 0.0f;

	// 落下開始タイマー
	FTimerHandle FallTimerHandle;
};