// Fill out your copyright notice in the Description page of Project Settings.

#include "Tetromino.h"

#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"

#include "Cell.h"
#include "Shape.h"

// Sets default values
ATetromino::ATetromino()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;

  this->SceneComponent = this->CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
  this->SetRootComponent(this->SceneComponent);
}

// Called when the game starts or when spawned
void ATetromino::BeginPlay()
{
	Super::BeginPlay();

  this->UpdateShape();
}

// Called every frame
void ATetromino::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector ATetromino::LocationForPoint(const FIntPoint& point)
{
  const float OffsetY = point.X * ACell::SIZE;
  const float OffsetZ = point.Y * ACell::SIZE;

  return { 0.0f, OffsetY, OffsetZ };
}

void ATetromino::SpawnCells()
{
  if (this->Cells.Num() > 0)
  {
    for (auto &&cell : this->Cells)
    {
      cell->Destroy();
    }

    this->Cells.Empty();
  }

  for (auto &&point : this->Shape.GetPoints())
  {
    ACell* Cell = this->GetWorld()->SpawnActor<ACell>(ACell::StaticClass(), this->LocationForPoint(point), FRotator::ZeroRotator);

    Cell->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);

    this->Cells.Add(Cell);
  }
}

const FShape& ATetromino::GetShape() const
{
  return this->Shape;
}

void ATetromino::SetCellLocations()
{
  const auto& points = this->Shape.GetPoints();

  if (points.Num() != this->Cells.Num())
  {
    UE_LOG(LogTemp, Warning, TEXT("Cell count-to-point count mismatch for Tetromino '%s'"), *this->GetName());

    return;
  }

  for (int i = 0; i < points.Num(); ++i)
  {
    this->Cells[i]->SetActorRelativeLocation(this->LocationForPoint(points[i]));
  }
}

// TODO
// Just literally rotate it? But care for rotation origin.
void ATetromino::Rotate()
{
  this->Shape.Rotate();
  this->SetCellLocations();
}

void ATetromino::UpdateShape()
{
  this->Shape = FShape{ this->ShapeType };
  this->SpawnCells();
}

void ATetromino::SetShapeType(EShapeType ShapeType)
{
  this->ShapeType = ShapeType;
  this->UpdateShape();
}

#if WITH_EDITOR
void ATetromino::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
  FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

  if (PropertyName == GET_MEMBER_NAME_CHECKED(ATetromino, ShapeType))
  {
    this->UpdateShape();

    UE_LOG(LogTemp, Warning, TEXT("Updated Shape for Tetromino '%s'"), *this->GetName());
  }

  Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif