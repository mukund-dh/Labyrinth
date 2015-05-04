// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "UnrealNetwork.h"
#include "LabyrinthGameMode.h"
#include "LabyrinthCharacter.h"

/** when you modify this, please note that this information can be saved with instances
* also DefaultEngine.ini [/Script/Engine.CollisionProfile] should match with this list **/
#define COLLISION_PROJECTILE			ECC_GameTraceChannel1
#define COLLISION_WEAPON				ECC_GameTraceChannel2


