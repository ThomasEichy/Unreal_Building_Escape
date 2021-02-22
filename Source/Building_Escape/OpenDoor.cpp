// Copyright Thomas Eichinger 2020


#include "OpenDoor.h"
#include "Components/AudioComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

#define OUT

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	InitialYaw = GetOwner()->GetActorRotation().Yaw;
	OpenAngle += InitialYaw;
	CurrentYaw = InitialYaw;

	PressurePlateCheck();

	FindAudioComponent();
}

void UOpenDoor::PressurePlateCheck()
{
	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("PressurePlate is Null."));
	}
}

void UOpenDoor::FindAudioComponent()
{
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();

	if (!AudioComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has no audio component!"), *GetOwner()->GetName());
	}
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (TotalMassOfActors() > TriggerWeight)
	{
		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds();
	}
	else
	{
		// if the door has been open longer than DoorClose Delay
		if (GetWorld()->GetTimeSeconds() - DoorLastOpened > DoorCloseDelay)
		{
			CloseDoor(DeltaTime);
		}
	}
}

void UOpenDoor::OpenDoor(float DeltaTime)
{
	CloseDoorSound = false;
	if(!AudioComponent) { return; }
	if (!OpenDoorSound)
	{
		AudioComponent->Play();
		OpenDoorSound = true;
	}
	
	CurrentYaw = FMath::FInterpTo(CurrentYaw, OpenAngle, DeltaTime, DoorOpeningSpeed);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);

	//CurrentRotation.Yaw = 90.f;
	//FRotator OpenDoor(0.f, 90.f, 0.f);
}

void UOpenDoor::CloseDoor(float DeltaTime)
{
	CurrentYaw = FMath::FInterpTo(CurrentYaw, InitialYaw, DeltaTime, DoorClosingSpeed);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);
	
	OpenDoorSound = false;
	if (!AudioComponent) { return; }
	if (!CloseDoorSound)
	{
		AudioComponent->Play();
		CloseDoorSound = true;
	}
	
}

float UOpenDoor::TotalMassOfActors() const
{
	float TotalMass = 0.f;

	// Find all overlapping actors
	TArray<AActor*> OverlappingActors;
	if (!PressurePlate) 
	{ 
		UE_LOG(LogTemp, Error, TEXT("PressurePlate is null"));
		return TotalMass;
	}
	PressurePlate->GetOverlappingActors(
		OUT OverlappingActors
	);


	// Add up all their masses
	for (AActor* Actor : OverlappingActors)
	{
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();;
	};

	return TotalMass;
}
