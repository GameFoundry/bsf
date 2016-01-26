#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorCommand.h"
#include "BsUndoRedo.h"

namespace BansheeEngine
{
	template <class InputFieldType, class ValueType>
	class CmdInputFieldValueChange : public EditorCommand
	{
	public:
		static void execute(InputFieldType* inputField, const ValueType& value)
		{
			CmdInputFieldValueChange* command = new (bs_alloc<CmdInputFieldValueChange>()) CmdInputFieldValueChange(inputField, value);
			UndoRedo::instance().registerCommand(command);
			command->commit();
		}

		void commit()
		{
			mInputField->setValue(mNewValue);
		}

		void revert()
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