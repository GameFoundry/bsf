#pragma once

#include "CmPrerequisites.h"
#include "CmComponent.h"

namespace CamelotEngine
{
	class CM_EXPORT Renderable : public Component
	{
	public:
		void setMesh(MeshRef mesh) { mMesh = mesh; }
		void setMaterial(MaterialRef material) { mMaterial = material; }

		MeshRef getMesh() const { return mMesh; }
		MaterialRef getMaterial() const { return mMaterial; }
	private:
		MeshRef mMesh;
		MaterialRef mMaterial;

		/************************************************************************/
		/* 							COMPONENT OVERRIDES                    		*/
		/************************************************************************/

	private:
		friend class GameObject;

		/** Standard constructor.
        */
		Renderable(GameObjectPtr parent);

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