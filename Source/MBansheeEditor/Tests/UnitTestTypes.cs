//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    #pragma warning disable 0649
    /** @addtogroup Tests
     *  @{
     */

    /// <summary>
    /// Helper component used for unit tests.
    /// </summary>
    internal class UT1_Component1 : ManagedComponent
    {
        public int a;
        public string b;
        public UT1_SerzObj complex = new UT1_SerzObj();
        public UT1_SerzCls complex2 = new UT1_SerzCls();

        public int[] arrA;
        public string[] arrB;
        public UT1_SerzObj[] arrComplex;
        public UT1_SerzCls[] arrComplex2;

        public List<int> listA;
        public List<string> listB;
        public List<UT1_SerzObj> listComplex;
        public List<UT1_SerzCls> listComplex2;

        public Dictionary<int, string> dictA;
        public Dictionary<string, UT1_SerzObj> dictB;

        public UT1_Component2 otherComponent;
        public UT1_Component1 otherComponent2;
        public SceneObject otherSO;
    }

    /// <summary>
    /// Helper component used for unit tests.
    /// </summary>
    internal class UT1_Component2 : ManagedComponent
    {
        public int a2;
    }

    /// <summary>
    /// Helper type used for unit tests.
    /// </summary>
    [SerializeObject]
    internal class UT1_SerzCls
    {
        public int someValue2;
        public string anotherValue2;

#pragma warning disable 649
        public UT1_SerzCls child;
#pragma warning restore 649
    }

    /// <summary>
    /// Helper type used for unit tests.
    /// </summary>
    [SerializeObject]
    internal struct UT1_SerzObj
    {
        public UT1_SerzObj(int someValue, string anotherValue)
        {
            this.someValue = someValue;
            this.anotherValue = anotherValue;
        }

        public int someValue;
        public string anotherValue;
    }

    /// <summary>
    /// Helper type used for unit tests.
    /// </summary>
    [SerializeObject]
    internal class UT_DiffChildObj
    {
        public int plain1 = 101;
        public string plain2 = "oneoone";
    }

    /// <summary>
    /// Helper type used for unit tests.
    /// </summary>
    [SerializeObject]
    internal class UT_DiffObj
    {
        public int plain1 = 5;
        public string plain2 = "six";
        public UT_DiffChildObj complex = null;
        public UT_DiffChildObj complex2 = new UT_DiffChildObj();
        public UT_DiffChildObj complex3 = new UT_DiffChildObj();

        public int[] arrPlain1 = { 10, 11, 12 };
        public string[] arrPlain2 = { "one", "two", "three" };
        public UT_DiffChildObj[] arrComplex = null;
        public UT_DiffChildObj[] arrComplex2 = new UT_DiffChildObj[2];

        public List<int> listPlain1 = new List<int>();
        public List<string> listPlain2 = new List<string>();
        public List<UT_DiffChildObj> listComplex = null;
        public List<UT_DiffChildObj> listComplex2 = new List<UT_DiffChildObj>();

        public Dictionary<int, int> dictPlain1 = new Dictionary<int, int>();
        public Dictionary<string, string> dictPlain2 = new Dictionary<string, string>();
        public Dictionary<int, UT_DiffChildObj> dictComplex = null;
        public Dictionary<int, UT_DiffChildObj> dictComplex2 = new Dictionary<int, UT_DiffChildObj>();

        public UT_DiffObj()
        {
            arrComplex2[0] = new UT_DiffChildObj();
            arrComplex2[1] = null;

            listPlain1.AddRange(new int[] { 4, 5, 6 });
            listPlain2.AddRange(new string[] {"four", "five", "six"});

            listComplex2.Add(new UT_DiffChildObj());
            listComplex2.Add(new UT_DiffChildObj());

            dictPlain1[20] = 20;
            dictPlain1[21] = 21;
            dictPlain1[22] = 22;

            dictPlain2["twenty"] = "twenty";
            dictPlain2["twentyone"] = "twentyone";
            dictPlain2["twentytwo"] = "twentytwo";

            dictComplex2[30] = new UT_DiffChildObj();
            dictComplex2[31] = new UT_DiffChildObj();
            dictComplex2[32] = new UT_DiffChildObj();
        }
    }

    /** @} */
    #pragma warning restore 0649
}
