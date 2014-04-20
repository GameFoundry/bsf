#pragma once

namespace BansheeEngine
{
	class GameObjectManager;

	struct CM_EXPORT GameObjectHandleData
	{
		GameObjectHandleData()
			:mPtr(nullptr), mInstanceId(0)
		{ }

		GameObjectHandleData(const std::shared_ptr<GameObjectInstanceData>& ptr)
		{
			mPtr = ptr;
			if(ptr != nullptr)
				mInstanceId = ptr->object->getInstanceId();
			else
				mInstanceId = 0;
		}

		std::shared_ptr<GameObjectInstanceData> mPtr;
		UINT64 mInstanceId;
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
	class CM_EXPORT GameObjectHandleBase : public IReflectable
	{
	public:
		GameObjectHandleBase();

		/**
		 * @brief	Checks if the object has been destroyed
		 */
		bool isDestroyed() const { return mData->mPtr == nullptr || mData->mPtr->object == nullptr; }

		/**
		 * @brief	Internal method only. Not meant to be called directly.
		 */
		std::shared_ptr<GameObjectHandleData> getHandleData() const { return mData; }

		UINT64 getInstanceId() const { return mData->mInstanceId; }

		void resolve(const GameObjectHandleBase& object);

		GameObject* get() const 
		{ 
			throwIfDestroyed();

			return mData->mPtr->object.get(); 
		}

		std::shared_ptr<GameObject> getInternalPtr() const
		{
			throwIfDestroyed();

			return mData->mPtr->object;
		}

		GameObject* operator->() const { return get(); }
		GameObject& operator*() const { return *get(); }

	protected:
		friend class SceneObject;
		friend class SceneObjectRTTI;
		friend class GameObjectManager;

		GameObjectHandleBase(const std::shared_ptr<GameObject> ptr);
		GameObjectHandleBase(const std::shared_ptr<GameObjectHandleData>& data);
		GameObjectHandleBase(std::nullptr_t ptr);

		inline void throwIfDestroyed() const;
		
		void destroy()
		{
			// We need to clear mData->mPtr before we clear mData->mPtr->object,
			// as this handle could be stored within the "object" and destroyed when
			// we set it to null 
			std::shared_ptr<GameObjectInstanceData> instanceData = mData->mPtr;
			mData->mPtr = nullptr;

			if(instanceData != nullptr)
				instanceData->object = nullptr;
		}

		std::shared_ptr<GameObjectHandleData> mData;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class GameObjectHandleRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
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

		GameObjectHandle(const GameObjectHandleBase& ptr)
			:GameObjectHandleBase()
		{ 	
			mData = ptr.getHandleData();
		}

		inline GameObjectHandle<T>& operator=(std::nullptr_t ptr)
		{ 	
			mData = cm_shared_ptr<GameObjectHandleData, PoolAlloc>();

			return *this;
		}

		inline operator GameObjectHandleBase()
		{
			GameObjectHandleBase base(mData);

			return base;
		}

		T* get() const 
		{ 
			throwIfDestroyed();

			return reinterpret_cast<T*>(mData->mPtr->object.get()); 
		}

		std::shared_ptr<T> getInternalPtr() const
		{
			throwIfDestroyed();

			return std::static_pointer_cast<T>(mData->mPtr->object);
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
			return (((mData->mPtr != nullptr) && (mData->mPtr->object != nullptr)) ? &CM_Bool_struct<T>::_Member : 0);
		}

	private:
		friend class SceneObject;
		friend class SceneObjectRTTI;
		friend class GameObjectManager;

		explicit GameObjectHandle(const std::shared_ptr<T> ptr)
			:GameObjectHandleBase(ptr)
		{ }
	};

	template<class _Ty1, class _Ty2>
		GameObjectHandle<_Ty1> static_object_cast(const GameObjectHandle<_Ty2>& other)
	{	
		return GameObjectHandle<_Ty1>(other);
	}

	template<class _Ty1, class _Ty2>
	bool operator==(const GameObjectHandle<_Ty1>& _Left, const GameObjectHandle<_Ty2>& _Right)
	{	
		return (_Left == nullptr && _Right == nullptr) || (_Left != nullptr && _Right != nullptr && _Left.get() == _Right.get());
	}

	template<class _Ty1, class _Ty2>
	bool operator!=(const GameObjectHandle<_Ty1>& _Left, const GameObjectHandle<_Ty2>& _Right)
	{	
		return (!(_Left == _Right));
	}
}