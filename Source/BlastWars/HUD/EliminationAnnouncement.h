// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EliminationAnnouncement.generated.h"

/**
 * 
 */
UCLASS()
class BLASTWARS_API UEliminationAnnouncement : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetEliminationAnnouncementText(FString AttackerName, FString VictimeName);

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* AnnouncementBox;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AnnouncementText;
	
};
