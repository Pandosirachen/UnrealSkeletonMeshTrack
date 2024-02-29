// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CorelinkUnrealWrapper.h"
#include "CorelinkActor.generated.h"

UCLASS()
class CORELINKSOURCE_API ACorelinkActor : public AActor
{
	GENERATED_BODY()
	Corelink::SendStream* sendStream1;
	Corelink::SendStream* sendStream2;
	Corelink::RecvStream* recvStream1;
	Corelink::RecvStream* recvStream2;

	bool success;
	int counter;
	long long last;

public:	
	// Sets default values for this actor's properties
	ACorelinkActor();
	~ACorelinkActor();

	void Print(const STREAM_ID& recv, const STREAM_ID& send, const char* msg, const int& msgLen);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Corelink Actor")
	FString CorelinkVar;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Corelink Actor")
	float CorelinkFloat;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
