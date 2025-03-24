#include "UObjectIterator.h"
#include "Core/UObject/Class.h"


void GetObjectsOfClass(const UClass* ClassToLookFor, TArray<UObject*>& Results)
{
	TMap<uint32, std::shared_ptr<UObject>> GObjects = UEngine::Get().GObjects;

	for (const auto& ObjectIter : GObjects)
	{
		if (ObjectIter.Value->GetClass()->IsChildOf(ClassToLookFor))
		{
			Results.Add(ObjectIter.Value.get());
		}
	}
}
