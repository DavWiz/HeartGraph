﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "UI/HeartWidgetFactory.h"

TSubclassOf<UUserWidget> UHeartWidgetFactory::FindWidgetClassForData_Implementation(const UObject* Data) const
{
	return TSubclassOf<UUserWidget>();
}

TSubclassOf<UUserWidget> FHeartWidgetFactoryRules::GetWidgetClass(const UObject* Data) const
{
	for (const UHeartWidgetFactory* Rule : FactoryRules)
	{
		if (IsValid(Rule))
		{
			if (const TSubclassOf<UUserWidget> EntryClass = Rule->FindWidgetClassForData(Data))
			{
				return EntryClass;
			}
		}
	}

	return nullptr;
}

TSubclassOf<UUserWidget> UHeartWidgetFactoryLibrary::GetWidgetClass(const FHeartWidgetFactoryRules& Rules, const UObject* Data)
{
	return Rules.GetWidgetClass(Data);
}
