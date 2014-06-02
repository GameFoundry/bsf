#pragma once

#include "BsPrerequisites.h"
#include "BsComponent.h"
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
	public:
		/**
		 * @brief	Sets the mesh to render.
		 */
		void setMesh(HMesh mesh) { mMesh = mesh; }

		/**
		 * @brief	Sets the number of materials used by the renderable. Each material
		 *			will be used for rendering a sub-mesh in the provided Mesh.
		 */
		void setNumMaterials(UINT32 numMaterials);

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
		 * @brief	Return the number of materials used by the renderable. Each material
		 *			will be used for rendering a sub-mesh in the provided Mesh.
		 */
		UINT32 getNumMaterials() const { return (UINT32)mMaterials.size(); }

		/**
		 * @brief	Returns the material used for rendering a sub-mesh with
		 *			the specified index.
		 */
		HMaterial& getMaterial(UINT32 idx) { return mMaterials[idx]; }

		/**
		 * @brief	Called by the renderer when it is about to queue the object for rendering.
		 */
		void render(RenderQueue& renderQueue, const Matrix4& viewProjMatrix);

		/**
		 * @brief	Update world bounds based on mesh local bounds and world transform.
		 */
		void updateWorldBounds();
	private:
		HMesh mMesh;
		Vector<HMaterial> mMaterials;
		UINT64 mLayer;
		Vector<AABox> mWorldBounds;

		Vector<GpuParamMat4> mMatViewProjParam;

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