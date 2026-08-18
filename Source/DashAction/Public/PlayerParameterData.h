// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PlayerParameterData.generated.h"

/**
 * 
 */
UCLASS()
class DASHACTION_API UPlayerParameterData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	//最高速度
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Speed")
	float MaxWalkSpeed = 0.0f;

	//最低速度
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Speed")
	float MinWalkSpeed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Speed")
	float InitializeSpeed = 0.0f;

	//加速度
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Speed")
	float Acceleration = 0.0f;

	//減速度
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Speed")
	float Deceleration = 0.0f;
	
};
