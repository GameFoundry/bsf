using System;
using System.Runtime.InteropServices;
using BansheeEngine;

namespace BansheeEditor
{
    public class ModalWindow : ScriptObject
    {
        //protected EditorGUIWidget GUI;

        public ModalWindow(int top, int left, int width, int height)
        {
            Internal_CreateInstance(this, top, left, width, height);

            //SceneObject modalWindowSO = new SceneObject("ModalWindow");
            //modalWindowSO.hideFlags = HideFlags.HideInInspector | HideFlags.DontSave;

            //modalWindowSO->addComponent<EditorGUIWidget>(this);
        }

        ~ModalWindow()
        {
            Internal_DestroyInstance(mCachedPtr);
        }

        [DllImport("__Internal")]
        private static extern void Internal_CreateInstance(ModalWindow instance, int top, int left, int width, int height);

        [DllImport("__Internal")]
        private static extern void Internal_DestroyInstance(IntPtr nativeInstance);
    }
}
