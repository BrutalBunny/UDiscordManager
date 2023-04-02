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


void AUDiscordManager::_initDiscord()
{
	discord::Result result = discord::Core::Create(DiscordProps.getAppId(), DiscordCreateFlags_NoRequireDiscord, &DiscordProps.core);
	if (result != discord::Result::Ok) {
		GLog->Logf(TEXT("initDiscord: failed with result: result=%d"), result);
		this->bInitialized = false;
		return;
	}

	GLog->Log(TEXT("initDiscord: success"));
	this->bInitialized = true;

	_initActivityManager();
}

void AUDiscordManager::_initActivityManager()
{
	discord::Activity activity = DiscordProps.activity;
	auto& activityProps = DiscordProps.activityProps;

	if (activityProps.details.Len() == 0) {
		activityProps.details = Level->Game->GameReplicationInfo->ServerName;
	}
	const ANSICHAR* DetailsANSI = TCHAR_TO_ANSI(*activityProps.details);
	activity.SetDetails(DetailsANSI);

	if (activityProps.state.Len() == 0) {
		activityProps.state = Level->Game->GameReplicationInfo->GameName;
	}
	const ANSICHAR* StateANSI = TCHAR_TO_ANSI(*activityProps.state);
	activity.SetState(StateANSI);

	if (activityProps.startTime == 0) {
		activityProps.startTime = std::time(nullptr);
	}
	activity.GetTimestamps().SetStart(activityProps.startTime);

	// Assets included in https://discord.com/developers/applications/ 'Rich Presence section'
	if (activityProps.largeImage.Len() == 0) {
		FString FilePath = appBaseDir();
		FilePath += TEXT("UDiscordManager.ini");

		activityProps.largeImage = GConfig->GetStr(TEXT("UDiscordManager"), TEXT("Assets_largeImage"), *FilePath);
	}
	const ANSICHAR* LargeImageANSI = TCHAR_TO_ANSI(*activityProps.largeImage);
	activity.GetAssets().SetLargeImage(LargeImageANSI);

	if (activityProps.largeImageText.Len() == 0) {
		FString FilePath = appBaseDir();
		FilePath += TEXT("UDiscordManager.ini");

		activityProps.largeImageText = GConfig->GetStr(TEXT("UDiscordManager"), TEXT("Assets_largeImageText"), *FilePath);
	}
	const ANSICHAR* LargeImageTextANSI = TCHAR_TO_ANSI(*activityProps.largeImageText);
	activity.GetAssets().SetLargeText(LargeImageTextANSI);

	if (activityProps.smallImage.Len() == 0) {
		FString FilePath = appBaseDir();
		FilePath += TEXT("UDiscordManager.ini");

		activityProps.smallImage = GConfig->GetStr(TEXT("UDiscordManager"), TEXT("Assets_smallImage"), *FilePath);
	}
	const ANSICHAR* SmallImageANSI = TCHAR_TO_ANSI(*activityProps.smallImage);
	activity.GetAssets().SetSmallImage(SmallImageANSI);

	if (activityProps.smallImageText.Len() == 0) {
		FString FilePath = appBaseDir();
		FilePath += TEXT("UDiscordManager.ini");

		activityProps.smallImageText = GConfig->GetStr(TEXT("UDiscordManager"), TEXT("Assets_smallImageText"), *FilePath);
	}
	const ANSICHAR* SmallImageTextANSI = TCHAR_TO_ANSI(*activityProps.smallImageText);
	activity.GetAssets().SetSmallText(SmallImageTextANSI);

	DiscordProps.core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {
		if (result != discord::Result::Ok) {
			GLog->Logf(TEXT("initActivityManager: failed with result: result=%d"), result);
		}
		else {
			GLog->Logf(TEXT("initActivityManager: success"));
		}
	});
}

void AUDiscordManager::_runCallbacks()
{
	if (!this->bInitialized) {
		GLog->Log(TEXT("runCallbacks: Discord is not initialized"));
		return;
	}

	// Run callbacks everytime we do some changes. You should call to this method on a Tick() or Timer() function
	discord::Result result = DiscordProps.core->RunCallbacks();
	if (result != discord::Result::Ok) {
		GLog->Logf(TEXT("runCallbacks: failed with result: result=%d"), result);
		this->bInitialized = false;
	}
}


AUDiscordManager::AUDiscordManager()
{
	this->bInitialized = false;
}

UBOOL AUDiscordManager::Tick(float DeltaTime, ELevelTick TickType)
{
	Super::Tick(DeltaTime, TickType);

	if (!this->bInitialized) {
		this->_initDiscord();
	}
	
	this->_runCallbacks();

	return true;
}


/*******************************************************************************
 *
 * @PARAM	details			FString details to be displayed (Ex: Playing Solo)
 * @PARAM	state			FString state to be displayed (Ex: In Game)
 * @PARAM	startTime		Integer unix timestamp to be displayed. This parameter will display the elapsed time in your activity
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
	P_FINISH;

	auto& activityProps = DiscordProps.activityProps;

	if (details.Len() > 0) {
		activityProps.details = details;
	}

	if (state.Len() > 0) {
		activityProps.state = state;
	}

	if (startTime != 0) {
		activityProps.startTime = startTime;
	}

	if (!this->bInitialized) {
		GLog->Log(TEXT("Discord is not initialized"));

		*(UBOOL*)Result = false;
		return;
	}

	discord::Activity activity = DiscordProps.activity;

	const ANSICHAR* DetailsANSI = TCHAR_TO_ANSI(*activityProps.details);
	activity.SetDetails(DetailsANSI);

	const ANSICHAR* StateANSI = TCHAR_TO_ANSI(*activityProps.state);
	activity.SetState(StateANSI);

	activity.GetTimestamps().SetStart(activityProps.startTime);

	DiscordProps.core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {
		if (result != discord::Result::Ok) {
			GLog->Logf(TEXT("execupdateAcitivty: failed with result: result=%d"), result);
		}
		else {
			GLog->Logf(TEXT("execupdateAcitivty: success"));
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
	P_FINISH;

	if (!this->bInitialized) {
		GLog->Log(TEXT("execclearActivity: Discord is not initialized"));

		*(UBOOL*)Result = false;
		return;
	}

	DiscordProps.core->ActivityManager().ClearActivity([](discord::Result result) {
		if (result != discord::Result::Ok) {
			GLog->Logf(TEXT("execclearActivity: failed with result: result=%d"), result);
		}
		else {
			GLog->Log(TEXT("execclearActivity: success"));
		}
	});

	*(UBOOL*)Result = true;

	unguard;
}
IMPLEMENT_FUNCTION(AUDiscordManager, -1, execclearActivity);