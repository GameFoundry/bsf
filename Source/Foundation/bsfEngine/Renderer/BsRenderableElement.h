//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsPrerequisites.h"
#include "RenderAPI/BsSubMesh.h"

namespace bs { namespace ct
{
	/** @addtogroup Renderer-Engine-Internal
	 *  @{
	 */

	/** Contains all information needed for rendering a single sub-mesh. Closely tied with Renderer. */
	class BS_EXPORT RenderableElement
	{
	public:
		/**	Reference to the mesh to render. */
		SPtr<Mesh> mesh;

		/**	Portion of the mesh to render. */
		SubMesh subMesh;

		/**	Material to render the mesh with. */
		SPtr<Material> material;
	};

	/** @} */
}}