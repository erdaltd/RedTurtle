// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameCont.h"
#include "CoreMinimal.h"


class  Debug
{
public:


	Debug(FString _msg)
	{
		if(AGameCont::INS->DebugMode > 1)
		UE_LOG(LogTemp, Warning, TEXT("%s"), *_msg);
	}


	Debug(FString _label, float _float)
	{
		FString _msg = _label.Len() > 0 ? _label + "-> " : "";
		Debug(_msg + *FString::SanitizeFloat(_float));
	}
	Debug(FString _label, int _int)
	{
		FString _msg = _label.Len() > 0 ? _label + "-> " : "";
		Debug(_msg + *FString::FromInt(_int));
	}



	static void Fatal(FString _msg)
	{
		UE_LOG(LogTemp, Fatal, TEXT("%s"), *_msg);
	}
	static void Warning(FString _msg)
	{
		FString m = "#OMCEMERR -> " + _msg;
		UE_LOG(LogTemp, Warning, TEXT("%s"), *m);
	}

};