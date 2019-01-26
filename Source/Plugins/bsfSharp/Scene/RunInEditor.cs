//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;

namespace BansheeEngine
{
    /** @addtogroup Scene
     *  @{
     */

    /// <summary>
    /// Attribute that allows Component's code to be executed while game is not running in the editor. Normally the
    /// initialize/update/destroy methods will not be running while the game is paused or not running, but this overrides
    /// that behaviour.
    /// </summary>
    [AttributeUsage(AttributeTargets.Class)]
    public sealed class RunInEditor : Attribute
    {
    }

    /** @} */
}