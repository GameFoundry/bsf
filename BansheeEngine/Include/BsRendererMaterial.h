//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPrerequisites.h"
#include "BsMaterial.h"
#include "BsRendererMaterialManager.h"

#define RMAT_DEF(path)\
	public: \
	static void _initMetaData() \
	{ \
		RendererMaterialManager::_registerMaterial(&mMetaData, path); \
	}; \

namespace BansheeEngine
{
	/**
	 * @brief	Contains data common to all render material instances of a specific type.
	 */
	struct RendererMaterialMetaData
	{
		SPtr<ShaderCore> shader;
	};

	/**
	 * @brief	Base class for all RendererMaterial instances, containing common data and methods.
	 */
	class BS_EXPORT RendererMaterialBase
	{
	public:
		virtual ~RendererMaterialBase() { }

		/**
		 * @brief	Returns the internal material.
		 */
		SPtr<MaterialCore> getMaterial() const { return mMaterial; }

	protected:
		friend class RendererMaterialManager;

		SPtr<MaterialCore> mMaterial;
	};

	/**
	 * @brief	Helper class to initialize all renderer materials as soon as the library is loaded.
	 */
	template <class T>
	struct InitRendererMaterialStart
	{
	public:
		InitRendererMaterialStart()
		{
			T::_initMetaData();
		}

		/**
		 * @brief	Forces the compiler to not optimize out construction of this type.
		 */
		void instantiate() { }
	};

	/**
	 * @brief	Wrapper class around Material that allows a simple way to load 
	 * 			and set up materials used by the renderer.
	 */
	template<class T>
	class RendererMaterial : public RendererMaterialBase
	{
	public:
		RendererMaterial()
		{
			mInitOnStart.instantiate();
			mMaterial = MaterialCore::create(mMetaData.shader);
		}

		virtual ~RendererMaterial() { }

	protected:
		friend class RendererMaterialManager;

		static RendererMaterialMetaData mMetaData;
		static InitRendererMaterialStart<T> mInitOnStart;
	};

	template<class T>
	InitRendererMaterialStart<T> RendererMaterial<T>::mInitOnStart;

	template<class T>
	RendererMaterialMetaData RendererMaterial<T>::mMetaData;
}