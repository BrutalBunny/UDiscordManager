class UDiscordManager extends Actor
	native;

var bool bInitialized;

native function bool initDiscord(string appId);
native function bool updateAcitivty(string details, string state, int startTime);
native function bool clearActivity();

function PostBeginPlay()
{
    if( bInitialized ) return;

	bInitialized = initDiscord("1088916577560297664");
	SetTimer(5.0, true);
}

function Timer()
{
    if( !bInitialized ) initDiscord("1088916577560297664");
    updateAcitivty("Server Name Pog", "Map Name Pog", 0);
}