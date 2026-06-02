// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SGWeapon.h"

ASGWeapon::ASGWeapon()
{
	PelletCount = 8;
}

void ASGWeapon::ApplyFire(const FVector& ViewLocation, const FVector& BaseDirection)
{
	const float SpreadRadians = FMath::DegreesToRadians(SpreadAngle);
	
	//여러 발 발사
	for (int32 i = 0; i < PelletCount; ++i){
		FVector ShotDirection = BaseDirection;
		
		// 탄 퍼짐, 명중 오차
		if (SpreadAngle > 0){
			ShotDirection = FMath::VRandCone(BaseDirection, SpreadRadians);
		}
		
		FireTrace(ViewLocation, ShotDirection);
	}
}
