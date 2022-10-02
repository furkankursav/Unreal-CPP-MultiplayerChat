// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MCChatWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Border.h"
#include "Components/EditableText.h"
#include "GameFramework/PlayerState.h"

bool UMCChatWidget::Initialize()
{
	if(Super::Initialize() == false) return false;


	if(EnterTextWidget)
	{
		EnterTextWidget->OnTextCommitted.AddDynamic(this, &UMCChatWidget::EnterTextCommitted);
	}
	

	return true;
}

void UMCChatWidget::EnterTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if(CommitMethod == ETextCommit::OnEnter)
	{
		if(Text.IsEmpty())
		{
			HideChatWidget();
		}

		else
		{
			const FString Sender = GetOwningPlayer()->GetPlayerState<APlayerState>()->GetPlayerName();
			const FChatMessageData MessageData(Sender, Text);
			OnMultiplayerChatMessageSent.Broadcast(MessageData);
			EnterTextWidget->SetText(FText::GetEmpty());
			HideChatWidget();
		}
	}
}

void UMCChatWidget::HideChatWidget()
{
	if(GetOwningPlayer())
	{
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(GetOwningPlayer());
		if(BackgroundBorder)
		{
			BackgroundBorder->SetBrushColor(FLinearColor::Transparent);
		}
	}
}
