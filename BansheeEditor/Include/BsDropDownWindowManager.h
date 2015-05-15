#pragma once

#include "BsEditorPrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT DropDownWindowManager : public Module <DropDownWindowManager>
	{
	public:
		DropDownWindowManager();
		~DropDownWindowManager();

		template<class T, class... Args>
		T* open(const RenderWindowPtr& parent, Viewport* target,
			const Vector2I& position, Args &&...args)
		{
			close();

			mOpenWindow = bs_new<T>(parent, target, position, std::forward<Args>(args)...);
			return static_cast<T*>(mOpenWindow);
		}

		void close();

		void update();
	protected:
		DropDownWindow* mOpenWindow;
	};
}