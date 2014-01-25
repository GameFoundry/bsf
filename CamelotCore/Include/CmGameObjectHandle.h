#pragma once

namespace CamelotFramework
{
	struct CM_EXPORT GameObjectHandleData
	{
		GameObjectHandleData()
			:mPtr(nullptr), mDeleter(nullptr)
		{ }

		GameObjectHandleData(GameObject* ptr, void(*deleter)(GameObject*))
		{
			mPtr = ptr;
			mDeleter = deleter;
		}

		typedef void(*HandleDeleter)(GameObject*);

		GameObject* mPtr;
		HandleDeleter mDeleter;
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

		GameObject* get() const 
		{ 
			throwIfDestroyed();

			return mData->mPtr; 
		}

		GameObject* operator->() const { return get(); }
		GameObject& operator*() const { return *get(); }

	protected:
		friend SceneObject;

		GameObjectHandleBase();

		inline void throwIfDestroyed() const;
		
		void destroy()
		{
			if(mData->mPtr != nullptr)
			{
				GameObject* toDelete = mData->mPtr;
				mData->mPtr = nullptr; // Need to set this to null before deleting, otherwise destructor thinks object isn't destroyed
				// and we end up here again

				if(mData->mDeleter != nullptr)
					mData->mDeleter(toDelete);
				else
					delete toDelete;
			}
		}

		std::shared_ptr<GameObjectHandleData> mData;
	};

	// NOTE: It is important this class contains no data since we often value 
	// cast it to its base 
	template <typename T>
	class GameObjectHandle : public GameObjectHandleBase
	{
	public:
		GameObjectHandle()
			:GameObjectHandleBase()
		{	
			mData = cm_shared_ptr<GameObjectHandleData, PoolAlloc>();
		}

		template <typename T1>
		GameObjectHandle(const GameObjectHandle<T1>& ptr)
			:GameObjectHandleBase()
		{ 	
			mData = ptr.getHandleData();
		}

		inline GameObjectHandle<T>& operator=(std::nullptr_t ptr)
		{ 	
			mData = cm_shared_ptr<GameObjectHandleData, PoolAlloc>();
			mData->mPtr = nullptr;

			return *this;
		}

		GameObjectHandleBase toBase()
		{
			GameObjectHandleBase base;
			base.mData = this->mData;

			return base;
		}

		T* get() const 
		{ 
			throwIfDestroyed();

			return reinterpret_cast<T*>(mData->mPtr); 
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
		friend SceneObject;

		explicit GameObjectHandle(T* ptr, void(*deleter)(GameObject*) = nullptr)
			:GameObjectHandleBase()
		{
			mData = cm_shared_ptr<GameObjectHandleData, PoolAlloc>((GameObject*)ptr, deleter);
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