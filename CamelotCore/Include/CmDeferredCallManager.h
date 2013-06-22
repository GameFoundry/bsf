#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"
#include "boost/function.hpp"

namespace CamelotFramework
{
	class DeferredCall
	{
	public:
		~DeferredCall()
		{
			disconnect();
		}

		DeferredCall(DeferredCall& copy)
		{
			copy.mOwner = false;

			mId = copy.mId;
			mOwner = true;
		}

		void disconnect()
		{
			if(mOwner)
			{
				DeferredCallManager::instance().removeCall(mId);
				mOwner = false;
			}
		}

	private:
		friend class DeferredCallManager;

		DeferredCall(UINT32 id)
			:mId(id), mOwner(true)
		{ }

		UINT32 mId;
		bool mOwner;
	};

	template<int Priority>
	struct DeferredCallPriority 
	{ 
		static_assert(false, 
			"Provided type isn't plain-old-data. You need to specialize RTTIPlainType template in order to serialize this type. "\
			" (Or call CM_ALLOW_MEMCPY_SERIALIZATION(type) macro if you are sure the type can be properly serialized using just memcpy.)");

	};

	/**
	 * @brief	Allows you to queue calls that can get executed later.
	 * 			
	 * @note	Sim thread only.
	 */
	class CM_EXPORT DeferredCallManager : public Module<DeferredCallManager>
	{
		struct DeferredCallData
		{
			DeferredCallData(boost::function<void()> _func, UINT32 _id)
				:func(_func), id(_id)
			{ }

			boost::function<void()> func;
			UINT32 id;
		};

	public:
		DeferredCallManager();

		/**
		 * @brief	Register a deferred call that will be executed once at the start of next frame.
		 *
		 * @param	func		The function to execute.
		 * @tparam	priority	Priority 0-255 determines in what order to execute the calls. Calls with lowest
		 * 						priority are executed first (i.e. calls with 0 are executed first).
		 *
		 * @note	You cannot use just any random priority number. You must use priorities defined during compile time using
		 * 			DEFERRED_CALL_PRIORITY macro. Otherwise you will receive a compiler error.
		 */
		template <UINT8 priority = 0>
		void queueDeferredCall(boost::function<void()> func)
		{
			DeferredCallPriority<priority> ensureValidPriority;

			mOneShotCallbacks[priority].push_back(DeferredCallData(func, (mMaxID++) | ((UINT32)priority << 24)));
			assert(mMaxID <= (0x00FFFFFF)); // ID is 24bit max, 8 bits are reserved for priority
		}

		/**
		 * @brief	Register a deferred call that will be executed once every frame.
		 *
		 * @param	func		The function to execute.
		 * @tparam	priority	Priority 0-255 determines in what order to execute the calls. Calls with lowest
		 * 						priority are executed first (i.e. calls with 0 are executed first).
		 * 						
		 * @return  DeferredCall structure you must keep safe. Deferred call will continue to be executed as long as that structure
		 * 			exists or until DeferredCall.disconnect is called. Do not copy the returned structure in any way, only one copy must always
		 * 			exists, although I do not provide any mechanic for guaranteeing that.
		 *
		 * @note	You cannot use just any random priority number. You must use priorities defined during compile time using
		 * 			DEFERRED_CALL_PRIORITY macro. Otherwise you will receive a compiler error.
		 */
		template <UINT8 priority = 0>
		DeferredCall queueOngoingDeferredCall(boost::function<void()> func)
		{
			DeferredCallPriority<priority> ensureValidPriority;

			mOngoingCallbacks[priority].push_back(DeferredCallData(func, (mMaxID++) | ((UINT32)priority << 24)));
			assert(mMaxID <= (0x00FFFFFF)); // ID is 24bit max, 8 bits are reserved for priority
		}

	private:
		friend class DeferredCall;

		UnorderedMap<UINT8, Vector<DeferredCallData>::type>::type mOneShotCallbacks;
		UnorderedMap<UINT8, Vector<DeferredCallData>::type>::type mOngoingCallbacks;

		UINT16 mMaxID;

		void removeCall(UINT32 combinedId)
		{
			UINT8 priority = (combinedId & 0xFF000000) >> 24;
			UINT32 callId = combinedId & 0x00FFFFFF;

			bool foundCall = false;
			auto iterFind1 = mOneShotCallbacks.find(priority);
			if(iterFind1 != mOneShotCallbacks.end())
			{
				auto callbacksForPriority = iterFind1->second;
				for(auto iter = callbacksForPriority.begin(); iter != callbacksForPriority.end(); ++iter)
				{
					if(iter->id == callId)
					{
						callbacksForPriority.erase(iter);
						foundCall = true;
						break;
					}
				}
			}

			if(foundCall)
				return;

			auto iterFind2 = mOngoingCallbacks.find(priority);
			if(iterFind2 != mOngoingCallbacks.end())
			{
				auto callbacksForPriority = iterFind2->second;
				for(auto iter = callbacksForPriority.begin(); iter != callbacksForPriority.end(); ++iter)
				{
					if(iter->id == callId)
					{
						callbacksForPriority.erase(iter);
						break;
					}
				}
			}
		}
	};
}