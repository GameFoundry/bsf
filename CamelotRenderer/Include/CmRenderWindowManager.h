#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"
#include "CmRenderWindow.h"

namespace CamelotEngine
{
	class CM_EXPORT RenderWindowManager : public Module<RenderWindowManager>
	{
	public:
		/** Creates a new rendering window.
		@remarks
		This method creates a new rendering window as specified
		by the parameters. The rendering system could be
		responsible for only a single window (e.g. in the case
		of a game), or could be in charge of multiple ones (in the
		case of a level editor). The option to create the window
		as a child of another is therefore given.
		This method will create an appropriate subclass of
		RenderWindow depending on the API and platform implementation.
		*/
		RenderWindowPtr create(const RENDER_WINDOW_DESC& desc);

	protected:
		virtual void createImpl(const RENDER_WINDOW_DESC& desc, AsyncOp& asyncOp) = 0;
	};
}