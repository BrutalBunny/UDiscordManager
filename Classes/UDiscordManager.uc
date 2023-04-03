class UDiscordManager extends Actor
	native
	noexport;

var bool bInitialized;

native function int initDiscord();
native function int initActivity();
native function int updateAcitivty(string details, string state, int startTime);
native function int clearActivity();
native function int runCallbacks();
native function int openGuildInvite(string guildId);

simulated function PostBeginPlay()
{
	initialize();
	SetTimer(1.0, true);
}

simulated function Timer()
{
	if( !bInitialized ) initialize();
	runCallbacks();
}

simulated function initialize()
{
	local int initDiscordResult;

	initDiscordResult = initDiscord();
	if( initDiscordResult == 0 ) initActivity();
}

simulated function Destroyed()
{
	Super.Destroyed();

	Log("im out");
	clearActivity();
}

defaultproperties
{
      bAlwaysRelevant=True
      RemoteRole=ROLE_SimulatedProxy
}