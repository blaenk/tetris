// Fill out your copyright notice in the Description page of Project Settings.

#include "Board.h"

#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/GameFramework/Pawn.h"

#include "Runtime/Engine/Classes/Camera/CameraComponent.h"

#include "Cell.h"
#include "Tetromino.h"

// Sets default values
ABoard::ABoard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	this->PrimaryActorTick.bCanEverTick = true;

  this->AutoPossessPlayer = EAutoReceiveInput::Player0;

  this->Cells.SetNumZeroed(this->Rows * this->Columns, true);

  // The border requires:
  // * Two side verticals, as tall as there are rows
  // * The bottom horizontal, as wide as there are columns
  // * One cell on each corner
  this->Border.Reserve(this->Rows * 2 + this->Columns + 2);

  this->bFindCameraComponentWhenViewTarget = true;

  this->Scene = this->CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
  this->SetRootComponent(Scene);

  this->BorderRoot = this->CreateDefaultSubobject<USceneComponent>(TEXT("BorderRoot"));
  this->BorderRoot->SetupAttachment(this->RootComponent);

  this->Camera = this->CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
  this->Camera->SetupAttachment(this->RootComponent);

  // TODO
  // Center this based on the center of the board
  // Use cameramanager to optimize this?
  FVector CameraCenter = this->BoardToLocal({ this->Columns / 2, this->Rows / 2 });
  CameraCenter.X = -220;

  this->Camera->SetRelativeLocation(CameraCenter);
  this->Camera->SetConstraintAspectRatio(false);
  this->Camera->SetAutoActivate(true);
}

// Called when the game starts or when spawned
void ABoard::BeginPlay()
{
  Super::BeginPlay();

  APlayerController* Controller = this->GetWorld()->GetFirstPlayerController();

  if (Controller)
  {
    Controller->SetViewTarget(this);

    this->EnableInput(Controller);
  }

  this->SpawnBorder();

  this->CurrentTetromino = this->GetWorld()->SpawnActor<ATetromino>(ATetromino::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
  this->CurrentTetromino->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);

  this->MovePieceToLocation(this->StartingPosition);
}

// Called every frame
void ABoard::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);
}

void ABoard::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
  Super::SetupPlayerInputComponent(InputComponent);

  if (InputComponent)
  {
    InputComponent->BindAction("Move Piece Down", IE_Pressed, this, &ABoard::MovePieceDown);
    InputComponent->BindAction("Move Piece Left", IE_Pressed, this, &ABoard::MovePieceLeft);
    InputComponent->BindAction("Move Piece Right", IE_Pressed, this, &ABoard::MovePieceRight);

    InputComponent->BindAction("Rotate Piece", IE_Pressed, this, &ABoard::RotatePiece);

    InputComponent->BindAction("Slam Piece Down", IE_Pressed, this, &ABoard::SlamPieceDown);
  }
}

void ABoard::SpawnBorderCellAt(FIntPoint Point)
{
  ACell* Cell = this->GetWorld()->SpawnActor<ACell>(ACell::StaticClass(), FVector(0, Point.X, Point.Y), FRotator::ZeroRotator);
    
  Cell->AttachToComponent(this->BorderRoot, FAttachmentTransformRules::KeepRelativeTransform);

  this->Border.Add(Cell);
}

void ABoard::SpawnBorder()
{
  // Draw the verticals.
  for (int i = 0; i < this->Rows; ++i)
  {
    // Left vertical.
    this->SpawnBorderCellAt({ -1 * ACell::SIZE, i * ACell::SIZE });

    // Right vertical.
    this->SpawnBorderCellAt({ this->Columns * ACell::SIZE, i * ACell::SIZE });
  }

  // Draw the bottom horizontal.
  for (int i = 0; i < this->Columns; ++i)
  {
    this->SpawnBorderCellAt({ i * ACell::SIZE, -1 * ACell::SIZE });
  }

  // Draw the empty corners.
  this->SpawnBorderCellAt({ -1 * ACell::SIZE, -1 * ACell::SIZE });
  this->SpawnBorderCellAt({ this->Columns * ACell::SIZE, -1 * ACell::SIZE });
}

// TODO
// Should the board grid be zero-based or not?
// It seems like it may be standard to be one-based
FVector ABoard::BoardToLocal(FIntPoint Location)
{
  return FVector(0, (Location.X - 1) * ACell::SIZE, (Location.Y - 1) * ACell::SIZE);
}

bool ABoard::IsValidLocation(FIntPoint Location)
{
  if (Location.X < 1 || Location.X > this->Columns)
  {
    UE_LOG(LogTemp, Warning, TEXT("Invalid X Location '%d'"), Location.X);

    return false;
  }

  if (Location.Y < 1 || Location.Y > this->Rows)
  {
    UE_LOG(LogTemp, Warning, TEXT("Invalid Y Location '%d'"), Location.Y);

    return false;
  }

  return true;
}

ACell* ABoard::GetCellAtLocation(FIntPoint Location)
{
  if (!this->IsValidLocation(Location))
  {
    return nullptr;
  }

  // TODO
  // Backing array is zero-based, grid is one-based.
  const int singleDimensionIndex = (Location.X - 1) + (Location.Y - 1) * this->Columns;

  return this->Cells[singleDimensionIndex];
}

// TODO
// DRY these.
bool ABoard::CanMoveToLocation(FIntPoint Location)
{
  for (const auto &point : this->CurrentTetromino->GetShape().GetPoints())
  {
    FIntPoint MovedPoint = Location + point;

    if (!this->IsValidLocation(MovedPoint))
    {
      return false;
    }

    ACell* CellAtPoint = this->GetCellAtLocation(MovedPoint);

    if (CellAtPoint)
    {
      return false;
    }
  }

  return true;
}

bool ABoard::CanRotate()
{
  for (const auto &point : this->CurrentTetromino->GetShape().GetPointsFromNextRotation())
  {
    FIntPoint MovedPoint = this->TetrominoLocation + point;

    if (!this->IsValidLocation(MovedPoint))
    {
      return false;
    }

    ACell* CellAtPoint = this->GetCellAtLocation(MovedPoint);

    if (CellAtPoint)
    {
      return false;
    }
  }

  return true;
}

void ABoard::MovePieceToLocation(FIntPoint Location)
{
  if (this->CanMoveToLocation(Location))
  {
    this->TetrominoLocation = Location;

    this->CurrentTetromino->SetActorLocation(this->BoardToLocal(Location));
  }
}

void ABoard::MovePieceDown()
{
  this->MovePieceToLocation(this->TetrominoLocation + FIntPoint{ 0, -1 });
}

void ABoard::MovePieceLeft()
{
  this->MovePieceToLocation(this->TetrominoLocation + FIntPoint{ -1, 0 });
}

void ABoard::MovePieceRight()
{
  this->MovePieceToLocation(this->TetrominoLocation + FIntPoint{ 1, 0 });
}

void ABoard::RotatePiece()
{
  // TODO
  // Reconcile the difference between this having the check externalized
  // and the above having the checks internalized.
  if (this->CanRotate())
  {
    this->CurrentTetromino->Rotate();
  }
}

void ABoard::SlamPieceDown()
{
  FIntPoint Down = this->TetrominoLocation;

  // Move down as long as it is possible.
  do
  {
    Down += FIntPoint{ 0, -1 };

    this->MovePieceToLocation(Down);
  } while (Down == this->TetrominoLocation);
}