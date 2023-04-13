#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	// Add more weapon types later
	EWT_AssaultRifle UMETA(DisplayName = "Assault Rifle"),

	EWT_MAX UMETA(DisplayName = "DefaultMAX"),
};
