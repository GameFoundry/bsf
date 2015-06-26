#include "BsEditorTestSuite.h"
#include "BsSceneObject.h"
#include "BsCmdRecordSO.h"
#include "BsUndoRedo.h"
#include "BsRTTIType.h"
#include "BsGameObjectRTTI.h"
#include "BsBinarySerializer.h"
#include "BsMemorySerializer.h"
#include "BsBinaryDiff.h"

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

	struct TestObjectB : IReflectable
	{
		UINT32 intA = 100;
		String strA = "100";

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class TestObjectBRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	struct TestObjectA : IReflectable
	{
		TestObjectA()
		{
			arrStrA = { "10", "11", "12" };
			arrStrB = { "13", "14", "15" };
			arrStrC = { "16", "17", "18" };

			arrObjA = { TestObjectB(), TestObjectB(), TestObjectB() };
			arrObjB = { TestObjectB(), TestObjectB(), TestObjectB() };

			arrObjPtrA = { bs_shared_ptr<TestObjectB>(), bs_shared_ptr<TestObjectB>(), bs_shared_ptr<TestObjectB>() };
			arrObjPtrB = { bs_shared_ptr<TestObjectB>(), bs_shared_ptr<TestObjectB>(), bs_shared_ptr<TestObjectB>() };
		}

		UINT32 intA = 5;
		String strA = "5";
		String strB = "7";

		TestObjectB objA;
		TestObjectB objB;

		SPtr<TestObjectB> objPtrA = bs_shared_ptr<TestObjectB>();
		SPtr<TestObjectB> objPtrB = bs_shared_ptr<TestObjectB>();
		SPtr<TestObjectB> objPtrC = bs_shared_ptr<TestObjectB>();
		SPtr<TestObjectB> objPtrD = nullptr;

		Vector<String> arrStrA;
		Vector<String> arrStrB;
		Vector<String> arrStrC;

		Vector<TestObjectB> arrObjA;
		Vector<TestObjectB> arrObjB;

		Vector<SPtr<TestObjectB>> arrObjPtrA;
		Vector<SPtr<TestObjectB>> arrObjPtrB;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class TestObjectARTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class TestObjectARTTI : public RTTIType < TestObjectA, IReflectable, TestObjectARTTI >
	{
	private:
		BS_PLAIN_MEMBER(intA);
		BS_PLAIN_MEMBER(strA);
		BS_PLAIN_MEMBER(strB);

		BS_REFL_MEMBER(objA);
		BS_REFL_MEMBER(objB);

		BS_REFLPTR_MEMBER(objPtrA);
		BS_REFLPTR_MEMBER(objPtrB);
		BS_REFLPTR_MEMBER(objPtrC);
		BS_REFLPTR_MEMBER(objPtrD);

		BS_PLAIN_MEMBER_VEC(arrStrA);
		BS_PLAIN_MEMBER_VEC(arrStrB);
		BS_PLAIN_MEMBER_VEC(arrStrC);

		BS_REFL_MEMBER_VEC(arrObjA);
		BS_REFL_MEMBER_VEC(arrObjB);

		BS_REFLPTR_MEMBER_VEC(arrObjPtrA);
		BS_REFLPTR_MEMBER_VEC(arrObjPtrB);

	public:
		TestObjectARTTI()
		{
			BS_ADD_PLAIN_FIELD(intA, 0);
			BS_ADD_PLAIN_FIELD(strA, 1);
			BS_ADD_PLAIN_FIELD(strB, 2);

			BS_ADD_REFL_FIELD(objA, 3);
			BS_ADD_REFL_FIELD(objB, 4);

			BS_ADD_REFLPTR_FIELD(objPtrA, 5);
			BS_ADD_REFLPTR_FIELD(objPtrB, 6);
			BS_ADD_REFLPTR_FIELD(objPtrC, 7);
			BS_ADD_REFLPTR_FIELD(objPtrD, 8);

			BS_ADD_PLAIN_FIELD_ARR(arrStrA, 9);
			BS_ADD_PLAIN_FIELD_ARR(arrStrB, 10);
			BS_ADD_PLAIN_FIELD_ARR(arrStrC, 11);

			BS_ADD_REFL_FIELD_ARR(arrObjA, 12);
			BS_ADD_REFL_FIELD_ARR(arrObjB, 13);

			BS_ADD_REFLPTR_FIELD_ARR(arrObjPtrA, 14);
			BS_ADD_REFLPTR_FIELD_ARR(arrObjPtrB, 15);
		}

		virtual const String& getRTTIName()
		{
			static String name = "TestObjectA";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_TestObjectA;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return bs_shared_ptr<TestObjectA>();
		}
	};

	class TestObjectBRTTI : public RTTIType < TestObjectB, IReflectable, TestObjectBRTTI >
	{
	private:
		BS_PLAIN_MEMBER(intA);
		BS_PLAIN_MEMBER(strA);

	public:
		TestObjectBRTTI()
		{
			BS_ADD_PLAIN_FIELD(intA, 0);
			BS_ADD_PLAIN_FIELD(strA, 1);
		}

		virtual const String& getRTTIName()
		{
			static String name = "TestObjectB";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_TestObjectB;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return bs_shared_ptr<TestObjectB>();
		}
	};

	RTTITypeBase* TestObjectB::getRTTIStatic()
	{
		return TestObjectBRTTI::instance();
	}

	RTTITypeBase* TestObjectB::getRTTI() const
	{
		return TestObjectB::getRTTIStatic();
	}

	RTTITypeBase* TestObjectA::getRTTIStatic()
	{
		return TestObjectARTTI::instance();
	}

	RTTITypeBase* TestObjectA::getRTTI() const
	{
		return TestObjectA::getRTTIStatic();
	}

	EditorTestSuite::EditorTestSuite()
	{
		BS_ADD_TEST(EditorTestSuite::SceneObjectRecord_UndoRedo);
		BS_ADD_TEST(EditorTestSuite::BinaryDiff);
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

	void EditorTestSuite::BinaryDiff()
	{
		SPtr<TestObjectA> orgObj = bs_shared_ptr<TestObjectA>();
		SPtr<TestObjectA> newObj = bs_shared_ptr<TestObjectA>();

		newObj->intA = 995;
		newObj->strA = "potato";
		newObj->arrStrB = { "orange", "carrot" };
		newObj->arrStrC[2] = "banana";
		newObj->objB.intA = 9940;
		newObj->objPtrB->strA = "kiwi";
		newObj->objPtrC = nullptr;
		newObj->objPtrD = bs_shared_ptr<TestObjectB>();
		newObj->arrObjB[1].strA = "strawberry";
		newObj->arrObjPtrB[0]->intA = 99100;

		MemorySerializer ms;
		UINT32 orgDataLength = 0;
		UINT8* orgData = ms.encode(orgObj.get(), orgDataLength, &bs_alloc);

		UINT32 newDataLength = 0;
		UINT8* newData = ms.encode(newObj.get(), newDataLength, &bs_alloc);

		BinarySerializer bs;
		SPtr<SerializedObject> orgSerialized = bs._decodeIntermediate(orgData, orgDataLength);
		SPtr<SerializedObject> newSerialized = bs._decodeIntermediate(newData, newDataLength);

		IDiff& diffHandler = orgObj->getRTTI()->getDiffHandler();
		SPtr<SerializedObject> objDiff = diffHandler.generateDiff(orgSerialized, newSerialized);
		diffHandler.applyDiff(orgObj, objDiff);

		bs_free(orgData);
		bs_free(newData);

		BS_TEST_ASSERT(orgObj->intA == newObj->intA);
		BS_TEST_ASSERT(orgObj->strA == newObj->strA);
		BS_TEST_ASSERT(orgObj->strB == newObj->strB);

		BS_TEST_ASSERT(orgObj->objA.intA == newObj->objA.intA);
		BS_TEST_ASSERT(orgObj->objB.intA == newObj->objB.intA);
		
		BS_TEST_ASSERT(orgObj->objPtrA->strA == newObj->objPtrA->strA);
		BS_TEST_ASSERT(orgObj->objPtrB->strA == newObj->objPtrB->strA);
		BS_TEST_ASSERT(orgObj->objPtrD->strA == newObj->objPtrD->strA);
		BS_TEST_ASSERT(orgObj->objPtrC == newObj->objPtrC);

		BS_TEST_ASSERT(orgObj->arrStrA.size() == newObj->arrStrA.size());
		for (UINT32 i = 0; i < (UINT32)orgObj->arrStrA.size(); i++)
			BS_TEST_ASSERT(orgObj->arrStrA[i] == newObj->arrStrA[i]);

		BS_TEST_ASSERT(orgObj->arrStrB.size() == newObj->arrStrB.size());
		for (UINT32 i = 0; i < (UINT32)orgObj->arrStrB.size(); i++)
			BS_TEST_ASSERT(orgObj->arrStrB[i] == newObj->arrStrB[i]);

		BS_TEST_ASSERT(orgObj->arrStrC.size() == newObj->arrStrC.size());
		for (UINT32 i = 0; i < (UINT32)orgObj->arrStrC.size(); i++)
			BS_TEST_ASSERT(orgObj->arrStrC[i] == newObj->arrStrC[i]);

		BS_TEST_ASSERT(orgObj->arrObjA.size() == newObj->arrObjA.size());
		for (UINT32 i = 0; i < (UINT32)orgObj->arrObjA.size(); i++)
			BS_TEST_ASSERT(orgObj->arrObjA[i].strA == newObj->arrObjA[i].strA);

		BS_TEST_ASSERT(orgObj->arrObjB.size() == newObj->arrObjB.size());
		for (UINT32 i = 0; i < (UINT32)orgObj->arrObjB.size(); i++)
			BS_TEST_ASSERT(orgObj->arrObjB[i].strA == newObj->arrObjB[i].strA);

		BS_TEST_ASSERT(orgObj->arrObjPtrA.size() == newObj->arrObjPtrA.size());
		for (UINT32 i = 0; i < (UINT32)orgObj->arrObjPtrA.size(); i++)
			BS_TEST_ASSERT(orgObj->arrObjPtrA[i]->intA == newObj->arrObjPtrA[i]->intA);

		BS_TEST_ASSERT(orgObj->arrObjPtrB.size() == newObj->arrObjPtrB.size());
		for (UINT32 i = 0; i < (UINT32)orgObj->arrObjPtrB.size(); i++)
			BS_TEST_ASSERT(orgObj->arrObjPtrB[i]->intA == newObj->arrObjPtrB[i]->intA);
	}

}