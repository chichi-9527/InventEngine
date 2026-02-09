#ifndef _ISAFEFASTPTRVECTOR_
#define _ISAFEFASTPTRVECTOR_

#include <vector>
#include <queue>
#include <mutex>

#include <concepts>

namespace INVENT
{
	template<typename PtrTy>
	requires std::is_pointer_v<PtrTy>
	class ISafeFastPtrVector
	{
	public:
		typedef size_t Handle;

		ISafeFastPtrVector()
		{

		}
		~ISafeFastPtrVector()
		{

		}

		void Reserve(size_t size)
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_vector.reserve(size);
		}

		Handle EmplaceBack(PtrTy&& input)
		{
			std::lock_guard<std::mutex> lock(_mutex);
			if (_queue.empty())
			{
				auto size = _vector.size();
				_vector.emplace_back(std::forward<PtrTy>(input));
				return size;
			}
			auto id = _queue.front();
			_queue.pop();
			_vector[id] = input;
			return id;
		}

		PtrTy Erase(Handle id)
		{
			if (id < _vector.size())
			{
				std::lock_guard<std::mutex> lock(_mutex);
				PtrTy ptr = _vector[id];
				_vector[id] = nullptr;
				_queue.push(id);
				return ptr;
			}
			return nullptr;
		}

		PtrTy& At(Handle id)
		{
			if (id < _vector.size())
			{
				std::lock_guard<std::mutex> lock(_mutex);
				return _vector[id];
			}
			return nullptr;
		}

		PtrTy& operator[](Handle id)
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _vector[id];
		}

		std::vector<PtrTy>::iterator begin() noexcept
		{
			_vector.begin();
		}
		std::vector<PtrTy>::iterator end() noexcept
		{
			_vector.end();
		}
		std::vector<PtrTy>::iterator rbegin() noexcept
		{
			_vector.rbegin();
		}
		std::vector<PtrTy>::iterator rend() noexcept
		{
			_vector.rend();
		}

	private:
		std::vector<PtrTy> _vector;
		std::queue<size_t> _queue;

		std::mutex _mutex;
	};

}

#endif // !_ISAFEFASTPTRVECTOR_


