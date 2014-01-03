using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public abstract class GUILayout : ScriptObject
    {
        public GUILabel AddLabel(GUIContent content, GUIElementStyle style, params GUIOption[] options)
        {
            return new GUILabel(this, content, style, options);
        }

        public GUILabel AddLabel(GUIContent content, params GUIOption[] options)
        {
            return new GUILabel(this, content, null, options);
        }

        public GUIButton AddButton(GUIContent content, GUIElementStyle style, params GUIOption[] options)
        {
            return new GUIButton(this, content, style, options);
        }

        public GUIButton AddButton(GUIContent content, GUIElementStyle style)
        {
            return new GUIButton(this, content, style, new GUIOption[0]);
        }

        public GUIButton AddButton(GUIContent content, params GUIOption[] options)
        {
            return new GUIButton(this, content, null, options);
        }

        public GUITexture AddTexture(SpriteTexture texture, GUIImageScaleMode scale, GUIElementStyle style, params GUIOption[] options)
        {
            return new GUITexture(this, texture, scale, style, options);
        }

        public GUITexture AddTexture(SpriteTexture texture, GUIImageScaleMode scale, params GUIOption[] options)
        {
            return new GUITexture(this, texture, scale, null, options);
        }

        public GUITexture AddTexture(SpriteTexture texture, GUIElementStyle style, params GUIOption[] options)
        {
            return new GUITexture(this, texture, GUIImageScaleMode.StretchToFit, style, options);
        }

        public GUITexture AddTexture(SpriteTexture texture, params GUIOption[] options)
        {
            return new GUITexture(this, texture, GUIImageScaleMode.StretchToFit, null, options);
        }

        public GUIFixedSpace AddSpace(int size)
        {
            return new GUIFixedSpace(this, size);
        }

        public GUIFlexibleSpace AddFlexibleSpace()
        {
            return new GUIFlexibleSpace(this);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_CreateInstanceXFromArea(GUILayout instance, GUIArea parentArea);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_CreateInstanceXFromLayout(GUILayout instance, GUILayout parentLayout);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_CreateInstanceYFromLayout(GUILayout instance, GUILayout parentLayout);
    }
}
