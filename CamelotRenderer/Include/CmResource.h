#pragma once

#include "CmPrerequisites.h"
#include "CmIReflectable.h"
#include "CmIDestroyable.h"

namespace CamelotEngine
{
	/**
	 * @brief	Base class for all resources used in the engine.
	 */
	class CM_EXPORT Resource : public IReflectable, public IDestroyable
	{
	public:
		Resource();
		virtual ~Resource() {};



		const String& getUUID() const { return mUUID; }

		/**
		 * @brief	Returns true if the object has been properly initialized. You are not
		 * 			allowed to call any methods on the resource until you are sure resource is initialized.
		 */
		bool isInitialized() const { return mIsInitialized; }

		/**
		 * @brief	Blocks the current thread until the resource is fully initialized.
		 */
		void waitUntilInitialized();

		/**
		 * @copydoc	IDestroyable::destroy()
		 * 			
		 * 	@note	Destruction is not done immediately, and is instead just scheduled on the
		 * 			render thread. Unless internalCall is specified, in which case it is destroyed
		 * 			right away. But you should only do this when calling from the render thread.
		 */
		void destroy(bool internalCall = false);

	protected:
		friend class Resources;
		
		/**
		 * @brief	Finishes up resource initialization. Usually called right after the resource is created.
		 * 			Make sure that derived classes implement their own initialize_internal, and make sure
		 * 			they call this implementation from it.
		 */
		virtual void initialize_internal();

		/**
		 * @brief	Destroys all texture resources, but doesn't actually delete the object.
		 */
		virtual void destroy_internal() {} // TODO - This is temporarily non-abstract

		/**
		 * @brief	Marks the resource as initialized.
		 */
		void setInitialized() { mIsInitialized = true; }

		String mUUID; 
		UINT32 mSize;

		// Transient
		bool mIsInitialized;

		CM_STATIC_THREAD_SYNCHRONISER(mResourceLoadedCondition)
		CM_STATIC_MUTEX(mResourceLoadedMutex)

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/
	public:
		friend class ResourceRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}