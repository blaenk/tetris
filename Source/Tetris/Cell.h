// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Cell.generated.h"

UCLASS()
class TETRIS_API ACell : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACell();

  static const int SIZE = 10;

  void SetMaterial(class UMaterialInstance* Material);

protected:
  UPROPERTY(EditAnywhere)
  class UMaterialInstance* Material = nullptr;

  UPROPERTY(EditAnywhere)
  class UStaticMeshComponent* Mesh = nullptr;

  // TODO
  // Material Instance component.

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
