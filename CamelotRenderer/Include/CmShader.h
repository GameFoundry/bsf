#pragma once

#include "CmPrerequisites.h"
#include "CmResource.h"

namespace CamelotEngine
{
	/**
	 * @brief	Shader represents a collection of techniques. They are used in Materials,
	 * 			which can be considered as instances of a Shader. Multiple materials
	 * 			may share the same shader but provide different parameters to it.
	 * 			
	 *			Shader will always choose the first supported technique based on the current render
	 *			system, render manager and other properties. So make sure to add most important techniques
	 *			first so you make sure they are used if they are supported.
	 */
	class CM_EXPORT Shader : public Resource
	{
	public:
		Shader(const String& name);

		/**
		 * @brief	Inherited from Resource.
		 */
		virtual void initImpl();

		TechniquePtr addTechnique(const String& renderSystem, const String& renderer);
		
		void removeTechnique(UINT32 idx);
		void removeTechnique(TechniquePtr technique);

		UINT32 getNumTechniques() const { return mTechniques.size(); }

		/**
		 * @brief	Gets the best supported technique based on current render and other systems.
		 * 			Throws an exception if not a single technique is supported.
		 */
		TechniquePtr getBestTechnique() const;

	private:
		String mName;
		vector<TechniquePtr>::type mTechniques;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
		
		/**
		 * @brief	Serialization only
		 */
		Shader() {}

	public:
		friend class ShaderRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};
}