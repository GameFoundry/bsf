#pragma once

#include "BsPrerequisites.h"
#include "BsRenderOperation.h"
#include "CmComponent.h"
#include "CmAABox.h"

namespace BansheeEngine
{
	class BS_EXPORT Renderable : public CM::Component
	{
	public:
		void setMesh(CM::HMesh mesh) { mMesh = mesh; }
		void setNumMaterials(CM::UINT32 numMaterials);
		void setMaterial(CM::UINT32 idx, CM::HMaterial material);
		void setLayer(CM::UINT64 layer);

		CM::UINT32 getNumRenderOperations() const;

		/**
		* @note  This method relies on getNumRenderOperations() being called and ensuring that idx doesn't exceed
		* the number returned by that method
		*/
		RenderOperation getRenderOperation(CM::UINT32 idx) const;
		CM::UINT64 getLayer() const { return mLayer; }

		void updateWorldBounds();
	private:
		CM::HMesh mMesh;
		CM::Vector<CM::HMaterial>::type mMaterials;
		CM::UINT64 mLayer;
		CM::Vector<CM::AABox>::type mWorldBounds;

		/************************************************************************/
		/* 							COMPONENT OVERRIDES                    		*/
		/************************************************************************/

	protected:
		friend class CM::SceneObject;

		/** Standard constructor.
        */
		Renderable(const CM::HSceneObject& parent);

	public:
		virtual void update() {}

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class RenderableRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;

	protected:
		Renderable() {} // Serialization only
	};
}