#pragma once

#include "BsPrerequisites.h"
#include "BsComponent.h"
#include "BsRenderableProxy.h"
#include "BsAABox.h"
#include "BsGpuParam.h"

namespace BansheeEngine
{
	/**
	 * @brief	Renderable represents any visible object in the scene. It has a mesh,
	 *			bounds and a set of materials. Renderer will render any Renderable objects
	 *			visible by the camera.
	 */
	class BS_EXPORT Renderable : public Component
	{
		struct MeshData
		{
			MeshData() {}
			MeshData(const HMesh& mesh);

			HMesh mesh;
			mutable bool isLoaded;
		};

		struct MaterialData
		{
			MaterialData() {}
			MaterialData(const HMaterial& material);

			HMaterial material;
			mutable bool isLoaded;
		};

	public:
		/**
		 * @brief	Sets the mesh to render. All sub-meshes of the mesh will be rendered,
		 *			and you may set individual materials for each sub-mesh.
		 */
		void setMesh(HMesh mesh);

		/**
		 * @brief	Sets a material that will be used for rendering a sub-mesh with
		 *			the specified index. If a sub-mesh doesn't have a specific material set
		 *			then the primary material will be used.
		 */
		void setMaterial(UINT32 idx, HMaterial material);

		/**
		 * @brief	Sets the primary material to use for rendering. Any sub-mesh that 
		 *			doesn't have an explicit material set will use this material.
		 *
		 * @note	This is equivalent to calling setMaterial(0, material).
		 */
		void setMaterial(HMaterial material);

		/**
		 * @brief	Sets the layer bitfield. Renderable layer must match camera layer
		 *			in order for the camera to render the component.
		 */
		void setLayer(UINT64 layer);

		/**
		 * @brief	Gets the layer bitfield. Renderable layer must match camera layer
		 *			in order for the camera to render the component.
		 */
		UINT64 getLayer() const { return mLayer; }

		/**
		 * @brief	Returns the mesh used for rendering.
		 */
		HMesh getMesh() const { return mMeshData.mesh; }

		/**
		 * @brief	Returns the material used for rendering a sub-mesh with
		 *			the specified index.
		 */
		HMaterial getMaterial(UINT32 idx) const;

		/************************************************************************/
		/* 								CORE PROXY                      		*/
		/************************************************************************/

		/**
		 * @brief	Checks is the core dirty flag set. This is used by external systems 
		 *			to know when internal data has changed and core thread potentially needs to be notified.
		 */
		bool _isCoreDirty() const;

		/**
		 * @brief	Marks the core dirty flag as clean.
		 */
		void _markCoreClean();

		/**
		 * @brief	Marks the core data as dirty.
		 */
		void _markCoreDirty() const { mCoreDirtyFlags = 0xFFFFFFFF; }

		/**
		 * @brief	Creates a new core proxy from the currently set Renderable data. Core proxies ensure
		 *			that the core thread has all the necessary Renderable data, while avoiding the need
		 *			to manage Renderable itself on the core thread.
		 *
		 * @note	You generally need to update the core thread with a new proxy whenever core 
		 *			dirty flag is set.
		 */
		RenderableProxyPtr _createProxy() const;

		/**
		 * @brief	Returns the currently active proxy object, if any.
		 */
		RenderableProxyPtr _getActiveProxy() const { return mActiveProxy; }

		/**
		 * @brief	Changes the currently active proxy object. 
		 */
		void _setActiveProxy(const RenderableProxyPtr& proxy) { mActiveProxy = proxy; }

	private:
		/**
		 * @brief	Checks if any resources were loaded since last time. Marks the core data as dirty
		 *			if they have (does nothing if all resources are already loaded).
		 */
		void updateResourceLoadStates() const;

	private:
		MeshData mMeshData;
		Vector<MaterialData> mMaterialData;
		UINT64 mLayer;
		Vector<AABox> mWorldBounds;

		RenderableProxyPtr mActiveProxy;
		mutable UINT32 mCoreDirtyFlags;

		/************************************************************************/
		/* 							COMPONENT OVERRIDES                    		*/
		/************************************************************************/

	protected:
		friend class SceneObject;

		Renderable(const HSceneObject& parent);

	public:
		/**
		 * @copydoc	Component::update
		 */
		virtual void update() {}

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class RenderableRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;

	protected:
		Renderable() {} // Serialization only
	};
}