//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "Testing/BsTestSuite.h"
#include "Scene/BsSceneObject.h"
#include "Reflection/BsRTTIType.h"
#include "Private/RTTI/BsGameObjectRTTI.h"
#include "Serialization/BsBinarySerializer.h"
#include "Serialization/BsMemorySerializer.h"
#include "Serialization/BsBinaryDiff.h"
#include "Scene/BsPrefab.h"
#include "Resources/BsResources.h"
#include "Scene/BsPrefabDiff.h"
#include "FileSystem/BsFileSystem.h"
#include "Scene/BsSceneManager.h"

namespace bs
{


	enum TestIds
	{
		TID_TestComponentA = 40008,
		TID_TestComponentB = 40009,
		TID_TestObjectA = 40013,
		TID_TestObjectB = 40014,
		TID_TestComponentC = 40015,
		TID_TestComponentD = 40016,
	};

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
		RTTITypeBase* getRTTI() const override;

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
		RTTITypeBase* getRTTI() const override;

	protected:
		TestComponentB() {} // Serialization only
	};

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

		const String& getRTTIName() override
		{
			static String name = "TestComponentA";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_TestComponentA;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return SceneObject::createEmptyComponent<TestComponentA>();
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

		const String& getRTTIName() override
		{
			static String name = "TestComponentB";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_TestComponentB;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return SceneObject::createEmptyComponent<TestComponentB>();
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
		RTTITypeBase* getRTTI() const override;
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

			arrObjPtrA = { bs_shared_ptr_new<TestObjectB>(), bs_shared_ptr_new<TestObjectB>(), bs_shared_ptr_new<TestObjectB>() };
			arrObjPtrB = { bs_shared_ptr_new<TestObjectB>(), bs_shared_ptr_new<TestObjectB>(), bs_shared_ptr_new<TestObjectB>() };
		}

		UINT32 intA = 5;
		String strA = "5";
		String strB = "7";

		TestObjectB objA;
		TestObjectB objB;

		SPtr<TestObjectB> objPtrA = bs_shared_ptr_new<TestObjectB>();
		SPtr<TestObjectB> objPtrB = bs_shared_ptr_new<TestObjectB>();
		SPtr<TestObjectB> objPtrC = bs_shared_ptr_new<TestObjectB>();
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
		RTTITypeBase* getRTTI() const override;
	};

	class TestObjectARTTI : public RTTIType < TestObjectA, IReflectable, TestObjectARTTI >
	{
	private:
		BS_BEGIN_RTTI_MEMBERS
			BS_RTTI_MEMBER_PLAIN(intA, 0)
			BS_RTTI_MEMBER_PLAIN(strA, 1)
			BS_RTTI_MEMBER_PLAIN(strB, 2)

			BS_RTTI_MEMBER_REFL(objA, 3)
			BS_RTTI_MEMBER_REFL(objB, 4)

			BS_RTTI_MEMBER_REFLPTR(objPtrA, 5)
			BS_RTTI_MEMBER_REFLPTR(objPtrB, 6)
			BS_RTTI_MEMBER_REFLPTR(objPtrC, 7)
			BS_RTTI_MEMBER_REFLPTR(objPtrD, 8)

			BS_RTTI_MEMBER_PLAIN_ARRAY(arrStrA, 9)
			BS_RTTI_MEMBER_PLAIN_ARRAY(arrStrB, 10)
			BS_RTTI_MEMBER_PLAIN_ARRAY(arrStrC, 11)

			BS_RTTI_MEMBER_REFL_ARRAY(arrObjA, 12)
			BS_RTTI_MEMBER_REFL_ARRAY(arrObjB, 13)

			BS_RTTI_MEMBER_REFLPTR_ARRAY(arrObjPtrA, 14)
			BS_RTTI_MEMBER_REFLPTR_ARRAY(arrObjPtrB, 15)
		BS_END_RTTI_MEMBERS

	public:
		TestObjectARTTI()
			:mInitMembers(this)
		{ }

		const String& getRTTIName() override
		{
			static String name = "TestObjectA";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_TestObjectA;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<TestObjectA>();
		}
	};

	class TestObjectBRTTI : public RTTIType < TestObjectB, IReflectable, TestObjectBRTTI >
	{
	private:
		BS_BEGIN_RTTI_MEMBERS
			BS_RTTI_MEMBER_PLAIN(intA, 0)
			BS_RTTI_MEMBER_PLAIN(strA, 1)
		BS_END_RTTI_MEMBERS

	public:
		TestObjectBRTTI()
			:mInitMembers(this)
		{ }

		const String& getRTTIName() override
		{
			static String name = "TestObjectB";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_TestObjectB;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<TestObjectB>();
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

	class TestComponentC : public Component
	{
	public:
		TestObjectA obj;

		/************************************************************************/
		/* 							COMPONENT OVERRIDES                    		*/
		/************************************************************************/

	protected:
		friend class SceneObject;

		TestComponentC(const HSceneObject& parent)
			:Component(parent)
		{}

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class TestComponentCRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;

	protected:
		TestComponentC() {} // Serialization only
	};

	class TestComponentD : public Component
	{
	public:
		TestObjectB obj;

		/************************************************************************/
		/* 							COMPONENT OVERRIDES                    		*/
		/************************************************************************/

	protected:
		friend class SceneObject;

		TestComponentD(const HSceneObject& parent)
			:Component(parent)
		{}

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class TestComponentDRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;

	protected:
		TestComponentD() {} // Serialization only
	};

	class TestComponentCRTTI : public RTTIType < TestComponentC, Component, TestComponentCRTTI >
	{
	private:
		BS_BEGIN_RTTI_MEMBERS
			BS_RTTI_MEMBER_REFL(obj, 0)
		BS_END_RTTI_MEMBERS

	public:
		TestComponentCRTTI()
			:mInitMembers(this)
		{ }

		const String& getRTTIName() override
		{
			static String name = "TestComponentC";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_TestComponentC;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return SceneObject::createEmptyComponent<TestComponentC>();
		}
	};

	class TestComponentDRTTI : public RTTIType < TestComponentD, Component, TestComponentDRTTI >
	{
	private:
		BS_BEGIN_RTTI_MEMBERS
			BS_RTTI_MEMBER_REFL(obj, 0)
		BS_END_RTTI_MEMBERS

	public:
		TestComponentDRTTI()
			:mInitMembers(this)
		{ }

		const String& getRTTIName() override
		{
			static String name = "TestComponentD";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_TestComponentD;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return SceneObject::createEmptyComponent<TestComponentD>();
		}
	};

	RTTITypeBase* TestComponentC::getRTTIStatic()
	{
		return TestComponentCRTTI::instance();
	}

	RTTITypeBase* TestComponentC::getRTTI() const
	{
		return TestComponentC::getRTTIStatic();
	}

	RTTITypeBase* TestComponentD::getRTTIStatic()
	{
		return TestComponentDRTTI::instance();
	}

	RTTITypeBase* TestComponentD::getRTTI() const
	{
		return TestComponentD::getRTTIStatic();
	}

	class EditorTestSuite : public bs::Test {};

	TEST(EditorTestSuite, BinaryDiff)
	{
		SPtr<TestObjectA> orgObj = bs_shared_ptr_new<TestObjectA>();
		SPtr<TestObjectA> newObj = bs_shared_ptr_new<TestObjectA>();

		newObj->intA = 995;
		newObj->strA = "potato";
		newObj->arrStrB = { "orange", "carrot" };
		newObj->arrStrC[2] = "banana";
		newObj->objB.intA = 9940;
		newObj->objPtrB->strA = "kiwi";
		newObj->objPtrC = nullptr;
		newObj->objPtrD = bs_shared_ptr_new<TestObjectB>();
		newObj->arrObjB[1].strA = "strawberry";
		newObj->arrObjPtrB[0]->intA = 99100;

		SPtr<SerializedObject> orgSerialized = SerializedObject::create(*orgObj.get());
		SPtr<SerializedObject> newSerialized = SerializedObject::create(*newObj.get());

		IDiff& diffHandler = orgObj->getRTTI()->getDiffHandler();
		SPtr<SerializedObject> objDiff = diffHandler.generateDiff(orgSerialized, newSerialized);
		diffHandler.applyDiff(orgObj, objDiff, nullptr);

		ASSERT_EQ(orgObj->intA,  newObj->intA);
		ASSERT_EQ(orgObj->strA, newObj->strA);
		ASSERT_EQ(orgObj->strB,  newObj->strB);

		ASSERT_EQ(orgObj->objA.intA,  newObj->objA.intA);
		ASSERT_EQ(orgObj->objB.intA, newObj->objB.intA);
		
		ASSERT_EQ(orgObj->objPtrA->strA, newObj->objPtrA->strA);
		ASSERT_EQ(orgObj->objPtrB->strA, newObj->objPtrB->strA);
		ASSERT_EQ(orgObj->objPtrD->strA, newObj->objPtrD->strA);
		ASSERT_EQ(orgObj->objPtrC, newObj->objPtrC);

		ASSERT_EQ(orgObj->arrStrA.size(),  newObj->arrStrA.size());
		for (UINT32 i = 0; i < (UINT32)orgObj->arrStrA.size(); i++)
			ASSERT_EQ(orgObj->arrStrA[i], newObj->arrStrA[i]);

		ASSERT_EQ(orgObj->arrStrB.size(),  newObj->arrStrB.size());
		for (UINT32 i = 0; i < (UINT32)orgObj->arrStrB.size(); i++)
			ASSERT_EQ(orgObj->arrStrB[i], newObj->arrStrB[i]);

		ASSERT_EQ(orgObj->arrStrC.size(),  newObj->arrStrC.size());
		for (UINT32 i = 0; i < (UINT32)orgObj->arrStrC.size(); i++)
			ASSERT_EQ(orgObj->arrStrC[i],  newObj->arrStrC[i]);

		ASSERT_EQ(orgObj->arrObjA.size(),  newObj->arrObjA.size());
		for (UINT32 i = 0; i < (UINT32)orgObj->arrObjA.size(); i++)
			ASSERT_EQ(orgObj->arrObjA[i].strA, newObj->arrObjA[i].strA);

		ASSERT_EQ(orgObj->arrObjB.size(), newObj->arrObjB.size());
		for (UINT32 i = 0; i < (UINT32)orgObj->arrObjB.size(); i++)
			ASSERT_EQ(orgObj->arrObjB[i].strA,  newObj->arrObjB[i].strA);

		ASSERT_EQ(orgObj->arrObjPtrA.size(), newObj->arrObjPtrA.size());
		for (UINT32 i = 0; i < (UINT32)orgObj->arrObjPtrA.size(); i++)
			ASSERT_EQ(orgObj->arrObjPtrA[i]->intA, newObj->arrObjPtrA[i]->intA);

		ASSERT_EQ(orgObj->arrObjPtrB.size(),  newObj->arrObjPtrB.size());
		for (UINT32 i = 0; i < (UINT32)orgObj->arrObjPtrB.size(); i++)
			ASSERT_EQ(orgObj->arrObjPtrB[i]->intA, newObj->arrObjPtrB[i]->intA);
	}

	TEST(EditorTestSuite, TestPrefabComplex)
	{
		HSceneObject aDeleteMe = SceneObject::create("A");

		HSceneObject box = SceneObject::create("Box");
		box->addComponent<TestComponentA>();

		HSceneObject light = SceneObject::create("Directional light");
		light->addComponent<TestComponentA>();

		HSceneObject sceneRoot = gSceneManager().getMainScene()->getRoot();
		HPrefab scenePrefab = Prefab::create(sceneRoot, true);

		HSceneObject bDeleteMe = SceneObject::create("B");

		HSceneObject _1 = SceneObject::create("1");

		scenePrefab->update(sceneRoot);

		aDeleteMe->destroy();
		bDeleteMe->destroy();

		HPrefab targetBoxPrefab = Prefab::create(_1, false);

		HSceneObject camera = SceneObject::create("Camera");
		camera->addComponent<TestComponentA>();

		scenePrefab->update(sceneRoot);

		HSceneObject target = SceneObject::create("Target");

		_1->setParent(target);

		HSceneObject _3 = targetBoxPrefab->instantiate();
		_3->setName("3");
		_3->setParent(target);

		HSceneObject _10 = targetBoxPrefab->instantiate();
		_10->setParent(target);
		_10->setName("10");

		// Ensure multiple instances of the same prefab don't have the same ID
		ASSERT_TRUE(_1->getLinkId() != _3->getLinkId() && _1->getLinkId() != _10->getLinkId());

		// Ensure new instances of a prefab have -1 root link ID
		ASSERT_TRUE(_3->getLinkId() == (UINT32)-1 && _10->getLinkId() == (UINT32)-1);

		scenePrefab->update(sceneRoot);

		_1->addComponent<TestComponentA>();
		_3->addComponent<TestComponentA>();
		_10->addComponent<TestComponentA>();

		_1->addComponent<TestComponentB>();
		_3->addComponent<TestComponentB>();
		_10->addComponent<TestComponentB>();

		_1->breakPrefabLink();
		_3->breakPrefabLink();
		_10->breakPrefabLink();

		HPrefab targetPrefab = Prefab::create(target, false);
		
		target->addComponent<TestComponentA>();
		scenePrefab->update(sceneRoot);

		targetPrefab->update(target);

		box->destroy();
		light->destroy();
		camera->destroy();
		target->destroy();
	}

	TEST(EditorTestSuite, TestPrefabDiff)
	{
		HSceneObject root = SceneObject::create("root");
		HSceneObject so0 = SceneObject::create("so0");
		HSceneObject so1 = SceneObject::create("so1");
		HSceneObject so2 = SceneObject::create("so2");
		HSceneObject so0_0 = SceneObject::create("so0_0");
		HSceneObject so0_1 = SceneObject::create("so0_1");
		HSceneObject so1_0 = SceneObject::create("so1_0");
		HSceneObject so1_1 = SceneObject::create("so1_1");
		HSceneObject so1_2 = SceneObject::create("so1_2");
		HSceneObject so2_0 = SceneObject::create("so2_0");

		so0->setParent(root);
		so1->setParent(root);
		so2->setParent(root);

		so0_0->setParent(so0);
		so0_1->setParent(so0);

		so1_0->setParent(so1);
		so1_1->setParent(so1);
		so1_2->setParent(so1);

		so2_0->setParent(so2);

		GameObjectHandle<TestComponentC> cmp0 = so0->addComponent<TestComponentC>();
		GameObjectHandle<TestComponentC> cmp0_1_A = so0_1->addComponent<TestComponentC>();
		GameObjectHandle<TestComponentD> cmp0_1_B = so0_1->addComponent<TestComponentD>();

		GameObjectHandle<TestComponentD> cmp1 = so1->addComponent<TestComponentD>();
		GameObjectHandle<TestComponentD> cmp1_2 = so1_2->addComponent<TestComponentD>();

		GameObjectHandle<TestComponentD> cmp2 = so2->addComponent<TestComponentD>();

		Path prefabPath = Path::combine(FileSystem::getTempDirectoryPath(), "testprefab.asset");
		HPrefab prefab = Prefab::create(root);
		gResources().save(prefab, prefabPath, true);

		// Perform modifications
		GameObjectHandle<TestComponentC> cmp1_3;
		GameObjectHandle<TestComponentD> cmp3;
		HSceneObject so1_3, so2_1, so3;
		{
			cmp0->obj.strA = "banana";
			so0_0->destroy();
			cmp0_1_A->destroy();

			so1_3 = SceneObject::create("so1_2");
			so1_3->setParent(so1);
			cmp1_3 = so1_3->addComponent<TestComponentC>();
			cmp1_3->obj.intA = 999;

			so1_0->setName("apple");
			so1_1->destroy();
			cmp1_2->destroy();

			cmp1_2 = so1_2->addComponent<TestComponentD>();
			cmp1_2->obj.strA = "orange";

			so2_1 = SceneObject::create("so2_1");
			so2_1->setParent(so2);

			so2_0->addComponent<TestComponentD>();

			so3 = SceneObject::create("so3");
			so3->setParent(root);
			cmp3 = so3->addComponent<TestComponentD>();
		}

		SPtr<PrefabDiff> prefabDiff = PrefabDiff::create(prefab->_getRoot(), root);

		prefab = gResources().load<Prefab>(prefabPath);
		HSceneObject newRoot = prefab->instantiate();
		prefabDiff->apply(newRoot);

		// Compare and assert
		ASSERT_EQ(root->getNumChildren(), newRoot->getNumChildren());
		HSceneObject nso0 = newRoot->getChild(0);

		GameObjectHandle<TestComponentC> ncmp0 = nso0->getComponent<TestComponentC>();
		ASSERT_EQ(cmp0->obj.strA, ncmp0->obj.strA);

		ASSERT_EQ(so0->getNumChildren(),  nso0->getNumChildren());
		HSceneObject nso0_1 = nso0->getChild(0);
		GameObjectHandle<TestComponentD> ncmp0_1 = nso0_1->getComponent<TestComponentD>();
		ASSERT_NE(ncmp0_1, nullptr);

		HSceneObject nso1 = newRoot->getChild(1);
		ASSERT_EQ(so1->getNumChildren(),  nso1->getNumChildren());

		HSceneObject nso1_0 = nso1->getChild(0);
		ASSERT_EQ(so1_0->getName(), nso1_0->getName());

		HSceneObject nso1_2 = nso1->getChild(1);
		GameObjectHandle<TestComponentD> ncmp1_2 = nso1_2->getComponent<TestComponentD>();
		ASSERT_EQ(cmp1_2->obj.strA, ncmp1_2->obj.strA);

		HSceneObject nso1_3 = nso1->getChild(2);
		GameObjectHandle<TestComponentC> ncmp1_3 = nso1_3->getComponent<TestComponentC>();
		ASSERT_EQ(cmp1_3->obj.intA,  ncmp1_3->obj.intA);

		HSceneObject nso2 = newRoot->getChild(2);
		ASSERT_EQ(so2->getNumChildren(),  nso2->getNumChildren());

		HSceneObject nso2_0 = nso2->getChild(0);
		GameObjectHandle<TestComponentD> ncmp2_0 = nso2_0->getComponent<TestComponentD>();
		ASSERT_NE(ncmp2_0, nullptr);

		HSceneObject nso3 = newRoot->getChild(3);
		GameObjectHandle<TestComponentD> ncmp3 = nso3->getComponent<TestComponentD>();
		ASSERT_NE(ncmp3, nullptr);

		root->destroy();
		newRoot->destroy();
	}

	TEST(EditorTestSuite, TestFrameAlloc)
	{
		FrameAlloc alloc(128);
		alloc.markFrame();
		UINT8* a1 = alloc.alloc(5);
		UINT8* a2 = alloc.alloc(10);
		UINT8* a3 = alloc.alloc(130);
		UINT8* a4 = alloc.alloc(5);

		alloc.free(a1);
		alloc.free(a2);
		alloc.free(a3);
		alloc.free(a4);

		alloc.clear();

		alloc.markFrame();
		UINT8* a5 = alloc.alloc(5);
		UINT8* a6 = alloc.alloc(10);
		UINT8* a7 = alloc.alloc(130);
		UINT8* a8 = alloc.alloc(5);

		alloc.free(a5);
		alloc.free(a6);
		alloc.free(a7);
		alloc.free(a8);

		alloc.markFrame();
		UINT8* a9 = alloc.alloc(5);
		UINT8* a10 = alloc.alloc(10);
		UINT8* a11 = alloc.alloc(130);
		UINT8* a12 = alloc.alloc(5);

		alloc.free(a9);
		alloc.free(a10);
		alloc.free(a11);
		alloc.free(a12);

		alloc.clear();
		alloc.clear();

		UINT8* a13 = alloc.alloc(5);
		alloc.free(a13);
		alloc.clear();
	}
}