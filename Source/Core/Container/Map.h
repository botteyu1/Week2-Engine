#pragma once
#include <unordered_map>
#include "ContainerAllocator.h"
#include "Pair.h"
#include "Core/UObject/NameTypes.h"


template <typename KeyType, typename ValueType, typename Allocator = FDefaultAllocator<std::pair<const KeyType, ValueType>>>
class TMap
{
public:
    using PairType = TPair<const KeyType, ValueType>;
    using MapType = std::unordered_map<KeyType, ValueType, std::hash<KeyType>, std::equal_to<KeyType>, Allocator>;
    using SizeType = typename MapType::size_type;

private:
    MapType PrivateMap;

public:
    class Iterator
    {
    private:
        typename MapType::iterator InnerIt;
    public:
        Iterator(typename MapType::iterator it) : InnerIt(std::move(it)) {}
        PairType& operator*() { return reinterpret_cast<PairType&>(*InnerIt); }
        PairType* operator->() { return reinterpret_cast<PairType*>(&(*InnerIt)); }
        Iterator& operator++() { ++InnerIt; return *this; }
        bool operator!=(const Iterator& other) const { return InnerIt != other.InnerIt; }
    };

    class ConstIterator
    {
    private:
        typename MapType::const_iterator InnerIt;
    public:
        ConstIterator(typename MapType::const_iterator it) : InnerIt(it) {}
        const PairType& operator*() const { return reinterpret_cast<const PairType&>(*InnerIt); }
        const PairType* operator->() const { return reinterpret_cast<const PairType*>(&(*InnerIt)); }
        ConstIterator& operator++() { ++InnerIt; return *this; }
        bool operator!=(const ConstIterator& other) const { return InnerIt != other.InnerIt; }
    };

public:
    // TPair를 반환하는 커스텀 반복자
    Iterator begin() noexcept { return Iterator(PrivateMap.begin()); }
    Iterator end() noexcept { return Iterator(PrivateMap.end()); }
    ConstIterator begin() const noexcept { return ConstIterator(PrivateMap.begin()); }
    ConstIterator end() const noexcept { return ConstIterator(PrivateMap.end()); }

    // 생성자 및 소멸자
    TMap() = default;
    ~TMap() = default;

    // 복사 생성자
    TMap(const TMap& Other) : PrivateMap(Other.PrivateMap) {}

    // 이동 생성자
    TMap(TMap&& Other) noexcept : PrivateMap(std::move(Other.PrivateMap)) {}

    // 복사 할당 연산자
    TMap& operator=(const TMap& Other)
    {
        if (this != &Other)
        {
            PrivateMap = Other.PrivateMap;
        }
        return *this;
    }

    // 이동 할당 연산자
    TMap& operator=(TMap&& Other) noexcept
    {
        if (this != &Other)
        {
            PrivateMap = std::move(Other.PrivateMap);
        }
        return *this;
    }

    // 요소 접근 및 수정
    ValueType& operator[](const KeyType& Key)
    {
        return PrivateMap[Key];
    }

    const ValueType& operator[](const KeyType& Key) const
    {
        return PrivateMap.at(Key);
    }

    void Add(const KeyType& Key, const ValueType& Value)
    {
        PrivateMap.insert_or_assign(Key, Value);
    }

    /**
     * Map에 새로운 Key-Value를 삽입합니다.
     * @param InKey 삽입할 키
     * @param InValue 삽입할 값
     * @return InValue의 참조
     */
    template <typename InitKeyType = KeyType, typename InitValueType = ValueType>
    ValueType& Emplace(InitKeyType&& InKey, InitValueType&& InValue)
    {
        auto it = PrivateMap.emplace(std::forward<InitKeyType>(InKey), std::forward<InitValueType>(InValue));
    	return it->second;
    }

	// Key만 넣고, Value는 기본값으로 삽입
	template <typename InitKeyType = KeyType>
    ValueType& Emplace(InitKeyType&& InKey)
    {
        auto it = PrivateMap.emplace(std::forward<InitKeyType>(InKey), ValueType{});
    	return it->second;
    }

    void Remove(const KeyType& Key)
    {
        PrivateMap.erase(Key);
    }

    void Empty()
    {
        PrivateMap.clear();
    }

    // 검색 및 조회
    bool Contains(const KeyType& Key) const
    {
        return PrivateMap.find(Key) != PrivateMap.end();
    }

    const ValueType* Find(const KeyType& Key) const
    {
        auto it = PrivateMap.find(Key);
        return it != PrivateMap.end() ? &(it->second) : nullptr;
    }

    ValueType* Find(const KeyType& Key)
    {
        auto it = PrivateMap.find(Key);
        return it != PrivateMap.end() ? &(it->second) : nullptr;
    }

    // 크기 관련
    SizeType Num() const
    {
        return PrivateMap.size();
    }

    bool IsEmpty() const
    {
        return PrivateMap.empty();
    }

    // 용량 관련
    void Reserve(SizeType Number)
    {
        PrivateMap.reserve(Number);
    }

};

// TMap 클래스 템플릿 정의 이후에 다음 코드를 추가합니다
namespace std {
	template <>
	struct hash<FName> {
		size_t operator()(const FName& Name) const {
			// 언리얼 엔진의 FName 해시 함수는 ComparisonIndex와 Number를 더하는 방식
			return Name.GetComparisonIndex();
		}
	};
}