// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage 
{
	GENERATED_BODY()
public:
	class UTexture2D* CrosshairCenter;
	UTexture2D* CrosshairLeft;
	UTexture2D* CrosshairRight;
	UTexture2D* CrosshairTop;
	UTexture2D* CrosshairBottom;
	float CrosshairSpread;
	FLinearColor CrosshairsColor;
};
/**
 * 
 */
UCLASS()
class BLASTWARS_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()
public:

	virtual void DrawHUD() override;

	void AddCharacterOverlay();
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<class UUserWidget> CharacterOverlayClass;
	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;

	void AddAnnouncement();
	UPROPERTY(EditAnywhere, Category = "Announcements")
	TSubclassOf<UUserWidget> AnnouncementClass;
	UPROPERTY()
	class UAnnouncement* Announcement;

	void AddEliminationAnnouncement(FString Attacker, FString Victim);

protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY()
	class APlayerController* OwningPlayer;

	FHUDPackage HUDPackage;

	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairsColor);

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UEliminationAnnouncement> EliminationAnnouncementClass;
	UPROPERTY(EditAnywhere)
	float EliminationAnnouncementTime = 1.5f;

	UFUNCTION()
	void EliminationAnnouncementTimerFinished(UEliminationAnnouncement* MessageToRemove);

	UPROPERTY()
	TArray<UEliminationAnnouncement*> EliminationMessages;

public:

	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; }
};
