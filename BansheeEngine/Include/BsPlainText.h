#pragma once

#include "BsPrerequisites.h"
#include "BsResource.h"

namespace BansheeEngine
{
	/**
	* @brief	Raw text resource.
	*/
	class BS_EXPORT PlainText : public Resource
	{
	public:
		/**
		 * @brief	Returns the text contained in the resource.
		 */
		const WString& getString() const { return mString; }

		/**
		 * @brief	Modifies the text contained in the resource.
		 */
		void setString(const WString& data) { mString = data; }

		/**
		 * @brief	Creates a new text file resource with the specified string.
		 */
		static HPlainText create(const WString& data);

		/**
		 * @brief	Creates an include file resource with the specified include string.
		 *
		 * @note	Internal method. Use "create" for normal use.
		 */
		static PlainTextPtr _createPtr(const WString& data);
	private:
		PlainText(const WString& data);

		WString mString;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class PlainTextRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};
}