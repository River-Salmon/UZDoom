#pragma once
#include "basics.h"
#include <cstdint>
#include <limits>
#include <stdexcept>
#include "tarray.h"
#include <optional>

#undef max
#undef min

enum struct EIndexState : uint8_t
{
	Uninitialized,
	Invalid,
	Plausible //we don't actually know if it's truly a valid index unless it's linked to a specific container
};

enum struct EIndexCompatibility : uint8_t
{
	ExactContainer, //Index is initialized with a specific Container and is only applicable to that specific container instance.
	MatchingType //Index is applicable to any container with a matching type.
};


//We can "tag" indices with the type of the thing in the Container they are an index for.
//We can also enforce proper semantics, exceptions etc for behavior related to indices instead of using raw ints.
template<typename T, EIndexCompatibility Compat = EIndexCompatibility::ExactContainer>
struct TIndex
{
	EIndexState state = EIndexState::Uninitialized;
	size_t idx = 0;
	const TArray<T>  *pContainer = nullptr; 
	
	constexpr explicit TIndex(const TArray<T>& Container, const size_t NewIdx) noexcept
	{
		idx   = NewIdx;
		state = EIndexState::Plausible;
		pContainer = &Container;
	}

	template<typename O>
	constexpr TIndex<T> FromOther(const TArray<O> &OtherContainer, const size_t NewIdx)
	{
		static_assert(Compat == EIndexCompatibility::MatchingType);

		if (pContainer && OtherContainer.IsValidIndex(NewIdx) && pContainer->IsValidIndex(NewIdx))
		{
			return TIndex<T>(*pContainer, NewIdx);
		}
		else
		{
			return TIndex<T>{EIndexState::Invalid, 0, nullptr};
		}
	}

	//for when you've already checked if the index is valid.
	[[nodiscard]] constexpr const T& GetUnsafe() const
	{
		return (*pContainer)[idx];
	}

	[[nodiscard]] constexpr T& GetMutableUnsafe() const
	{
		return (*pContainer)[idx];
	}

	[[nodiscard]] bool IsValid() const
	{
		if (pContainer)
		{
			return pContainer->IsValidIndex(idx);
		}
		return false;
	}

	[[nodiscard]] constexpr int16_t AsInt16() const
	{
		return ConvertWithNarrowingCheck<int16_t>();
	}

	[[nodiscard]] constexpr unsigned AsUnsigned() const
	{
		return ConvertWithNarrowingCheck<unsigned>();
	}

	[[nodiscard]] constexpr size_t AsSize() const
	{
		return idx;
	}

	template<typename IntType>
	[[nodiscard]] constexpr IntType ConvertWithNarrowingCheck() const
	{
		if (state == EIndexState::Plausible)
		{
			if (idx > (std::numeric_limits<IntType>::max()))
			{
				throw std::runtime_error("Lossy narrowing conversion");
				return 0;
			}
			else if (pContainer)
			{
				if (pContainer->IsValidIndex(idx))
				{
					return static_cast<IntType>(idx);
				}
				else
				{
					throw std::runtime_error("retreived index for container was out of bounds");
					return 0;
				}
			}
		}
		throw std::runtime_error("Invalid or uninitialized Index retreived.");
		return 0;
	}

	void operator++(int)
	{
		idx++;
	}

	[[nodiscard]] bool operator>(const TIndex<T> &rhs) const
	{
		return idx > rhs.AsUnsigned();
	}

	[[nodiscard]] bool operator<(const TIndex<T> &rhs) const
	{
		return idx < rhs.AsUnsigned();
	}

	[[nodiscard]] bool operator==(const TIndex<T> &rhs) const
	{
		return idx == rhs.AsUnsigned();
	}

	TIndex()
	{
		idx = 0;
		state = EIndexState::Uninitialized;
	}

	TIndex(const TIndex&)			= default;
	TIndex(TIndex&&)				= default;
	TIndex(TIndex&)					= default;
	TIndex& operator=(TIndex&)		= default;
	TIndex& operator=(TIndex&&)		= default;
	~TIndex()                       = default;
};


