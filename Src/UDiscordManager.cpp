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
	P_FINISH; // Call even we don't pass any parameters.

	FString FilePath = appBaseDir();
	FilePath += TEXT("UDiscordManager.ini");

	FString _appId = GConfig->GetStr(TEXT("UDiscordManager"), TEXT("appId"), *FilePath);
	discord::ClientId appId = _atoi64(TCHAR_TO_ANSI(*_appId));

	if (appId == discord::ClientId()) {
		GLog->Log(TEXT("No config found. Make sure you have UDiscordManager.ini -> [UDiscordManager] appId=YourAppId"));
		AUDiscordManager::bInitialized = false;
	}
	else {
		discord::Result result = discord::Core::Create(appId, DiscordCreateFlags_NoRequireDiscord, &core);
		if (result != discord::Result::Ok) {
			GLog->Logf(TEXT("Discord initialize failed with result: result=%d"), result);
			AUDiscordManager::bInitialized = false;
		}
		else {
			GLog->Log(TEXT("Discord initialize success"));
			AUDiscordManager::bInitialized = true;
		}
	}

	*(UBOOL*)Result = AUDiscordManager::bInitialized;

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
		GLog->Log(TEXT("Discord is not initialized"));

		*(UBOOL*)Result = false;
		return;
	}

	discord::Activity activity{};

	if (details) activity.SetDetails(_details);
	if (state) activity.SetState(_state);
	if (startTime) activity.GetTimestamps().SetStart(startTime);

	activity.GetAssets().SetLargeImage("ut-logo");
	activity.GetAssets().SetLargeText("Unreal Tournament 99");
	activity.GetAssets().SetSmallImage("bunny-logo-mini");
	activity.GetAssets().SetSmallText("BunnyTrack");

	core->ActivityManager().UpdateActivity(activity, [this](discord::Result result) {
		if (result != discord::Result::Ok) {
			GLog->Logf(TEXT("Discord update activity failed with result: result=%d"), result);
		}
		else {
			GLog->Logf(TEXT("Discord update activity success"));
		}
	});

	*(UBOOL*)Result = true;

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
	P_FINISH; // Call even we don't pass any parameters.

	if (!AUDiscordManager::bInitialized) {
		GLog->Log(TEXT("Discord is not initialized"));

		*(UBOOL*)Result = false;
		return;
	}

	core->ActivityManager().ClearActivity([this](discord::Result result) {
		if (result != discord::Result::Ok) {
			GLog->Logf(TEXT("Discord clear activity failed with result: result=%d"), result);
		}
		else {
			GLog->Log(TEXT("Discord clear activity success"));
		}
	});

	*(UBOOL*)Result = true;

	unguard;
}
IMPLEMENT_FUNCTION(AUDiscordManager, -1, execclearActivity);

/*******************************************************************************
 *
 * @RETURN	bool
 *
 *******************************************************************************
 */
void AUDiscordManager::execrunCallbacks(FFrame& Stack, RESULT_DECL)
{
	guard(AUDiscordManager::execrunCallbacks);
	P_FINISH; // Call even we don't pass any parameters.

	if (!AUDiscordManager::bInitialized) {
		GLog->Log(TEXT("Discord is not initialized"));

		*(UBOOL*)Result = false;
		return;
	}

	//Run callbacks everytime we do some changes. You should call to this method on a Tick() or Timer() function
	core->RunCallbacks();

	*(UBOOL*)Result = true;

	unguard;
}
IMPLEMENT_FUNCTION(AUDiscordManager, -1, execrunCallbacks);