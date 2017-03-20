// Copyright 2017 Smogworks
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http ://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "ModSkeleton.h"
#include "ModSkeletonBpFunctionLib.h"

UModSkeletonRegistry* UModSkeletonBpFunctionLib::GlobalModRegistryRef = nullptr;

UModSkeletonRegistry* UModSkeletonBpFunctionLib::ModSkeletonRegistryGet()
{
	return GlobalModRegistryRef;
}

FString UModSkeletonBpFunctionLib::GetFullDescription(const FModSkeletonHookDescription& HookDescription)
{
	FString out(HookDescription.HookName);
	out.Append(" : ");
	out.Append(HookDescription.HookDescription);
	out.Append("\nHookIO:");
	for (auto P : HookDescription.HookIODescription)
	{
		out.Append("\n - ");
		out.Append(P);
	}
	return out;
}