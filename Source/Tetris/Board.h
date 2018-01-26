// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Board.generated.h"

UCLASS()
class TETRIS_API ABoard : public APawn
{
GENERATED_BODY()
	
public:	
  // Sets default values for this actor's properties
  ABoard();

protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere)
  int Columns = 10;

  UPROPERTY(EditAnywhere)
  int Rows = 20;

  UPROPERTY(EditAnywhere)
  int Level = 1;

  UPROPERTY(EditAnywhere)
  int ShowNextTetrominoCount = 4;

  UPROPERTY(EditAnywhere)
  FIntPoint StartingPosition = FIntPoint{ 5, 19 };

  UPROPERTY(EditAnywhere)
  FIntPoint TetrominoLocation = FIntPoint{ 5, 19 };

  // The Tetromino currently being dropped.
  UPROPERTY(BlueprintReadOnly)
  class ATetromino* CurrentTetromino = nullptr;

  // The Tetromino held Tetromino.
  UPROPERTY(BlueprintReadOnly)
  class ATetromino* HeldTetromino = nullptr;

  // The queued Tetrominoes.
  UPROPERTY(EditAnywhere)
  TArray<class ATetromino*> NextTetrominoes;

  // Build the whole board or just the sides?
  TArray<class ACell*> Border;

  // Representation of the cells of the board.
  TArray<class ACell*> Cells;

  UPROPERTY(EditAnywhere)
  int InitialLevel = 1;

  UPROPERTY(EditAnywhere)
  class USceneComponent* Scene = nullptr;

  UPROPERTY(EditAnywhere)
  class USceneComponent* BorderRoot = nullptr;

  UPROPERTY(EditAnywhere)
  class UCameraComponent* Camera = nullptr;

public:	
  // Called every frame
  virtual void Tick(float DeltaTime) override;

  void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

  UFUNCTION(BlueprintCallable)
  void MovePieceDown();

  UFUNCTION(BlueprintCallable)
  void MovePieceLeft();

  UFUNCTION(BlueprintCallable)
  void MovePieceRight();

  UFUNCTION(BlueprintCallable)
  void RotatePiece();

  UFUNCTION(BlueprintCallable)
  void HoldPiece();

  UFUNCTION(BlueprintCallable)
  void SlamPieceDown();

private:
  FTimerHandle DropTimerHandle;
  int CompletedRows = 0;
  bool UsedHeldPiece = false;

  void DropTick();

  int EarnedLevel() const;
  int ActualLevel() const;
  float GetCountdown() const;

  void SpawnBorderCellAt(FIntPoint Location);
  void SpawnBorder();
  class ATetromino* SpawnTetromino(const FVector& Location = FVector::ZeroVector);
  void SpawnNextTetrominoes();

  void CycleTetrominoes();
  ATetromino* const GetCurrentTetromino() const;

  int PointToIndex(FIntPoint Location) const;
  FVector BoardLocationToLocalSpace(FIntPoint Location) const;
  bool IsLocationWithinBounds(FIntPoint Location) const;

  class ACell* GetCellAtLocation(FIntPoint Location) const;
  void SetCellAtLocation(FIntPoint Location, class  ACell* Cell);
  void ClearCellAtLocation(FIntPoint Location);
  void DestroyCellAtLocation(FIntPoint Location);
  void FillCellAtLocation(FIntPoint Location, class UMaterialInstance* Material);

  void DrawPieceToBoard();

  bool IsRowFull(int Row) const;
  void ShiftRowDown(int Row);
  void ClearRow(int Row);
  void ClearFullRows();

  bool CollidesAtLocation(FIntPoint Location, const TArray<FIntPoint> &Points) const;

  bool CanMoveToLocation(FIntPoint Location) const;
  bool CanRotate() const;

  void MovePieceToLocation(FIntPoint Location);
};
