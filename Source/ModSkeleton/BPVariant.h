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

#pragma once

#include "UObject/NoExportTypes.h"
#include "BPVariant.generated.h"

/**
 */
UENUM(BlueprintType)
enum class EBPVariantType : uint8
{
	VT_None UMETA(DisplayName="None"),
	VT_Boolean UMETA(DisplayName="Boolean"),
	VT_Integer UMETA(DisplayName="Integer"),
	VT_Float UMETA(DisplayName="Float"),
	VT_String UMETA(DisplayName="String"),
	VT_Class UMETA(DisplayName="Class"),
	VT_Object UMETA(DisplayName="Object"),
	VT_Array UMETA(DisplayName="Array")
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class MODSKELETON_API UBPVariant : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (CompactNodeTitle = "Type"))
	virtual EBPVariantType GetType() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (CompactNodeTitle = "DebugValue"))
	virtual FString GetDebugValue() const;

	UFUNCTION(BlueprintCallable, meta = (HidePin = Outer, DefaultToSelf = Outer))
	static UBPVariant* NewBPVariantAsBoolean(UObject* Outer, bool Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (CompactNodeTitle = "Boolean"))
	virtual bool GetAsBoolean() const;

	UFUNCTION(BlueprintCallable)
	virtual bool SetAsBoolean(bool Value);

	UFUNCTION(BlueprintCallable, meta = (HidePin = Outer, DefaultToSelf = Outer))
	static UBPVariant* NewBPVariantAsInteger(UObject* Outer, int32 Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (CompactNodeTitle = "Integer"))
	virtual int32 GetAsInteger() const;

	UFUNCTION(BlueprintCallable)
	virtual int32 SetAsInteger(int32 Value);

	UFUNCTION(BlueprintCallable, meta = (HidePin = Outer, DefaultToSelf = Outer))
	static UBPVariant* NewBPVariantAsFloat(UObject* Outer, float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (CompactNodeTitle = "Float"))
	virtual float GetAsFloat() const;

	UFUNCTION(BlueprintCallable)
	virtual float SetAsFloat(float Value);

	UFUNCTION(BlueprintCallable, meta = (HidePin = Outer, DefaultToSelf = Outer))
	static UBPVariant* NewBPVariantAsString(UObject* Outer, const FString& Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (CompactNodeTitle = "String"))
	virtual const FString& GetAsString() const;

	UFUNCTION(BlueprintCallable)
	virtual const FString& SetAsString(const FString& Value);

	UFUNCTION(BlueprintCallable, meta = (HidePin = Outer, DefaultToSelf = Outer))
	static UBPVariant* NewBPVariantAsClass(UObject* Outer, UClass* Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (CompactNodeTitle = "Class"))
	virtual UClass* GetAsClass() const;

	UFUNCTION(BlueprintCallable)
	virtual UClass* SetAsClass(UClass* Value);

	UFUNCTION(BlueprintCallable, meta = (HidePin = Outer, DefaultToSelf = Outer))
	static UBPVariant* NewBPVariantAsObject(UObject* Outer, UObject* Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (CompactNodeTitle = "Object"))
	virtual UObject* GetAsObject() const;

	UFUNCTION(BlueprintCallable)
	virtual UObject* SetAsObject(UObject* Value);

	UFUNCTION(BlueprintCallable, meta = (HidePin = Outer, DefaultToSelf = Outer))
	static UBPVariant* NewBPVariantAsArray(UObject* Outer);

	UFUNCTION(BlueprintCallable)
	virtual void SetAsArray();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BPVariant")
	TArray< UBPVariant* > AsArray;

private:
	EBPVariantType Type;
	void SetType(EBPVariantType NewType);

	union PrivStoreUnion
	{
		bool StoreBool;
		int32 StoreInt32;
		float StoreFloat;
		FString* StoreString;
		UClass* StoreClass;
	};
	PrivStoreUnion StoreUnion;

	UPROPERTY()
	UObject* StoreObject;
};
