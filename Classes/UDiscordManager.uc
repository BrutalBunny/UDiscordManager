class UDiscordManager extends Actor
	native
	noexport;

var bool bInitialized;

native function bool updateAcitivty(string details, string state, int startTime);
native function bool clearActivity();
native function bool openGuildInvite(string guildId);

function PostBeginPlay()
{
	SetTimer(10.0, false);
}

function Timer()
{
	updateAcitivty("Server Name Pog", "Map Name Pog", 0);
}