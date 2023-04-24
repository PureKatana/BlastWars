// Fill out your copyright notice in the Description page of Project Settings.


#include "EliminationAnnouncement.h"
#include "Components/TextBlock.h"

void UEliminationAnnouncement::SetEliminationAnnouncementText(FString AttackerName, FString VictimeName)
{
	FString ElimimationAnnouncementText = FString::Printf(TEXT("%s eliminated %s!"), *AttackerName, *VictimeName);
	if (AnnouncementText)
	{
		AnnouncementText->SetText(FText::FromString(ElimimationAnnouncementText));
	}
}
