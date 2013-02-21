#pragma once

#include "CmPrerequisites.h"
#include "CmComponent.h"

namespace CamelotEngine
{
	class CM_EXPORT Renderable : public Component
	{
	public:
		void setMesh(MeshHandle mesh) { mMesh = mesh; }
		void setMaterial(MaterialHandle material) { mMaterial = material; }

		MeshHandle getMesh() const { return mMesh; }
		MaterialHandle getMaterial() const { return mMaterial; }
	private:
		MeshHandle mMesh;
		MaterialHandle mMaterial;

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