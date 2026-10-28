// A clean Pawn base for Tempo-controlled robot Blueprints.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "TempoTestRobot.generated.h"

UCLASS(Blueprintable)
class TEMPOSAMPLE_API ATempoTestRobot : public APawn
{
	GENERATED_BODY()

public:
	ATempoTestRobot();
};
