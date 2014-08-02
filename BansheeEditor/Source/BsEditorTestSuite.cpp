#include "BsEditorTestSuite.h"
#include "BsSceneObject.h"
#include "BsCmdRecordSO.h"
#include "BsUndoRedo.h"
#include "BsRTTIType.h"
#include "BsGameObjectRTTI.h"

namespace BansheeEngine
{
	class TestComponentARTTI : public RTTIType<TestComponentA, Component, TestComponentARTTI>
	{
	private:
		HSceneObject& getRef1(TestComponentA* obj) { return obj->ref1; }
		void setRef1(TestComponentA* obj, HSceneObject& val) { obj->ref1 = val; }

		HComponent& getRef2(TestComponentA* obj) { return obj->ref2; }
		void setRef2(TestComponentA* obj, HComponent& val) { obj->ref2 = val; }

	public:
		TestComponentARTTI()
		{
			addReflectableField("ref1", 0, &TestComponentARTTI::getRef1, &TestComponentARTTI::setRef1);
			addReflectableField("ref2", 1, &TestComponentARTTI::getRef2, &TestComponentARTTI::setRef2);
		}

		virtual const String& getRTTIName()
		{
			static String name = "TestComponentA";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_TestComponentA;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return GameObjectRTTI::createGameObject<TestComponentA>();
		}
	};

	class TestComponentBRTTI : public RTTIType<TestComponentB, Component, TestComponentBRTTI>
	{
	private:
		HSceneObject& getRef1(TestComponentB* obj) { return obj->ref1; }
		void setRef1(TestComponentB* obj, HSceneObject& val) { obj->ref1 = val; }

		String& getVal1(TestComponentB* obj) { return obj->val1; }
		void setVal1(TestComponentB* obj, String& val) { obj->val1 = val; }

	public:
		TestComponentBRTTI()
		{
			addReflectableField("ref1", 0, &TestComponentBRTTI::getRef1, &TestComponentBRTTI::setRef1);
			addPlainField("val1", 1, &TestComponentBRTTI::getVal1, &TestComponentBRTTI::setVal1);
		}

		virtual const String& getRTTIName()
		{
			static String name = "TestComponentB";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_TestComponentB;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return GameObjectRTTI::createGameObject<TestComponentB>();
		}
	};

	TestComponentA::TestComponentA(const HSceneObject& parent)
		:Component(parent)
	{}

	RTTITypeBase* TestComponentA::getRTTIStatic()
	{
		return TestComponentARTTI::instance();
	}

	RTTITypeBase* TestComponentA::getRTTI() const
	{
		return TestComponentA::getRTTIStatic();
	}

	TestComponentB::TestComponentB(const HSceneObject& parent)
		:Component(parent)
	{}

	RTTITypeBase* TestComponentB::getRTTIStatic()
	{
		return TestComponentBRTTI::instance();
	}

	RTTITypeBase* TestComponentB::getRTTI() const
	{
		return TestComponentB::getRTTIStatic();
	}

	EditorTestSuite::EditorTestSuite()
	{
		BS_ADD_TEST(EditorTestSuite::SceneObjectRecord_UndoRedo)
	}

	void EditorTestSuite::SceneObjectRecord_UndoRedo()
	{
		HSceneObject so0_0 = SceneObject::create("so0_0");
		HSceneObject so1_0 = SceneObject::create("so1_0");
		HSceneObject so1_1 = SceneObject::create("so1_1");
		HSceneObject so2_0 = SceneObject::create("so2_0");

		so1_0->setParent(so0_0);
		so1_1->setParent(so0_0);
		so2_0->setParent(so1_0);

		GameObjectHandle<TestComponentA> cmpA1_1 = so1_1->addComponent<TestComponentA>();
		GameObjectHandle<TestComponentB> cmpB1_1 = so1_1->addComponent<TestComponentB>();

		HSceneObject soExternal = SceneObject::create("soExternal");
		GameObjectHandle<TestComponentA> cmpExternal = soExternal->addComponent<TestComponentA>();

		cmpA1_1->ref1 = so2_0;
		cmpA1_1->ref2 = cmpB1_1;
		cmpB1_1->ref1 = soExternal;
		cmpB1_1->val1 = "InitialValue";
		cmpExternal->ref1 = so1_1;
		cmpExternal->ref2 = cmpA1_1;

		CmdRecordSO::execute(so0_0);
		cmpB1_1->val1 = "ModifiedValue";
		UndoRedo::instance().undo();

		BS_TEST_ASSERT(!so0_0.isDestroyed());
		BS_TEST_ASSERT(!so1_0.isDestroyed());
		BS_TEST_ASSERT(!so1_1.isDestroyed());
		BS_TEST_ASSERT(!so2_0.isDestroyed());
		BS_TEST_ASSERT(!cmpA1_1.isDestroyed());
		BS_TEST_ASSERT(!cmpB1_1.isDestroyed());
		BS_TEST_ASSERT(!cmpA1_1->ref1.isDestroyed());
		BS_TEST_ASSERT(!cmpA1_1->ref2.isDestroyed());
		BS_TEST_ASSERT(!cmpB1_1->ref1.isDestroyed());
		BS_TEST_ASSERT(!cmpExternal->ref1.isDestroyed());
		BS_TEST_ASSERT(!cmpExternal->ref2.isDestroyed());
		BS_TEST_ASSERT(cmpB1_1->val1 == "InitialValue");
	}
}