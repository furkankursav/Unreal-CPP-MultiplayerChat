// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MCChatWidget.h"
#include "Blueprint/UserWidget.h"
#include "MCMainWidget.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERCHAT_API UMCMainWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess))
	UMCChatWidget* ChatWidget;

public:
	void StartTyping(class APlayerController* PlayerController);

	FORCEINLINE UMCChatWidget* GetChatWidget() const { return ChatWidget;}

	virtual bool Initialize() override;

private:
	UFUNCTION()
	void SendMessage(const FChatMessageData& MessageData);
	
};
