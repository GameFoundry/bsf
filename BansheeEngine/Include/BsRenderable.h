#pragma once

#include "BsPrerequisites.h"
#include "CmComponent.h"

namespace BansheeEngine
{
	class BS_EXPORT Renderable : public CM::Component
	{
	public:
		void setMesh(CM::HMesh mesh) { mMesh = mesh; }
		void setMaterial(CM::HMaterial material) { mMaterial = material; }

		CM::HMesh getMesh() const { return mMesh; }
		CM::HMaterial getMaterial() const { return mMaterial; }
	private:
		CM::HMesh mMesh;
		CM::HMaterial mMaterial;

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