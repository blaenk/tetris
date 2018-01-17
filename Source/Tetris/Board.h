// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Board.generated.h"

// TODO
// Is UMETA necessary?
// Does it pick up the name of the enumeration by default?

/*
* Enumeration for each movement direction.
*/
UENUM(BlueprintType)
enum class EDirection : uint8
{
  Up    UMETA(DisplayName = "Up"),
  Right UMETA(DisplayName = "Right"),
  Down  UMETA(DisplayName = "Down"),
  Left  UMETA(DisplayName = "Left")
};

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
  FIntPoint StartingPosition = FIntPoint{6, 20};

  UPROPERTY(EditAnywhere)
  FIntPoint TetrominoLocation = FIntPoint{ 6, 20 };

  // The Tetromino currently being dropped.
  UPROPERTY(BlueprintReadOnly)
  class ATetromino* CurrentTetromino = nullptr;

  // The Tetromino that was previously held.
  UPROPERTY(BlueprintReadOnly)
  class ATetromino* HeldTetromino = nullptr;

  // TODO
  // Consider spawning the full grid and using SetActorHiddenInGame?

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
  ACell* GetCellAtLocation(FIntPoint Location);

  bool IsValidLocation(FIntPoint Location);
  bool CanMoveToLocation(FIntPoint Location);
  bool CanRotate();

  void MovePieceToLocation(FIntPoint Location);
};
