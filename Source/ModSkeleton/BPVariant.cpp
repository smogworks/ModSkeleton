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
#include "BPVariant.h"

EBPVariantType UBPVariant::GetType() const
{
	return Type;
}

void UBPVariant::SetType(EBPVariantType NewType)
{
	if (Type == EBPVariantType::VT_String)
	{
		delete StoreUnion.StoreString;
	}
	Type = NewType;
	AsArray.Empty();
	StoreObject = nullptr;
	FMemory::Memset(&StoreUnion, 0, sizeof(PrivStoreUnion));
	if (Type == EBPVariantType::VT_String) {
		StoreUnion.StoreString = new FString();
	}
}

FString UBPVariant::GetDebugValue() const
{
	switch (Type)
	{
	case EBPVariantType::VT_Boolean:
		return FString::Printf(TEXT("Boolean(%s)"), GetAsBoolean() ? TEXT("true") : TEXT("false"));
	case EBPVariantType::VT_Integer:
		return FString::Printf(TEXT("Integer(%d)"), GetAsInteger());
	case EBPVariantType::VT_Float:
		return FString::Printf(TEXT("Float(%f)"), GetAsFloat());
	case EBPVariantType::VT_String:
		return FString::Printf(TEXT("String(\"%s\")"), *GetAsString());
	case EBPVariantType::VT_Class:
		return FString::Printf(TEXT("Class(%s)"), *GetAsClass()->GetFullName());
	case EBPVariantType::VT_Object:
		return FString::Printf(TEXT("Object(%s)"), *GetAsObject()->GetFullName());
	case EBPVariantType::VT_Array:
	{
		FString Out("Array[");
		for (int32 i = 0; i < AsArray.Num(); ++i)
		{
			if (i > 0) Out.Append(",");
			Out.Append("\n  ");
			if (AsArray[i] == nullptr)
			{
				Out.Append("None");
			}
			else
			{
				Out.Append(AsArray[i]->GetDebugValue());
			}
		}
		Out.Append("\n]");
		return Out;
	}
	default:
	case EBPVariantType::VT_None:
		return TEXT("None");
	}
}

UBPVariant* UBPVariant::NewBPVariantAsBoolean(UObject* Outer, bool Value)
{
	UBPVariant* Out = NewObject<UBPVariant>(Outer, UBPVariant::StaticClass());
	Out->SetAsBoolean(Value);
	return Out;
}

bool UBPVariant::GetAsBoolean() const
{
	if (Type == EBPVariantType::VT_Boolean)
	{
		return StoreUnion.StoreBool;
	}
	else
	{
		return false;
	}
}

bool UBPVariant::SetAsBoolean(bool Value)
{
	SetType(EBPVariantType::VT_Boolean);
	StoreUnion.StoreBool = Value;
	return Value;
}

UBPVariant* UBPVariant::NewBPVariantAsInteger(UObject* Outer, int32 Value)
{
	UBPVariant* Out = NewObject<UBPVariant>(Outer, UBPVariant::StaticClass());
	Out->SetAsInteger(Value);
	return Out;
}

int32 UBPVariant::GetAsInteger() const
{
	if (Type == EBPVariantType::VT_Integer)
	{
		return StoreUnion.StoreInt32;
	}
	else
	{
		return 0;
	}
}

int32 UBPVariant::SetAsInteger(int32 Value)
{
	SetType(EBPVariantType::VT_Integer);
	StoreUnion.StoreInt32 = Value;
	return Value;
}


UBPVariant* UBPVariant::NewBPVariantAsFloat(UObject* Outer, float Value)
{
	UBPVariant* Out = NewObject<UBPVariant>(Outer, UBPVariant::StaticClass());
	Out->SetAsFloat(Value);
	return Out;
}

float UBPVariant::GetAsFloat() const
{
	if (Type == EBPVariantType::VT_Float)
	{
		return StoreUnion.StoreFloat;
	}
	else
	{
		return 0.0f;
	}
}

float UBPVariant::SetAsFloat(float Value)
{
	SetType(EBPVariantType::VT_Float);
	StoreUnion.StoreFloat = Value;
	return Value;
}

UBPVariant* UBPVariant::NewBPVariantAsString(UObject* Outer, const FString& Value)
{
	UBPVariant* Out = NewObject<UBPVariant>(Outer, UBPVariant::StaticClass());
	Out->SetAsString(Value);
	return Out;
}

const FString& UBPVariant::GetAsString() const
{
	if (Type == EBPVariantType::VT_String)
	{
		return *StoreUnion.StoreString;
	}
	else
	{
		// TODO - BAD
		return *((FString*)nullptr);
	}
}

const FString& UBPVariant::SetAsString(const FString& Value)
{
	SetType(EBPVariantType::VT_String);
	*StoreUnion.StoreString = Value;
	return Value;
}

UBPVariant* UBPVariant::NewBPVariantAsClass(UObject* Outer, UClass* Value)
{
	UBPVariant* Out = NewObject<UBPVariant>(Outer, UBPVariant::StaticClass());
	Out->SetAsClass(Value);
	return Out;
}

UClass* UBPVariant::GetAsClass() const
{
	if (Type == EBPVariantType::VT_Class)
	{
		return StoreUnion.StoreClass;
	}
	else
	{
		return nullptr;
	}
}

UClass* UBPVariant::SetAsClass(UClass* Value)
{
	SetType(EBPVariantType::VT_Class);
	StoreUnion.StoreClass = Value;
	return Value;
}

UBPVariant* UBPVariant::NewBPVariantAsObject(UObject* Outer, UObject* Value)
{
	UBPVariant* Out = NewObject<UBPVariant>(Outer, UBPVariant::StaticClass());
	Out->SetAsObject(Value);
	return Out;
}

UObject* UBPVariant::GetAsObject() const
{
	if (Type == EBPVariantType::VT_Object)
	{
		return StoreObject;
	}
	else
	{
		return nullptr;
	}
}

UObject* UBPVariant::SetAsObject(UObject* Value)
{
	SetType(EBPVariantType::VT_Object);
	StoreObject = Value;
	return Value;
}

UBPVariant* UBPVariant::NewBPVariantAsArray(UObject* Outer)
{
	UBPVariant* Out = NewObject<UBPVariant>(Outer, UBPVariant::StaticClass());
	Out->SetAsArray();
	return Out;
}

void UBPVariant::SetAsArray()
{
	SetType(EBPVariantType::VT_Array);
}