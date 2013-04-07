#pragma once

#include "CmPrerequisites.h"
#include "CmOverlay.h"

namespace CamelotEngine
{
	class CM_EXPORT GUIWidget : public Overlay
	{
	public:
		virtual ~GUIWidget();

		virtual void render(const Camera* camera, DeferredRenderContextPtr& renderContext) const;
	protected:
		friend class GameObject;

		GUIWidget(const HGameObject& parent);

		GUILabel* addLabel(const String& text);

		void setSkin(const GUISkin* skin);
		const GUISkin* getGUISkin() const;
	private:

		void updateMeshes() const;

		vector<GUIElement*>::type mElements;
		mutable vector<HMesh>::type mCachedMeshes;
		mutable vector<HMaterial>::type mCachedMaterials;
		const GUISkin* mSkin;
		static GUISkin DefaultSkin;
	};
}