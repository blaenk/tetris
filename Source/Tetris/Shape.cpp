// Fill out your copyright notice in the Description page of Project Settings.

#include "Shape.h"

FShape::FShape(EShapeType ShapeType) : ShapeType(ShapeType)
{
  switch (this->ShapeType)
  {
    case EShapeType::SHAPE_I:
      this->Points = FShape::ShapeI();
      break;

    case EShapeType::SHAPE_J:
      this->Points = FShape::ShapeJ();
      break;

    case EShapeType::SHAPE_L:
      this->Points = FShape::ShapeL();
      break;

    case EShapeType::SHAPE_O:
      this->Points = FShape::ShapeO();
      break;

    case EShapeType::SHAPE_S:
      this->Points = FShape::ShapeS();
      break;

    case EShapeType::SHAPE_T:
      this->Points = FShape::ShapeT();
      break;

    case EShapeType::SHAPE_Z:
      this->Points = FShape::ShapeZ();
      break;
  }
}

void FShape::ResetRotation()
{
  this->Rotation = 0;
}

int FShape::GetRotation() const
{
  return -(this->Rotation * 90);
}

int FShape::RotationCount() const
{
  return this->Points.Num();
}

bool FShape::Rotate()
{
  const int Num = this->RotationCount();

  // In the case of the O shape, no rotation ever occurs.
  // Might as well generalize it to the rotation count here.
  if (Num == 1)
  {
    return false;
  }

  // Otherwise cycle through all rotations.
  this->Rotation = (this->Rotation + 1) % Num;

  return true;
}

const TArray<FIntPoint>& FShape::GetPoints() const
{
  return this->Points[this->Rotation];
}

const TArray<FIntPoint>& FShape::GetPointsFromNextRotation() const
{
  const int NextRotation = (this->Rotation + 1) % this->RotationCount();

  return this->Points[NextRotation];
}

int FShape::PointCount() const
{
  return this->GetPoints().Num();
}

inline TArray<TArray<FIntPoint>> FShape::ShapeO()
{
  return {
    { { 0, 0 }, { -1, 0 }, { -1, -1 }, { 0, -1 } }, // Rotated 0
  };
}

inline TArray<TArray<FIntPoint>> FShape::ShapeI()
{
  return {
    { { 0, 0 }, { -1, 0 }, { -2,  0 }, { 1,  0 } }, // Rotated 0
    { { 0, 0 }, {  0, 1 }, {  0, -1 }, { 0, -2 } }, // Rotated 90
  };
}

inline TArray<TArray<FIntPoint>> FShape::ShapeS()
{
  return {
    { { 0, 0 }, { 1, 0 }, { 0, -1 }, { -1, -1 } }, // Rotated 0
    { { 0, 0 }, { 0, 1 }, { 1,  0 }, {  1, -1 } }, // Rotated 90
  };
}

inline TArray<TArray<FIntPoint>> FShape::ShapeZ()
{
  return {
    { { 0, 0 }, { 0, -1 }, { -1, 0 }, { 1, -1 } }, // Rotated 0
    { { 0, 0 }, { 0, -1 }, {  1, 0 }, { 1,  1 } }, // Rotated 90
  };
}

inline TArray<TArray<FIntPoint>> FShape::ShapeL()
{
  return {
    { { 0, 0 }, { -1,  0 }, { -1, -1 }, {  1, 0 } }, // Rotated 0
    { { 0, 0 }, {  0, -1 }, {  1, -1 }, {  0, 1 } }, // Rotated 90
    { { 0, 0 }, { -1,  0 }, {  1,  0 }, {  1, 1 } }, // Rotated 180
    { { 0, 0 }, {  0, -1 }, {  0,  1 }, { -1, 1 } }  // Rotated 270
  };
}

inline TArray<TArray<FIntPoint>> FShape::ShapeJ()
{
  return {
    { { 0, 0 }, { -1,  0 }, {  1,  0 }, { 1, -1 } }, // Rotated 0
    { { 0, 0 }, {  0, -1 }, {  0,  1 }, { 1,  1 } }, // Rotated 90
    { { 0, 0 }, { -1,  0 }, { -1,  1 }, { 1,  0 } }, // Rotated 180
    { { 0, 0 }, {  0, -1 }, { -1, -1 }, { 0,  1 } }  // Rotated 270
  };
}

inline TArray<TArray<FIntPoint>> FShape::ShapeT()
{
  return {
    { { 0, 0 }, { -1,  0 }, { 0, -1 }, { 1, 0 } }, // Rotated 0
    { { 0, 0 }, {  0, -1 }, { 0,  1 }, { 1, 0 } }, // Rotated 90
    { { 0, 0 }, { -1,  0 }, { 0,  1 }, { 1, 0 } }, // Rotated 180
    { { 0, 0 }, { -1,  0 }, { 0, -1 }, { 0, 1 } }  // Rotated 270
  };
}
