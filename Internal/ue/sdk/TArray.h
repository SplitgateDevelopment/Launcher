#pragma once

#include "Fwd.h"

template <class T>
struct TArray
{
	friend struct FString;

  public:
	inline TArray()
		: Count(0), Max(0), Data(nullptr) {
		  };

	inline TArray(int Size)
		: Count(0), Max(Size), Data(reinterpret_cast<T*>(malloc(sizeof(T) * Size))) {
		  };

	inline int Num() const
	{
		return Count;
	};

	inline T& operator[](int i)
	{
		return Data[i];
	};

	inline bool IsValidIndex(int i) const
	{
		return i >= 0 && i < Num();
	}

  private:
	T* Data;
	int32_t Count;
	int32_t Max;
};
