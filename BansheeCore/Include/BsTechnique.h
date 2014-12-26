#pragma once

#include "BsCorePrerequisites.h"
#include "BsIReflectable.h"
#include "BsCoreObject.h"

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
	class BS_CORE_EXPORT TechniqueBase
	{
	public:
		TechniqueBase(const String& renderSystem, const String& renderer);
		virtual ~TechniqueBase() { }

		/**
		 * @brief	Checks if this technique is supported based on current
		 *			render and other systems.
		 */
		bool isSupported() const;

	protected:

		String mRenderSystem;
		String mRenderer;
	};

	/**
	 * @copydoc	TechniqueBase
	 *
	 * @note	Templated version that is used for implementing 
	 *			both sim and core versions of Technique.
	 */
	template<bool Core>
	class BS_CORE_EXPORT TTechnique : public TechniqueBase
	{
	public:
		template<bool Core> struct TPassType { };
		template<> struct TPassType < false > { typedef Pass Type; };
		template<> struct TPassType < true > { typedef PassCore Type; };

		typedef typename TPassType<Core>::Type PassType;
		
		TTechnique();
		TTechnique(const String& renderSystem, const String& renderer, const Vector<SPtr<PassType>>& passes);
		virtual ~TTechnique() { }

		/**
		 * @brief	Returns a pass with the specified index.
		 */
		SPtr<PassType> getPass(UINT32 idx) const;

		/**
		 * @brief	Returns total number of passes.
		 */
		UINT32 getNumPasses() const { return (UINT32)mPasses.size(); }

	protected:
		Vector<SPtr<PassType>> mPasses;
	};

	/**
	 * @copydoc	TechniqueBase
	 *
	 * @note	Core thread.
	 */
	class BS_CORE_EXPORT TechniqueCore : public CoreObjectCore, public TTechnique<true>
	{
	public:
		TechniqueCore(const String& renderSystem, const String& renderer, const Vector<SPtr<PassCore>>& passes);

		/**
		 * @brief	Creates a new technique.
		 */
		static SPtr<TechniqueCore> create(const String& renderSystem, const String& renderer, const Vector<SPtr<PassCore>>& passes);
	};

	/**
	 * @copydoc	TechniqueBase
	 *
	 * @note	Sim thread.
	 */
	class BS_CORE_EXPORT Technique : public IReflectable, public CoreObject, public TTechnique<false>
	{
	public:
		Technique(const String& renderSystem, const String& renderer, const Vector<SPtr<Pass>>& passes);

		/**
		 * @brief	Retrieves an implementation of a technique usable only from the
		 *			core thread.
		 */
		SPtr<TechniqueCore> getCore() const;

		/**
		 * @brief	Creates a new technique.
		 */
		static TechniquePtr create(const String& renderSystem, const String& renderer, const Vector<SPtr<Pass>>& passes);

	protected:
		/**
		 * @copydoc	CoreObject::createCore
		 */
		SPtr<CoreObjectCore> createCore() const;

		/**
		 * @copydoc	CoreObject::getCoreDependencies
		 */
		void getCoreDependencies(Vector<SPtr<CoreObject>>& dependencies);

		/**
		 * @brief	Creates a new technique but doesn't initialize it.
		 */
		static TechniquePtr createEmpty();

	private:
		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
		
		/**
		 * @brief	Serialization only constructor.
		 */
		Technique();

	public:
		friend class TechniqueRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};
}