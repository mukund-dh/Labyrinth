// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "LHUD.generated.h"

/**
 * 
 */
UCLASS()
class LABYRINTH_API ALHUD : public AHUD
{
	GENERATED_BODY()

public:

	ALHUD(const FObjectInitializer& ObjectInitializer);

	FCanvasIcon CenterDotIcon;
	
	// Main HUD update loop
	virtual void DrawHUD() override;

	void DrawCenterDot();

	// An event hook to call the HUD text events to display in the HUD.
	// Blueprint and HUD class must implement how to deal with this event
	UFUNCTION(BlueprintImplementableEvent, Category = "HUDEvents")
	void MessageReceived(const FString& TextMessage);
	
};
