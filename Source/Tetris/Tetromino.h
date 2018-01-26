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

  UFUNCTION(BlueprintCallable)
  void MoveToLocation(const FVector& Location);

  UFUNCTION(BlueprintCallable)
  void Reset();

  UFUNCTION(BlueprintCallable)
  void SetRandomShape();

  const FShape& GetShape() const;

  class UMaterialInstance* GetMaterial() const;

protected:
  UPROPERTY(EditAnywhere)
  EShapeType ShapeType = EShapeType::SHAPE_J;

  // The Tetromino's color.
  UPROPERTY(EditAnywhere)
  class UMaterialInstance* Material;

  UPROPERTY(EditAnywhere)
  class USceneComponent* SceneComponent = nullptr;

  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
  // The canonical filled-cell positions of the Tetromino in each of the four rotations.
  FShape Shape{ EShapeType::SHAPE_J };

  // The individual ACell instances for each filled-cell position.
  TArray<class ACell*> Cells;

  static class UMaterialInstance* MaterialCyan;
  static class UMaterialInstance* MaterialBlue;
  static class UMaterialInstance* MaterialOrange;
  static class UMaterialInstance* MaterialYellow;
  static class UMaterialInstance* MaterialGreen;
  static class UMaterialInstance* MaterialPurple;
  static class UMaterialInstance* MaterialRed;

  bool IsRotating = false;
  FRotator TargetRotation;

  bool IsTranslating = false;
  FVector TargetLocation;

  FVector LocationForPoint(FIntPoint point);

  // TODO
  // DRY These? Combine them into a single FTransform interpolation?
  void ScheduleRotationTo(const FRotator& Rotation);
  void InterpolateTargetRotation();
  void StopRotating();

  void ScheduleTranslationTo(const FVector& Location);
  void InterpolateTargetTranslation();
  void StopTranslating();

  void SetShape(EShapeType ShapeType);
  void UpdateShape();

  void DestroyCells();
  void SpawnCells();
  void SetCellLocations();

#if WITH_EDITOR
  virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
