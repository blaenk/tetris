// Fill out your copyright notice in the Description page of Project Settings.

#include "TetrisGameModeBase.h"

#include "TetrisPlayerController.h"
#include "Board.h"

ATetrisGameModeBase::ATetrisGameModeBase()
{
  this->DefaultPawnClass = ABoard::StaticClass();
  this->PlayerControllerClass = ATetrisPlayerController::StaticClass();
}

