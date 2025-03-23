#pragma once
#include "Core/HAL/PlatformType.h"

struct FFlag {
	template <typename T>
	static FORCEINLINE void On(T& target, const T flag) {
		target = static_cast<T>(static_cast<uint32>(target) | static_cast<uint32>(flag));
	}

	template <typename T>
	static FORCEINLINE void Off(T& target, const T flag) {
		target = static_cast<T>(static_cast<uint32>(target) & ~static_cast<uint32>(flag));
	}

	template <typename T>
	static FORCEINLINE T Switch(T& target, const T flag) {
		target = static_cast<T>(static_cast<uint32>(target) ^ static_cast<uint32>(flag));
	}

	template <typename T>
	static FORCEINLINE bool Get(const T target, const T flag) {
		return (static_cast<uint32>(target) & static_cast<uint32>(flag)) > 0 ? true : false;
	}
};