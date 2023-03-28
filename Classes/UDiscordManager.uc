class UDiscordManager extends Actor
	native
	noexport;

var bool bInitialized;

native function bool initDiscord();
native function bool updateAcitivty(string details, string state, int startTime);
native function bool clearActivity();
native function bool runCallbacks();

function PostBeginPlay()
{
    if( bInitialized ) return;

    initialize();
	SetTimer(1.0, true);
}

function Timer()
{
    if( !bInitialized ) initialize();
    runCallbacks();
}

function initialize()
{
    if( bInitialized ) return;

	bInitialized = initDiscord();
	updateAcitivty("Server Name Pog", "Map Name Pog", 0);
}