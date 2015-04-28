#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElement.h"
#include "BsMath.h"

namespace BansheeEngine
{
	/**
	 * @brief	Displays a Camera view in the form of a GUI element.
	 */
	class BS_EXPORT GUIViewport : public GUIElement
	{
	public:
		/**
		 * Returns type name of the GUI element used for finding GUI element styles. 
		 */
		static const String& getGUITypeName();

		/**
		 * @brief	Creates a new GUI viewport element.
		 *
		 * @param	camera			Camera whos view to display in the element. Element will update the camera
		 *							as it resizes.
		 * @param	aspectRatio		Initial aspect of the camera. Determines how is FOV adjusted as the element resizes.
		 * @param	fieldOfView		Initial FOV of the camera. Determines how is FOV adjusted as the element resizes.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 *
		 * @note Render target used by the GUIWidget and Camera must be the same. 
		 */
		static GUIViewport* create(const HCamera& camera, float aspectRatio, Degree fieldOfView, const String& styleName = StringUtil::BLANK);

		/**
		 * @brief	Creates a new GUI viewport element.
		 *
		 * @param	camera			Camera whos view to display in the element. Element will update the camera
		 *							as it resizes.
		 * @param	aspectRatio		Initial aspect of the camera. Determines how is FOV adjusted as the element resizes.
		 * @param	fieldOfView		Initial FOV of the camera. Determines how is FOV adjusted as the element resizes.
		 * @param	layoutOptions	Options that allows you to control how is the element positioned in
		 *							GUI layout. This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 *
		 * @note Render target used by the GUIWidget and Camera must be the same. 
		 */
		static GUIViewport* create(const GUIOptions& layoutOptions, const HCamera& camera, 
			float aspectRatio, Degree fieldOfView, const String& styleName = StringUtil::BLANK);

		/**
		 * @copydoc	GUIElement::_getOptimalSize
		 */
		virtual Vector2I _getOptimalSize() const;

	protected:
		~GUIViewport();

		/**
		 * @copydoc GUIElement::getNumRenderElements
		 */
		virtual UINT32 _getNumRenderElements() const;

		/**
		 * @copydoc GUIElement::getMaterial
		 */
		virtual const GUIMaterialInfo& _getMaterial(UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::getNumQuads
		 */
		virtual UINT32 _getNumQuads(UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::fillBuffer
		 */
		virtual void _fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, 
			UINT32 maxNumQuads, UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::updateBounds
		 */
		virtual void updateClippedBounds();

		/**
		 * @copydoc GUIElement::updateRenderElementsInternal
		 */
		virtual void updateRenderElementsInternal();

	private:
		GUIViewport(const String& styleName, const HCamera& camera, float aspectRatio, Degree fieldOfView, const GUIDimensions& layoutOptions);

		/**
		 * @copydoc	GUIElement::_changeParentWidget
		 */
		void _changeParentWidget(GUIWidget* widget);

		HCamera mCamera;
		float mAspectRatio;
		Degree mFieldOfView;
		Radian mVerticalFOV;
	};
}