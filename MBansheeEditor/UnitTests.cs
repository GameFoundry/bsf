using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;
using DebugUnit = System.Diagnostics.Debug;

namespace BansheeEditor
{
    /// <summary>
    /// Contains various managed unit tests.
    /// </summary>
    class UnitTests
    {
        /// <summary>
        /// Tests managed object serialization and deserialization.
        /// </summary>
        static void UnitTest1_ManagedSerialization()
        {
            SceneObject otherSO = new SceneObject("OtherSO");
            UT1_Component2 dbgComponent2 = otherSO.AddComponent<UT1_Component2>();
            dbgComponent2.a2 = 33;

            SceneObject so = new SceneObject("TestSO");
            UT1_Component1 dbgComponent = so.AddComponent<UT1_Component1>();

            dbgComponent.a = 5;
            dbgComponent.b = "SomeTestVal";
            dbgComponent.complex.someValue = 19;
            dbgComponent.complex.anotherValue = "AnotherValue";
            dbgComponent.complex2.someValue2 = 21;
            dbgComponent.complex2.anotherValue2 = "AnotherValue2";

            dbgComponent.arrA = new int[5];
            dbgComponent.arrA[4] = 5;
            dbgComponent.arrB = new string[5];
            dbgComponent.arrB[4] = "ArrAnotherValue";
            dbgComponent.arrComplex = new UT1_SerzObj[5];
            dbgComponent.arrComplex[4].someValue = 99;
            dbgComponent.arrComplex[4].anotherValue = "ArrComplexAnotherValue";
            dbgComponent.arrComplex2 = new UT1_SerzCls[5];
            dbgComponent.arrComplex2[4] = new UT1_SerzCls();
            dbgComponent.arrComplex2[4].someValue2 = 101;
            dbgComponent.arrComplex2[4].anotherValue2 = "ArrComplex2AnotherValue";

            dbgComponent.listA = new List<int>();
            dbgComponent.listA.Add(5);
            dbgComponent.listB = new List<string>();
            dbgComponent.listB.Add("ListAnotherValue");
            dbgComponent.listB.Add(null);
            dbgComponent.listComplex = new List<UT1_SerzObj>();
            dbgComponent.listComplex.Add(new UT1_SerzObj());
            dbgComponent.listComplex.Add(new UT1_SerzObj(99, "ListComplexAnotherValue"));
            dbgComponent.listComplex2 = new List<UT1_SerzCls>();
            dbgComponent.listComplex2.Add(new UT1_SerzCls());
            dbgComponent.listComplex2[0].someValue2 = 101;
            dbgComponent.listComplex2[0].anotherValue2 = "ListComplexAnotherValue";
            dbgComponent.listComplex2.Add(null);

            dbgComponent.dictA = new Dictionary<int, string>();
            dbgComponent.dictA[5] = "value";
            dbgComponent.dictA[10] = "anotherValue";

            dbgComponent.dictB = new Dictionary<string, UT1_SerzObj>();
            dbgComponent.dictB["key1"] = new UT1_SerzObj(99, "DictComplexValue");

            dbgComponent.otherComponent = dbgComponent2;
            dbgComponent.otherSO = otherSO;

            Internal_UT1_GameObjectClone(so);

            System.Diagnostics.Debug.Assert(so.GetNumChildren() == 1);

            for (int i = 0; i < so.GetNumChildren(); i++)
            {
                SceneObject childSO = so.GetChild(i);

                UT1_Component1 otherComponent = childSO.GetComponent<UT1_Component1>();
                DebugUnit.Assert(otherComponent.a == 5);
                DebugUnit.Assert(otherComponent.b == "SomeTestVal");
                DebugUnit.Assert(otherComponent.complex.someValue == 19);
                DebugUnit.Assert(otherComponent.complex2.anotherValue2 == "AnotherValue2");

                DebugUnit.Assert(otherComponent.arrA[4] == 5);
                DebugUnit.Assert(otherComponent.arrB[4] == "ArrAnotherValue");
                DebugUnit.Assert(otherComponent.arrComplex[4].someValue == 99);
                DebugUnit.Assert(otherComponent.arrComplex2[4].anotherValue2 == "ArrComplex2AnotherValue");

                DebugUnit.Assert(otherComponent.listA[0] == 5);
                DebugUnit.Assert(otherComponent.listB[0] == "ListAnotherValue");
                DebugUnit.Assert(otherComponent.listComplex[1].someValue == 99);
                DebugUnit.Assert(otherComponent.listComplex2[0].anotherValue2 == "ListComplexAnotherValue");
            }

            so.Destroy();
            otherSO.Destroy();
        }

        /// <summary>
        /// Tests serializable properties used for inspection.
        /// </summary>
        static void UnitTest2_SerializableProperties()
        {
            SerializableObject obj = new SerializableObject(typeof(UT1_SerzCls), new UT1_SerzCls());

            SerializableProperty prop = obj.Fields[0].GetProperty();
            prop.SetValue(33);
            DebugUnit.Assert(prop.GetValue<int>() == 33);

            SerializableProperty prop2 = obj.Fields[2].GetProperty();

            UT1_SerzCls child = new UT1_SerzCls();
            child.anotherValue2 = "potato";
            prop2.SetValue<UT1_SerzCls>(child);

            DebugUnit.Assert(prop2.GetValue<UT1_SerzCls>() != null);
            DebugUnit.Assert(prop2.GetValue<UT1_SerzCls>().anotherValue2 == "potato");
        }

        /// <summary>
        /// Tests managed diff creation used by prefabs.
        /// </summary>
        static void UnitTest3_ManagedDiff()
        {
            UT_DiffObj original = new UT_DiffObj();
            UT_DiffObj modified = new UT_DiffObj();

            modified.plain2 = "banana";
            modified.complex = new UT_DiffChildObj();
            modified.complex2 = null;
            modified.complex3.plain2 = "tomato";

            modified.arrPlain1 = new[] {-1, -2, -3, -4};
            modified.arrPlain2[2] = "cherry";
            modified.arrComplex = new UT_DiffChildObj[3];
            modified.arrComplex2[0].plain1 = -10;

            modified.listPlain1[0] = -20;
            modified.listPlain2 = new List<string>();
            modified.listComplex = new List<UT_DiffChildObj>();
            modified.listComplex.Add(new UT_DiffChildObj());
            modified.listComplex2[1].plain2 = "orange";

            modified.dictPlain1.Remove(20);
            modified.dictPlain1[-30] = -30;
            modified.dictComplex = new Dictionary<int, UT_DiffChildObj>();
            modified.dictComplex[-40] = new UT_DiffChildObj();
            modified.dictComplex2[31].plain1 = -50;

            Internal_UT3_GenerateDiff(original, modified);
            Internal_UT3_ApplyDiff(original);

            DebugUnit.Assert(original.plain1 == modified.plain1);
            DebugUnit.Assert(original.plain2 == modified.plain2);
            DebugUnit.Assert(original.complex.plain2 == modified.complex.plain2);
            DebugUnit.Assert(original.complex2 == modified.complex2);
            DebugUnit.Assert(original.complex3.plain2 == modified.complex3.plain2);

            DebugUnit.Assert(original.arrPlain1.Length == modified.arrPlain1.Length);
            for (int i = 0; i < original.arrPlain1.Length; i++)
                DebugUnit.Assert(original.arrPlain1[i] == modified.arrPlain1[i]);

            for (int i = 0; i < original.arrPlain2.Length; i++)
                DebugUnit.Assert(original.arrPlain2[i] == modified.arrPlain2[i]);

            for (int i = 0; i < original.arrComplex.Length; i++)
                DebugUnit.Assert(original.arrComplex[i] == modified.arrComplex[i]);

            DebugUnit.Assert(original.arrComplex2[0].plain1 == modified.arrComplex2[0].plain1);

            for (int i = 0; i < original.listPlain1.Count; i++)
                DebugUnit.Assert(original.listPlain1[i] == modified.listPlain1[i]);

            DebugUnit.Assert(original.listPlain2.Count == modified.listPlain2.Count);

            for (int i = 0; i < original.listComplex.Count; i++)
                DebugUnit.Assert(original.listComplex[i].plain1 == modified.listComplex[i].plain1);

            DebugUnit.Assert(original.listComplex2[1].plain2 == modified.listComplex2[1].plain2);

            foreach (var entry in modified.dictPlain1)
            {
                if (!original.dictPlain1.ContainsKey(entry.Key))
                    DebugUnit.Assert(false);

                DebugUnit.Assert(entry.Value == original.dictPlain1[entry.Key]);
            }

            foreach (var entry in modified.dictPlain2)
            {
                if (!original.dictPlain2.ContainsKey(entry.Key))
                    DebugUnit.Assert(false);

                DebugUnit.Assert(entry.Value == original.dictPlain2[entry.Key]);
            }

            foreach (var entry in modified.dictComplex)
            {
                if (!original.dictComplex.ContainsKey(entry.Key))
                    DebugUnit.Assert(false);

                DebugUnit.Assert(entry.Value.plain1 == original.dictComplex[entry.Key].plain1);
            }

            foreach (var entry in modified.dictComplex2)
            {
                if (!original.dictComplex2.ContainsKey(entry.Key))
                    DebugUnit.Assert(false);

                DebugUnit.Assert(entry.Value.plain1 == original.dictComplex2[entry.Key].plain1);
            }
        }

        /// <summary>
        /// Tests saving, loading and updating of prefabs.
        /// </summary>
        private static void UnitTest4_Prefabs()
        {
            if (EditorApplication.IsProjectLoaded)
            {
                Debug.LogWarning("Skipping unit test as no project is loaded.");
                return;
            }

            if (EditorApplication.IsSceneModified())
            {
                Debug.LogWarning("Cannot perform unit test as the current scene is modified.");
                return;
            }

            string oldScene = Scene.ActiveSceneUUID;
            Scene.Clear();

            // Simple scene save & load
            {
                {
                    SceneObject so0 = new SceneObject("so0");
                    SceneObject so1 = new SceneObject("so1");
                    SceneObject so0_0 = new SceneObject("so0_0");
                    SceneObject so0_1 = new SceneObject("so0_1");
                    SceneObject so1_0 = new SceneObject("so1_0");
                    SceneObject so0_1_0 = new SceneObject("so0_1_0");

                    so0_0.Parent = so0;
                    so0_1.Parent = so0;
                    so1_0.Parent = so1;
                    so0_1_0.Parent = so0_1;

                    so0_1_0.Position = new Vector3(10.0f, 15.0f, 20.0f);
                    so0_1.Position = new Vector3(1.0f, 2.0f, 3.0f);
                    so1_0.Position = new Vector3(0, 123.0f, 0.0f);

                    UT1_Component1 comp0 = so0.AddComponent<UT1_Component1>();
                    UT1_Component2 comp1 = so1.AddComponent<UT1_Component2>();
                    UT1_Component1 comp0_1_0 = so0_1_0.AddComponent<UT1_Component1>();

                    comp0.otherSO = so0_1_0;
                    comp0.otherComponent = comp1;

                    comp0_1_0.b = "testValue";
                    comp0_1_0.otherSO = so0;
                    comp0_1_0.otherComponent2 = comp0;

                    EditorApplication.SaveScene("unitTest4Scene_0");
                }
                {
                    EditorApplication.LoadScene("unitTest4Scene_0");

                    SceneObject sceneRoot = Scene.Root;
                    SceneObject so0 = sceneRoot.FindChild("so0", false);
                    SceneObject so1 = sceneRoot.FindChild("so1", false);
                    SceneObject so0_0 = so0.FindChild("so0_0", false);
                    SceneObject so0_1 = so0.FindChild("so0_1", false);
                    SceneObject so1_0 = so1.FindChild("so1_0", false);
                    SceneObject so0_1_0 = so1_0.FindChild("so0_1_0", false);

                    DebugUnit.Assert(so0_0 != null);
                    DebugUnit.Assert(so0_1 != null);
                    DebugUnit.Assert(so0_1_0 != null);

                    UT1_Component1 comp0 = so0.GetComponent<UT1_Component1>();
                    UT1_Component2 comp1 = so1.GetComponent<UT1_Component2>();
                    UT1_Component1 comp0_1_0 = so0_1_0.GetComponent<UT1_Component1>();

                    DebugUnit.Assert(comp0 != null);
                    DebugUnit.Assert(comp1 != null);
                    DebugUnit.Assert(comp0_1_0 != null);
                    DebugUnit.Assert(comp0_1_0.b == "testValue");
                    DebugUnit.Assert(comp0.otherSO == so0_1_0);
                    DebugUnit.Assert(comp0.otherComponent == comp1);
                    DebugUnit.Assert(comp0_1_0.otherSO == so0);
                    DebugUnit.Assert(comp0_1_0.otherComponent2 == comp0);
                }
            }

            // Load & save a scene that contains a prefab and references its objects
            {
                {
                    Scene.Clear();

                    SceneObject parentSO0 = new SceneObject("parentSO0", false);
                    SceneObject parentSO1 = new SceneObject("parentSO1", false);
                    SceneObject parentSO1_0 = new SceneObject("parentSO1_0", false);

                    parentSO1_0.Parent = parentSO1;

                    UT1_Component1 parentComp1_0 = parentSO1_0.AddComponent<UT1_Component1>();

                    Prefab scene0Prefab = ProjectLibrary.Load<Prefab>("unitTest4Scene_0");
                    SceneObject prefabInstance = scene0Prefab.Instantiate();
                    prefabInstance.Parent = parentSO0;

                    SceneObject so1 = prefabInstance.FindChild("so1", false);
                    SceneObject so1_0 = so1.FindChild("so1_0", false);
                    SceneObject so0_1_0 = so1_0.FindChild("so0_1_0", false);

                    UT1_Component1 comp0_1_0 = so0_1_0.GetComponent<UT1_Component1>();

                    parentComp1_0.otherSO = so1_0;
                    parentComp1_0.otherComponent2 = comp0_1_0;

                    EditorApplication.SaveScene("unitTest4Scene_1");
                }
                {
                    EditorApplication.LoadScene("unitTest4Scene_1");

                    SceneObject parentSO0 = Scene.Root.FindChild("parentSO0", false);
                    SceneObject parentSO1_0 = parentSO0.FindChild("parentSO1_0", false);

                    UT1_Component1 parentComp1_0 = parentSO1_0.GetComponent<UT1_Component1>();

                    SceneObject prefabInstance = parentSO0.GetChild(0);
                    SceneObject so1 = prefabInstance.FindChild("so1");
                    SceneObject so1_0 = so1.FindChild("so1_0");
                    SceneObject so0_1_0 = so1_0.FindChild("so0_1_0");

                    UT1_Component1 comp0_1_0 = so0_1_0.GetComponent<UT1_Component1>();

                    DebugUnit.Assert(parentComp1_0.otherSO == so1_0);
                    DebugUnit.Assert(parentComp1_0.otherComponent2 == comp0_1_0);
                }
            }

            // Modify prefab, reload the scene and ensure it is updated with modified prefab
            {
                {
                    Scene.Load("unitTest4Scene_0");

                    SceneObject sceneRoot = Scene.Root;
                    SceneObject so0 = sceneRoot.FindChild("so0", false);
                    SceneObject so0_0 = so0.FindChild("so0_0", false);
                    SceneObject so1 = sceneRoot.FindChild("so1", false);
                    SceneObject so1_0 = so1.FindChild("so1_0", false);
                    SceneObject so0_1_0 = so1_0.FindChild("so0_1_0", false);

                    SceneObject so1_1 = new SceneObject("so1_1");
                    so1_1.Parent = so1_0;

                    so0.RemoveComponent<UT1_Component1>();
                    UT1_Component1 comp1 = so1.AddComponent<UT1_Component1>();
                    UT1_Component1 comp0_1_0 = so0_1_0.GetComponent<UT1_Component1>();

                    so0_0.Destroy();

                    comp1.otherSO = so1_0;
                    comp1.otherComponent2 = comp0_1_0;

                    comp0_1_0.otherSO = so1_1;
                    comp0_1_0.otherComponent2 = comp1;
                    comp0_1_0.a = 123;
                    comp0_1_0.b = "modifiedValue";

                    so1.Name = "so1_modified";
                    so1.Position = new Vector3(0, 999.0f, 0.0f);

                    EditorApplication.SaveScene("unitTest4Scene_0");
                }

                {
                    EditorApplication.LoadScene("unitTest4Scene_1");

                    SceneObject parentSO0 = Scene.Root.FindChild("parentSO0", false);
                    SceneObject parentSO1_0 = parentSO0.FindChild("parentSO1_0", false);

                    UT1_Component1 parentComp1_0 = parentSO1_0.GetComponent<UT1_Component1>();

                    SceneObject prefabInstance = parentSO0.GetChild(0);
                    SceneObject so0 = prefabInstance.FindChild("so0", false);
                    SceneObject so1 = prefabInstance.FindChild("so1_modified", false);
                    SceneObject so0_0 = so0.FindChild("so0_0", false);
                    SceneObject so1_0 = so1.FindChild("so1_0", false);
                    SceneObject so0_1_0 = so1_0.FindChild("so0_1_0", false);
                    SceneObject so1_1 = so1_0.FindChild("so1_1", false);

                    UT1_Component1 comp0 = so0.GetComponent<UT1_Component1>();
                    UT1_Component1 comp1 = so1.GetComponent<UT1_Component1>();
                    UT1_Component1 comp0_1_0 = so0_1_0.GetComponent<UT1_Component1>();

                    DebugUnit.Assert(parentComp1_0.otherSO == so1_0);
                    DebugUnit.Assert(parentComp1_0.otherComponent2 == comp0_1_0);
                    DebugUnit.Assert(so1_1 != null);
                    DebugUnit.Assert(so0_0 == null);
                    DebugUnit.Assert(comp0 == null);
                    DebugUnit.Assert(comp0_1_0.otherSO == so1_1);
                    DebugUnit.Assert(comp0_1_0.otherComponent2 == comp1);
                    DebugUnit.Assert(comp1.otherSO == so1_0);
                    DebugUnit.Assert(comp1.otherComponent2 == comp0_1_0);
                    DebugUnit.Assert(MathEx.ApproxEquals(so1.Position.y, 999.0f));
                }
            }

            // Make instance specific changes to the prefab, modify the prefab itself and ensure
            // both changes persist
            {
                // Create new scene referencing the prefab and make instance modifications
                {
                    Scene.Clear();

                    SceneObject parent2SO0 = new SceneObject("parent2SO0");
                    SceneObject parent2SO1 = new SceneObject("parent2SO1");
                    SceneObject parent2SO1_0 = new SceneObject("parent2SO1_0");

                    parent2SO1_0.Parent = parent2SO1;

                    UT1_Component1 parentComp1_0 = parent2SO1_0.AddComponent<UT1_Component1>();

                    Prefab scene0Prefab = ProjectLibrary.Load<Prefab>("unitTest4Scene_0");
                    SceneObject prefabInstance = scene0Prefab.Instantiate();
                    prefabInstance.Parent = parent2SO0;

                    SceneObject so0 = prefabInstance.FindChild("so0", false);
                    SceneObject so1 = prefabInstance.FindChild("so1", false);
                    
                    SceneObject so1_0 = so1.FindChild("so1_0", false);
                    SceneObject so1_1 = so1_0.FindChild("so1_1", false);
                    SceneObject so0_1_0 = so1_0.FindChild("so0_1_0", false);

                    UT1_Component2 comp1 = so1.GetComponent<UT1_Component2>();
                    UT1_Component1 comp0_1_0 = so0_1_0.GetComponent<UT1_Component1>();

                    SceneObject so0_0 = new SceneObject("so0_0");
                    so0_0.Parent = so0;
                    UT1_Component1 comp0 = so0.AddComponent<UT1_Component1>();

                    so1.RemoveComponent<UT1_Component1>();
                    so1_1.Destroy();

                    comp0.otherSO = so0_1_0;
                    comp0.otherComponent = comp1;

                    parentComp1_0.otherSO = so1_0;
                    parentComp1_0.otherComponent2 = comp0_1_0;

                    comp0_1_0.otherSO = parent2SO1_0;
                    comp0_1_0.otherComponent2 = parentComp1_0;
                    comp0_1_0.b = "instanceValue";

                    EditorApplication.SaveScene("unitTest4Scene_2");
                }

                // Reload the scene and ensure instance modifications remain
                {
                    EditorApplication.LoadScene("unitTest4Scene_2");

                    SceneObject root = Scene.Root;
                    SceneObject parent2SO0 = root.FindChild("parent2SO0", false);
                    SceneObject parent2SO1 = root.FindChild("parent2SO1", false);
                    SceneObject parent2SO1_0 = root.FindChild("parent2SO1_0", false);

                    SceneObject prefabInstance = parent2SO0.GetChild(0);

                    SceneObject so0 = prefabInstance.FindChild("so0", false);
                    SceneObject so1 = prefabInstance.FindChild("so1", false);
                    SceneObject so0_0 = so0.FindChild("so0_0", false);
                    SceneObject so1_0 = so1.FindChild("so1_0", false);
                    SceneObject so1_1 = so1_0.FindChild("so1_1", false);
                    SceneObject so0_1_0 = so1_0.FindChild("so0_1_0", false);

                    UT1_Component1 parentComp1_0 = parent2SO1_0.GetComponent<UT1_Component1>();
                    UT1_Component1 comp0 = so0.GetComponent<UT1_Component1>();
                    UT1_Component2 comp1 = so1.GetComponent<UT1_Component2>();
                    UT1_Component1 comp11 = so1.GetComponent<UT1_Component1>();
                    UT1_Component1 comp0_1_0 = so0_1_0.GetComponent<UT1_Component1>();

                    DebugUnit.Assert(so0_0 != null);
                    DebugUnit.Assert(comp0 != null);
                    DebugUnit.Assert(so1_1 == null);
                    DebugUnit.Assert(comp11 == null);

                    DebugUnit.Assert(comp0.otherSO == so0_1_0);
                    DebugUnit.Assert(comp0.otherComponent == comp1);

                    DebugUnit.Assert(parentComp1_0.otherSO == so1_0);
                    DebugUnit.Assert(parentComp1_0.otherComponent2 == comp0_1_0);

                    DebugUnit.Assert(comp0_1_0.otherSO == parent2SO1_0);
                    DebugUnit.Assert(comp0_1_0.otherComponent2 == parentComp1_0);
                    DebugUnit.Assert(comp0_1_0.b == "instanceValue");
                }

                // Load original scene and ensure instance modifications didn't influence it
                {
                    // TODO
                }

                // Modify prefab and ensure both prefab and instance modifications remain
                {
                    // TODO
                }
            }

            if (!string.IsNullOrEmpty(oldScene))
                Scene.Load(ProjectLibrary.GetPath(oldScene));
            else
                Scene.Clear();

            ProjectLibrary.Delete("unitTest4Scene_0");
        }

        /// <summary>
        /// Runs all tests.
        /// </summary>
        static void RunTests()
        {
            UnitTest1_ManagedSerialization();
            UnitTest2_SerializableProperties();
            UnitTest3_ManagedDiff();
            UnitTest4_Prefabs();
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_UT1_GameObjectClone(SceneObject so);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_UT3_GenerateDiff(UT_DiffObj oldObj, UT_DiffObj newObj);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_UT3_ApplyDiff(UT_DiffObj obj);
    }
}
