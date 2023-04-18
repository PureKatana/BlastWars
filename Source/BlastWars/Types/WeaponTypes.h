#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	// Add more weapon types later
	EWT_AssaultRifle UMETA(DisplayName = "Assault Rifle"),
	EWT_RocketLauncher UMETA(DisplayName = "Rocket Launcher"),
	EWT_Pistol UMETA(DisplayName = "Pistol"),
	EWT_SubmachineGun UMETA(DisplayName = "SubmachineGun"),

	EWT_MAX UMETA(DisplayName = "DefaultMAX"),
};