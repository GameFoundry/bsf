using System;
using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Renders an inspector for the <see cref="GUISkin"/> resource.
    /// </summary>
    [CustomInspector(typeof(GUISkin))]
    public class GUISkinInspector : Inspector
    {
        private GUIDictionaryField<string, GUIElementStyle> valuesField = new GUIDictionaryField<string, GUIElementStyle>();

        private Dictionary<string, GUIElementStyle> styles = new Dictionary<string, GUIElementStyle>();

        /// <inheritdoc/>
        protected internal override void Initialize()
        {
            BuildGUI();
        }

        /// <inheritdoc/>
        protected internal override void Refresh()
        {
            valuesField.Refresh(); ;
        }

        /// <summary>
        /// Recreates all the GUI elements used by this inspector.
        /// </summary>
        private void BuildGUI()
        {
            Layout.Clear();
            styles.Clear();

            GUISkin guiSkin = InspectedObject as GUISkin;
            if (guiSkin == null)
                return;

            string[] styleNames = guiSkin.StyleNames;
            foreach (var styleName in styleNames)
                styles[styleName] = guiSkin.GetStyle(styleName);

            valuesField.Update<GUIElementStyleEntry>(new LocEdString("Styles"), styles, Layout);

            valuesField.OnChanged += x =>
            {
                if (x != null)
                {
                    foreach (var KVP in x)
                    {
                        if (guiSkin.HasStyle(KVP.Key))
                        {
                            GUIElementStyle oldValue = guiSkin.GetStyle(KVP.Key);
                            if (oldValue != KVP.Value)
                                guiSkin.SetStyle(KVP.Key, KVP.Value);
                        }
                        else
                            guiSkin.SetStyle(KVP.Key, KVP.Value);
                    }

                    string[] oldStyleNames = guiSkin.StyleNames;
                    foreach (var styleName in oldStyleNames)
                    {
                        if (!x.ContainsKey(styleName))
                            guiSkin.RemoveStyle(styleName);
                    }
                }
                else
                {
                    foreach (var KVP in styles)
                        guiSkin.RemoveStyle(KVP.Key);
                }

                EditorApplication.SetDirty(guiSkin);

                BuildGUI();
                Refresh();
            };

            valuesField.OnValueChanged += x =>
            {
                guiSkin.SetStyle(x, styles[x]);
                EditorApplication.SetDirty(guiSkin);
            };

            Layout.AddSpace(10);
        }

        /// <summary>
        /// Row element used for displaying GUI for GUI element style dictionary elements.
        /// </summary>
        public class GUIElementStyleEntry : GUIDictionaryFieldRow
        {
            private GUITextField keyField;
            private GUIElementStyleGUI valueField;

            /// <inheritdoc/>
            protected override GUILayoutX CreateKeyGUI(GUILayoutY layout)
            {
                GUILayoutX titleLayout = layout.AddLayoutX();
                keyField = new GUITextField(new LocEdString("Style name"));
                titleLayout.AddElement(keyField);

                keyField.OnChanged += SetKey;

                return titleLayout;
            }

            /// <inheritdoc/>
            protected override void CreateValueGUI(GUILayoutY layout)
            {
                GUIElementStyle value = GetValue<GUIElementStyle>();
                valueField = new GUIElementStyleGUI(value, layout);
            }

            /// <inheritdoc/>
            internal protected override bool Refresh()
            {
                keyField.Value = GetKey<string>();
                valueField.Refresh();

                return false;
            }
        }

        /// <summary>
        /// Creates GUI elements for editing/displaying <see cref="GUIElementStyle"/>
        /// </summary>
        private class GUIElementStyleGUI
        {
            private GUIResourceField fontField;
            private GUIIntField fontSizeField;
            private GUIEnumField horzAlignField;
            private GUIEnumField vertAlignField;
            private GUIEnumField imagePositionField;
            private GUIToggleField wordWrapField;

            private GUIElementStateStyleGUI normalGUI;
            private GUIElementStateStyleGUI hoverGUI;
            private GUIElementStateStyleGUI activeGUI;
            private GUIElementStateStyleGUI focusedGUI;
            private GUIElementStateStyleGUI normalOnGUI;
            private GUIElementStateStyleGUI hoverOnGUI;
            private GUIElementStateStyleGUI activeOnGUI;
            private GUIElementStateStyleGUI focusedOnGUI;

            private RectOffsetGUI borderGUI;
            private RectOffsetGUI marginsGUI;
            private RectOffsetGUI contentOffsetGUI;

            private GUIToggleField fixedWidthField;
            private GUIIntField widthField;
            private GUIIntField minWidthField;
            private GUIIntField maxWidthField;

            private GUIToggleField fixedHeightField;
            private GUIIntField heightField;
            private GUIIntField minHeightField;
            private GUIIntField maxHeightField;

            private GUIElementStyle style;

            /// <summary>
            /// Creates a new GUI element style GUI.
            /// </summary>
            /// <param name="style">Style to display in the GUI.</param>
            /// <param name="layout">Layout to append the GUI elements to.</param>
            public GUIElementStyleGUI(GUIElementStyle style, GUILayout layout)
            {
                this.style = style;

                if (style == null)
                    return;

                fontField = new GUIResourceField(typeof (Font), new LocEdString("Font"));
                fontSizeField = new GUIIntField(new LocEdString("Font size"));
                horzAlignField = new GUIEnumField(typeof (TextHorzAlign), new LocEdString("Horizontal alignment"));
                vertAlignField = new GUIEnumField(typeof(TextVertAlign), new LocEdString("Vertical alignment"));
                imagePositionField = new GUIEnumField(typeof(GUIImagePosition), new LocEdString("Image position"));
                wordWrapField = new GUIToggleField(new LocEdString("Word wrap"));

                layout.AddElement(fontField);
                layout.AddElement(fontSizeField);
                layout.AddElement(horzAlignField);
                layout.AddElement(vertAlignField);
                layout.AddElement(imagePositionField);
                layout.AddElement(wordWrapField);

                normalGUI = new GUIElementStateStyleGUI(new LocEdString("Normal"), style.Normal, layout);
                hoverGUI = new GUIElementStateStyleGUI(new LocEdString("Hover"), style.Hover, layout);
                activeGUI = new GUIElementStateStyleGUI(new LocEdString("Active"), style.Active, layout);
                focusedGUI = new GUIElementStateStyleGUI(new LocEdString("Focused"), style.Focused, layout);
                normalOnGUI = new GUIElementStateStyleGUI(new LocEdString("NormalOn"), style.NormalOn, layout);
                hoverOnGUI = new GUIElementStateStyleGUI(new LocEdString("HoverOn"), style.HoverOn, layout);
                activeOnGUI = new GUIElementStateStyleGUI(new LocEdString("ActiveOn"), style.ActiveOn, layout);
                focusedOnGUI = new GUIElementStateStyleGUI(new LocEdString("FocusedOn"), style.FocusedOn, layout);

                borderGUI = new RectOffsetGUI(new LocEdString("Border"), style.Border, layout);
                marginsGUI = new RectOffsetGUI(new LocEdString("Margins"), style.Margins, layout);
                contentOffsetGUI = new RectOffsetGUI(new LocEdString("Content offset"), style.ContentOffset, layout);

                fixedWidthField = new GUIToggleField(new LocEdString("Fixed width"));
                widthField = new GUIIntField(new LocEdString("Width"));
                minWidthField = new GUIIntField(new LocEdString("Min. width"));
                maxWidthField = new GUIIntField(new LocEdString("Max. width"));

                fixedHeightField = new GUIToggleField(new LocEdString("Fixed height"));
                heightField = new GUIIntField(new LocEdString("Height"));
                minHeightField = new GUIIntField(new LocEdString("Min. height"));
                maxHeightField = new GUIIntField(new LocEdString("Max. height"));

                layout.AddElement(fixedWidthField);
                layout.AddElement(widthField);
                layout.AddElement(minWidthField);
                layout.AddElement(maxWidthField);

                layout.AddElement(fixedHeightField);
                layout.AddElement(heightField);
                layout.AddElement(minHeightField);
                layout.AddElement(maxHeightField);

                fontField.OnChanged += x => style.Font = (Font)x;
                fontSizeField.OnChanged += x => style.FontSize = x;
                horzAlignField.OnSelectionChanged += x => style.TextHorzAlign = (TextHorzAlign)x;
                vertAlignField.OnSelectionChanged += x => style.TextVertAlign = (TextVertAlign)x;
                imagePositionField.OnSelectionChanged += x => style.ImagePosition = (GUIImagePosition)x;
                wordWrapField.OnChanged += x => style.WordWrap = x;

                normalGUI.OnChanged += x => style.Normal = x;
                hoverGUI.OnChanged += x => style.Hover = x;
                activeGUI.OnChanged += x => style.Active = x;
                focusedGUI.OnChanged += x => style.Focused = x;
                normalOnGUI.OnChanged += x => style.NormalOn = x;
                hoverOnGUI.OnChanged += x => style.HoverOn = x;
                activeOnGUI.OnChanged += x => style.ActiveOn = x;
                focusedOnGUI.OnChanged += x => style.FocusedOn = x;

                borderGUI.OnChanged += x => style.Border = x;
                marginsGUI.OnChanged += x => style.Margins = x;
                contentOffsetGUI.OnChanged += x => style.ContentOffset = x;

                fixedWidthField.OnChanged += x =>
                {
                    widthField.Active = x;
                    minWidthField.Active = !x;
                    maxWidthField.Active = !x;
                };

                widthField.OnChanged += x => style.Width = x;
                minWidthField.OnChanged += x => style.MinWidth = x;
                maxWidthField.OnChanged += x => style.MaxWidth = x;

                fixedHeightField.OnChanged += x =>
                {
                    heightField.Active = x;
                    minHeightField.Active = !x;
                    maxHeightField.Active = !x;
                };

                heightField.OnChanged += x => style.Height = x;
                minHeightField.OnChanged += x => style.MinHeight = x;
                maxHeightField.OnChanged += x => style.MaxHeight = x;
            }

            /// <summary>
            /// Updates all GUI elements from the style if style changes.
            /// </summary>
            public void Refresh()
            {
                if (style == null)
                    return;

                fontField.Value = style.Font;
                fontSizeField.Value = style.FontSize;
                horzAlignField.Value = (ulong)style.TextHorzAlign;
                vertAlignField.Value = (ulong)style.TextVertAlign;
                imagePositionField.Value = (ulong)style.ImagePosition;
                wordWrapField.Value = style.WordWrap;

                normalGUI.Refresh(style.Normal);
                hoverGUI.Refresh(style.Hover);
                activeGUI.Refresh(style.Active);
                focusedGUI.Refresh(style.Focused);
                normalOnGUI.Refresh(style.NormalOn);
                hoverOnGUI.Refresh(style.HoverOn);
                activeOnGUI.Refresh(style.ActiveOn);
                focusedOnGUI.Refresh(style.FocusedOn);

                borderGUI.Refresh(style.Border);
                marginsGUI.Refresh(style.Margins);
                contentOffsetGUI.Refresh(style.ContentOffset);

                fixedWidthField.Value = style.FixedWidth;
                widthField.Value = style.Width;
                minWidthField.Value = style.MinWidth;
                maxWidthField.Value = style.MaxWidth;
                fixedHeightField.Value = style.FixedHeight;
                heightField.Value = style.Height;
                minHeightField.Value = style.MinHeight;
                maxHeightField.Value = style.MaxHeight;
            }

            /// <summary>
            /// Creates GUI elements for editing/displaying <see cref="GUIElementStateStyle"/>
            /// </summary>
            public class GUIElementStateStyleGUI
            {
                private GUIToggleField foldout;
                private GUIResourceField textureField;
                private GUIColorField textColorField;

                /// <summary>
                /// Triggered when some value in the style state changes.
                /// </summary>
                public Action<GUIElementStateStyle> OnChanged;

                /// <summary>
                /// Creates a new GUI element state style GUI.
                /// </summary>
                /// <param name="title">Text to display on the title bar.</param>
                /// <param name="state">State object to display in the GUI.</param>
                /// <param name="layout">Layout to append the GUI elements to.</param>
                public GUIElementStateStyleGUI(LocString title, GUIElementStateStyle state, GUILayout layout)
                {
                    foldout = new GUIToggleField(title, 100, EditorStyles.Foldout);
                    textureField = new GUIResourceField(typeof(SpriteTexture), new LocEdString("Texture"));
                    textColorField = new GUIColorField(new LocEdString("Text color"));

                    foldout.OnChanged += x =>
                    {
                        textureField.Active = x;
                        textColorField.Active = x;
                    };

                    textureField.OnChanged += x =>
                    {
                        state.Texture = x as SpriteTexture;

                        if (OnChanged != null)
                            OnChanged(state);
                    };

                    textColorField.OnChanged += x =>
                    {
                        state.TextColor = x;

                        if (OnChanged != null)
                            OnChanged(state);
                    };

                    layout.AddElement(foldout);
                    layout.AddElement(textureField);
                    layout.AddElement(textColorField);

                    foldout.Value = false;
                    textureField.Active = false;
                    textColorField.Active = false;
                }

                /// <summary>
                /// Updates all GUI elements from the current state values.
                /// </summary>
                /// <param name="state">State to update the GUI to.</param>
                public void Refresh(GUIElementStateStyle state)
                {
                    textureField.Value = state.Texture;
                    textColorField.Value = state.TextColor;
                }
            }

            /// <summary>
            /// Creates GUI elements for editing/displaying <see cref="RectOffset"/>
            /// </summary>
            public class RectOffsetGUI
            {
                private GUIIntField offsetLeftField;
                private GUIIntField offsetRightField;
                private GUIIntField offsetTopField;
                private GUIIntField offsetBottomField;

                /// <summary>
                /// Triggered when some value in the offset rectangle changes.
                /// </summary>
                public Action<RectOffset> OnChanged;

                /// <summary>
                /// Creates a new rectangle offset GUI.
                /// </summary>
                /// <param name="title">Text to display on the title bar.</param>
                /// <param name="offset">Rectangle offset object to display in the GUI.</param>
                /// <param name="layout">Layout to append the GUI elements to.</param>
                public RectOffsetGUI(LocString title, RectOffset offset, GUILayout layout)
                {
                    GUILayoutX rectLayout = layout.AddLayoutX();
                    rectLayout.AddElement(new GUILabel(title, GUIOption.FixedWidth(100)));
                    GUILayoutY rectContentLayout = rectLayout.AddLayoutY();

                    GUILayoutX rectTopRow = rectContentLayout.AddLayoutX();
                    GUILayoutX rectBotRow = rectContentLayout.AddLayoutX();

                    offsetLeftField = new GUIIntField(new LocEdString("Left"));
                    offsetRightField = new GUIIntField(new LocEdString("Right"));
                    offsetTopField = new GUIIntField(new LocEdString("Top"));
                    offsetBottomField = new GUIIntField(new LocEdString("Bottom"));

                    rectTopRow.AddElement(offsetLeftField);
                    rectTopRow.AddElement(offsetRightField);
                    rectBotRow.AddElement(offsetTopField);
                    rectBotRow.AddElement(offsetBottomField);

                    offsetLeftField.OnChanged += x =>
                    {
                        offset.left = x;

                        if(OnChanged != null)
                            OnChanged(offset);
                    };

                    offsetRightField.OnChanged += x =>
                    {
                        offset.right = x;

                        if (OnChanged != null)
                            OnChanged(offset);
                    };

                    offsetTopField.OnChanged += x =>
                    {
                        offset.top = x;

                        if (OnChanged != null)
                            OnChanged(offset);
                    };

                    offsetBottomField.OnChanged += x =>
                    {
                        offset.bottom = x;

                        if (OnChanged != null)
                            OnChanged(offset);
                    };
                }

                /// <summary>
                /// Updates all GUI elements from the offset.
                /// </summary>
                /// <param name="offset">Offset to update the GUI to.</param>
                public void Refresh(RectOffset offset)
                {
                    offsetLeftField.Value = offset.left;
                    offsetRightField.Value = offset.right;
                    offsetTopField.Value = offset.top;
                    offsetBottomField.Value = offset.bottom;
                }
            }
        }
    }
}
