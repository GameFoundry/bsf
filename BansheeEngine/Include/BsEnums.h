#pragma once

namespace BansheeEngine
{
	enum class CursorType
	{
		Arrow,
		ArrowDrag,
		ArrowLeftRight,
		Wait,
		IBeam,
		SizeNESW,
		SizeNS,
		SizeNWSE,
		SizeWE,
		Deny,

		// Keep at the end
		Count
	};

	/**
	 * @brief	Contains a basic sent of renderable types that may be supported by a renderer.
	 *			These can be used as a guide and renderer plugins can use their own types as needed.
	 */
	enum RenderableType
	{
		RenType_UnlitUntextured,
		RenType_UnlitTextured,
		RenType_LitTextured
		// TODO - Add more types as I improve the Renderer with advanced techniques
	};
}