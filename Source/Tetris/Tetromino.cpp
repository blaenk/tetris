// Fill out your copyright notice in the Description page of Project Settings.

#include "Tetromino.h"

#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"

#include "Runtime/Core/Public/Math/UnrealMathUtility.h"

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

void ATetromino::ScheduleRotationTo(const FRotator& Rotation)
{
  this->IsRotating = true;
  this->TargetRotation = Rotation;
}

void ATetromino::StopRotating()
{
  this->IsRotating = false;
  this->TargetRotation = FRotator::ZeroRotator;
}

void ATetromino::InterpolateTargetRotation()
{
  if (!this->IsRotating)
  {
    return;
  }

  FRotator Current = this->GetActorRotation();

  // FRotator Interpolation = FMath::RInterpConstantTo(Current, this->TargetRotation, GetWorld()->GetDeltaSeconds(), 360.0);
  FRotator Interpolation = FMath::RInterpTo(Current, this->TargetRotation, GetWorld()->GetDeltaSeconds(), 50.0);

  if (Interpolation.Equals(Current, 0.001))
  {
    this->SetActorRotation(this->TargetRotation);

    this->StopRotating();
  }
  else
  {
    this->SetActorRotation(Interpolation);
  }
}

void ATetromino::ScheduleTranslationTo(const FVector& Location)
{
  this->IsTranslating = true;
  this->TargetLocation = Location;
}

void ATetromino::StopTranslating()
{
  this->IsTranslating = false;
  this->TargetLocation = FVector::ZeroVector;
}

void ATetromino::InterpolateTargetTranslation()
{
  if (!this->IsTranslating)
  {
    return;
  }

  FVector Current = this->GetActorLocation();

  // FVector Interpolation = FMath::VInterpConstantTo(Current, this->TargetLocation, GetWorld()->GetDeltaSeconds(), 360.0);
  FVector Interpolation = FMath::VInterpTo(Current, this->TargetLocation, GetWorld()->GetDeltaSeconds(), 50.0);

  if (Interpolation.Equals(Current, 0.001))
  {
    this->SetActorLocation(this->TargetLocation);

    this->StopTranslating();
  }
  else
  {
    this->SetActorLocation(Interpolation);
  }
}

// Called every frame
void ATetromino::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

  // TODO
  // Should these somehow be combined into a single FTransform interpolation?
  // Should these instead expose ways of working with Blueprint Timelines?
  // Does FTimeline help in any way?
  this->InterpolateTargetTranslation();
  this->InterpolateTargetRotation();
}

FVector ATetromino::LocationForPoint(FIntPoint point)
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

void ATetromino::Rotate()
{
  if (this->Shape.Rotate())
  {
    this->ScheduleRotationTo(FRotator(0, 0, this->Shape.GetRotation()));
  }
}

void ATetromino::MoveToLocation(const FVector& Location)
{
  this->ScheduleTranslationTo(Location);
}

void ATetromino::UpdateShape()
{
  this->SetActorRotation(FRotator::ZeroRotator);

  this->StopRotating();

  this->Shape = FShape{ this->ShapeType };
  this->SpawnCells();
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