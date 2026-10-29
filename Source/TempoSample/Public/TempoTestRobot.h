// A clean Pawn base for Tempo-controlled robot Blueprints.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "TempoTestRobot.generated.h"

class UTempoLidar;
class UTempoROSNode;
class UStaticMeshComponent;
class USpringArmComponent;
struct FTwist;

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

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void HandleTempoEcho(const FString& Message);
	void HandleCommandVelocity(const FTwist& Command);
	void PublishROSPose();

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

	// Retained as a UPROPERTY so Unreal does not garbage-collect the ROS node.
	// This is a standalone TempoROS smoke test; it does not bridge movement or lidar.
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category="Tempo|ROS")
	TObjectPtr<UTempoROSNode> ROSNode;

	// Publish robot pose and TF at a modest rate. The transform is converted
	// from Unreal centimetres/left-handed coordinates to ROS metres/right-handed
	// coordinates by TempoROSCommonConverters.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Tempo|ROS")
	float ROSPosePublishIntervalSeconds = 0.1f;

	float ROSPosePublishAccumulatorSeconds = 0.0f;

};
