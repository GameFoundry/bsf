#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElement.h"
#include "CmMath.h"

namespace BansheeEngine
{
	/**
	 * @brief	Displays a Camera view in the form of a GUI element.
	 */
	class BS_EXPORT GUIViewport : public GUIElement
	{
	public:
		static const String& getGUITypeName();

		/**
		 * @note Render target used by the GUIWidget and Camera must be the same. 
		 */
		static GUIViewport* create(const HCamera& camera, float aspectRatio, Degree fieldOfView, const String& styleName = StringUtil::BLANK);

		/**
		 * @note Render target used by the GUIWidget and Camera must be the same. 
		 */
		static GUIViewport* create(const GUIOptions& layoutOptions, const HCamera& camera, 
			float aspectRatio, Degree fieldOfView, const String& styleName = StringUtil::BLANK);

		virtual Vector2I _getOptimalSize() const;

	protected:
		~GUIViewport();

		/**
		 * @copydoc GUIElement::getNumRenderElements()
		 */
		virtual UINT32 getNumRenderElements() const;

		/**
		 * @copydoc GUIElement::getMaterial()
		 */
		virtual const GUIMaterialInfo& getMaterial(UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::getNumQuads()
		 */
		virtual UINT32 getNumQuads(UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::fillBuffer()
		 */
		virtual void fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, 
			UINT32 maxNumQuads, UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::updateBounds()
		 */
		virtual void updateClippedBounds();

		/**
		 * @copydoc GUIElement::updateRenderElementsInternal()
		 */
		virtual void updateRenderElementsInternal();

	private:
		HCamera mCamera;
		float mAspectRatio;
		Degree mFieldOfView;
		Radian mVerticalFOV;

		GUIViewport(const String& styleName, const HCamera& camera, float aspectRatio, Degree fieldOfView, const GUILayoutOptions& layoutOptions);

		void _changeParentWidget(GUIWidget* widget);
	};
}