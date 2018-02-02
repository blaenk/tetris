// Fill out your copyright notice in the Description page of Project Settings.

#include "Tetromino.h"

#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"

#include "Runtime/Engine/Classes/Materials/MaterialInstance.h"

#include "Runtime/Core/Public/Math/UnrealMathUtility.h"

#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

#include "Cell.h"
#include "Shape.h"

UMaterialInstance* ATetromino::MaterialCyan = nullptr;
UMaterialInstance* ATetromino::MaterialBlue = nullptr;
UMaterialInstance* ATetromino::MaterialOrange = nullptr;
UMaterialInstance* ATetromino::MaterialYellow = nullptr;
UMaterialInstance* ATetromino::MaterialGreen = nullptr;
UMaterialInstance* ATetromino::MaterialPurple = nullptr;
UMaterialInstance* ATetromino::MaterialRed = nullptr;

// Sets default values
ATetromino::ATetromino()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;

  this->SceneComponent = this->CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
  this->SetRootComponent(this->SceneComponent);

  // TODO
  // There must be a better way to do this.
  if (!ATetromino::MaterialCyan)
  {
    static ConstructorHelpers::FObjectFinder<UMaterialInstance> FindCyan(TEXT("MaterialInstanceConstant'/Game/MI_Cyan.MI_Cyan'"));

    if (FindCyan.Succeeded())
    {
      ATetromino::MaterialCyan = FindCyan.Object;
    }
    else
    {
      UE_LOG(LogTemp, Error, TEXT("Couldn't find MI_Cyan."));
    }
  }

  if (!ATetromino::MaterialBlue)
  {
    static ConstructorHelpers::FObjectFinder<UMaterialInstance> FindBlue(TEXT("MaterialInstanceConstant'/Game/MI_Blue.MI_Blue'"));

    if (FindBlue.Succeeded())
    {
      ATetromino::MaterialBlue = FindBlue.Object;
    }
    else
    {
      UE_LOG(LogTemp, Error, TEXT("Couldn't find MI_Blue."));
    }
  }

  if (!ATetromino::MaterialOrange)
  {
    static ConstructorHelpers::FObjectFinder<UMaterialInstance> FindOrange(TEXT("MaterialInstanceConstant'/Game/MI_Orange.MI_Orange'"));

    if (FindOrange.Succeeded())
    {
      ATetromino::MaterialOrange = FindOrange.Object;
    }
    else
    {
      UE_LOG(LogTemp, Error, TEXT("Couldn't find MI_Orange."));
    }
  }

  if (!ATetromino::MaterialYellow)
  {
    static ConstructorHelpers::FObjectFinder<UMaterialInstance> FindYellow(TEXT("MaterialInstanceConstant'/Game/MI_Yellow.MI_Yellow'"));

    if (FindYellow.Succeeded())
    {
      ATetromino::MaterialYellow = FindYellow.Object;
    }
    else
    {
      UE_LOG(LogTemp, Error, TEXT("Couldn't find MI_Yellow."));
    }
  }

  if (!ATetromino::MaterialGreen)
  {
    static ConstructorHelpers::FObjectFinder<UMaterialInstance> FindGreen(TEXT("MaterialInstanceConstant'/Game/MI_Green.MI_Green'"));

    if (FindGreen.Succeeded())
    {
      ATetromino::MaterialGreen = FindGreen.Object;
    }
    else
    {
      UE_LOG(LogTemp, Error, TEXT("Couldn't find MI_Green."));
    }
  }

  if (!ATetromino::MaterialPurple)
  {
    static ConstructorHelpers::FObjectFinder<UMaterialInstance> FindPurple(TEXT("MaterialInstanceConstant'/Game/MI_Purple.MI_Purple'"));

    if (FindPurple.Succeeded())
    {
      ATetromino::MaterialPurple = FindPurple.Object;
    }
    else
    {
      UE_LOG(LogTemp, Error, TEXT("Couldn't find MI_Purple."));
    }
  }

  if (!ATetromino::MaterialRed)
  {
    static ConstructorHelpers::FObjectFinder<UMaterialInstance> FindRed(TEXT("MaterialInstanceConstant'/Game/MI_Red.MI_Red'"));

    if (FindRed.Succeeded())
    {
      ATetromino::MaterialRed = FindRed.Object;
    }
    else
    {
      UE_LOG(LogTemp, Error, TEXT("Couldn't find MI_Red."));
    }
  }
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
  if (!this->IsRotating || !this->RootComponent)
  {
    return;
  }

  FRotator Current = this->RootComponent->RelativeRotation;

  // FRotator Interpolation = FMath::RInterpConstantTo(Current, this->TargetRotation, GetWorld()->GetDeltaSeconds(), 360.0);
  FRotator Interpolation = FMath::RInterpTo(Current, this->TargetRotation, GetWorld()->GetDeltaSeconds(), 50.0);

  if (Interpolation.Equals(Current, 0.001))
  {
    this->SetActorRelativeRotation(this->TargetRotation);

    this->StopRotating();
  }
  else
  {
    this->SetActorRelativeRotation(Interpolation);
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
  if (!this->IsTranslating || !this->RootComponent)
  {
    return;
  }

  FVector Current = this->RootComponent->RelativeLocation;

  // FVector Interpolation = FMath::VInterpConstantTo(Current, this->TargetLocation, GetWorld()->GetDeltaSeconds(), 360.0);
  FVector Interpolation = FMath::VInterpTo(Current, this->TargetLocation, GetWorld()->GetDeltaSeconds(), 50.0);

  if (Interpolation.Equals(Current, 0.001))
  {
    this->SetActorRelativeLocation(this->TargetLocation);

    this->StopTranslating();
  }
  else
  {
    this->SetActorRelativeLocation(Interpolation);
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

void ATetromino::DestroyCells()
{
  for (auto cell : this->Cells)
  {
    cell->Destroy();
  }

  this->Cells.Empty();
}

void ATetromino::SpawnCells()
{
  this->DestroyCells();

  for (auto &&point : this->Shape.GetPoints())
  {
    ACell* Cell = this->GetWorld()->SpawnActor<ACell>(ACell::StaticClass(), this->LocationForPoint(point), FRotator::ZeroRotator);

    Cell->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);

    if (this->Material)
    {
      Cell->SetMaterial(this->Material);
    }

    this->Cells.Add(Cell);
  }
}

void ATetromino::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  this->DestroyCells();

  Super::EndPlay(EndPlayReason);
}

const FShape& ATetromino::GetShape() const
{
  return this->Shape;
}

UMaterialInstance * ATetromino::GetMaterial() const
{
  return this->Material;
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

void ATetromino::Reset()
{
  this->Shape.ResetRotation();
  this->StopRotating();
  this->StopTranslating();
  this->SetActorRelativeRotation(FRotator::ZeroRotator);
}

void ATetromino::MoveToLocation(const FVector& Location)
{
  this->ScheduleTranslationTo(Location);
}

void ATetromino::SetShape(EShapeType ShapeType)
{
  this->ShapeType = ShapeType;
  this->Reset();
  this->Shape = FShape{ this->ShapeType };

  switch (this->ShapeType)
  {
  case EShapeType::SHAPE_I:
    this->Material = this->MaterialCyan;
    break;

  case EShapeType::SHAPE_J:
    this->Material = this->MaterialBlue;
    break;

  case EShapeType::SHAPE_L:
    this->Material = this->MaterialOrange;
    break;

  case EShapeType::SHAPE_O:
    this->Material = this->MaterialYellow;
    break;

  case EShapeType::SHAPE_S:
    this->Material = this->MaterialGreen;
    break;

  case EShapeType::SHAPE_T:
    this->Material = this->MaterialPurple;
    break;

  case EShapeType::SHAPE_Z:
    this->Material = this->MaterialRed;
    break;
  }

  this->SpawnCells();
}

void ATetromino::UpdateShape()
{
  this->SetShape(this->ShapeType);
}

void ATetromino::SetRandomShape()
{
  EShapeType RandomShape = static_cast<EShapeType>(FMath::RandRange(0, static_cast<int>(EShapeType::SHAPE_Z)));

  this->SetShape(RandomShape);
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