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
	class CmdInputFieldValueChange : public EditorCommand
	{
	public:
		/**
		 * @brief	Creates and executes the command on the provided object and field.
		 *			Automatically registers the command with undo/redo system.
		 *
		 * @param	inputField	Input field to modify the value on.
		 * @param	value		New value for the field.
		 */
		static void execute(InputFieldType* inputField, const ValueType& value)
		{
			CmdInputFieldValueChange* command = new (bs_alloc<CmdInputFieldValueChange>()) CmdInputFieldValueChange(inputField, value);
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

		CmdInputFieldValueChange(InputFieldType* inputField, const ValueType& value)
			:mInputField(inputField), mOldValue(inputField->getValue()), mNewValue(value)
		{ }

		ValueType mOldValue;
		ValueType mNewValue;
		InputFieldType* mInputField;
	};
}