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
            dbgComponent.complex.someValue = 19;
            dbgComponent.complex.anotherValue = "AnotherValue";
            dbgComponent.otherComponent = dbgComponent2;
            dbgComponent.otherSO = otherSO;
            dbgComponent.zeArray = new int[5];
            dbgComponent.zeArray[4] = 129;

            dbgTestComponentClone(so);

            for (int i = 0; i < so.GetNumChildren(); i++)
            {
                SceneObject childSO = so.GetChild(i);
                reportDbgValue(childSO.GetComponent<DbgComponent>().otherComponent.a2, childSO.GetComponent<DbgComponent>().zeArray[4], typeof(DbgComponent));
            }

            //Color newColor = Color.red;

            //dbgStyle.textColor = newColor;
            //Color myColor = dbgStyle.textColor;
            //dbgStyle.textColor = myColor;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void dbgTestComponentClone(SceneObject so);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void reportDbgValue(int dbgVal, int dbgVal2, Type type);
    }
}
