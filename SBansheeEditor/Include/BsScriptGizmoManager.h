//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Available flags to be used when defining gizmos.
	 */
	// Note: Must match the C# enum DrawGizmoFlags
	enum class DrawGizmoFlags
	{
		Selected = 0x01, /**< Gizmo is only displayed when its scene object is selected. */
		ParentSelected = 0x02, /**< Gizmo is only displayed when its parent scene object is selected. */
		NotSelected = 0x04, /**< Gizmo is only displayed when its scene object is not selected. */
		Pickable = 0x08 /**< Gizmo can be clicked on in scene view, which will select its scene object. */
	};

	/**
	 * @brief	Manages all active managed gizmo methods.
	 */
	class BS_SCR_BED_EXPORT ScriptGizmoManager : public Module<ScriptGizmoManager>
	{
		/**
		 * @brief	Data about a managed gizmo method.
		 */
		struct GizmoData
		{
			MonoClass* componentType; /**< Component the gizmo method belongs to. */
			MonoMethod* drawGizmosMethod; /**< Method that displays the gizmo. */
			UINT32 flags; /**< Gizmo flags of type DrawGizmoFlags that control gizmo properties. */
		};

	public:
		ScriptGizmoManager(ScriptAssemblyManager& scriptObjectManager);
		~ScriptGizmoManager();

		/**
		 * @brief	Iterates over all managed gizmos, calls their draw methods and registers
		 *			the gizmos with the native GizmoManager.
		 */
		void update();

	private:
		/**
		 * @brief	Finds all gizmo methods (marked with the DrawGizmo attribute). Clears any previously found methods.
		 */
		void reloadAssemblyData();

		/**
		 * @brief	Checks is the provided method a valid gizmo draw method and if it is, returns
		 *			properties of that method.
		 *
		 * @param	method			Method to check.
		 * @param	componentType	Output parameter containing the component the method is part of. Only valid if this method returns true.
		 * @param	drawGizmoFlags	Output parameters containing optional flags that control gizmo properties. Only valid if this method returns true.
		 *
		 * @return	True if the method is a valid draw gizmo method.
		 */
		bool isValidDrawGizmoMethod(MonoMethod* method, MonoClass*& componentType, UINT32& drawGizmoFlags);

		ScriptAssemblyManager& mScriptObjectManager;
		HEvent mDomainLoadedConn;

		MonoClass* mDrawGizmoAttribute;
		MonoField* mFlagsField;
		Map<String, GizmoData> mGizmoDrawers;
	};
}