//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Base class for implementation of commands that can be recorded in the UndoRedo system.
    /// </summary>
    public abstract class UndoableCommand : ScriptObject
    {
        public UndoableCommand()
        {
            Internal_CreateInstance(this);
        }

        /// <summary>
        /// Applies the command, committing the change.
        /// </summary>
        protected abstract void Commit();

        /// <summary>
        /// Reverts the command, reverting the change previously done with <see cref="Commit"/>.
        /// </summary>
        protected abstract void Revert();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(UndoableCommand instance);
    }
}
