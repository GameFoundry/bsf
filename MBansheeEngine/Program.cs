using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

[assembly: InternalsVisibleTo("MBansheeEditor")]

namespace BansheeEngine
{
    class Program
    {
        static void Main()
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

            dbgTestComponentClone(so);

            for (int i = 0; i < so.GetNumChildren(); i++)
            {
                SceneObject childSO = so.GetChild(i);

                DbgComponent otherComponent = childSO.GetComponent<DbgComponent>();
                reportDbgValue(otherComponent.a, otherComponent.b, otherComponent.complex.someValue,
                               otherComponent.complex2.anotherValue2);

                //reportDbgValue(childSO.GetComponent<DbgComponent>().zeDict["lolz"], childSO.GetComponent<DbgComponent>().zeList[2].someValue, childSO.GetComponent<DbgComponent>().zeArray[4][1][3], typeof(DbgComponent));
            }

            //Color newColor = Color.red;

            //dbgStyle.textColor = newColor;
            //Color myColor = dbgStyle.textColor;
            //dbgStyle.textColor = myColor;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void dbgTestComponentClone(SceneObject so);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void reportDbgValue(int a, string b, int a2, string b2);
    }
}
