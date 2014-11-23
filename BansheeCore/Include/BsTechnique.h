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
		/**
		 * @brief	Checks is the index between 0 and provided bound and throws an exception if its not.
		 */
		void checkBounds(UINT32 idx, UINT32 bound) const;

		/**
		 * @copydoc	CoreObject::markCoreDirty
		 */
		virtual void _markCoreDirty() { }

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
		
		TTechnique(const String& renderSystem, const String& renderer)
			:TechniqueBase(renderSystem, renderer)
		{ }

		virtual ~TTechnique() { }

		/**
		 * @brief	Removes a pass with the specified index.
		 */
		void removePass(UINT32 idx)
		{
			checkBounds(idx, (UINT32)mPasses.size());

			int count = 0;
			auto iter = mPasses.begin();
			while (count != idx)
			{
				++count;
				++iter;
			}

			mPasses.erase(iter);
			_markCoreDirty();
		}

		/**
		 * @brief	Returns a pass with the specified index.
		 */
		SPtr<PassType> getPass(UINT32 idx) const
		{
			checkBounds(idx, (UINT32)mPasses.size());

			return mPasses[idx];
		}

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
		TechniqueCore(const String& renderSystem, const String& renderer);

		/**
		 * @brief	Registers a new pass with the technique. It's up to the caller
		 *			to register GPU programs in the returned pass.
		 *
		 * @note	Passes added first will be executed first when rendering.
		 */
		SPtr<PassCore> addPass();

		/**
		 * @brief	Creates a new technique.
		 */
		static SPtr<TechniqueCore> create(const String& renderSystem, const String& renderer);

	protected:
		friend class ShaderCore;

		/**
		 * @copydoc	CoreObjectCore::syncToCore
		 */
		void syncToCore(const CoreSyncData& data);
	};

	/**
	 * @copydoc	TechniqueBase
	 *
	 * @note	Sim thread.
	 */
	class BS_CORE_EXPORT Technique : public IReflectable, public CoreObject, public TTechnique<false>
	{
	public:
		Technique(const String& renderSystem, const String& renderer);

		/**
		 * @brief	Registers a new pass with the technique. It's up to the caller
		 *			to register GPU programs in the returned pass.
		 *
		 * @note	Passes added first will be executed first when rendering.
		 */
		SPtr<Pass> addPass();

		/**
		 * @brief	Retrieves an implementation of a technique usable only from the
		 *			core thread.
		 */
		SPtr<TechniqueCore> getCore() const;

		/**
		 * @brief	Creates a new technique.
		 */
		static TechniquePtr create(const String& renderSystem, const String& renderer);

	protected:
		/**
		 * @copydoc	CoreObject::createCore
		 */
		SPtr<CoreObjectCore> createCore() const;

		/**
		 * @copydoc	CoreObject::markCoreDirty
		 */
		void _markCoreDirty();

		/**
		 * @copydoc	CoreObject::syncToCore
		 */
		CoreSyncData syncToCore(FrameAlloc* allocator);

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