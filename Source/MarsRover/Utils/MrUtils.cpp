#include "MrUtils.h"
#include "GameCont.h"
#include "DrawDebugHelpers.h"



void UMrUtils::DrawLine(FVector _from, FVector _to, FColor _color, bool _persistance)
{
	if (AGameCont::INS->DebugMode < 1) return;
	DrawDebugLine(
		AGameCont::INS->GetWorld(),
		_from,
		_to,
		_color,
		_persistance, -1, 0,
		1
	);
}

void UMrUtils::DrawPoint(FVector _loc,  FColor _color, float _size, bool _persistance)
{
	if (AGameCont::INS->DebugMode < 1) return;
	DrawDebugPoint(
		AGameCont::INS->GetWorld(),
		_loc,
		_size,
		_color,
		_persistance, -1, 0
	);
}



