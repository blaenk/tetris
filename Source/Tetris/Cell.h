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

protected:
  // TODO
  // Is this necessary? Can simply expose a SetColor() function?
  // The color of this cell.
  UPROPERTY()
  FColor Color;

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
