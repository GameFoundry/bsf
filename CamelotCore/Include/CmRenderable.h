#pragma once

#include "CmPrerequisites.h"
#include "CmComponent.h"

namespace CamelotEngine
{
	class CM_EXPORT Renderable : public Component
	{
	public:
		void setMesh(HMesh mesh) { mMesh = mesh; }
		void setMaterial(HMaterial material) { mMaterial = material; }

		HMesh getMesh() const { return mMesh; }
		HMaterial getMaterial() const { return mMaterial; }
	private:
		HMesh mMesh;
		HMaterial mMaterial;

		/************************************************************************/
		/* 							COMPONENT OVERRIDES                    		*/
		/************************************************************************/

	private:
		friend class GameObject;

		/** Standard constructor.
        */
		Renderable(const HGameObject& parent);

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