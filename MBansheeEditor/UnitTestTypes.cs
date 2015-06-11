using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    public class UT1_Component1 : Component
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

        public UT1_Component2 otherComponent;
        public SceneObject otherSO;
    }

    public class UT1_Component2 : Component
    {
        public int a2;
    }

    [SerializeObject]
    public class UT1_SerzCls
    {
        public int someValue2;
        public string anotherValue2;

        public UT1_SerzCls child;
    }

    [SerializeObject]
    public struct UT1_SerzObj
    {
        public UT1_SerzObj(int someValue, string anotherValue)
        {
            this.someValue = someValue;
            this.anotherValue = anotherValue;
        }

        public int someValue;
        public string anotherValue;
    }

    [SerializeObject]
    public class UT_DiffChildObj
    {
        public int plain1 = 101;
        public string plain2 = "oneoone";
    }

    [SerializeObject]
    public class UT_DiffObj
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
}
