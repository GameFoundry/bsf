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

	class EditorTestSuite : public TestSuite
	{
	public:
		EditorTestSuite();

	private:
		void SceneObjectRecord_UndoRedo();
	};
}