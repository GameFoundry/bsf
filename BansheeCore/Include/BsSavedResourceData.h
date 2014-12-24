#pragma once

#include "BsCorePrerequisites.h"
#include "BsIReflectable.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains information about a resource saved to the disk.
	 *
	 * @note	Purpose of this class is primarily to be a wrapper around a list of objects
	 *			to make serialization easier.
	 */
	class BS_CORE_EXPORT SavedResourceData : public IReflectable
	{
	public:
		SavedResourceData();
		SavedResourceData(const Vector<HResource>& dependencies, bool allowAsync);

		/**
		 * @brief	Returns a list of all resource dependencies.
		 */
		const Vector<HResource>& getDependencies() const { return mDependencies; }

		/**
		 * @brief	Returns true if this resource is allow to be asynchronously loaded.
		 */
		bool allowAsyncLoading() const { return mAllowAsync; }

	private:
		Vector<HResource> mDependencies;
		bool mAllowAsync;

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/
	public:
		friend class SavedResourceDataRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}