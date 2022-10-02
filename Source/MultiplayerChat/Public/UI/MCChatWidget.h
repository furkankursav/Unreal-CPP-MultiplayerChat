// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableText.h"
#include "MCChatWidget.generated.h"



USTRUCT(BlueprintType)
struct FChatMessageData
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	FString SenderName;

	UPROPERTY(BlueprintReadWrite)
	FText ChatMessage;

	FChatMessageData()
	{
		
	}
	
	FChatMessageData(FString Sender, FText Message)
	{
		SenderName = Sender;
		ChatMessage = Message;
	}
	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMultiplayerChatMessageSent, const FChatMessageData&, MessageData);

/**
 * 
 */
UCLASS()
class MULTIPLAYERCHAT_API UMCChatWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess))
	class UScrollBox* ChatMessages;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess))
	UEditableText* EnterTextWidget;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess))
	class UBorder* BackgroundBorder;

	
	FOnMultiplayerChatMessageSent OnMultiplayerChatMessageSent;
	
public:
	UFUNCTION()
	void EnterTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	void HideChatWidget();
};
