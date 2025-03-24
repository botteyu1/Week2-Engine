#pragma once

#include "Core/UObject/Object.h"
#include "Core/Hal/PlatformType.h"
#include "Core/Container/Array.h"
#include <Core/Engine.h>

void GetObjectsOfClass(const UClass* ClassToLookFor, TArray<UObject*>& Results);


template< class T >
class TObjectIterator
{
public:
	enum EEndTagType
	{
		EndTag
	};

	explicit TObjectIterator()
		: Index(-1)
	{
		GetObjectsOfClass(T::StaticClass(), ObjectArray);
		Advance();
	}


	TObjectIterator(EEndTagType, const TObjectIterator& Begin)
		: Index(Begin.ObjectArray.Num())
	{
	}


	//Iterator advance
	FORCEINLINE void operator++()
	{
		Advance();
	}

	/** Conversion to "bool" returning true if the iterator is valid. */
	FORCEINLINE explicit operator bool() const
	{
		return ObjectArray.IsValidIndex(Index);
	}

	/** Conversion to "bool" returning true if the iterator is valid. */
	FORCEINLINE bool operator !() const
	{
		return !(bool)*this;
	}



	FORCEINLINE T* operator* () const
	{
		return (T*)GetObject();
	}
		/**
		 //* Iterator dereference
		 //* @return	the object pointer pointed at by the iterator
		 //*/
	FORCEINLINE T* operator-> () const
	{
		return (T*)GetObject();
	}


	FORCEINLINE bool operator==(const TObjectIterator& Rhs) const { return Index == Rhs.Index; }

	FORCEINLINE bool operator!=(const TObjectIterator& Rhs) const { return Index != Rhs.Index; }

protected:

	/**
// * Dereferences the iterator with an ordinary name for clarity in derived classes
//*
//* @return	the UObject at the iterator
//*/
	FORCEINLINE UObject* GetObject() const
	{
		return ObjectArray[Index];
	}

	/**
	//* Iterator advance with ordinary name for clarity in subclasses
	//* @return	true if the iterator points to a valid object, false if iteration is complete
	//*/
	FORCEINLINE bool Advance()
	{
		//@todo UE check this for LHS on Index on consoles
		while (++Index < ObjectArray.Num())
		{
			if (GetObject())
			{
				return true;
			}
		}
		return false;
	}
	/** Results from the GetObjectsOfClass query */
	TArray<UObject*> ObjectArray;
	/** index of the current element in the object array */
	int32 Index;
};

