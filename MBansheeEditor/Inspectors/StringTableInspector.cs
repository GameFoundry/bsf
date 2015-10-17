using System;
using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Renders an inspector for the <see cref="StringTable"/> resource.
    /// </summary>
    [CustomInspector(typeof(StringTable))]
    internal class StringTableInspector : Inspector
    {
        private GUIEnumField languageField;
        private GUIDictionaryField<string, string, StringTableEntry> valuesField;

        private Dictionary<string, string> strings = new Dictionary<string,string>();

        /// <inheritdoc/>
        protected internal override void Initialize()
        {
            BuildGUI();
        }

        /// <inheritdoc/>
        protected internal override InspectableState Refresh()
        {
            // Note: We're ignoring changes to the string table made externally here in order to avoid a lot of checks.
            if ((Language) languageField.Value != StringTables.ActiveLanguage)
            {
                languageField.Value = (ulong)StringTables.ActiveLanguage;
                BuildGUI();
            }

            valuesField.Refresh();

            return InspectableState.NotModified;
        }

        /// <summary>
        /// Recreates all the GUI elements used by this inspector.
        /// </summary>
        private void BuildGUI()
        {
            Layout.Clear();
            strings.Clear();

            StringTable stringTable = InspectedObject as StringTable;
            if(stringTable == null)
                return;

            string[] identifiers = stringTable.Identifiers;
            foreach (var identifier in identifiers)
                strings[identifier] = stringTable.GetString(identifier);

            languageField = new GUIEnumField(typeof (Language));
            languageField.OnSelectionChanged += x =>
            {
                StringTables.ActiveLanguage = (Language)x;
                BuildGUI();
                Refresh();
            };

            Layout.AddElement(languageField);

            valuesField = GUIDictionaryField<string, string, StringTableEntry>.Create(
                new LocEdString("Strings"), strings, Layout);

            valuesField.OnChanged += x =>
            {
                if (x != null)
                {
                    foreach (var KVP in x)
                    {
                        if (stringTable.Contains(KVP.Key))
                        {
                            string oldValue = stringTable.GetString(KVP.Key);
                            if (oldValue != KVP.Value)
                                stringTable.SetString(KVP.Key, KVP.Value);
                        }
                        else
                            stringTable.SetString(KVP.Key, KVP.Value);
                    }

                    string[] oldIdentifiers = stringTable.Identifiers;
                    foreach (var identifier in oldIdentifiers)
                    {
                        if (!x.ContainsKey(identifier))
                            stringTable.RemoveString(identifier);
                    }

                    strings = x;
                }
                else
                {
                    foreach (var KVP in strings)
                        stringTable.RemoveString(KVP.Key);

                    strings.Clear();
                }

                EditorApplication.SetDirty(stringTable);
            };

            valuesField.OnValueChanged += x =>
            {
                stringTable.SetString(x, strings[x]);
                EditorApplication.SetDirty(stringTable);
            };

            valuesField.OnValueRemoved += x =>
            {
                stringTable.RemoveString(x);
                EditorApplication.SetDirty(stringTable);
            };
            
            Layout.AddSpace(10);
        }

        /// <summary>
        /// Row element used for displaying GUI for string table dictionary elements.
        /// </summary>
        public class StringTableEntry : GUIDictionaryFieldRow
        {
            private GUITextField keyField;
            private GUITextField valueField;

            /// <inheritdoc/>
            protected override GUILayoutX CreateKeyGUI(GUILayoutY layout)
            {
                GUILayoutX titleLayout = layout.AddLayoutX();
                keyField = new GUITextField(new LocEdString("Identifier"));
                titleLayout.AddElement(keyField);

                keyField.OnChanged += SetKey;

                return titleLayout;
            }

            /// <inheritdoc/>
            protected override void CreateValueGUI(GUILayoutY layout)
            {
                string value = GetValue<string>();

                valueField = new GUITextField(new LocEdString(value));
                layout.AddElement(valueField);

                valueField.OnChanged += x => { SetValue(x); MarkAsModified(); };
                valueField.OnConfirmed += ConfirmModify;
                valueField.OnFocusLost += ConfirmModify;
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
                valueField.Value = GetValue<string>();

                return base.Refresh();
            }
        }
    }
}
