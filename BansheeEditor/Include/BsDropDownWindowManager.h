#pragma once

#include "BsEditorPrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Handles opening and closing of a drop down window. Only a single drop down window
	 *			can be open at a time.
	 */
	class BS_ED_EXPORT DropDownWindowManager : public Module <DropDownWindowManager>
	{
	public:
		DropDownWindowManager();
		~DropDownWindowManager();

		/**
		 * @brief	Opens a new drop down window with the specified type and arguments.
		 *
		 * @param	parent		Render window parent in which to open the drop down window.
		 * @param	target		Viewport parent in which to open the drop down window.
		 * @param	position	Position relative to the viewport at which to open the drop down window.
		 * @param	...args		A set of arguments to be passed along to the drop down window constructor.
		 *
		 * @returns	An instance of the newly created drop down window.
		 *
		 * @note	This method will automatically close any existing drop down windows before opening
		 *			a new one.
		 */
		template<class T, class... Args>
		T* open(const RenderWindowPtr& parent, Viewport* target,
			const Vector2I& position, Args &&...args)
		{
			close();

			mOpenWindow = bs_new<T>(parent, target, position, std::forward<Args>(args)...);
			return static_cast<T*>(mOpenWindow);
		}

		/**
		 * @brief	Closes a drop down window if one is open.
		 */
		void close();

		/**
		 * @brief	To be called once per frame.
		 *
		 * @note	Internal method.
		 */
		void update();
	protected:
		DropDownWindow* mOpenWindow;
	};
}