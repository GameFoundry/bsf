using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public abstract class GUILayout : GUIElement
    {
        internal GUILayout(GUIElement parent)
            :base(parent)
        { }

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

        public GUIToggle AddToggle(GUIContent content, GUIElementStyle style, params GUIOption[] options)
        {
            return new GUIToggle(this, content, null, style, options);
        }

        public GUIToggle AddToggle(GUIContent content, GUIElementStyle style)
        {
            return new GUIToggle(this, content, null, style, new GUIOption[0]);
        }

        public GUIToggle AddToggle(GUIContent content, params GUIOption[] options)
        {
            return new GUIToggle(this, content, null, null, options);
        }

        public GUIToggle AddToggle(GUIContent content, GUIToggleGroup toggleGroup, GUIElementStyle style, params GUIOption[] options)
        {
            return new GUIToggle(this, content, toggleGroup, style, options);
        }

        public GUIToggle AddToggle(GUIContent content, GUIToggleGroup toggleGroup, GUIElementStyle style)
        {
            return new GUIToggle(this, content, toggleGroup, style, new GUIOption[0]);
        }

        public GUIToggle AddToggle(GUIContent content, GUIToggleGroup toggleGroup, params GUIOption[] options)
        {
            return new GUIToggle(this, content, toggleGroup, null, options);
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

        public GUITextBox AddTextBox(bool multiline, GUIElementStyle style, params GUIOption[] options)
        {
            return new GUITextBox(this, multiline, style, options);
        }

        public GUITextBox AddTextBox(bool multiline, params GUIOption[] options)
        {
            return new GUITextBox(this, multiline, null, options);
        }

        public GUITextBox AddTextBox(GUIElementStyle style, params GUIOption[] options)
        {
            return new GUITextBox(this, false, style, options);
        }

        public GUITextBox AddTextBox(params GUIOption[] options)
        {
            return new GUITextBox(this, false, null, options);
        }

        public GUIScrollArea AddScrollArea(ScrollBarType vertBarType, ScrollBarType horzBarType, GUIElementStyle style, params GUIOption[] options)
        {
            return new GUIScrollArea(this, vertBarType, horzBarType, null, style, options);
        }

        public GUIScrollArea AddScrollArea(ScrollBarType vertBarType, ScrollBarType horzBarType, params GUIOption[] options)
        {
            return new GUIScrollArea(this, vertBarType, horzBarType, null, null, options);
        }

        public GUIScrollArea AddScrollArea(GUIElementStyle style, params GUIOption[] options)
        {
            return new GUIScrollArea(this, ScrollBarType.ShowIfDoesntFit, ScrollBarType.ShowIfDoesntFit, null, style, options);
        }

        public GUIScrollArea AddScrollArea(params GUIOption[] options)
        {
            return new GUIScrollArea(this, ScrollBarType.ShowIfDoesntFit, ScrollBarType.ShowIfDoesntFit, null, null, options);
        }

        public GUIScrollArea AddScrollArea(ScrollBarType vertBarType, ScrollBarType horzBarType, GUIElementStyle scrollBarStyle, GUIElementStyle scrollAreaStyle, params GUIOption[] options)
        {
            return new GUIScrollArea(this, vertBarType, horzBarType, scrollBarStyle, scrollAreaStyle, options);
        }

        public GUIScrollArea AddScrollArea(GUIElementStyle scrollBarStyle, GUIElementStyle scrollAreaStyle, params GUIOption[] options)
        {
            return new GUIScrollArea(this, ScrollBarType.ShowIfDoesntFit, ScrollBarType.ShowIfDoesntFit, scrollBarStyle, scrollAreaStyle, options);
        }

        public GUIListBox AddListBox(LocString[] elements, GUIElementStyle style, params GUIOption[] options)
        {
            return new GUIListBox(this, elements, style, options);
        }

        public GUIListBox AddListBox(LocString[] elements, params GUIOption[] options)
        {
            return new GUIListBox(this, elements, null, options);
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
        protected static extern void Internal_CreateInstanceYFromScrollArea(GUILayout instance, GUIScrollArea parentArea);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_CreateInstanceYFromLayout(GUILayout instance, GUILayout parentLayout);
    }
}
