// Fill out your copyright notice in the Description page of Project Settings.

#include "Labyrinth.h"
#include "ALWeapon.h"


// Sets default values
AALWeapon::AALWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AALWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AALWeapon::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

