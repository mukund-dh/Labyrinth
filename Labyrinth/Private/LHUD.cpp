// Fill out your copyright notice in the Description page of Project Settings.

#include "Labyrinth.h"
#include "Public/LHUD.h"
#include "Public/LPlayerController.h"


ALHUD::ALHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> HUDCenterDotObj(TEXT("/Game/UI/HUD/T_CenterDot_M.T_CenterDot_M"));

	CenterDotIcon = UCanvas::MakeIcon(HUDCenterDotObj.Object);
}


void ALHUD::DrawHUD()
{
	Super::DrawHUD();

	DrawCenterDot();
}

void ALHUD::DrawCenterDot()
{
	float CenterX = Canvas->ClipX / 2;
	float CenterY = Canvas->ClipY / 2;

	float CenterDotScale = 0.07f;

	ALPlayerController* PCOwner = Cast<ALPlayerController>(PlayerOwner);
	if (PCOwner)
	{
		ALabyrinthCharacter* Pawn = Cast<ALabyrinthCharacter>(PCOwner->GetPawn());
		if (Pawn)
		{
			Canvas->SetDrawColor(255, 255, 255, 255);
			Canvas->DrawIcon(CenterDotIcon, CenterX - CenterDotIcon.UL*CenterDotScale / 2.0f, CenterY - CenterDotIcon.VL*CenterDotScale / 2.0f, CenterDotScale);
		}
	}
}
