#pragma once
#include "basics.h"
#include <cstdint>
#include <limits>
#include <stdexcept>
#include "tarray.h"
#include <optional>

#undef max
#undef min

//We can "tag" indices with the type of the thing in the Container they are an index for.
//We can also enforce proper semantics, exceptions etc for behavior related to indices instead of using raw ints.
template<typename T>
struct TIndex
{
	size_t idx = 0;
	const TArray<T>  *pContainer = nullptr; 
	
	constexpr explicit TIndex(const TArray<T>& Container, const size_t NewIdx) noexcept
	{
		idx   = NewIdx;
		pContainer = &Container;
	}

	template<typename O>
	constexpr TIndex<O> MatchingIndexInOtherContainer(TArray<O> &OtherContainer)
	{
		assert(OtherContainer.IsValidIndex(idx));
		assert(pContainer);
		assert(pContainer->IsValidIndex(idx));
		return TIndex<O>(OtherContainer, idx);
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
		if (IsValid())
		{
			if (idx > (std::numeric_limits<IntType>::max()))
			{
				throw std::runtime_error("Lossy narrowing conversion");
				return 0;
			}
			else if (pContainer)
			{
				return static_cast<IntType>(idx);
			}
		}
		throw std::runtime_error("Invalid or uninitialized Index retreived.");
		return 0;
	}

	void operator++(int)
	{
		idx++;
	}

	void operator--(int)
	{
		idx--;
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

	TIndex()                        = default;
	TIndex(const TIndex&)			= default;
	TIndex(TIndex&&)				= default;
	TIndex(TIndex&)					= default;
	TIndex& operator=(TIndex&)		= default;
	TIndex& operator=(TIndex&&)		= default;
	~TIndex()                       = default;
};


