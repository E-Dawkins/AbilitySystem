// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "AbilitySystem/AbilitySystemGameModeBase.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeAbilitySystemGameModeBase() {}
// Cross Module References
	ABILITYSYSTEM_API UClass* Z_Construct_UClass_AAbilitySystemGameModeBase_NoRegister();
	ABILITYSYSTEM_API UClass* Z_Construct_UClass_AAbilitySystemGameModeBase();
	ENGINE_API UClass* Z_Construct_UClass_AGameModeBase();
	UPackage* Z_Construct_UPackage__Script_AbilitySystem();
// End Cross Module References
	void AAbilitySystemGameModeBase::StaticRegisterNativesAAbilitySystemGameModeBase()
	{
	}
	UClass* Z_Construct_UClass_AAbilitySystemGameModeBase_NoRegister()
	{
		return AAbilitySystemGameModeBase::StaticClass();
	}
	struct Z_Construct_UClass_AAbilitySystemGameModeBase_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_AAbilitySystemGameModeBase_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AGameModeBase,
		(UObject* (*)())Z_Construct_UPackage__Script_AbilitySystem,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AAbilitySystemGameModeBase_Statics::Class_MetaDataParams[] = {
		{ "Comment", "/**\n * \n */" },
		{ "HideCategories", "Info Rendering MovementReplication Replication Actor Input Movement Collision Rendering Utilities|Transformation" },
		{ "IncludePath", "AbilitySystemGameModeBase.h" },
		{ "ModuleRelativePath", "AbilitySystemGameModeBase.h" },
		{ "ShowCategories", "Input|MouseInput Input|TouchInput" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_AAbilitySystemGameModeBase_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AAbilitySystemGameModeBase>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_AAbilitySystemGameModeBase_Statics::ClassParams = {
		&AAbilitySystemGameModeBase::StaticClass,
		"Game",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x009002ACu,
		METADATA_PARAMS(Z_Construct_UClass_AAbilitySystemGameModeBase_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_AAbilitySystemGameModeBase_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_AAbilitySystemGameModeBase()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_AAbilitySystemGameModeBase_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(AAbilitySystemGameModeBase, 45550084);
	template<> ABILITYSYSTEM_API UClass* StaticClass<AAbilitySystemGameModeBase>()
	{
		return AAbilitySystemGameModeBase::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_AAbilitySystemGameModeBase(Z_Construct_UClass_AAbilitySystemGameModeBase, &AAbilitySystemGameModeBase::StaticClass, TEXT("/Script/AbilitySystem"), TEXT("AAbilitySystemGameModeBase"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(AAbilitySystemGameModeBase);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
