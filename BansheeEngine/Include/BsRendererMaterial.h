#pragma once

#include "BsPrerequisites.h"
#include "BsRendererMaterialManager.h"

#define RMAT_DEF(path) virtual Path getShaderPath() const override { return path; }

namespace BansheeEngine
{
	class RendererMaterialManager;

	/**
	 * @brief	Base class for all RendererMaterial instances, containing common data and methods.
	 */
	class BS_EXPORT RendererMaterialBase
	{
	public:
		virtual ~RendererMaterialBase() { }

		/**
		 * @brief	Returns path relative to the default shader folder where the material shader is located.
		 */
		virtual Path getShaderPath() const { return Path::BLANK; }

		/**
		 * @brief	Returns the internal material.
		 */
		SPtr<MaterialCore> getMaterial() const { return mMaterial; }

	private:
		/**
		 * @brief	Initializes the internal material. Should be called by the renderer material manager before
		 * 			material may be used.
		 */
		void _initialize(const SPtr<ShaderCore>& shader);

	protected:
		friend class RendererMaterialManager;

		/**
		 * @brief	Allows derived classes to initialize their data.
		 */
		virtual void initialize() { }

		SPtr<MaterialCore> mMaterial;
	};

	/**
	 * @brief	Wrapper class around Material that allows a simple way to load 
	 * 			and set up materials used by the renderer.
	 */
	template<class T>
	class RendererMaterial : public RendererMaterialBase
	{
	private:
		/**
		 * @brief	Helper class that allows renderer materials be registered on program/library load.
		 */
		struct InitOnStart
		{
		public:
			InitOnStart()
			{
				RendererMaterialManager::_registerMaterial(&instance);
			}

			void makeSureIAmInstantiated() { }
		};

	public:
		RendererMaterial()
		{
			mInit.makeSureIAmInstantiated();
		}
		virtual ~RendererMaterial() { }

		static T instance;
	private:
		friend class RendererMaterialManager;

		static InitOnStart mInit;
	};

	template<class T>
	T RendererMaterial<T>::instance;

	template<class T>
	typename RendererMaterial<T>::InitOnStart RendererMaterial<T>::mInit;
}