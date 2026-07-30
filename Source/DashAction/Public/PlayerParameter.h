// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerParameter.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DASHACTION_API UPlayerParameter : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerParameter();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


public:

	//通常時のダッシュモードか
	//falseのときドリフトで横移動が無効化される
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "通常時のダッシュモードか")
	bool bIsDefalutDashMode;

	//プレイヤーの移動速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerDefalutSpeed")
	float PlayerSpeed;
};
