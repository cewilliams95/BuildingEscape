// Copyright Christopher Williams 2020

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

// Check for Physics Handle
void UGrabber::FindPhysicsHandle()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if(PhysicsHandle == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("No physics handle component found on %s"), *(GetOwner()->GetName()) )
	}
}

// Set up the Input Component and bind grab/release
void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if(InputComponent) {
			UE_LOG(LogTemp, Warning, TEXT("%s: Input Component found!"), *GetOwner()->GetName());
			InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
			InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("%s: Input Component missing!"), *GetOwner()->GetName());

		}
}

void UGrabber::Grab() 
{
	// Raycast and check if we reach any actors with PhysicsBody collision channel set
	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
	
	// If we get a hit
	if(HitResult.GetActor()) 
	{
		// Attach physics handle if we hit something
		PhysicsHandle->GrabComponentAtLocation
		(
			ComponentToGrab,
			NAME_None,
			GetPlayerReach()
		);
	}
}

void UGrabber::Release()
{
	PhysicsHandle->ReleaseComponent();
	
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// If phys handle attached
	if(PhysicsHandle->GrabbedComponent)
	{
		// Move obj we are holding
		PhysicsHandle->SetTargetLocation(GetPlayerReach());
	}

}

// Returns the line trace to the player's reach
FVector UGrabber::GetPlayerReach() const
{
	// Get viewpoint
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint
	(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);
	return PlayerViewPointLocation + (PlayerViewPointRotation.Vector() * Reach);
}

// Get player's position in World
FVector UGrabber::GetPlayerWorldPos() const {
	// Get viewpoint
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint
	(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	return PlayerViewPointLocation;
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{	
	FHitResult Hit;
	// Raycast to a distance
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		GetPlayerWorldPos(),
		GetPlayerReach(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);

	return Hit;
}

// DEBUG - Draw green line to show player reach (Doesn't work now but left in for reference)
// USAGE: DrawReachHelper(PlayerViewPointLocation, GetPlayerReach());
void UGrabber::DrawReachHelper(FVector Start, FVector End) {
	DrawDebugLine(
		GetWorld(),
		Start,
		End,
		FColor(30,255,30),
		false,
		0.0f,
		0,
		5.0f
	);
}