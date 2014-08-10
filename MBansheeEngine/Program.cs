using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.CompilerServices;
using System.Diagnostics;

[assembly: InternalsVisibleTo("MBansheeEditor")]

namespace BansheeEngine
{
    class Program
    {
        // TODO: Make this an actual unit test
        static void UnitTest1_ManagedSerialization()
        {
            SceneObject otherSO = new SceneObject("OtherSO");
            DbgComponent2 dbgComponent2 = otherSO.AddComponent<DbgComponent2>();
            dbgComponent2.a2 = 33;

            GUIElementStateStyle dbgStyle = new GUIElementStateStyle();
            SceneObject so = new SceneObject("TestSO");
            DbgComponent dbgComponent = so.AddComponent<DbgComponent>();

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
            dbgComponent.arrComplex = new DbgSerzObj[5];
            dbgComponent.arrComplex[4].someValue = 99;
            dbgComponent.arrComplex[4].anotherValue = "ArrComplexAnotherValue";
            dbgComponent.arrComplex2 = new DbgSerzCls[5];
            dbgComponent.arrComplex2[4] = new DbgSerzCls();
            dbgComponent.arrComplex2[4].someValue2 = 101;
            dbgComponent.arrComplex2[4].anotherValue2 = "ArrComplex2AnotherValue";

            dbgComponent.listA = new List<int>();
            dbgComponent.listA.Add(5);
            dbgComponent.listB = new List<string>();
            dbgComponent.listB.Add("ListAnotherValue");
            dbgComponent.listB.Add(null);
            dbgComponent.listComplex = new List<DbgSerzObj>();
            dbgComponent.listComplex.Add(new DbgSerzObj());
            dbgComponent.listComplex.Add(new DbgSerzObj(99, "ListComplexAnotherValue"));
            dbgComponent.listComplex2 = new List<DbgSerzCls>();
            dbgComponent.listComplex2.Add(new DbgSerzCls());
            dbgComponent.listComplex2[0].someValue2 = 101;
            dbgComponent.listComplex2[0].anotherValue2 = "ListComplexAnotherValue";
            dbgComponent.listComplex2.Add(null);

            dbgComponent.otherComponent = dbgComponent2;
            dbgComponent.otherSO = otherSO;

            //dbgComponent.zeArray = new int[5][][];
            //dbgComponent.zeList = new List<DbgSerzObj>();
            //dbgComponent.zeDict = new Dictionary<string, int>();

            //dbgComponent.zeList.Add(new DbgSerzObj());
            //dbgComponent.zeList.Add(new DbgSerzObj());
            //dbgComponent.zeList.Add(new DbgSerzObj(101, ""));
            //dbgComponent.zeList.Add(new DbgSerzObj());

            //dbgComponent.zeDict["supSup"] = 10001;
            //dbgComponent.zeDict["lolz"] = 696969;

            //var enumerator = dbgComponent.zeDict.GetEnumerator();
            //int all = 0;
            //while (enumerator.MoveNext())
            //{
            //    all += enumerator.Current.Value;
            //}

            //for (int i = 0; i < dbgComponent.zeArray.Length; i++)
            //{
            //    dbgComponent.zeArray[i] = new int[6][];

            //    for (int j = 0; j < dbgComponent.zeArray[i].Length; j++)
            //        dbgComponent.zeArray[i][j] = new int[7];
            //}

            //dbgComponent.zeArray[4][1][3] = 129;

            UnitTest1_GameObjectClone(so);

            System.Diagnostics.Debug.Assert(so.GetNumChildren() == 1);

            for (int i = 0; i < so.GetNumChildren(); i++)
            {
                SceneObject childSO = so.GetChild(i);

                DbgComponent otherComponent = childSO.GetComponent<DbgComponent>();
                System.Diagnostics.Debug.Assert(otherComponent.a == 5);
                System.Diagnostics.Debug.Assert(otherComponent.b == "SomeTestVal");
                System.Diagnostics.Debug.Assert(otherComponent.complex.someValue == 19);
                System.Diagnostics.Debug.Assert(otherComponent.complex2.anotherValue2 == "AnotherValue2");

                System.Diagnostics.Debug.Assert(otherComponent.arrA[4] == 5);
                System.Diagnostics.Debug.Assert(otherComponent.arrB[4] == "ArrAnotherValue");
                System.Diagnostics.Debug.Assert(otherComponent.arrComplex[4].someValue == 99);
                System.Diagnostics.Debug.Assert(otherComponent.arrComplex2[4].anotherValue2 == "ArrComplex2AnotherValue");

                System.Diagnostics.Debug.Assert(otherComponent.listA[0] == 5);
                System.Diagnostics.Debug.Assert(otherComponent.listB[0] == "ListAnotherValue");
                System.Diagnostics.Debug.Assert(otherComponent.listComplex[1].someValue == 99);
                System.Diagnostics.Debug.Assert(otherComponent.listComplex2[0].anotherValue2 == "ListComplexAnotherValue");
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void UnitTest1_GameObjectClone(SceneObject so);

        static void Main()
        {
            UnitTest1_ManagedSerialization();

            SerializableObject obj = new SerializableObject(typeof(DbgSerzCls), new DbgSerzCls());

            Debug.Log(obj.fields.Length);
            for (int i = 0; i < obj.fields.Length; i++)
            {
                Debug.Log(i + ". " + obj.fields[i].Name + " - " + obj.fields[i].Type.ToString());
            }

            SerializableProperty prop = obj.fields[0].GetProperty();
            Debug.Log("Old value: " + prop.GetValue<int>());
            prop.SetValue<int>(33);
            Debug.Log("New value: " + prop.GetValue<int>());

            SerializableProperty prop2 = obj.fields[2].GetProperty();
            Debug.Log("Old value: " + (prop2.GetValue<DbgSerzCls>() == null));

            DbgSerzCls child = new DbgSerzCls();
            child.anotherValue2 = "ass";
            prop2.SetValue<DbgSerzCls>(child);

            if (prop2.GetValue<DbgSerzCls>() == null)
                Debug.Log("New value: null");
            else
                Debug.Log("New value: " + prop2.GetValue<DbgSerzCls>().anotherValue2);
        }

    }
}
