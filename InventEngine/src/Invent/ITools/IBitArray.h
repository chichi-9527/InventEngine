#ifndef _IBITARRAY_
#define _IBITARRAY_

#include <cstdint>
#include <array>
#include <vector>
#include <utility>
#include <bit>

namespace INVENT
{
	struct IBitSet64
	{
		std::uint64_t Data = 0;

		IBitSet64() = default;

		IBitSet64(std::uint64_t data) : Data(data) {}

		IBitSet64(const IBitSet64&) = default;
		IBitSet64(IBitSet64&&) noexcept = default;
		IBitSet64& operator=(const IBitSet64&) = default;
		IBitSet64& operator=(IBitSet64&&) noexcept = default;

		template<typename T>
		IBitSet64& operator=(T) = delete;

		bool operator[](size_t n) const
		{
			if (n >= 64)return false;
			return (Data & (std::uint64_t{ 1 } << n));
		}

		template<bool V>
		void SetValue(size_t index)
		{
			if (index >= 64)return;
			if constexpr (V)
			{
				Data |= std::uint64_t{ 1 } << index;
			}
			else
			{
				Data &= ~(std::uint64_t{ 1 } << index);
			}
		}

		// 如果找不到（全都是 1），則回傳 64
		size_t FindFirstZeroBit() const
		{
			int trailing_ones = std::countr_one(Data);
			return static_cast<size_t>(trailing_ones);
		}
		
	};

	template<size_t Size>
	class IBitArray
	{
	public:
		IBitArray() = default;
		~IBitArray() = default;
		IBitArray(const IBitArray&) = delete;
		IBitArray(IBitArray&&) noexcept = delete;
		IBitArray& operator=(const IBitArray&) = delete;
		IBitArray& operator=(IBitArray&&) noexcept = delete;

		const IBitSet64& operator[](size_t n) const
		{
			return _array[n];
		}

		IBitSet64& operator[](size_t n)
		{
			return _array[n];
		}

		template<bool V>
		void SetValue(size_t arr_index, size_t bit_index)
		{
			if (arr_index >= Size) return;
			_array[arr_index].template SetValue<V>(bit_index);
		}

		/// <summary>
		/// 查找并返回第一个值为0的位的位置。
		/// </summary>
		/// <returns>第一个元素为包含该位的数组索引（arr_idx），第二个元素为该数组内的位索引（bit_idx）。若未找到零位，返回 { arraySize, 64 }</returns>
		std::pair<size_t, size_t> FindFirstZero() const
		{
			for (size_t arr_idx = 0; arr_idx < Size; ++arr_idx)
			{
				size_t bit_idx = _array[arr_idx].FindFirstZeroBit();

				if (bit_idx < 64)
				{
					return { arr_idx, bit_idx };
				}
			}

			return { Size, 64 };
		}

		constexpr size_t ArrSize() const
		{
			return Size;
		}

		constexpr size_t BitCount() const
		{
			return Size * 64;
		}


	private:
		std::array<IBitSet64, Size> _array = {};
	};

	class IBitVector
	{
	public:
		explicit IBitVector(size_t size = 0) : _vector(size, IBitSet64{}) {}
		~IBitVector() = default;
		IBitVector(const IBitVector&) = delete;
		IBitVector(IBitVector&&) noexcept = delete;
		IBitVector& operator=(const IBitVector&) = delete;
		IBitVector& operator=(IBitVector&&) noexcept = delete;

		const IBitSet64& operator[](size_t n) const
		{
			return _vector[n];
		}

		IBitSet64& operator[](size_t n)
		{
			return _vector[n];
		}

		template<bool V>
		void SetValue(size_t arr_index, size_t bit_index)
		{
			if (arr_index >= _vector.size()) return;
			_vector[arr_index].SetValue<V>(bit_index);
		}

		/// <summary>
		/// 查找并返回第一个值为0的位的位置。
		/// </summary>
		/// <returns>第一个元素为包含该位的数组索引（arr_idx），第二个元素为该数组内的位索引（bit_idx）。若未找到零位，返回 { vectorSize, 64 }</returns>
		std::pair<size_t, size_t> FindFirstZero() const
		{
			const size_t vec_size = _vector.size();

			for (size_t arr_idx = 0; arr_idx < vec_size; ++arr_idx)
			{
				size_t bit_idx = _vector[arr_idx].FindFirstZeroBit();

				if (bit_idx < 64)
				{
					return { arr_idx, bit_idx };
				}
			}

			return { vec_size, 64 };
		}

		void Resize(size_t new_size)
		{
			_vector.resize(new_size, IBitSet64{});
		}

		void Clear() noexcept
		{
			_vector.clear();
		}

		size_t ArrSize() const noexcept
		{
			return _vector.size();
		}

		size_t BitCount() const noexcept
		{
			return _vector.size() * 64;
		}

	private:

		std::vector<IBitSet64> _vector;

	};

}

#endif // !_IBITARRAY_