#pragma once

#include "CmPrerequisites.h"
#include "CmIReflectable.h"

namespace BansheeEngine
{
	/**
	 * @brief	Technique represents a specific implementation of a shader. Contains
	 *			a number of passes that will be executed when rendering objects using this technique.
	 *
	 * @note	Normally you want to have a separate technique for every render system and renderer your 
	 *			application supports. For example, if you are supporting DirectX11 and OpenGL you will
	 *			want to have two techniques, one using HLSL based GPU programs, other using GLSL. Those
	 *			techniques should try to mirror each others end results.
	 */
	class CM_EXPORT Technique : public IReflectable
	{
	public:
		Technique(const String& renderSystem, const String& renderer);

		/**
		 * @brief	Registers a new pass with the technique. It's up to the caller
		 *			to register GPU programs in the returned pass.
		 *
		 * @note	Passes added first will be executed first when rendering.
		 */
		PassPtr addPass();

		/**
		 * @brief	Removes a pass with the specified index.
		 */
		void removePass(UINT32 idx);

		/**
		 * @brief	Returns a pass with the specified index.
		 */
		PassPtr getPass(UINT32 idx) const;

		/**
		 * @brief	Returns total number of passes.
		 */
		UINT32 getNumPasses() const { return (UINT32)mPasses.size(); }

		/**
		 * @brief	Checks if this technique is supported based on current
		 *			render and other systems.
		 */
		bool isSupported() const;

	private:
		String mRenderSystem;
		String mRenderer;

		Vector<PassPtr>::type mPasses;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
		
		/**
		 * @brief	Serialization only constructor.
		 */
		Technique() {}

	public:
		friend class TechniqueRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};
}