// A clean Pawn base for Tempo-controlled robot Blueprints.

#include "TempoTestRobot.h"

#include "Engine/CollisionProfile.h"
#include "Engine/World.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SlateFwd.h"
#include "Tickable.h"
#include "UObject/Package.h"

#include "TempoLidar.h"
#include "TempoROSCommonConverters.h"
#include "TempoROSNode.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/SpringArmComponent.h"
#include "UObject/ConstructorHelpers.h"

ATempoTestRobot::ATempoTestRobot()
{
	PrimaryActorTick.bCanEverTick = true;

	RobotBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RobotBody"));
	RootComponent = RobotBody;
	RobotBody->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BodyMesh(
		TEXT("/Game/StreetSweeper/Meshes/StreetSweeperBody.StreetSweeperBody"));
	if (BodyMesh.Succeeded())
	{
		RobotBody->SetStaticMesh(BodyMesh.Object);
	}

	RobotSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("RobotSpringArm"));
	RobotSpringArm->SetupAttachment(RobotBody);

	RobotBrushLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RobotBrushLeft"));
	RobotBrushLeft->SetupAttachment(RobotBody);
	RobotBrushLeft->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RobotBrushRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RobotBrushRight"));
	RobotBrushRight->SetupAttachment(RobotBody);
	RobotBrushRight->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RobotWheelFront = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RobotWheelFront"));
	RobotWheelFront->SetupAttachment(RobotBody);
	RobotWheelFront->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RobotWheelRearLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RobotWheelRearLeft"));
	RobotWheelRearLeft->SetupAttachment(RobotBody);
	RobotWheelRearLeft->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RobotWheelRearRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RobotWheelRearRight"));
	RobotWheelRearRight->SetupAttachment(RobotBody);
	RobotWheelRearRight->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BrushMesh(
		TEXT("/Game/StreetSweeper/Meshes/StreetSweeperBrush.StreetSweeperBrush"));
	if (BrushMesh.Succeeded())
	{
		RobotBrushLeft->SetStaticMesh(BrushMesh.Object);
		RobotBrushRight->SetStaticMesh(BrushMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> FrontWheelMesh(
		TEXT("/Game/StreetSweeper/Meshes/StreetSweeperFrontWheel.StreetSweeperFrontWheel"));
	if (FrontWheelMesh.Succeeded())
	{
		RobotWheelFront->SetStaticMesh(FrontWheelMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> RearWheelMesh(
		TEXT("/Game/StreetSweeper/Meshes/StreetSweeperRearWheel.StreetSweeperRearWheel"));
	if (RearWheelMesh.Succeeded())
	{
		RobotWheelRearLeft->SetStaticMesh(RearWheelMesh.Object);
		RobotWheelRearRight->SetStaticMesh(RearWheelMesh.Object);
	}

	TempoLidarComponent = CreateDefaultSubobject<UTempoLidar>(TEXT("TempoRosLidar"));
	TempoLidarComponent->SetupAttachment(RobotSpringArm);
	TempoLidarComponent->SetRelativeLocationAndRotation(LidarMountLocationCm, LidarMountRotation);
}

void ATempoTestRobot::BeginPlay()
{
	Super::BeginPlay();

	ROSNode = UTempoROSNode::Create(TEXT("TempoTestRobotSmoke"), this);
	if (!ROSNode)
	{
		UE_LOG(LogTemp, Error, TEXT("%s could not create its TempoROS smoke-test node."), *GetName());
		return;
	}

	ROSNode->AddPublisher<FString>(TEXT("/tempo_smoke"), FROSQOSProfile(), false);
	ROSNode->AddSubscription<FString>(TEXT("/tempo_echo"),
		TROSSubscriptionDelegate<FString>::CreateUObject(this, &ATempoTestRobot::HandleTempoEcho));

	// This is the standard ROS 2 velocity-command type. For this milestone the
	// command is deliberately logged only; it does not control the Pawn yet.
	ROSNode->AddSubscription<FTwist>(TEXT("/cmd_vel"),
		TROSSubscriptionDelegate<FTwist>::CreateUObject(this, &ATempoTestRobot::HandleCommandVelocity));
	ROSNode->AddPublisher<FTransform>(TEXT("/tempo_test_robot/pose"), FROSQOSProfile(), false);
	ROSNode->Publish<FString>(TEXT("/tempo_smoke"), TEXT("Hello from Unreal TempoROS"));
	PublishROSPose();
}

void ATempoTestRobot::HandleTempoEcho(const FString& Message)
{
	UE_LOG(LogTemp, Display, TEXT("TempoROS received: %s"), *Message);
}

void ATempoTestRobot::HandleCommandVelocity(const FTwist& Command)
{
	LatestCommandVelocity = Command;
	LatestCommandVelocity.LinearVelocity.X = FMath::Clamp(
		Command.LinearVelocity.X, -MaxLinearSpeedCmPerSecond, MaxLinearSpeedCmPerSecond);
	LatestCommandVelocity.AngularVelocity.Z = FMath::Clamp(
		Command.AngularVelocity.Z, -MaxYawRateDegreesPerSecond, MaxYawRateDegreesPerSecond);
	SecondsSinceLastCommandVelocity = 0.0f;
	bHasCommandVelocity = true;

	UE_LOG(LogTemp, Display,
		TEXT("TempoROS received /cmd_vel: forward=%.1f cm/s, yaw=%.1f deg/s%s"),
		LatestCommandVelocity.LinearVelocity.X, LatestCommandVelocity.AngularVelocity.Z,
		bEnableROSVelocityControl ? TEXT("") : TEXT(" (movement disabled)"));
}

void ATempoTestRobot::ApplyVelocityCommand(float DeltaTime)
{
	if (!bEnableROSVelocityControl || !bHasCommandVelocity)
	{
		return;
	}

	SecondsSinceLastCommandVelocity += DeltaTime;
	if (SecondsSinceLastCommandVelocity > CommandVelocityTimeoutSeconds)
	{
		bHasCommandVelocity = false;
		return;
	}

	// ROS convention for a ground robot is linear.x (forward) and angular.z
	// (yaw). TempoROS has already converted their units and handedness here.
	const FVector TranslationDelta = GetActorForwardVector() * LatestCommandVelocity.LinearVelocity.X * DeltaTime;
	FHitResult Hit;
	AddActorWorldOffset(TranslationDelta, true, &Hit);
	FRotator RotationDelta = FRotator::ZeroRotator;
	RotationDelta.Yaw = LatestCommandVelocity.AngularVelocity.Z * DeltaTime;
	AddActorWorldRotation(RotationDelta);

	if (Hit.IsValidBlockingHit())
	{
		const AActor* BlockingActor = Hit.GetActor();
		UE_LOG(LogTemp, Verbose, TEXT("TempoTestRobot movement blocked by %s"),
			BlockingActor ? *BlockingActor->GetName() : TEXT("world geometry"));
	}
}

void ATempoTestRobot::PublishROSPose()
{
	if (!ROSNode)
	{
		return;
	}

	const FTransform RobotTransform = GetActorTransform();
	ROSNode->Publish<FTransform>(TEXT("/tempo_test_robot/pose"), RobotTransform);
	ROSNode->PublishDynamicTransform(RobotTransform, TEXT("base_link"), TEXT("map"));
}

void ATempoTestRobot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ApplyVelocityCommand(DeltaTime);

	const FRotator BrushTurn(0.0f, BrushRotationRateDegreesPerSecond * DeltaTime, 0.0f);
	RobotBrushLeft->AddLocalRotation(BrushTurn);
	RobotBrushRight->AddLocalRotation(BrushTurn);

	if (!ROSNode)
	{
		return;
	}

	ROSPosePublishAccumulatorSeconds += DeltaTime;
	if (ROSPosePublishAccumulatorSeconds >= ROSPosePublishIntervalSeconds)
	{
		ROSPosePublishAccumulatorSeconds = 0.0f;
		PublishROSPose();
	}
}

