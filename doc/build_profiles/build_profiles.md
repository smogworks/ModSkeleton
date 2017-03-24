# Custom Build Profile For Generating ModSkeleton Mods

ModSkeleton Mods build on top of Unreal Engine 4's DLC system. In order to cook and package a Mod, we're going to create two Custom Project Launcher Profiles. First, one to build the base game. Second, we're going to create a Profile for our DLC / Plugin / Mod, referencing the base game version.

## 1 - The Base Game

#### 1.1 - Open the Project Launcher

![Open Project Launcher](open_project_launcher.jpg)

#### 1.2 - Create a New Custom Profile

![Create Custom Profile](new_profile.jpg)

#### 1.3 - Give Your Profile A Name

![Name Profile](main_1_name.jpg)

#### 1.4 - Tell the Profile to Build

![Build](main_2_build.jpg)

#### 1.5 - Tell it to Cook "By the Book"

- pick the platform you would like to cook for

![Cook By the Book](main_3_cook_book.jpg)

#### 1.6 - Cooking "Release" Settings

- Check "Create a release version of the game for distribution"
- Give a version name under "Name of the new release to create"

![Cook Release Settings](main_4_cook_release.jpg)

#### 1.7 - Cooking "Advanced" Settings

- Uncheck "Save packages without versions"
- Check "Store all contents in a single file (UnrealPak)"

![Cook Advanced Settings](main_5_cook_advanced.jpg)

#### 1.8 - Package Locally

- Set to Package and Store Locally

![Package and Store Locally](main_6_package.jpg)

#### 1.9 - Do Not Deploy

![Do Not Deploy](main_7_deploy.jpg)

## 2 - The Mod (ModSkeletonExamplePluginA)

#### 2.1 - Open the Project Launcher

![Open Project Launcher](open_project_launcher.jpg)

#### 2.2 - Create a New Custom Profile

![Create Custom Profile](new_profile.jpg)

#### 2.3 - Give Your Profile A Name

![Name Profile](plugin_1_name.jpg)

#### 2.4 - Tell the Profile to Build

![Build](main_2_build.jpg)

#### 2.5 - Tell it to Cook "By the Book"

- pick the platform you would like to cook for

![Cook By the Book](main_3_cook_book.jpg)

#### 2.6 - Cooking "Release" Settings

- Un-Check "Create a release version of the game for distribution"
- Leave "Name of the new release to create" blank
- Set "Release version this is based on." the same version string as in your base profile
- Check "Build DLC"
- Set "Name of the DLC to build." to "ModSkeletonExamplePluginA"

![Cook Release Settings](plugin_4_cook_release.jpg)

#### 2.7 - Cooking "Advanced" Settings

- Uncheck "Save packages without versions"
- Check "Store all contents in a single file (UnrealPak)"

![Cook Advanced Settings](main_5_cook_advanced.jpg)

#### 2.8 - Package Locally

- Set to Package and Store Locally

![Package and Store Locally](main_6_package.jpg)

#### 2.9 - Do Not Deploy

![Do Not Deploy](main_7_deploy.jpg)
