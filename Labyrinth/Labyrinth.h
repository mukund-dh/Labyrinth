// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "UnrealNetwork.h"
#include "LabyrinthGameMode.h"
#include "LabyrinthCharacter.h"
#include "LWeapon.h"
#include "LWeaponPickup.h"

/** when you modify this, please note that this information can be saved with instances
* also DefaultEngine.ini [/Script/Engine.CollisionProfile] should match with this list **/
#define COLLISION_PROJECTILE			ECC_GameTraceChannel1
#define COLLISION_WEAPON				ECC_GameTraceChannel2

/** when you modify this, please note that this information can be saved with instances
* also DefaultEngine.ini [/Script/Engine.PhysicsSettings] should match with this list **/
#define SURFACE_DEFAULT				SurfaceType_Default
#define SURFACE_FLESH				SurfaceType1


