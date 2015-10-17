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
        private GUIDictionaryField<string, GUIElementStyle, GUIElementStyleEntry> valuesField;

        private Dictionary<string, GUIElementStyle> styles = new Dictionary<string, GUIElementStyle>();

        /// <inheritdoc/>
        protected internal override void Initialize()
        {
            BuildGUI();
        }

        /// <inheritdoc/>
        protected internal override InspectableState Refresh()
        {
            valuesField.Refresh();

            return InspectableState.NotModified;
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

            valuesField = GUIDictionaryField<string, GUIElementStyle, GUIElementStyleEntry>.Create
                (new LocEdString("Styles"), styles, Layout);

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

                    styles = x;
                }
                else
                {
                    foreach (var KVP in styles)
                        guiSkin.RemoveStyle(KVP.Key);

                    styles.Clear();
                }

                EditorApplication.SetDirty(guiSkin);
            };

            valuesField.OnValueChanged += x =>
            {
                guiSkin.SetStyle(x, styles[x]);
                EditorApplication.SetDirty(guiSkin);
            };

            valuesField.OnValueRemoved += x =>
            {
                guiSkin.RemoveStyle(x);
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
                keyField = new GUITextField(new LocEdString("Name"));
                titleLayout.AddElement(keyField);

                keyField.OnChanged += SetKey;

                return titleLayout;
            }

            /// <inheritdoc/>
            protected override void CreateValueGUI(GUILayoutY layout)
            {
                GUIElementStyle value = GetValue<GUIElementStyle>();

                if(valueField == null)
                    valueField = new GUIElementStyleGUI();

                valueField.BuildGUI(value, layout, Depth);
            }

            /// <inheritdoc/>
            protected override void OnEditModeChanged(bool editMode)
            {
                keyField.Disabled = !editMode;
            }

            /// <inheritdoc/>
            internal protected override InspectableState Refresh()
            {
                keyField.Value = GetKey<string>();
                return valueField.Refresh();
            }
        }

        /// <summary>
        /// Creates GUI elements for editing/displaying <see cref="GUIElementStyle"/>
        /// </summary>
        private class GUIElementStyleGUI
        {
            private const int IndentAmount = 5;

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
            private bool isExpanded;
            private InspectableState modifiedState;

            /// <summary>
            /// Creates a new GUI element style GUI.
            /// </summary>
            public GUIElementStyleGUI()
            {
                normalGUI = new GUIElementStateStyleGUI();
                hoverGUI = new GUIElementStateStyleGUI();
                activeGUI = new GUIElementStateStyleGUI();
                focusedGUI = new GUIElementStateStyleGUI();
                normalOnGUI = new GUIElementStateStyleGUI();
                hoverOnGUI = new GUIElementStateStyleGUI();
                activeOnGUI = new GUIElementStateStyleGUI();
                focusedOnGUI = new GUIElementStateStyleGUI();
            }

            /// <summary>
            /// Builds GUI for the specified GUI element style.
            /// </summary>
            /// <param name="style">Style to display in the GUI.</param>
            /// <param name="layout">Layout to append the GUI elements to.</param>
            /// <param name="depth">Determines the depth at which the element is rendered.</param>
            public void BuildGUI(GUIElementStyle style, GUILayout layout, int depth)
            {
                this.style = style;

                short backgroundDepth = (short)(Inspector.START_BACKGROUND_DEPTH - depth - 1);
                string bgPanelStyle = depth % 2 == 0
                    ? EditorStyles.InspectorContentBgAlternate
                    : EditorStyles.InspectorContentBg;

                GUIToggle foldout = new GUIToggle(new LocEdString("Style"), EditorStyles.Foldout);
                GUITexture inspectorContentBg = new GUITexture(null, bgPanelStyle);

                layout.AddElement(foldout);
                GUIPanel panel = layout.AddPanel();
                GUIPanel backgroundPanel = panel.AddPanel(backgroundDepth);
                
                backgroundPanel.AddElement(inspectorContentBg);

                GUILayoutX guiIndentLayoutX = panel.AddLayoutX();
                guiIndentLayoutX.AddSpace(IndentAmount);
                GUILayoutY guiIndentLayoutY = guiIndentLayoutX.AddLayoutY();
                guiIndentLayoutY.AddSpace(IndentAmount);
                GUILayoutY contentLayout = guiIndentLayoutY.AddLayoutY();
                guiIndentLayoutY.AddSpace(IndentAmount);
                guiIndentLayoutX.AddSpace(IndentAmount);

                fontField = new GUIResourceField(typeof (Font), new LocEdString("Font"));
                fontSizeField = new GUIIntField(new LocEdString("Font size"));
                horzAlignField = new GUIEnumField(typeof (TextHorzAlign), new LocEdString("Horizontal alignment"));
                vertAlignField = new GUIEnumField(typeof(TextVertAlign), new LocEdString("Vertical alignment"));
                imagePositionField = new GUIEnumField(typeof(GUIImagePosition), new LocEdString("Image position"));
                wordWrapField = new GUIToggleField(new LocEdString("Word wrap"));

                contentLayout.AddElement(fontField);
                contentLayout.AddElement(fontSizeField);
                contentLayout.AddElement(horzAlignField);
                contentLayout.AddElement(vertAlignField);
                contentLayout.AddElement(imagePositionField);
                contentLayout.AddElement(wordWrapField);

                normalGUI.BuildGUI(new LocEdString("Normal"), style.Normal, contentLayout);
                hoverGUI.BuildGUI(new LocEdString("Hover"), style.Hover, contentLayout);
                activeGUI.BuildGUI(new LocEdString("Active"), style.Active, contentLayout);
                focusedGUI.BuildGUI(new LocEdString("Focused"), style.Focused, contentLayout);
                normalOnGUI.BuildGUI(new LocEdString("NormalOn"), style.NormalOn, contentLayout);
                hoverOnGUI.BuildGUI(new LocEdString("HoverOn"), style.HoverOn, contentLayout);
                activeOnGUI.BuildGUI(new LocEdString("ActiveOn"), style.ActiveOn, contentLayout);
                focusedOnGUI.BuildGUI(new LocEdString("FocusedOn"), style.FocusedOn, contentLayout);

                borderGUI = new RectOffsetGUI(new LocEdString("Border"), style.Border, contentLayout);
                marginsGUI = new RectOffsetGUI(new LocEdString("Margins"), style.Margins, contentLayout);
                contentOffsetGUI = new RectOffsetGUI(new LocEdString("Content offset"), style.ContentOffset, contentLayout);

                fixedWidthField = new GUIToggleField(new LocEdString("Fixed width"));
                widthField = new GUIIntField(new LocEdString("Width"));
                minWidthField = new GUIIntField(new LocEdString("Min. width"));
                maxWidthField = new GUIIntField(new LocEdString("Max. width"));

                fixedHeightField = new GUIToggleField(new LocEdString("Fixed height"));
                heightField = new GUIIntField(new LocEdString("Height"));
                minHeightField = new GUIIntField(new LocEdString("Min. height"));
                maxHeightField = new GUIIntField(new LocEdString("Max. height"));

                contentLayout.AddElement(fixedWidthField);
                contentLayout.AddElement(widthField);
                contentLayout.AddElement(minWidthField);
                contentLayout.AddElement(maxWidthField);

                contentLayout.AddElement(fixedHeightField);
                contentLayout.AddElement(heightField);
                contentLayout.AddElement(minHeightField);
                contentLayout.AddElement(maxHeightField);

                foldout.OnToggled += x =>
                {
                    panel.Active = x;
                    isExpanded = x;
                };

                fontField.OnChanged += x => { style.Font = (Font) x; MarkAsModified(); ConfirmModify(); };
                fontSizeField.OnChanged += x => { style.FontSize = x; MarkAsModified(); };
                fontSizeField.OnFocusLost += ConfirmModify;
                fontSizeField.OnConfirmed += ConfirmModify;
                horzAlignField.OnSelectionChanged += x =>
                {
                    style.TextHorzAlign = (TextHorzAlign)x; 
                    MarkAsModified(); 
                    ConfirmModify();
                };
                vertAlignField.OnSelectionChanged += x =>
                {
                    style.TextVertAlign = (TextVertAlign)x; 
                    MarkAsModified(); 
                    ConfirmModify();
                };
                imagePositionField.OnSelectionChanged += x =>
                {
                    style.ImagePosition = (GUIImagePosition)x; 
                    MarkAsModified(); 
                    ConfirmModify();
                };
                wordWrapField.OnChanged += x => { style.WordWrap = x; MarkAsModified(); ConfirmModify(); };

                normalGUI.OnChanged += x => {style.Normal = x; MarkAsModified(); ConfirmModify(); };
                hoverGUI.OnChanged += x => {style.Hover = x; MarkAsModified(); ConfirmModify(); };
                activeGUI.OnChanged += x => {style.Active = x; MarkAsModified(); ConfirmModify(); };
                focusedGUI.OnChanged += x => {style.Focused = x; MarkAsModified(); ConfirmModify(); };
                normalOnGUI.OnChanged += x => {style.NormalOn = x; MarkAsModified(); ConfirmModify(); };
                hoverOnGUI.OnChanged += x => {style.HoverOn = x; MarkAsModified(); ConfirmModify(); };
                activeOnGUI.OnChanged += x => {style.ActiveOn = x; MarkAsModified(); ConfirmModify(); };
                focusedOnGUI.OnChanged += x => { style.FocusedOn = x; MarkAsModified(); ConfirmModify(); };

                borderGUI.OnChanged += x => { style.Border = x; MarkAsModified(); };
                marginsGUI.OnChanged += x => { style.Margins = x; MarkAsModified(); };
                contentOffsetGUI.OnChanged += x => { style.ContentOffset = x; MarkAsModified(); };

                borderGUI.OnConfirmed += ConfirmModify;
                marginsGUI.OnConfirmed += ConfirmModify;
                contentOffsetGUI.OnConfirmed += ConfirmModify;

                fixedWidthField.OnChanged += x => { style.FixedWidth = x; MarkAsModified(); ConfirmModify(); };
                widthField.OnChanged += x => style.Width = x;
                widthField.OnFocusLost += ConfirmModify;
                widthField.OnConfirmed += ConfirmModify;
                minWidthField.OnChanged += x => style.MinWidth = x;
                minWidthField.OnFocusLost += ConfirmModify;
                minWidthField.OnConfirmed += ConfirmModify;
                maxWidthField.OnChanged += x => style.MaxWidth = x;
                maxWidthField.OnFocusLost += ConfirmModify;
                maxWidthField.OnConfirmed += ConfirmModify;

                fixedHeightField.OnChanged += x => { style.FixedHeight = x; MarkAsModified(); ConfirmModify(); };
                heightField.OnChanged += x => style.Height = x;
                heightField.OnFocusLost += ConfirmModify;
                heightField.OnConfirmed += ConfirmModify;
                minHeightField.OnChanged += x => style.MinHeight = x;
                minHeightField.OnFocusLost += ConfirmModify;
                minHeightField.OnConfirmed += ConfirmModify;
                maxHeightField.OnChanged += x => style.MaxHeight = x;
                maxHeightField.OnFocusLost += ConfirmModify;
                maxHeightField.OnConfirmed += ConfirmModify;

                foldout.Value = isExpanded;
                panel.Active = isExpanded;
            }

            /// <summary>
            /// Updates all GUI elements from the style if style changes.
            /// </summary>
            /// <returns>State representing was anything modified between two last calls to <see cref="Refresh"/>.</returns>
            public InspectableState Refresh()
            {
                InspectableState oldModifiedState = modifiedState;
                if (modifiedState.HasFlag(InspectableState.Modified))
                    modifiedState = InspectableState.NotModified;

                if (style == null)
                    return oldModifiedState;

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

                widthField.Active = style.FixedWidth;
                minWidthField.Active = !style.FixedWidth;
                maxWidthField.Active = !style.FixedWidth;

                heightField.Active = style.FixedHeight;
                minHeightField.Active = !style.FixedHeight;
                maxHeightField.Active = !style.FixedHeight;

                return oldModifiedState;
            }

            /// <summary>
            /// Marks the contents of the style as modified.
            /// </summary>
            private void MarkAsModified()
            {
                modifiedState |= InspectableState.ModifyInProgress;
            }

            /// <summary>
            /// Confirms any queued modifications, signaling parent element.
            /// </summary>
            private void ConfirmModify()
            {
                if (modifiedState.HasFlag(InspectableState.ModifyInProgress))
                    modifiedState |= InspectableState.Modified;
            }

            /// <summary>
            /// Creates GUI elements for editing/displaying <see cref="GUIElementStateStyle"/>
            /// </summary>
            private class GUIElementStateStyleGUI
            {
                private GUIToggle foldout;
                private GUIResourceField textureField;
                private GUIColorField textColorField;
                private bool isExpanded;

                /// <summary>
                /// Triggered when some value in the style state changes.
                /// </summary>
                public Action<GUIElementStateStyle> OnChanged;

                /// <summary>
                /// Creates a new GUI element state style GUI.
                /// </summary>
                public GUIElementStateStyleGUI()
                { }

                /// <summary>
                /// Builds the GUI for the specified state style.
                /// </summary>
                /// <param name="title">Text to display on the title bar.</param>
                /// <param name="state">State object to display in the GUI.</param>
                /// <param name="layout">Layout to append the GUI elements to.</param>
                public void BuildGUI(LocString title, GUIElementStateStyle state, GUILayout layout)
                {
                    foldout = new GUIToggle(title, EditorStyles.Foldout);
                    textureField = new GUIResourceField(typeof(SpriteTexture), new LocEdString("Texture"));
                    textColorField = new GUIColorField(new LocEdString("Text color"));

                    foldout.OnToggled += x =>
                    {
                        textureField.Active = x;
                        textColorField.Active = x;
                        isExpanded = x;
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

                    foldout.Value = isExpanded;
                    textureField.Active = isExpanded;
                    textColorField.Active = isExpanded;
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
            private class RectOffsetGUI
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
                /// Triggered when the user confirms input.
                /// </summary>
                public Action OnConfirmed;

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

                    offsetLeftField = new GUIIntField(new LocEdString("Left"), 40);
                    offsetRightField = new GUIIntField(new LocEdString("Right"), 40);
                    offsetTopField = new GUIIntField(new LocEdString("Top"), 40);
                    offsetBottomField = new GUIIntField(new LocEdString("Bottom"), 40);

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

                    Action DoOnConfirmed = () =>
                    {
                        if (OnConfirmed != null)
                            OnConfirmed();
                    };

                    offsetLeftField.OnConfirmed += DoOnConfirmed;
                    offsetLeftField.OnFocusLost += DoOnConfirmed;
                    offsetRightField.OnConfirmed += DoOnConfirmed;
                    offsetRightField.OnFocusLost += DoOnConfirmed;
                    offsetTopField.OnConfirmed += DoOnConfirmed;
                    offsetTopField.OnFocusLost += DoOnConfirmed;
                    offsetBottomField.OnConfirmed += DoOnConfirmed;
                    offsetBottomField.OnFocusLost += DoOnConfirmed;
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
