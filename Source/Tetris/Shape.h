// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/*
* Enumeration for each type of classical Tetromino.
*/
UENUM(BlueprintType)
enum class EShapeType : uint8
{
  SHAPE_I UMETA(DisplayName = "I"),
  SHAPE_J UMETA(DisplayName = "J"),
  SHAPE_L UMETA(DisplayName = "L"),
  SHAPE_O UMETA(DisplayName = "O"),
  SHAPE_S UMETA(DisplayName = "S"),
  SHAPE_T UMETA(DisplayName = "T"),
  SHAPE_Z UMETA(DisplayName = "Z")
};

/**
* Represents a Tetromino shape in four different rotations.
*
* Uniform dimensions of the shape in each rotation are statically enforced.
*
* Note that rot0 mirrors rot270 and rot90 mirrors rot180
*/
class FShape
{
public:
  FShape() = delete;

  FShape(EShapeType ShapeType);

  /**
  * Rotate in 90 degree increments.
  *
  * CCW by default, or CW if -1 is passed for the `direction`.
  */
  void Rotate(int direction = 1);

  int RotationCount() const;
  int PointCount() const;

  const TArray<FIntPoint>& GetPoints() const;
  const TArray<FIntPoint>& GetPointsFromNextRotation() const;

private:
  static TArray<TArray<FIntPoint>> ShapeO();
  static TArray<TArray<FIntPoint>> ShapeI();
  static TArray<TArray<FIntPoint>> ShapeS();
  static TArray<TArray<FIntPoint>> ShapeZ();
  static TArray<TArray<FIntPoint>> ShapeL();
  static TArray<TArray<FIntPoint>> ShapeJ();
  static TArray<TArray<FIntPoint>> ShapeT();

private:
  TArray<TArray<FIntPoint>> Points;

  EShapeType ShapeType;

  int Rotation = 0;
};
