#pragma once

#include "BsPrerequisites.h"
#include "CmComponent.h"
#include "CmAABox.h"
#include "CmGpuParam.h"

namespace BansheeEngine
{
	class BS_EXPORT Renderable : public Component
	{
	public:
		void setMesh(HMesh mesh) { mMesh = mesh; }
		void setNumMaterials(UINT32 numMaterials);
		void setMaterial(UINT32 idx, HMaterial material);
		void setLayer(UINT64 layer);

		UINT64 getLayer() const { return mLayer; }
		UINT32 getNumMaterials() const { return (UINT32)mMaterials.size(); }
		HMaterial& getMaterial(UINT32 idx) { return mMaterials[idx]; }

		void render(RenderQueue& renderQueue, const Matrix4& viewProjMatrix);
		void updateWorldBounds();
	private:
		HMesh mMesh;
		Vector<HMaterial>::type mMaterials;
		UINT64 mLayer;
		Vector<AABox>::type mWorldBounds;

		Vector<GpuParamMat4>::type mMatViewProjParam;

		/************************************************************************/
		/* 							COMPONENT OVERRIDES                    		*/
		/************************************************************************/

	protected:
		friend class SceneObject;

		/** Standard constructor.
        */
		Renderable(const HSceneObject& parent);

	public:
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