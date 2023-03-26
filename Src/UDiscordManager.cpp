#include "UDiscordManager.h"
#include <cstdlib>

IMPLEMENT_PACKAGE(UDiscordManager);
IMPLEMENT_CLASS(AUDiscordManager);

/* Global variable */
discord::Core* core{};

/*******************************************************************************
 *
 * @PARAM	appId           String Discord Client Application ID
 * @RETURN	bool
 *
 *******************************************************************************
 */
void AUDiscordManager::execinitDiscord(FFrame& Stack, RESULT_DECL)
{
	guard(AUDiscordManager::execinitDiscord);

	// Parse the UnrealScript parameters.
	P_GET_STR(appId);
	P_FINISH; // You must call me when finished parsing all parameters.

	char* _appId = TCHAR_TO_ANSI(*appId); //Convert from some FString to char*
	int64_t _clientId = std::strtoll(_appId, nullptr, 10);

	discord::Result result = discord::Core::Create(_clientId, DiscordCreateFlags_NoRequireDiscord, &core);
	if (result != discord::Result::Ok) {
		FString Msg = FString::Printf(TEXT("Discord initialize failed with result: result=%d"), result);
		this->eventBroadcastMessage(Msg, 0, NAME_None);

		AUDiscordManager::bInitialized = false;
	}
	else {
		FString Msg2 = FString::Printf(TEXT("Discord initialize success with appId: appId=%d"), appId);
		this->eventBroadcastMessage(Msg2, 0, NAME_None);

		AUDiscordManager::bInitialized = true;
	}

	*(UBOOL*) Result = AUDiscordManager::bInitialized;

	unguard;
}
IMPLEMENT_FUNCTION(AUDiscordManager, -1, execinitDiscord);

/*******************************************************************************
 *
 * @PARAM	details			FString details to be displayed (Ex: Playing Solo)
 * @PARAM	state				FString state to be displayed (Ex: In Game)
 * @PARAM	startTime         Integer unix timestamp to be displayed. This parameter will display the elapsed time in your activity
 * @RETURN	bool
 *
 *******************************************************************************
 */
void AUDiscordManager::execupdateAcitivty(FFrame& Stack, RESULT_DECL)
{
	guard(AUDiscordManager::execupdateAcitivty);

	// Parse the UnrealScript parameters.
	P_GET_STR(details);
	P_GET_STR(state);
	P_GET_INT(startTime);
	P_FINISH; // You must call me when finished parsing all parameters.

	char* _details = TCHAR_TO_ANSI(*details); //Convert from some FString to char*
	char* _state = TCHAR_TO_ANSI(*state); //Convert from some FString to char*

	if (!AUDiscordManager::bInitialized) {
		*(UBOOL*) Result = false;
		return;
	}

	discord::Activity activity{};

	if (state) activity.SetState(_details);
	if (details) activity.SetDetails(_state);
	if (startTime) activity.GetTimestamps().SetStart(startTime);

	activity.GetAssets().SetLargeImage("ut-logo");
	activity.GetAssets().SetLargeText("Unreal Tournament 99");
	activity.GetAssets().SetSmallImage("bunny-logo-mini");
	activity.GetAssets().SetSmallText("BunnyTrack");

	core->ActivityManager().UpdateActivity(activity, [this](discord::Result result) {
		if (result != discord::Result::Ok) {
			FString Msg = FString::Printf(TEXT("Discord update activity failed with result: result=%d"), result);
			this->eventBroadcastMessage(Msg, 0, NAME_None);
		}
		else {
			FString Msg = FString::Printf(TEXT("Discord update activity success"));
			this->eventBroadcastMessage(Msg, 0, NAME_None);
		}
	});

	//Run callbacks everytime we do some changes
	core->RunCallbacks();

	*(UBOOL*) Result = true;

	unguard;
}
IMPLEMENT_FUNCTION(AUDiscordManager, -1, execupdateAcitivty);

/*******************************************************************************
 *
 * @RETURN	bool
 *
 *******************************************************************************
 */
void AUDiscordManager::execclearActivity(FFrame& Stack, RESULT_DECL)
{
	guard(AUDiscordManager::execupdateAcitivty);

	if (!AUDiscordManager::bInitialized) {
		*(UBOOL* )Result = false;
		return;
	}

	core->ActivityManager().ClearActivity([this](discord::Result result) {
		if (result != discord::Result::Ok) {
			FString Msg = FString::Printf(TEXT("Discord clear activity failed with result: result=%d"), result);
			this->eventBroadcastMessage(Msg, 0, NAME_None);
		}
		else {
			FString Msg = FString::Printf(TEXT("Discord clear activity success"));
			this->eventBroadcastMessage(Msg, 0, NAME_None);
		}
	});

	//Run callbacks everytime we do some changes
	core->RunCallbacks();

	*(UBOOL*)Result = true;

	unguard;
}
IMPLEMENT_FUNCTION(AUDiscordManager, -1, execclearActivity);