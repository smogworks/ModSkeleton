# ModSkeleton

- Targets UE 4.15

```
Copyright 2017 Smogworks

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```

## Goals

- To be a go-to example for Unreal Engine 4 modding support.
- To not require core engine changes - work with prebuilt UE4 Editor from Epig Games Launcher.

## Get it Running (Scripted)

1. Download NodeJS - https://nodejs.org
1. Clone the Repo
1. Run the build script once to generate a config: `node ue4build.js`
1. Inspect the config (`.ue4build.json`), if you built ue4 from source, you may need to change the `uatCommand` path.
1. Run the build script again to build: `node ue4build.js`
1. If there are errors, AutomationTool.exe output can be found in the log file specified in `.ue4build.json`
1. If it succeeds, a runable project complete with mod paks should be found in the `outputPath` directory specified in `.ue4build.json`

## Get it Running (Manually)

1. Clone the Repo
1. Open ModSkeleton.uproject
1. Be sure to select "View Options" -> "Show Plugin Content" in the Content browser to see Example Plugin Content
1. Follow [Instruction to Build Custom Launcher Profiles](doc/build_profiles/build_profiles.md)
1. Disable the ModSkeletonExamplePluginA plugin (Edit -> Plugins -> "ModSkeleton" section)
1. Execute the Full Game launch profile for your platform
1. Enable the ModSkeletonExamplePluginA plugin
1. Execute the Mod launch profile for you platform - note this may fail, but not before generating the needed .pak and AssetRegistry files
1. Move/Rename "Plugins/ModSkeletonExamplePluginA/Saved/Cooked/[platform]/ModSkeleton/AssetRegistry.bin" to "Saved/StagedBuilds/[platform]/ModSkeleton/Content/Paks/ModSkeletonExamplePluginA.bin"
1. Move/Rename "Plugins/ModSkeletonExamplePluginA/Saved/StagedBuilds/[platform]/ModSkeleton/Content/Paks/ModSkeleton-[platform].pak" to "Saved/StagedBuilds/[platform]/ModSkeleton/Content/Paks/ModSkeletonExamplePluginA.pak"
1. Execute "Saved/StagedBuilds/[platform]/[ModSkeleton executable]

## Architecture

### Startup

- ModSkeletonGameInstance initializes and keeps a reference to a single ModSkeletonRegistry instance
- ModSkeletonRegistry scans the Content/Paks directory for matching AssetRegistry (".bin") files and Content (".pak") files loading all.
- ModSkeletonRegistry searches the in-memory AssetRegistry for all classes whos name begins with "MOD_SKELETON" and who implement ModSkeletonPluginInterface
- The plugin interface is invoked once as "ModSkeletonInit" allowing these mods to register, connect, and/or invoke mod Hooks.

### ModSkeleton Hooks

- BPVariant is a uobject based blueprint friendly variant class to support easy data interchange through hook invokes
- Hooks marked "Always Invoke" (like the "ModSkeletonInit" hook) will be called once for every loaded MOD_SKELETON init interface
- Hooks NOT marked "Always Invoke" will only be called if they have been Connected, and will be called in priority order
- Hooks will be passed a reference to an array of BPVariants. This "HookIO" will be used as both input and output, and allows hooks to modify core behavior:

Imagine a registered hook that is requesting a list of main menu items. The base game could begin this list with buttons labeled "New Game", "Load Game", and "Exit". Someone could create a mod that adjusts this list, replacing the "New Game" button with one that leads to a different character creation screen. Psuedo Code:

```
class CoreGame implements ModSkeletonPluginInterface
  function ModSkeletonHook(String HookName, BPVariantArray HookIO)
    if HookName == "PopulateMainMenu"
      HookIO[0].Add( NewGameButton )
      HookIO[0].Add( LoadGameButton )
      HookIO[0].Add( ExitButton )
    end if
  end function
end class

class MyNewCharacterMod implements ModSkeletonPluginInterface
  function ModSkeletonHook(String HookName, BPVariantArray HookIO)
    if HookName == "PopulateMainMenu"
      HookIO[0].RemoveItem( NewGameButton )
      HookIO[0].Prepend( MyBetterNewGameButton )
    end if
  end function
end class
```

## TODO

- switch to using `FCoreDelegates::OnMountPak` (I haven't figured out the mountpoint paths with this method)
- Example CPP plugin

## Questions

- Better way to distribute? Could make the core stuff a plugin... but then other plugins would have to depend on headers in it...
- BPVariant blueprint constructor helpers seem a little kludgy (especially with the hidden world context -> outer pins) any way to make that better?
- HookIO TArray< BPVariant* > in-out pins cause more copying than I was hoping for... better way to solve that? I'd like to just modify the passed in reference, but then it shows up as an output on the return node...
