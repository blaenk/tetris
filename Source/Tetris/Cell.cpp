// Fill out your copyright notice in the Description page of Project Settings.

#include "Cell.h"

#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

// Sets default values
ACell::ACell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

  this->Mesh = this->CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
  this->SetRootComponent(this->Mesh);

  static ConstructorHelpers::FObjectFinder<UStaticMesh> CellMesh(TEXT("StaticMesh'/Game/S_Box.S_Box'"));

  if (CellMesh.Succeeded())
  {
    this->Mesh->SetStaticMesh(CellMesh.Object);
  }
  else
  {
    UE_LOG(LogTemp, Error, TEXT("Couldn't find S_Box static mesh."));
  }
}

// Called when the game starts or when spawned
void ACell::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}