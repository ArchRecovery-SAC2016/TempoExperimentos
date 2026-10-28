// A clean Pawn base for Tempo-controlled robot Blueprints.

#include "TempoTestRobot.h"

#include "TempoLidar.h"
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

	if (!TempoLidarComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("%s could not create its native TempoRosLidar component."), *GetName());
		return;
	}

	// Go through TempoROS's exported factory instead of constructing rclcpp
	// NodeOptions in this game module. The factory keeps all rclcpp linkage
	// inside the TempoROS plugin.
	ROSNode = UTempoROSNodeBlueprintFunctionLibrary::CreateTempoROSNode(ROSNodeName, this);
	if (!ROSNode)
	{
		UE_LOG(LogTemp, Error, TEXT("%s found TempoLidar '%s', but could not create the TempoROS node '%s'."),
			*GetName(), *TempoLidarComponent->GetName(), *ROSNodeName);
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("%s connected TempoLidar '%s' to TempoROS node '%s'."),
		*GetName(), *TempoLidarComponent->GetName(), *ROSNodeName);
}

void ATempoTestRobot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	const FRotator BrushTurn(0.0f, BrushRotationRateDegreesPerSecond * DeltaTime, 0.0f);
	RobotBrushLeft->AddLocalRotation(BrushTurn);
	RobotBrushRight->AddLocalRotation(BrushTurn);
	
}

