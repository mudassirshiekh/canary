/**
 * Canary - A free and open-source MMORPG server emulator
 * Copyright (©) 2019-2024 OpenTibiaBR <opentibiabr@outlook.com>
 * Repository: https://github.com/opentibiabr/canary
 * License: https://github.com/opentibiabr/canary/blob/main/LICENSE
 * Contributors: https://github.com/opentibiabr/canary/graphs/contributors
 * Website: https://docs.opentibiabr.com/
 */

#pragma once

template <typename T>
class ObjectPool {
public:
	using Ptr = std::shared_ptr<T>;

	template <typename... Args>
	static Ptr acquireObject(Args&&... args) {
		std::lock_guard<std::mutex> lock(mutex_);
		if (!pool_.empty()) {
			T* obj = pool_.back();
			pool_.pop_back();

			new (obj) T(std::forward<Args>(args)...);
			return Ptr(obj, [](T* ptr) {
				ptr->~T();
				releaseObject(ptr);
			});
		}

		T* rawPtr = allocator_.allocate(1);
		new (rawPtr) T(std::forward<Args>(args)...);
		return Ptr(rawPtr, [](T* ptr) {
			ptr->~T();
			releaseObject(ptr);
		});
	}

private:
	static void releaseObject(T* obj) {
		std::lock_guard<std::mutex> lock(mutex_);
		pool_.push_back(obj);
	}

	static inline std::vector<T*> pool_;
	static inline std::allocator<T> allocator_;
	static inline std::mutex mutex_;
};
