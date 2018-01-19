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
  FIntPoint StartingPosition = FIntPoint{ 5, 19 };

  UPROPERTY(EditAnywhere)
  FIntPoint TetrominoLocation = FIntPoint{ 5, 19 };

  // The Tetromino currently being dropped.
  UPROPERTY(BlueprintReadOnly)
  class ATetromino* CurrentTetromino = nullptr;

  // The Tetromino that was previously held.
  UPROPERTY(BlueprintReadOnly)
  class ATetromino* HeldTetromino = nullptr;

  // Build the whole board or just the sides?
  TArray<class ACell*> Border;

  // Representation of the cells of the board.
  TArray<class ACell*> Cells;

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
  void SlamPieceDown();

private:
  void SpawnBorderCellAt(FIntPoint Location);
  void SpawnBorder();

  FVector BoardToLocal(FIntPoint Location);
  ACell* GetCellAtLocation(FIntPoint Location) const;

  bool IsLocationWithinBounds(FIntPoint Location) const;

  bool CollidesAtLocation(FIntPoint Location, const TArray<FIntPoint> &Points) const;

  bool CanMoveToLocation(FIntPoint Location) const;
  bool CanRotate() const;

  void MovePieceToLocation(FIntPoint Location);
};
