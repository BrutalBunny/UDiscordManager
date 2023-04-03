
# Discord Manager for UT99

Implementation of the Discord GameSDK for UnrealTournament 99 using native C++

![example](https://github.com/BrutalBunny/UDiscordManager/blob/main/example.PNG?raw?true)

## Important notes

* Not fully tested yet. Tested already on UT99 versions 436 and 469c
* Not tested in all Windows enviroments 32/64 bits
* Not tested yet in dedicated servers

## Features

* Activity manager (known also as rich presence)
* Overlay Guild Invite

## Installation

- You will need first to download the latest [SDK version for UT99](https://github.com/OldUnreal/UnrealTournamentPatches/releases)

- You also need to download the [Discord GameSDK](https://discord.com/developers/docs/game-sdk/sdk-starter-guide). In my case i used v2.5.6.

- Create a empty a new project in VisualStudio (Dynamic-Link Library)

- Copy /Inc into "Header Files" and /Src into "Source files". Also add the .h and .cpp files from Discord GameSDK (https://discord.com/developers/docs/game-sdk/sdk-starter-guide#code-primer-no-engine-cpp)

- Make the following configuration in your project "Configuration Properties":
    * C/C++ (General): Include Core\Inc and Engine\Inc from UT99 and \cpp from Discord GameSDK as additional include directories
    * C/C++ (Language): Disable "Conformance mode" and "Treat WChar_t as Built in Type"
    * Linker (General): Include Core\Lib and Engine\Lib from UT99 and \lib from Discord GameSDK
    * Linker (Input): Add the following lines "discord_game_sdk.dll.lib", "Core.lib" and "Engine.lib" as additional dependencies.  

## Documentation

Sadly no documentation for UT99 :') but here are some links of interest

* [Hello World project](https://www.oldunreal.com/phpBB3/viewtopic.php?f=37&t=3938)
* [Sample Native package](https://github.com/FaultyRAM/Ut99PubSrc/tree/master/SampleNativePackage)
* [Some Native Guide](http://web.archive.org/web/20010412044804/http://www.sleepwalkertc.com/members/blitz/native/native.html)

## Authors

- [@BrutalBunny](https://www.github.com/BrutalBunny)
- Thanks to >@tack!< also for providing useful documentation
## Contributing

Contributions are always welcome!

See `contributing.md` for ways to get started.

Please adhere to this project's `code of conduct`.
