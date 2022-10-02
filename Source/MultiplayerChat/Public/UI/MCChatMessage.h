// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MCChatMessage.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERCHAT_API UMCChatMessage : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UTextBlock* SenderNameText;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UTextBlock* SenderMessage;
	
};
