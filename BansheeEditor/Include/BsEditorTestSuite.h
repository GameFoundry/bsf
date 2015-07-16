#pragma once

#include "BsEditorPrerequisites.h"
#include "BsTestSuite.h"
#include "BsComponent.h"

namespace BansheeEngine
{
	class TestComponentA : public Component
	{
	public:
		HSceneObject ref1;
		HComponent ref2;

		/************************************************************************/
		/* 							COMPONENT OVERRIDES                    		*/
		/************************************************************************/

	protected:
		friend class SceneObject;

		TestComponentA(const HSceneObject& parent);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class TestComponentARTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;

	protected:
		TestComponentA() {} // Serialization only
	};

	class TestComponentB : public Component
	{
	public:
		HSceneObject ref1;
		String val1;

		/************************************************************************/
		/* 							COMPONENT OVERRIDES                    		*/
		/************************************************************************/

	protected:
		friend class SceneObject;

		TestComponentB(const HSceneObject& parent);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class TestComponentBRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;

	protected:
		TestComponentB() {} // Serialization only
	};

	/**
	 * @brief	Contains a set of unit tests for the editor.
	 */
	class EditorTestSuite : public TestSuite
	{
	public:
		EditorTestSuite();

	private:
		/**
		 * @brief	Tests SceneObject record undo/redo operation.
		 */
		void SceneObjectRecord_UndoRedo();

		/**
		 * @brief	Tests native diff by modifiying an object, generating a diff
		 *			and re-applying the modifications.
		 */
		void BinaryDiff();

		/**
		 * @brief	Tests prefab diff by modifiying a prefab, generating a diff
		 *			and re-applying the modifications.
		 */
		void TestPrefabDiff();
	};
}