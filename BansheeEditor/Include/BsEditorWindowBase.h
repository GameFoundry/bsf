#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	class EditorWindowBase
	{
	public:
		virtual ~EditorWindowBase();

		virtual void setPosition(INT32 x, INT32 y);
		virtual void setSize(UINT32 width, UINT32 height);

		INT32 getLeft() const;
		INT32 getTop() const;

		UINT32 getWidth() const;
		UINT32 getHeight() const;

		virtual void close();
		void hide();

		/**
		 * @brief	Called once every frame. Internal method.
		 */
		virtual void update() { }

		/**
		 * @brief	Returns the render window that this editor window is being rendered to.
		 */
		RenderWindowPtr _getRenderWindow() const { return mRenderWindow; }

	protected:
		EditorWindowBase();
		EditorWindowBase(const RenderWindowPtr& renderWindow);

		RenderWindowPtr mRenderWindow;
		HSceneObject mSceneObject;
		HGUIWidget mGUI;
		HCamera mCamera;
		GameObjectHandle<WindowFrameWidget> mWindowFrame;
		bool mOwnsRenderWindow;

		void construct(const RenderWindowPtr& renderWindow);
		virtual void initialize();
		virtual void resized() { }
	private:
		HEvent mResizedConn;
	};
}