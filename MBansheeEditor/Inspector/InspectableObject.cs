using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Displays GUI for a serializable property containing a generic object. Inspectable object fields are displayed
    /// in separate rows.
    /// </summary>
    public class InspectableObject : InspectableField
    {
        private const int IndentAmount = 5;

        private object propertyValue;
        private List<InspectableField> children = new List<InspectableField>();

        private GUILayoutX guiChildLayout;
        private GUILayoutX guiTitleLayout;
        private bool isExpanded;
        private bool forceUpdate = true;

        /// <summary>
        /// Creates a new inspectable array GUI for the specified property.
        /// </summary>
        /// <param name="title">Name of the property, or some other value to set as the title.</param>
        /// <param name="depth">Determines how deep within the inspector nesting hierarchy is this field.Some fields may
        ///                     contain other fields, in which case you should increase this value by one.</param>
        /// <param name="layout">Parent layout that all the field elements will be added to.</param>
        /// <param name="property">Serializable property referencing the array whose contents to display.</param>
        public InspectableObject(string title, int depth, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, depth, layout, property)
        {
            
        }

        /// <inheritdoc/>
        public override GUILayoutX GetTitleLayout()
        {
            return guiTitleLayout;
        }

        /// <inheritdoc/>
        public override bool IsModified()
        {
            if (forceUpdate)
                return true;
                
            object newPropertyValue = property.GetValue<object>();
            if (propertyValue == null)
                return newPropertyValue != null;

            if (newPropertyValue == null)
                return propertyValue != null;

            return base.IsModified();
        }

        /// <inheritdoc/>
        public override bool Refresh(int layoutIndex)
        {
            bool anythingModified = base.Refresh(layoutIndex);

            int currentIndex = 0;
            for (int i = 0; i < children.Count; i++)
            {
                anythingModified |= children[i].Refresh(currentIndex);
                currentIndex += children[i].GetNumLayoutElements();
            }

            return anythingModified;
        }

        /// <inheritdoc/>
        public override bool GetRebuildOnModify()
        {
            return true;
        }

        /// <inheritdoc/>
        protected internal override void BuildGUI(int index)
        {
            guiTitleLayout = null;
            guiChildLayout = null;

            layout.DestroyElements();

            if (property.Type != SerializableProperty.FieldType.Object)
                return;

            if (propertyValue == null)
            {
                guiChildLayout = null;
                guiTitleLayout = layout.AddLayoutX(index);

                guiTitleLayout.AddElement(new GUILabel(title));
                guiTitleLayout.AddElement(new GUILabel("Empty", GUIOption.FixedWidth(100)));

                if (!property.IsValueType)
                {
                    GUIContent createIcon = new GUIContent(EditorBuiltin.GetInspectorWindowIcon(InspectorWindowIcon.Create));
                    GUIButton createBtn = new GUIButton(createIcon, GUIOption.FixedWidth(30));
                    createBtn.OnClick += OnCreateButtonClicked;
                    guiTitleLayout.AddElement(createBtn);
                }
            }
            else
            {
                guiTitleLayout = layout.AddLayoutX(index);

                GUIToggle guiFoldout = new GUIToggle(title, EditorStyles.Foldout);
                guiFoldout.Value = isExpanded;
                guiFoldout.OnToggled += OnFoldoutToggled;
                guiTitleLayout.AddElement(guiFoldout);

                GUIContent clearIcon = new GUIContent(EditorBuiltin.GetInspectorWindowIcon(InspectorWindowIcon.Clear));
                GUIButton clearBtn = new GUIButton(clearIcon, GUIOption.FixedWidth(20));
                clearBtn.OnClick += OnClearButtonClicked;
                guiTitleLayout.AddElement(clearBtn);

                if (isExpanded)
                {
                    SerializableObject serializableObject = property.GetObject();
                    SerializableField[] fields = serializableObject.Fields;

                    if (fields.Length > 0)
                    {
                        guiChildLayout = layout.AddLayoutX(index);
                        guiChildLayout.AddSpace(IndentAmount);

                        GUIPanel guiContentPanel = guiChildLayout.AddPanel();
                        GUILayoutX guiIndentLayoutX = guiContentPanel.AddLayoutX();
                        guiIndentLayoutX.AddSpace(IndentAmount);
                        GUILayoutY guiIndentLayoutY = guiIndentLayoutX.AddLayoutY();
                        guiIndentLayoutY.AddSpace(IndentAmount);
                        GUILayoutY guiContentLayout = guiIndentLayoutY.AddLayoutY();
                        guiIndentLayoutY.AddSpace(IndentAmount);
                        guiIndentLayoutX.AddSpace(IndentAmount);
                        guiChildLayout.AddSpace(IndentAmount);

                        short backgroundDepth = (short)(Inspector.START_BACKGROUND_DEPTH - depth - 1);
                        string bgPanelStyle = depth % 2 == 0 ? EditorStyles.InspectorContentBgAlternate : EditorStyles.InspectorContentBg;
                        GUIPanel backgroundPanel = guiContentPanel.AddPanel(backgroundDepth);
                        GUITexture inspectorContentBg = new GUITexture(null, bgPanelStyle);
                        backgroundPanel.AddElement(inspectorContentBg);

                        int currentIndex = 0;
                        foreach (var field in fields)
                        {
                            if (!field.Inspectable)
                                continue;

                            InspectableField inspectable = CreateInspectable(field.Name, currentIndex, depth + 1,
                                new InspectableFieldLayout(guiContentLayout), field.GetProperty());

                            children.Add(inspectable);
                            currentIndex += inspectable.GetNumLayoutElements();
                        }
                    }
                }
                else
                    guiChildLayout = null;
            }
        }

        /// <inheritdoc/>
        protected internal override void Update(int layoutIndex)
        {
            foreach (var child in children)
                child.Destroy();

            children.Clear();

            propertyValue = property.GetValue<object>();
            BuildGUI(layoutIndex);
            forceUpdate = false;
        }

        /// <summary>
        /// Triggered when the user clicks on the expand/collapse toggle in the title bar.
        /// </summary>
        /// <param name="expanded">Determines whether the contents were expanded or collapsed.</param>
        private void OnFoldoutToggled(bool expanded)
        {
            isExpanded = expanded;
            forceUpdate = true;
        }

        /// <summary>
        /// Triggered when the user clicks on the create button on the title bar. Creates a brand new object with default
        /// values in the place of the current array.
        /// </summary>
        private void OnCreateButtonClicked()
        {
            property.SetValue(property.CreateObjectInstance<object>());
        }

        /// <summary>
        /// Triggered when the user clicks on the clear button on the title bar. Deletes the current object and sets
        /// the reference to the object in the parent object to null. This is only relevant for objects of reference types.
        /// </summary>
        private void OnClearButtonClicked()
        {
            property.SetValue<object>(null);
        }
    }
}
