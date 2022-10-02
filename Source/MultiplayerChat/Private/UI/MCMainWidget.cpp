// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MCMainWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Border.h"
#include "MultiplayerChat/MultiplayerChatCharacter.h"
#include "UI/MCChatWidget.h"

bool UMCMainWidget::Initialize()
{
	if(Super::Initialize() == false) return false;

	if(GetChatWidget())
	{
		GetChatWidget()->OnMultiplayerChatMessageSent.AddDynamic(this, &ThisClass::SendMessage);
	}

	return true;
}


void UMCMainWidget::StartTyping(APlayerController* PlayerController)
{
	if(ChatWidget)
	{
		ChatWidget->SetVisibility(ESlateVisibility::Visible);
		const FLinearColor SemiTransparentBlackColor = FLinearColor(0.f, 0.f, 0.f, 0.5f);
		GetChatWidget()->BackgroundBorder->SetBrushColor(SemiTransparentBlackColor);
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(PlayerController, ChatWidget->EnterTextWidget, EMouseLockMode::LockAlways);
		ChatWidget->EnterTextWidget->SetFocus();
		PlayerController->SetShowMouseCursor(false);
	}
}

void UMCMainWidget::SendMessage(const FChatMessageData& MessageData)
{
	if(AMultiplayerChatCharacter* ChatCharacter = Cast<AMultiplayerChatCharacter>(GetOwningPlayerPawn()))
	{
		ChatCharacter->AddChatText(MessageData);
	}
}
