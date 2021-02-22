// Copyright Thomas Eichinger 2020

#include "Grabber.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandle();
	SetupInputComponent();
}

// Sets up physics handle for physics manipulation
void UGrabber::FindPhysicsHandle()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("No physics handle component found on %s."), *GetOwner()->GetName());
	}
}

// Sets up input buttons that trigger grab event
void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
}

void UGrabber::Grab()
{
	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
	AActor* ActorHit = HitResult.GetActor();

	// If we hit something then attach the physics handle
	// Attach physics handle
	if (ActorHit) 
	{
		if (!PhysicsHandle) {return;}
		PhysicsHandle->GrabComponentAtLocation
			(
				ComponentToGrab,
				NAME_None,
				GetLineTraceEnd()
			);
	}
}

void UGrabber::Release()
{
	if (!PhysicsHandle) { return; }
	// Remove/release the physics handle.
	PhysicsHandle->ReleaseComponent();
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// If the physics handle is attached.
	if (!PhysicsHandle) { return; }
	if (PhysicsHandle->GrabbedComponent)
	{
		FVector LineTraceEnd = GetLineTraceEnd();
		PhysicsHandle->SetTargetLocation(LineTraceEnd);
	}
		// Move the object we are holding
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
	
	/* // Draws Out Visible Arm Reach Raycast when active
	DrawDebugLine(
		GetWorld(),
		PlayerLocation,
		LineTraceEnd,
		FColor(0, 255, 0),
		false,
		0.f,
		0,
		5.f
	);*/

	FHitResult Hit;

	// Ray-cast out to a certain distance (Reach)
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
	
	// Outputs Hit with object collided
	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		GetPlayerWorldPos(),
		GetLineTraceEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);

	return Hit;
}

// Gets End point vector of virtual arm grabber
FVector UGrabber::GetLineTraceEnd() const
{
	// Get players viewpoint
	FVector PlayerLocation;
	FRotator PlayerRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerLocation,
		OUT PlayerRotation
	);

	// Draw Line from player showing the reach
	return PlayerLocation + PlayerRotation.Vector() * Reach;
}

FVector UGrabber::GetPlayerWorldPos() const
{
	// Get players viewpoint
	FVector PlayerLocation;
	FRotator PlayerRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerLocation,
		OUT PlayerRotation
	);
	return PlayerLocation;
}