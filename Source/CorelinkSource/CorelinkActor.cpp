// Fill out your copyright notice in the Description page of Project Settings.


#include "CorelinkActor.h"

static void StaticPrint(const STREAM_ID& recv, const STREAM_ID& send, const char* msg, const int& msgLen) {
	UE_LOG(LogTemp, Warning, TEXT("(Static) From %d to %d: %s"), recv, send, UTF8_TO_TCHAR(std::string(msg, msgLen).c_str()));
}

static void StaticForwarder(void* actor, const STREAM_ID& recv, const STREAM_ID& send, const char* msg, const int& msgLen) {
	((ACorelinkActor*)actor)->Print(recv, send, msg, msgLen);
}

// Sets default values
ACorelinkActor::ACorelinkActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Corelink::DLLInit::Init();
}


ACorelinkActor::~ACorelinkActor()
{
}

void ACorelinkActor::Print(const STREAM_ID& recv, const STREAM_ID& send, const char* msg, const int& msgLen)
{
	//UE_LOG(LogTemp, Warning, TEXT("(Member) From %d to %d, #%d: %s"), recv, send, ++counter, UTF8_TO_TCHAR(std::string(msg, msgLen).c_str()));
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, UTF8_TO_TCHAR(std::string(msg, msgLen).c_str()));
	CorelinkVar = UTF8_TO_TCHAR(std::string(msg, msgLen).c_str());
	CorelinkFloat += 0.1;
}

// Called when the game starts or when spawned
void ACorelinkActor::BeginPlay()
{
	success = false;
	counter = 0;

	Super::BeginPlay();

	last = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	/**
	 * Disable TCP streams (not using in this example)
	 */
	Corelink::DLLInit::setInitState(Corelink::Const::STREAM_STATE_UDP);

	Corelink::DLLInit::setServerCredentials("Testuser", "Testpassword");

	sendStream1 = nullptr;
	sendStream2 = nullptr;
	recvStream1 = nullptr;
	recvStream2 = nullptr;
	
	try {
		// set update and subscribe first
		auto lambdaU = [](const int& recvID, const int& sendID) {
			Corelink::Client::subscribe(recvID, sendID);
			UE_LOG(LogTemp, Warning, TEXT("setOnUpdate recvID:%d sendID:%d"), recvID, sendID);
		};
		Corelink::DLLInit::setOnUpdate(lambdaU);


		auto lambdaS = [](const int& recvID, const int& sendID) {
			Corelink::Client::subscribe(recvID, sendID);
			UE_LOG(LogTemp, Warning, TEXT("setOnSubscribe sendID:%d recvID:%d"), recvID, sendID);
		};
		Corelink::DLLInit::setOnSubscribe(lambdaS);
		UE_LOG(LogTemp, Warning, TEXT("%s"), UTF8_TO_TCHAR("setOnUpdate setOnSubscribe"));

		Corelink::Client::connect("216.165.12.41", 20010);
		//The old client cannot solve the ip
		//Corelink::Client::connect("corelink.hsrn.nyu.edu", 20010);
		//Corelink::Client::connect("127.0.0.1", 20010);

		//sendStream1 = new Corelink::SendStream(Corelink::Client::createSender("Holodeck", "distance", "Testing corelink sender in unreal", true, true, Corelink::Const::STREAM_STATE_SEND_UDP));
		//sendStream2 = new Corelink::SendStream(Corelink::Client::createSender("Chalktalk", "testing", "Testing corelink sender in unreal", true, true, Corelink::Const::STREAM_STATE_UDP));
		
		recvStream1 = new Corelink::RecvStream(Corelink::Client::createReceiver("Chalktalk", { "bones" }, "Testing corelink sender in unreal", true, true, Corelink::Const::STREAM_STATE_RECV_UDP));
		//workspace,types, meta, echo, alert, protocol


		//recvStream2 = new Corelink::RecvStream(Corelink::Client::createReceiver("Chalktalk", { "testing" }, "Testing corelink sender in unreal", true, true, Corelink::Const::STREAM_STATE_UDP));
		//workspace,types, meta, echo, alert,protocol)
		/**
		 * Example of setting recv to member function and then static function
		 */
		recvStream1->setOnReceive(&StaticForwarder, this);
		//recvStream2->setOnReceive(StaticPrint);

		success = true;
		UE_LOG(LogTemp, Warning, TEXT("Created Streams"));
	}
	catch (const Corelink::CorelinkException & e) {
		UE_LOG(LogTemp, Warning, TEXT("%s"), UTF8_TO_TCHAR(e.what()));
		sendStream1 = nullptr;
		sendStream2 = nullptr;
		recvStream1 = nullptr;
		recvStream2 = nullptr;
	}
}

void ACorelinkActor::BeginDestroy()
{
	success = false;
	Corelink::Client::cleanup();
	UE_LOG(LogTemp, Warning, TEXT("%s"), UTF8_TO_TCHAR("Cleaned up"));
	Super::BeginDestroy();
}

// Called every frame
void ACorelinkActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*
	long long t = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	std::vector<int> streams;
	std::string streamsOut;
	if (success) {
		if (last + 3 > t) { return; }
		last = t;
		if (last % 2 == 0) {
			sendStream1->send(std::to_string(DeltaTime));
			streams = Corelink::Client::listStreams({ "Holodeck" }, {});
			streamsOut = "Holodeck: ";
		}
		
		else {
			sendStream2->send("HI");
			streams = Corelink::Client::listStreams({ "Chalktalk" }, {});
			streamsOut = "Chalktalk: ";
		}
		
		for (int i = 0; i < streams.size(); ++i) {
			streamsOut += std::to_string(streams[i]) + " ";
		}

		UE_LOG(LogTemp, Warning, TEXT("%s"), UTF8_TO_TCHAR(streamsOut.c_str()));

		
	}
	*/
}

