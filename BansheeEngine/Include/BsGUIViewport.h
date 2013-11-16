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
		static const CM::String& getGUITypeName();

		/**
		 * @note Render target used by the GUIWidget and Camera must be the same. 
		 */
		static GUIViewport* create(GUIWidget& parent, const HCamera& camera, float aspectRatio, CM::Degree fieldOfView, const GUIElementStyle* style = nullptr);

		/**
		 * @note Render target used by the GUIWidget and Camera must be the same. 
		 */
		static GUIViewport* create(GUIWidget& parent, const GUIOptions& layoutOptions, const HCamera& camera, 
			float aspectRatio, CM::Degree fieldOfView, const GUIElementStyle* style = nullptr);
	protected:
		~GUIViewport();

		/**
		 * @copydoc GUIElement::getNumRenderElements()
		 */
		virtual CM::UINT32 getNumRenderElements() const;

		/**
		 * @copydoc GUIElement::getMaterial()
		 */
		virtual const GUIMaterialInfo& getMaterial(CM::UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::getNumQuads()
		 */
		virtual CM::UINT32 getNumQuads(CM::UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::fillBuffer()
		 */
		virtual void fillBuffer(CM::UINT8* vertices, CM::UINT8* uv, CM::UINT32* indices, CM::UINT32 startingQuad, 
			CM::UINT32 maxNumQuads, CM::UINT32 vertexStride, CM::UINT32 indexStride, CM::UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::updateBounds()
		 */
		virtual void updateClippedBounds();

		/**
		 * @copydoc GUIElement::updateRenderElementsInternal()
		 */
		virtual void updateRenderElementsInternal();

		virtual CM::Int2 _getOptimalSize() const;

	private:
		HCamera mCamera;
		float mAspectRatio;
		CM::Degree mFieldOfView;
		CM::Radian mVerticalFOV;

		GUIViewport(GUIWidget& parent, const GUIElementStyle* style, const HCamera& camera, float aspectRatio, CM::Degree fieldOfView, const GUILayoutOptions& layoutOptions);
	};
}