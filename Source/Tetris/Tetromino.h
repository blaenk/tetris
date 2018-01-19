// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Core/Public/Containers/StaticArray.h"

#include "Shape.h"

#include "Tetromino.generated.h"

/**
 * A Tetris Tetromino representation.
 * 
 * Each Tetromino consists of a Rotation and Descriptor.
 */
UCLASS()
class TETRIS_API ATetromino : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ATetromino();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

  UFUNCTION(BlueprintCallable)
  void Rotate();

  // TODO
  // const FVector& everything.
  UFUNCTION(BlueprintCallable)
  void MoveToLocation(FVector Location);

  const FShape& GetShape() const;

protected:
  UPROPERTY(EditAnywhere)
  EShapeType ShapeType = EShapeType::SHAPE_J;

  // The Tetromino's color.
  UPROPERTY(EditAnywhere)
  FColor Color = FColor(0, 0, 0);

  UPROPERTY(EditAnywhere)
  class USceneComponent* SceneComponent = nullptr;

  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

private:
  // The canonical filled-cell positions of the Tetromino in each of the four rotations.
  FShape Shape{ EShapeType::SHAPE_J };

  // The individual ACell instances for each filled-cell position.
  TArray<class ACell*> Cells;

  bool IsRotating = false;
  FRotator TargetRotation;

  bool IsTranslating = false;
  FVector TargetLocation;

  FVector LocationForPoint(const FIntPoint& point);

  void ScheduleRotationTo(FRotator Rotation);
  void InterpolateTargetRotation();
  void StopRotating();

  void ScheduleTranslationTo(FVector Location);
  void InterpolateTargetTranslation();
  void StopTranslating();

  void UpdateShape();

  void SpawnCells();
  void SetCellLocations();

#if WITH_EDITOR
  virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
