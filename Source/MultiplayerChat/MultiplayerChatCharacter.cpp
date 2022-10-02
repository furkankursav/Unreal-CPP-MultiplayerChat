// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerChatCharacter.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Public/UI/MCMainWidget.h"
#include "UI/MCChatMessage.h"

//////////////////////////////////////////////////////////////////////////
// AMultiplayerChatCharacter

AMultiplayerChatCharacter::AMultiplayerChatCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMultiplayerChatCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AMultiplayerChatCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AMultiplayerChatCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AMultiplayerChatCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AMultiplayerChatCharacter::LookUpAtRate);
}

void AMultiplayerChatCharacter::AddChatText(const FChatMessageData& MessageData)
{
	Server_AddChatText(MessageData);
}

void AMultiplayerChatCharacter::StartTypeMessage()
{
	if(APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if(MainWidget)
		{
			MainWidget->StartTyping(PC);
		}
	}
	
}

void AMultiplayerChatCharacter::MC_AddChatText_Implementation(const FChatMessageData& MessageData)
{
	TArray<UUserWidget*> FoundedWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, FoundedWidgets, MainWidgetClass);
	TArray<UMCMainWidget*> MainWidgets;
	for(UUserWidget* NextWidget : FoundedWidgets)
	{
		if(UMCMainWidget* MainWidgetToAdd = Cast<UMCMainWidget>(NextWidget))
		{
			MainWidgets.Add(MainWidgetToAdd);
		}
	}

	UMCChatMessage* ChatMessage = CreateWidget<UMCChatMessage>(GetWorld(), ChatMessageClass);
	if(ChatMessage)
	{
		ChatMessage->SenderNameText->SetText(FText::FromString(MessageData.SenderName));
		ChatMessage->SenderMessage->SetText(MessageData.ChatMessage);
	}
	
	for(UMCMainWidget* NextMainWidget : MainWidgets)
	{
		NextMainWidget->GetChatWidget()->ChatMessages->AddChild(ChatMessage);
		NextMainWidget->GetChatWidget()->SetVisibility(ESlateVisibility::Visible);
		NextMainWidget->GetChatWidget()->ChatMessages->ScrollToEnd();
	}
}

void AMultiplayerChatCharacter::Server_AddChatText_Implementation(const FChatMessageData& MessageData)
{
	MC_AddChatText(MessageData);
}

void AMultiplayerChatCharacter::BeginPlay()
{
	Super::BeginPlay();

	if(IsLocallyControlled() && IsValid(MainWidgetClass))
	{
		MainWidget = CreateWidget<UMCMainWidget>(GetWorld(), MainWidgetClass);
		if(MainWidget)
		{
			MainWidget->AddToViewport();
		}
	}
}



void AMultiplayerChatCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AMultiplayerChatCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AMultiplayerChatCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMultiplayerChatCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
