#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for GUILayout derived classes.
	 */
	class BS_SCR_BE_EXPORT ScriptGUILayout : public TScriptGUIElementBase<ScriptGUILayout>
	{
		/**
		 * @brief	Contains information about an interop object that represents
		 *			a child of the layout.
		 */
		struct ChildInfo
		{
			ScriptGUIElementBaseTBase* element;
			uint32_t gcHandle;
		};

	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "GUILayout")

		/**
		 * @brief	Returns the internal wrapped GUILayout object.
		 */
		GUILayout* getInternalValue() const { return mLayout; }

		/**
		 * @brief	Registers a new managed child GUI element and inserts it at
		 *			the end of the layout.
		 */
		void addChild(ScriptGUIElementBaseTBase* element);

		/**
		 * @brief	Registers a new managed child GUI element and inserts it at a specific
		 *			location in the layout.
		 */
		void insertChild(UINT32 index, ScriptGUIElementBaseTBase* element);

		/**
		 * @brief	Removes a managed GUI element from the layout.
		 */
		void removeChild(ScriptGUIElementBaseTBase* element);

		/**
		 * @copydoc	ScriptGUIElementBaseTBase::destroy
		 *
		 * Destroys the layout and all of its managed children.
		 */
		void destroy() override;

		/**
		 * @brief	Destroys all of the layout managed children.
		 *
		 * @note	Useful if the children are managed but the layer itself is not.
		 */
		void destroyChildren();

		/**
		 * @brief	Marks the layout as destroyed. When destroyed managed operations will be ignored
		 *			or return default values.
		 */
		void markAsDestroyed();
	private:
		friend class ScriptGUIPanel;

		ScriptGUILayout(MonoObject* instance, GUILayout* layout, bool ownsNative = true);

		GUILayout* mLayout;
		Vector<ChildInfo> mChildren;

		bool mIsDestroyed;
		bool mOwnsNative;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_createInstanceX(MonoObject* instance, MonoArray* guiOptions);
		static void internal_createInstanceY(MonoObject* instance, MonoArray* guiOptions);
		static void internal_createInstancePanel(MonoObject* instance, INT16 depth, UINT16 depthRangeMin, UINT32 depthRangeMax, MonoArray* guiOptions);
		static void internal_addElement(ScriptGUILayout* instance, ScriptGUIElementBaseTBase* element);
		static void internal_insertElement(ScriptGUILayout* instance, UINT32 index, ScriptGUIElementBaseTBase* element);
		static UINT32 internal_getChildCount(ScriptGUILayout* instance);
		static MonoObject* internal_getChild(ScriptGUILayout* instance, UINT32 index);

		static void internal_createInstanceYFromScrollArea(MonoObject* instance, MonoObject* parentScrollArea);
	};

	/**
	 * @brief	Interop class between C++ & CLR for GUIPanel. 
	 */
	class BS_SCR_BE_EXPORT ScriptGUIPanel : public ScriptObject<ScriptGUIPanel>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "GUIPanel")

		/**
		 * @brief	Creates a new managed GUIPanel that wraps the provided native GUIPanel.
		 */
		static MonoObject* createFromExisting(GUIPanel* panel);
	private:
		ScriptGUIPanel(MonoObject* instance);
	};

}