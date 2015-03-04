#pragma once

namespace BansheeEngine
{
	/**
	 * @brief	Available cursor types.
	 */
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
	 *			These can be used just as a guide since renderer plugins can use their own types as needed.
	 */
	enum RenderableType
	{
		RenType_LitTextured
	};

	/**
	 * @brief	Type of layout that a GUIArea can initially be created with.
	 */
	enum class GUILayoutType
	{
		LayoutX, LayoutY, LayoutExplicit
	};

	/**
	 * @brief	Languages used for writing GPU programs.
	 */
	enum class GpuLanguage
	{
		HLSL, GLSL, Undefined
	};
}