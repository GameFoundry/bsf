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
        }

        /// <summary>
        /// Tests serializable properties used for inspection.
        /// </summary>
        static void UnitTest2_SerializableProperties()
        {
            SerializableObject obj = new SerializableObject(typeof(UT1_SerzCls), new UT1_SerzCls());

            Debug.Log(obj.Fields.Length);
            for (int i = 0; i < obj.Fields.Length; i++)
            {
                Debug.Log(i + ". " + obj.Fields[i].Name + " - " + obj.Fields[i].Type.ToString());
            }

            SerializableProperty prop = obj.Fields[0].GetProperty();
            Debug.Log("Old value: " + prop.GetValue<int>());
            prop.SetValue<int>(33);
            Debug.Log("New value: " + prop.GetValue<int>());

            SerializableProperty prop2 = obj.Fields[2].GetProperty();
            Debug.Log("Old value: " + (prop2.GetValue<UT1_SerzCls>() == null));

            UT1_SerzCls child = new UT1_SerzCls();
            child.anotherValue2 = "potato";
            prop2.SetValue<UT1_SerzCls>(child);

            if (prop2.GetValue<UT1_SerzCls>() == null)
                Debug.Log("New value: null");
            else
                Debug.Log("New value: " + prop2.GetValue<UT1_SerzCls>().anotherValue2);
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
        /// Runs all tests.
        /// </summary>
        static void RunTests()
        {
            UnitTest1_ManagedSerialization();
            UnitTest2_SerializableProperties();
            UnitTest3_ManagedDiff();
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_UT1_GameObjectClone(SceneObject so);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_UT3_GenerateDiff(UT_DiffObj oldObj, UT_DiffObj newObj);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_UT3_ApplyDiff(UT_DiffObj obj);
    }
}
