#pragma once

#include "CmGameObjectREAL.h"

namespace CamelotEngine
{
	struct CM_EXPORT GameObjectHandleData
	{
		GameObjectHandleData()
		{ }

		GameObjectHandleData(GameObjectREAL* ptr)
			:mPtr(ptr)
		{ }

		std::shared_ptr<GameObjectREAL> mPtr;
	};

	/**
	 * @brief	A handle that can point to various types of game objects.
	 * 			It primarily keeps track if the object is still alive, so anything
	 * 			still referencing it doesn't accidentally use it.
	 * 			
	 * @note	This class exists because I want the references between game objects be quite loose.
	 * 			For example one game object should be able to reference another one without the other
	 * 			one knowing. But if that is the case I also need to handle the case when the other
	 * 			object we're referencing has been deleted, and that is the main purpose of this class.
	 * 			
	 */
	class CM_EXPORT GameObjectHandleBase
	{
	public:
		/**
		 * @brief	Checks if the object has been destroyed
		 */
		bool isDestroyed() const { return mData->mPtr == nullptr; }

		/**
		 * @brief	Internal method only. Not meant to be called directly.
		 */
		std::shared_ptr<GameObjectHandleData> getHandleData() const { return mData; }
	protected:
		GameObjectHandleBase();

		inline void throwIfDestroyed() const;
		

		std::shared_ptr<GameObjectHandleData> mData;
	};

	template <typename T>
	class GameObjectHandle : public GameObjectHandleBase
	{
	public:
		GameObjectHandle()
			:GameObjectHandleBase()
		{	
			mData = std::shared_ptr<GameObjectHandleData>(new GameObjectHandleData());
		}

		template <typename T1>
		GameObjectHandle(const GameObjectHandle<T1>& ptr)
			:GameObjectHandleBase()
		{ 	
			mData = ptr.getHandleData();
		}

		T* get() const 
		{ 
			throwIfDestroyed();

			return reinterpret_cast<T*>(mData->mPtr.get()); 
		}
		T* operator->() const { return get(); }
		T& operator*() const { return *get(); }

		template<class _Ty>
		struct CM_Bool_struct
		{
			int _Member;
		};

		// Conversion to bool
		// (Why not just directly convert to bool? Because then we can assign pointer to bool and that's weird)
		operator int CM_Bool_struct<T>::*() const
		{
			return (((mData->mPtr != nullptr)) ? &CM_Bool_struct<T>::_Member : 0);
		}

	private:
		friend GameObject;

		explicit GameObjectHandle(T* ptr)
			:GameObjectHandleBase()
		{
			mData = std::shared_ptr<GameObjectHandleData>(new GameObjectHandleData((GameObjectREAL*)ptr));
		}

		static GameObjectHandle<T> _create(T* ptr)
		{
			return GameObjectHandle<T>(ptr);
		}

		void destroy()
		{
			mData->mPtr = nullptr;
		}
	};

	template<class _Ty1, class _Ty2>
		GameObjectHandle<_Ty1> static_object_cast(const GameObjectHandle<_Ty2>& other)
	{	
		return GameObjectHandle<_Ty1>(other);
	}

	template<class _Ty1, class _Ty2>
	bool operator==(const GameObjectHandle<_Ty1>& _Left, const GameObjectHandle<_Ty2>& _Right)
	{	
		return (_Left.get() == _Right.get());
	}

	template<class _Ty1, class _Ty2>
	bool operator!=(const GameObjectHandle<_Ty1>& _Left, const GameObjectHandle<_Ty2>& _Right)
	{	
		return (!(_Left == _Right));
	}
}