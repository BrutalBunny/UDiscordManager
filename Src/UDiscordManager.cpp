#include <stdexcept>
#include <cstdlib>
#include <ctime>

#include "UDiscordManager.h"

IMPLEMENT_PACKAGE(UDiscordManager);
IMPLEMENT_CLASS(AUDiscordManager);

class UDiscordManagerProperties
{
public:
	struct ActivityProperties
	{
		FString details;
		FString state;
		int64_t startTime;

		FString largeImage;
		FString largeImageText;
		FString smallImage;
		FString smallImageText;
	};
	ActivityProperties activityProps{};

	discord::Core* core{};
	discord::Activity activity{};

	discord::ClientId getAppId()
	{
		if (this->appId == discord::ClientId()) {
			FString FilePath = appBaseDir();
			FilePath += TEXT("UDiscordManager.ini");

			FString _appId = GConfig->GetStr(TEXT("UDiscordManager"), TEXT("appId"), *FilePath);
			this->appId = _atoi64(TCHAR_TO_ANSI(*_appId));
		}

		return this->appId;
	}

private:
	discord::ClientId appId;
};
UDiscordManagerProperties DiscordProps;

AUDiscordManager::AUDiscordManager()
{
	this->bInitialized = false;
}

/*******************************************************************************
 *
 * @RETURN	int (https://discord.com/developers/docs/game-sdk/discord#data-models)
 *
 *******************************************************************************
 */
void AUDiscordManager::execinitDiscord(FFrame& Stack, RESULT_DECL)
{
	guard(AUDiscordManager::execinitDiscord);
	P_FINISH;

	discord::Result result = discord::Core::Create(DiscordProps.getAppId(), DiscordCreateFlags_NoRequireDiscord, &DiscordProps.core);
	if (result != discord::Result::Ok) {
		GLog->Logf(TEXT("initDiscord: failed with result: result=%d"), result);
		this->bInitialized = false;
	}
	else {
		this->bInitialized = true;
	}

	*(int*)Result = (int)result;

	unguard;
}
IMPLEMENT_FUNCTION(AUDiscordManager, -1, execinitDiscord);

/*******************************************************************************
 *
 * @RETURN	int (https://discord.com/developers/docs/game-sdk/discord#data-models)
 *
 *******************************************************************************
 */
void AUDiscordManager::execinitActivity(FFrame& Stack, RESULT_DECL)
{
	guard(AUDiscordManager::execinitActivity);
	P_FINISH;

	auto& activityProps = DiscordProps.activityProps;

	if (activityProps.startTime == 0) {
		activityProps.startTime = std::time(nullptr);
	}

	if (activityProps.details.Len() == 0) {
		activityProps.details = Level->Game->GameReplicationInfo->ServerName;
	}
	const ANSICHAR* DetailsANSI = TCHAR_TO_ANSI(*activityProps.details);

	if (activityProps.state.Len() == 0) {
		activityProps.state = Level->Game->GameReplicationInfo->GameName;
	}
	const ANSICHAR* StateANSI = TCHAR_TO_ANSI(*activityProps.state);
	
	// Assets included in https://discord.com/developers/applications/ 'Rich Presence section'
	if (activityProps.largeImage.Len() == 0) {
		FString FilePath = appBaseDir();
		FilePath += TEXT("UDiscordManager.ini");

		activityProps.largeImage = GConfig->GetStr(TEXT("UDiscordManager"), TEXT("Assets_largeImage"), *FilePath);
	}
	const ANSICHAR* LargeImageANSI = TCHAR_TO_ANSI(*activityProps.largeImage);
	
	if (activityProps.largeImageText.Len() == 0) {
		FString FilePath = appBaseDir();
		FilePath += TEXT("UDiscordManager.ini");

		activityProps.largeImageText = GConfig->GetStr(TEXT("UDiscordManager"), TEXT("Assets_largeImageText"), *FilePath);
	}
	const ANSICHAR* LargeImageTextANSI = TCHAR_TO_ANSI(*activityProps.largeImageText);

	if (activityProps.smallImage.Len() == 0) {
		FString FilePath = appBaseDir();
		FilePath += TEXT("UDiscordManager.ini");

		activityProps.smallImage = GConfig->GetStr(TEXT("UDiscordManager"), TEXT("Assets_smallImage"), *FilePath);
	}
	const ANSICHAR* SmallImageANSI = TCHAR_TO_ANSI(*activityProps.smallImage);

	if (activityProps.smallImageText.Len() == 0) {
		FString FilePath = appBaseDir();
		FilePath += TEXT("UDiscordManager.ini");

		activityProps.smallImageText = GConfig->GetStr(TEXT("UDiscordManager"), TEXT("Assets_smallImageText"), *FilePath);
	}
	const ANSICHAR* SmallImageTextANSI = TCHAR_TO_ANSI(*activityProps.smallImageText);

	if (!this->bInitialized) {
		*(int*)Result = -1;
		return;
	}

	discord::Activity activity = DiscordProps.activity;
	activity.SetDetails(DetailsANSI);
	activity.SetState(StateANSI);
	activity.GetTimestamps().SetStart(activityProps.startTime);
	activity.GetAssets().SetLargeImage(LargeImageANSI);
	activity.GetAssets().SetLargeText(LargeImageTextANSI);
	activity.GetAssets().SetSmallImage(SmallImageANSI);
	activity.GetAssets().SetSmallText(SmallImageTextANSI);

	DiscordProps.core->ActivityManager().UpdateActivity(activity, [Result = (int*)Result](discord::Result result) {
		if (result != discord::Result::Ok) {
			GLog->Logf(TEXT("initActivityManager: failed with result: result=%d"), result);
		}

		*Result = (int)result;
	});

	unguard;
}
IMPLEMENT_FUNCTION(AUDiscordManager, -1, execinitActivity);

/*******************************************************************************
 *
 * @PARAM	details			FString details to be displayed (Ex: Playing Solo)
 * @PARAM	state			FString state to be displayed (Ex: In Game)
 * @PARAM	startTime		Integer unix timestamp to be displayed. This parameter will display the elapsed time in your activity
 * @RETURN	int (https://discord.com/developers/docs/game-sdk/discord#data-models)
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
	P_FINISH;

	auto& activityProps = DiscordProps.activityProps;

	if (details.Len() > 0) {
		activityProps.details = details;
	}
	const ANSICHAR* DetailsANSI = TCHAR_TO_ANSI(*activityProps.details);

	if (state.Len() > 0) {
		activityProps.state = state;
	}
	const ANSICHAR* StateANSI = TCHAR_TO_ANSI(*activityProps.state);

	if (startTime != 0) {
		activityProps.startTime = startTime;
	}

	if (!this->bInitialized) {
		*(int*)Result = -1;
		return;
	}

	discord::Activity activity = DiscordProps.activity;
	activity.SetDetails(DetailsANSI);
	activity.SetState(StateANSI);
	activity.GetTimestamps().SetStart(activityProps.startTime);

	DiscordProps.core->ActivityManager().UpdateActivity(activity, [Result = (int*)Result](discord::Result result) {
		if (result != discord::Result::Ok) {
			GLog->Logf(TEXT("execupdateAcitivty: failed with result: result=%d"), result);
		}
		
		*Result = (int)result;
	});

	unguard;
}
IMPLEMENT_FUNCTION(AUDiscordManager, -1, execupdateAcitivty);

/*******************************************************************************
 *
 * @RETURN	int (https://discord.com/developers/docs/game-sdk/discord#data-models)
 *
 *******************************************************************************
 */
void AUDiscordManager::execclearActivity(FFrame& Stack, RESULT_DECL)
{
	guard(AUDiscordManager::execclearActivity);
	P_FINISH;

	if (!this->bInitialized) {
		*(int*)Result = -1;
		return;
	}

	DiscordProps.core->ActivityManager().ClearActivity([Result = (int*)Result](discord::Result result) {
		if (result != discord::Result::Ok) {
			GLog->Logf(TEXT("execclearActivity: failed with result: result=%d"), result);
		}

		*Result = (int)result;
	});

	unguard;
}
IMPLEMENT_FUNCTION(AUDiscordManager, -1, execclearActivity);

/*******************************************************************************
 *
 * @RETURN	int (https://discord.com/developers/docs/game-sdk/discord#data-models)
 *
 *******************************************************************************
 */
void AUDiscordManager::execopenGuildInvite(FFrame& Stack, RESULT_DECL)
{
	guard(AUDiscordManager::execopenGuildInvite);
	P_GET_STR(guildId);
	P_FINISH;

	const ANSICHAR* GuildIdANSI = TCHAR_TO_ANSI(*guildId);

	if (!this->bInitialized) {
		*(int*)Result = -1;
		return;
	}

	DiscordProps.core->OverlayManager().OpenGuildInvite(GuildIdANSI, [Result = (int*)Result](discord::Result result) {
		if (result != discord::Result::Ok) {
			GLog->Logf(TEXT("execopenGuildInvite: failed with result: result=%d"), result);
		}

		*Result = (int)result;
	});

	unguard;
}
IMPLEMENT_FUNCTION(AUDiscordManager, -1, execopenGuildInvite);

/*******************************************************************************
 *
 * @RETURN	int (https://discord.com/developers/docs/game-sdk/discord#data-models)
 *
 *******************************************************************************
 */
void AUDiscordManager::execrunCallbacks(FFrame& Stack, RESULT_DECL)
{
	guard(AUDiscordManager::execrunCallbacks);
	P_FINISH;

	if (!this->bInitialized) {
		*(int*)Result = -1;
		return;
	}

	// Run callbacks everytime we do some changes. You should call to this method on a Tick() or Timer() function
	discord::Result result = DiscordProps.core->RunCallbacks();
	if (result != discord::Result::Ok) {
		GLog->Logf(TEXT("runCallbacks: failed with result: result=%d"), result);
		this->bInitialized = false;
	}

	*(int*)Result = (int)result;

	unguard;
}
IMPLEMENT_FUNCTION(AUDiscordManager, -1, execrunCallbacks);