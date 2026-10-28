// A clean Pawn base for Tempo-controlled robot Blueprints.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "TempoTestRobot.generated.h"

class UTempoLidar;
class UTempoROSNode;
class UStaticMeshComponent;
class USpringArmComponent;

UCLASS(Blueprintable)
class TEMPOSAMPLE_API ATempoTestRobot : public APawn
{
	GENERATED_BODY()

public:
	ATempoTestRobot();

	// Native Tempo lidar owned by this C++ robot class. It is named TempoRosLidar
	// in the Blueprint component tree, so it does not collide with the legacy
	// Blueprint component named TempoLidar.
	UFUNCTION(BlueprintPure, Category="Tempo|ROS")
	UTempoLidar* GetTempoLidar() const { return TempoLidarComponent; }

	UFUNCTION(BlueprintPure, Category="Tempo|Robot")
	UStaticMeshComponent* GetRobotBody() const { return RobotBody; }

	UFUNCTION(BlueprintPure, Category="Tempo|Robot")
	USpringArmComponent* GetRobotSpringArm() const { return RobotSpringArm; }

	// Node embedded in Unreal that will own the robot's ROS publishers and
	// subscriptions.
	UFUNCTION(BlueprintPure, Category="Tempo|ROS")
	UTempoROSNode* GetROSNode() const { return ROSNode; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Native collision and visual root. After compiling, move Blueprint-only
	// parts below this component and remove the legacy Blueprint Body.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tempo|Robot")
	TObjectPtr<UStaticMeshComponent> RobotBody;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tempo|Robot")
	TObjectPtr<USpringArmComponent> RobotSpringArm;

	// Visual parts migrated from BP_MyStreetSweeper. The Robot prefix keeps
	// their names distinct while the legacy Blueprint components still exist.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tempo|Robot|Visual")
	TObjectPtr<UStaticMeshComponent> RobotBrushLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tempo|Robot|Visual")
	TObjectPtr<UStaticMeshComponent> RobotBrushRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tempo|Robot|Visual")
	TObjectPtr<UStaticMeshComponent> RobotWheelFront;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tempo|Robot|Visual")
	TObjectPtr<UStaticMeshComponent> RobotWheelRearLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tempo|Robot|Visual")
	TObjectPtr<UStaticMeshComponent> RobotWheelRearRight;

	// Equivalent to the Blueprint Tick graph: degrees per second around local Z.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Tempo|Robot|Visual")
	float BrushRotationRateDegreesPerSecond = 180.0f;

	// Native lidar child of RobotSpringArm.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tempo|ROS")
	TObjectPtr<UTempoLidar> TempoLidarComponent;

	// Mount pose relative to BP_MyStreetSweeper's SpringArm. Unreal units are
	// centimetres and degrees. These values reproduce the legacy lidar mount.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Tempo|ROS|Lidar Mount")
	FVector LidarMountLocationCm = FVector(-24.228645, 0.0, 191.892339);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Tempo|ROS|Lidar Mount")
	FRotator LidarMountRotation = FRotator::ZeroRotator;

	// Kept as a UPROPERTY so Unreal's garbage collector retains the native ROS
	// node for as long as this robot exists.
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category="Tempo|ROS")
	TObjectPtr<UTempoROSNode> ROSNode;

	// A ROS graph-safe node name. It can be changed per Blueprint instance if
	// the level later contains more than one robot.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Tempo|ROS")
	FString ROSNodeName = TEXT("tempo_test_robot");
};
