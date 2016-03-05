//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using BansheeEngine;
using System.Collections.Generic;

namespace BansheeEditor
{
    /// <summary>
    /// Contains editor data that should persist assembly refresh.
    /// </summary>
    internal class EditorPersistentData : Component
    {
        [SerializeField]
        internal Dictionary<string, bool> dirtyResources = new Dictionary<string, bool>();
    }
}
