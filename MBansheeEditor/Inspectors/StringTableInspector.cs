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
        private GUIDictionaryField<string, string> valuesField = new GUIDictionaryField<string,string>();

        private Dictionary<string, string> strings = new Dictionary<string,string>();

        /// <inheritdoc/>
        protected internal override void Initialize()
        {
            BuildGUI();
        }

        /// <inheritdoc/>
        protected internal override bool Refresh()
        {
            bool anythingModified = false;

            // Note: We're ignoring changes to the string table made externally here in order to avoid a lot of checks.
            if ((Language) languageField.Value != StringTables.ActiveLanguage)
            {
                languageField.Value = (ulong)StringTables.ActiveLanguage;
                anythingModified = true;
                BuildGUI();
            }

            anythingModified |= valuesField.Refresh();

            return anythingModified;
        }

        /// <summary>
        /// Recreates all the GUI elements used by this inspector.
        /// </summary>
        private void BuildGUI()
        {
            layout.Clear();
            strings.Clear();

            StringTable stringTable = referencedObject as StringTable;
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

            layout.AddElement(languageField);

            valuesField.Update<StringTableEntry>(new LocEdString("Strings"), strings, layout);

            valuesField.OnChanged += x =>
            {
                foreach (var KVP in x)
                {
                    string oldValue;
                    if (strings.TryGetValue(KVP.Key, out oldValue))
                    {
                        if (oldValue != KVP.Value)
                            stringTable.SetString(KVP.Key, KVP.Value);
                    }
                    else
                        stringTable.SetString(KVP.Key, KVP.Value);
                }

                foreach (var KVP in strings)
                {
                    if (!x.ContainsKey(KVP.Key))
                        stringTable.RemoveString(KVP.Key);
                }

                BuildGUI();
                Refresh();
            };

            valuesField.OnValueChanged += x =>
            {
                stringTable.SetString(x, strings[x]);
            };
            
            layout.AddSpace(10);
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
                string key = GetKey<string>();

                GUILayoutX titleLayout = layout.AddLayoutX();
                keyField = new GUITextField(new LocEdString(key));
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

                valueField.OnChanged += SetValue;
            }

            /// <inheritdoc/>
            internal protected override bool Refresh(out bool rebuildGUI)
            {
                rebuildGUI = false;

                string newKey = GetKey<string>();
                if (keyField.Value != newKey)
                {
                    keyField.Value = newKey;
                    return true;
                }

                string newValue = GetValue<string>();
                if (valueField.Value != newValue)
                {
                    valueField.Value = newValue;
                    return true;
                }

                return false;
            }
        }
    }
}
