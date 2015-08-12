#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorCommand.h"
#include "BsUndoRedo.h"

namespace BansheeEngine
{
	/**
	 * @brief	A command used for undo/redo purposes. It records a value of a GUI input field
	 *			(specified by template type) and allows you to apply or revert a change to that field
	 *			as needed.
	 */
	template <class InputFieldType, class ValueType>
	class BS_ED_EXPORT CmdInputFieldValueChange : public EditorCommand
	{
	public:
		/**
		 * @brief	Creates and executes the command on the provided object and field.
		 *			Automatically registers the command with undo/redo system.
		 *
		 * @param	inputField	Input field to modify the value on.
		 * @param	value		New value for the field.
		 * @param	description	Optional description of what exactly the command does.
		 */
		static void execute(InputFieldType* inputField, const ValueType& value, const WString& description = StringUtil::WBLANK)
		{
			CmdInputFieldValueChange* command = new (bs_alloc<CmdInputFieldValueChange>()) CmdInputFieldValueChange(description, inputField, value);
			UndoRedo::instance().registerCommand(command);
			command->commit();
		}

		/**
		 * @copydoc	EditorCommand::commit
		 */
		void commit() override
		{
			mInputField->setValue(mNewValue);
		}

		/**
		 * @copydoc	EditorCommand::revert
		 */
		void revert() override
		{
			mInputField->setValue(mOldValue);
		}

	private:
		friend class UndoRedo;

		CmdInputFieldValueChange(const WString& description, InputFieldType* inputField, const ValueType& value)
			:EditorCommand(description), mInputField(inputField), mOldValue(inputField->getValue()), mNewValue(value)
		{ }

		ValueType mOldValue;
		ValueType mNewValue;
		InputFieldType* mInputField;
	};
}