// Fill out your copyright notice in the Description page of Project Settings.

#include "Board.h"

#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/GameFramework/Pawn.h"
#include "Runtime/Engine/Public/TimerManager.h"

#include "Runtime/Engine/Classes/Camera/CameraComponent.h"

#include "Cell.h"
#include "Tetromino.h"
#include "TetrisPlayerController.h"

// Sets default values
ABoard::ABoard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	this->PrimaryActorTick.bCanEverTick = true;

  this->AutoPossessPlayer = EAutoReceiveInput::Player0;

  this->Cells.SetNumZeroed(this->Rows * this->Columns, true);

  this->NextTetrominoes.SetNumZeroed(this->ShowNextTetrominoCount);

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
  FVector CameraCenter = this->BoardLocationToLocalSpace({ this->Columns / 2, this->Rows / 2 });
  CameraCenter.X = -220;

  this->Camera->SetRelativeLocation(CameraCenter);
  this->Camera->SetConstraintAspectRatio(false);
  this->Camera->SetAutoActivate(true);
}

void ABoard::CycleTetrominoes()
{
  this->CurrentTetromino->SetRandomShape();

  if (this->NextTetrominoes.Num() > 0)
  {
    ATetromino* NewLast = this->CurrentTetromino;

    this->NextTetrominoes[0]->MoveToLocation(this->CurrentTetromino->GetActorLocation());
    this->CurrentTetromino = this->NextTetrominoes[0];
    
    const int Length = this->NextTetrominoes.Num();

    for (int i = 1; i < Length; ++i)
    {
      this->NextTetrominoes[i]->MoveToLocation(this->NextTetrominoes[i - 1]->GetActorLocation());
      this->NextTetrominoes[i - 1] = this->NextTetrominoes[i];
    }

    NewLast->MoveToLocation(this->NextTetrominoes[Length - 1]->GetActorLocation());
    this->NextTetrominoes[Length - 1] = NewLast;
  }
}

ATetromino* ABoard::SpawnTetromino(const FVector& Location)
{
  ATetromino* Tetromino = this->GetWorld()->SpawnActor<ATetromino>(ATetromino::StaticClass(), Location, FRotator::ZeroRotator);
  Tetromino->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
  Tetromino->SetRandomShape();

  return Tetromino;
}

void ABoard::SpawnNextTetrominoes()
{
  const int BorderWidth = 1;
  const int Gap = 3;
  const float OffsetY = (this->Columns + BorderWidth + Gap) * ACell::SIZE;
  float OffsetZ = this->Rows * ACell::SIZE;

  for (int i = 0; i < this->NextTetrominoes.Num(); ++i)
  {
    const FVector Location(0, OffsetY, OffsetZ);

    this->NextTetrominoes[i] = this->SpawnTetromino(Location);

    OffsetZ -= 40;
  }
}

ATetromino* const ABoard::GetCurrentTetromino() const
{
  return this->CurrentTetromino;
}

// Called when the game starts or when spawned
void ABoard::BeginPlay()
{
  Super::BeginPlay();

  // TODO
  // This is hardcoding the first player controller? What about extra networked controllers?
  APlayerController* PlayerController = this->GetWorld()->GetFirstPlayerController();

  if (PlayerController)
  {
    PlayerController->SetViewTarget(this);

    this->EnableInput(PlayerController);
  }

  this->SpawnBorder();

  this->CurrentTetromino = this->SpawnTetromino();
  this->SpawnNextTetrominoes();

  this->MovePieceToLocation(this->StartingPosition);

  const float Countdown = this->GetCountdown();
  this->GetWorldTimerManager().SetTimer(this->DropTimerHandle, this, &ABoard::DropTick, Countdown, false, Countdown);
}

int ABoard::EarnedLevel() const
{
  if (this->CompletedRows <= 0)
  {
    return 1;
  }
  else if (this->CompletedRows >= 1 && this->CompletedRows <= 90)
  {
    return 1 + (this->CompletedRows - 1) / 10;
  }
  else
  {
    return 10;
  }
}

int ABoard::ActualLevel() const
{
  return FMath::Max(this->InitialLevel, this->EarnedLevel());
}

float ABoard::GetCountdown() const
{
  return 0.05f * (11 - this->ActualLevel());
}

FVector ABoard::BoardLocationToLocalSpace(FIntPoint Location) const
{
  return FVector(0, Location.X * ACell::SIZE, Location.Y * ACell::SIZE);
}

bool ABoard::IsLocationWithinBounds(FIntPoint Location) const
{
  return FMath::IsWithin(Location.X, 0, this->Columns) &&
         FMath::IsWithin(Location.Y, 0, this->Rows);
}

int ABoard::PointToIndex(FIntPoint Location) const
{
  return Location.X + Location.Y * this->Columns;
}

ACell* ABoard::GetCellAtLocation(FIntPoint Location) const
{
  if (this->IsLocationWithinBounds(Location))
  {
    return this->Cells[this->PointToIndex(Location)];
  }

  return nullptr;
}

void ABoard::SetCellAtLocation(FIntPoint Location, ACell* Cell)
{
  if (this->IsLocationWithinBounds(Location))
  {
    this->Cells[this->PointToIndex(Location)] = Cell;
  }
}

void ABoard::FillCellAtLocation(FIntPoint Location, class UMaterialInstance* Material)
{
  if (this->IsLocationWithinBounds(Location) && !this->GetCellAtLocation(Location))
  {
    ACell* Cell = this->GetWorld()->SpawnActor<ACell>(ACell::StaticClass(), this->BoardLocationToLocalSpace(Location), FRotator::ZeroRotator);

    Cell->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
    Cell->SetMaterial(Material);

    this->SetCellAtLocation(Location, Cell);
  }
}

void ABoard::ClearCellAtLocation(FIntPoint Location)
{
  if (this->IsLocationWithinBounds(Location))
  {
    this->Cells[this->PointToIndex(Location)] = nullptr;
  }
}

void ABoard::DestroyCellAtLocation(FIntPoint Location)
{
  if (this->IsLocationWithinBounds(Location))
  {
    ACell* Cell = this->GetCellAtLocation(Location);

    if (Cell)
    {
      Cell->Destroy();
    }
  }
}

bool ABoard::IsRowFull(int Row) const
{
  for (int j = 0; j < this->Columns; ++j)
  {
    if (!this->GetCellAtLocation({ j, Row }))
    {
      return false;
    }
  }

  return true;
}

void ABoard::ShiftRowDown(int Row)
{
  if (Row == 0)
  {
    return;
  }

  for (int i = 0; i < this->Columns; ++i)
  {
    FIntPoint Location{ i, Row };
    FIntPoint NewLocation = Location + FIntPoint{ 0, -1 };

    ACell* Cell = this->GetCellAtLocation(Location);

    if (Cell)
    {
      // Move the cell to its new location in the grid.
      this->ClearCellAtLocation(Location);
      this->SetCellAtLocation(NewLocation, Cell);

      // TODO
      // Add Translation Interpolation to ACell.
      // Perhaps make them a component? Does UMovementComponent do this?
      // Cell->MoveToLocation({ i, Row - 1 });

      // Move the cell to its new board-local location.
      Cell->SetActorLocation(this->BoardLocationToLocalSpace(NewLocation));
    }
  }
}

void ABoard::ClearRow(int Row)
{
  // Clear each cell in this row.
  for (int i = 0; i < this->Columns; ++i)
  {
    FIntPoint Location{ i, Row };

    this->DestroyCellAtLocation(Location);
    this->ClearCellAtLocation(Location);
  }

  // Go through each successively higher row and shift each cell down by one row.
  for (int i = Row + 1; i < this->Rows; ++i)
  {
    this->ShiftRowDown(i);
  }

  ++this->CompletedRows;
}

// TODO
// Should each row be removed one by one, or should the longest
// contiguous gap be removed as a whole?
// TODO
// Potential optimization is to only consider rows >= the lowest row the piece touches.
// Likely unnecessary.
void ABoard::ClearFullRows()
{
  for (int i = 0; i < this->Rows; ++i)
  {
    // Consider two full rows. The first one is cleared and the second one
    // is shifted into the first row's position, so the first row is again full
    // and must be cleared again.
    while (this->IsRowFull(i))
    {
      this->ClearRow(i);
    }
  }
}

void ABoard::DrawPieceToBoard()
{
  for (const auto &point : this->GetCurrentTetromino()->GetShape().GetPoints())
  {
    FIntPoint BoardPoint = this->TetrominoLocation + point;

    this->FillCellAtLocation(BoardPoint, this->GetCurrentTetromino()->GetMaterial());
  }
}

// TODO
// Implement "empty" ticks, such as after landing a piece.
void ABoard::DropTick()
{
  if (this->CanMoveToLocation(this->TetrominoLocation + FIntPoint{ 0, -1 }))
  {
    this->MovePieceDown();
  }
  else
  {
    this->DrawPieceToBoard();

    this->UsedHeldPiece = false;

    this->ClearFullRows();

    // Set a new random shape and move it to the starting position.
    this->CycleTetrominoes();

    // If it doesn't fit in the starting position, it's game over.
    if (this->CanMoveToLocation(this->StartingPosition))
    {
      this->MovePieceToLocation(this->StartingPosition);
    }
    else
    {
      UE_LOG(LogTemp, Error, TEXT("Game Over!"));

      APlayerController* const PlayerController = this->GetWorld()->GetFirstPlayerController();

      if (PlayerController)
      {
        PlayerController->SetPause(true);
      }
    }
  }

  const float Countdown = this->GetCountdown();
  this->GetWorldTimerManager().SetTimer(this->DropTimerHandle, this, &ABoard::DropTick, Countdown);
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

    InputComponent->BindAction("Hold Piece", IE_Pressed, this, &ABoard::HoldPiece);

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

bool ABoard::CollidesAtLocation(FIntPoint Location, const TArray<FIntPoint> &Points) const
{
  for (const auto &point : Points)
  {
    FIntPoint MovedPoint = Location + point;

    if (!this->IsLocationWithinBounds(MovedPoint) || this->GetCellAtLocation(MovedPoint))
    {
      return true;
    }
  }

  return false;
}

bool ABoard::CanMoveToLocation(FIntPoint Location) const
{
  const auto &Points = this->GetCurrentTetromino()->GetShape().GetPoints();

  return !this->CollidesAtLocation(Location, Points);
}

bool ABoard::CanRotate() const
{
  const auto &Points = this->GetCurrentTetromino()->GetShape().GetPointsFromNextRotation();

  return !this->CollidesAtLocation(this->TetrominoLocation, Points);
}

void ABoard::MovePieceToLocation(FIntPoint Location)
{
  if (this->CanMoveToLocation(Location))
  {
    // TODO
    // If non-CurrentTetromino pieces are moved, this will clobber its location.
    this->TetrominoLocation = Location;

    this->GetCurrentTetromino()->MoveToLocation(this->BoardLocationToLocalSpace(Location));
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
  if (this->CanRotate())
  {
    this->GetCurrentTetromino()->Rotate();
  }
}

void ABoard::HoldPiece()
{
  if (this->HeldTetromino)
  {
    if (!this->UsedHeldPiece)
    {
      // Replace (?) the current tetromino with the held piece.
      this->CurrentTetromino->MoveToLocation(this->HeldTetromino->GetActorLocation());

      ATetromino* Temporary = this->HeldTetromino;
      this->HeldTetromino = this->CurrentTetromino;
      this->CurrentTetromino = Temporary;

      this->MovePieceToLocation(this->StartingPosition);

      this->UsedHeldPiece = true;
    }
    else
    {
      // TODO
      // Show the user that they can't hold the piece that was just relinquished.
    }
  }
  else
  {
    // Save the current tetromino and cycle the tetrominoes.
    this->CurrentTetromino->MoveToLocation(FVector(0, -50, 100));
    this->HeldTetromino = this->CurrentTetromino;

    // Need to reset piece position.
    this->TetrominoLocation = this->StartingPosition;

    // Need to spawn this into the last spot of the NextTetrominoes.
    this->CurrentTetromino = this->SpawnTetromino(this->BoardLocationToLocalSpace(this->StartingPosition));
    this->CycleTetrominoes();
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