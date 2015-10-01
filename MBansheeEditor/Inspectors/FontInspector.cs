using System;
using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Renders an inspector for the <see cref="Font"/> resource.
    /// </summary>
    [CustomInspector(typeof(Font))]
    internal class FontInspector : Inspector
    {
        private GUIArrayField fontSizes;
        private GUIArrayField charRanges;
        private GUIToggleField antialiasingField;
        private GUIIntField dpiField;
        private GUIButton reimportButton;

        private FontImportOptions importOptions;

        /// <inheritdoc/>
        protected internal override void Initialize()
        {
            if (referencedObject != null)
            {
                importOptions = GetImportOptions();
                BuildGUI();
            }
        }

        /// <inheritdoc/>
        protected internal override bool Refresh()
        {
            FontImportOptions newImportOptions = GetImportOptions();

            bool rebuildGUI = false;

            int[] newFontSizes = newImportOptions.FontSizes;
            if (newFontSizes == null)
                rebuildGUI |= fontSizes.Array != null;
            else
            {
                if (fontSizes.Array == null)
                    rebuildGUI = true;
                else
                    rebuildGUI |= newFontSizes.Length != fontSizes.Array.GetLength(0);
            }

            CharRange[] newCharRanges = newImportOptions.CharRanges;
            if (newCharRanges == null)
                rebuildGUI |= charRanges.Array != null;
            else
            {
                if (charRanges.Array == null)
                    rebuildGUI = true;
                else
                    rebuildGUI |= newCharRanges.Length != charRanges.Array.GetLength(0);
            }

            if (rebuildGUI)
                BuildGUI();

            bool anythingModified = fontSizes.Refresh();
            anythingModified |= charRanges.Refresh();

            if (antialiasingField.Value != newImportOptions.Antialiasing)
            {
                antialiasingField.Value = newImportOptions.Antialiasing;
                anythingModified = true;
            }

            if (dpiField.Value != newImportOptions.DPI)
            {
                dpiField.Value = newImportOptions.DPI;
                anythingModified = true;
            }

            if (anythingModified)
                importOptions = newImportOptions;

            return anythingModified;
        }

        /// <summary>
        /// Recreates all the GUI elements used by this inspector.
        /// </summary>
        private void BuildGUI()
        {
            layout.Clear();

            fontSizes = GUIArrayField.Create<FontSizeArrayRow, int>(
                new LocEdString("Font sizes"), importOptions.FontSizes, layout);
            fontSizes.OnChanged += x =>
            {
                int[] newFontSizes = x as int[];
                importOptions.FontSizes = newFontSizes;

                BuildGUI();
            };

            charRanges = GUIArrayField.Create<CharRangeArrayRow, CharRange>(
                new LocEdString("Character ranges"), importOptions.CharRanges, layout);
            charRanges.OnChanged += x =>
            {
                CharRange[] newRanges = x as CharRange[];
                importOptions.CharRanges = newRanges;

                BuildGUI();
            };

            antialiasingField = new GUIToggleField(new LocEdString("Antialiasing"));
            dpiField = new GUIIntField(new LocEdString("DPI"));

            reimportButton = new GUIButton(new LocEdString("Reimport"));
            reimportButton.OnClick += TriggerReimport;

            layout.AddElement(antialiasingField);
            layout.AddElement(dpiField);
            layout.AddSpace(10);

            GUILayout reimportButtonLayout = layout.AddLayoutX();
            reimportButtonLayout.AddFlexibleSpace();
            reimportButtonLayout.AddElement(reimportButton);
        }

        /// <summary>
        /// Retrieves import options for the texture we're currently inspecting.
        /// </summary>
        /// <returns>Font import options object.</returns>
        private FontImportOptions GetImportOptions()
        {
            Font font = referencedObject as Font;
            FontImportOptions output = null;

            if (font != null)
            {
                LibraryEntry texEntry = ProjectLibrary.GetEntry(ProjectLibrary.GetPath(font));
                if (texEntry != null && texEntry.Type == LibraryEntryType.File)
                {
                    FileEntry texFileEntry = (FileEntry)texEntry;
                    output = texFileEntry.Options as FontImportOptions;
                }
            }

            if (output == null)
            {
                if (importOptions == null)
                    output = new FontImportOptions();
                else
                    output = importOptions;
            }

            return output;
        }

        /// <summary>
        /// Reimports the texture resource according to the currently set import options.
        /// </summary>
        private void TriggerReimport()
        {
            Texture2D texture = (Texture2D)referencedObject;
            string resourcePath = ProjectLibrary.GetPath(texture);

            ProjectLibrary.Reimport(resourcePath, importOptions, true);
        }

        /// <summary>
        /// Row element used for displaying GUI for font size array elements.
        /// </summary>
        public class FontSizeArrayRow : GUIListFieldRow
        {
            private GUIIntField sizeField;

            /// <inheritdoc/>
            protected override GUILayoutX CreateGUI(GUILayoutY layout)
            {
                GUILayoutX titleLayout = layout.AddLayoutX();
                sizeField = new GUIIntField(new LocEdString(seqIndex + ". "));
                titleLayout.AddElement(sizeField);

                sizeField.OnChanged += SetValue;

                return titleLayout;
            }

            /// <inheritdoc/>
            internal protected override bool Refresh(out bool rebuildGUI)
            {
                rebuildGUI = false;

                int newValue = GetValue<int>();
                if (sizeField.Value != newValue)
                {
                    sizeField.Value = newValue;
                    return true;
                }

                return false;
            }
        }

        /// <summary>
        /// Row element used for displaying GUI for character range array elements.
        /// </summary>
        public class CharRangeArrayRow : GUIListFieldRow
        {
            private GUIIntField rangeStartField;
            private GUIIntField rangeEndField;

            /// <inheritdoc/>
            protected override GUILayoutX CreateGUI(GUILayoutY layout)
            {
                GUILayoutX titleLayout = layout.AddLayoutX();

                rangeStartField = new GUIIntField(new LocEdString(seqIndex + ". Start"));
                rangeEndField = new GUIIntField(new LocEdString("End"));

                titleLayout.AddElement(rangeStartField);
                titleLayout.AddElement(rangeEndField);

                rangeStartField.OnChanged += x =>
                {
                    CharRange range = GetValue<CharRange>();
                    range.start = x;
                    SetValue(range);
                };

                rangeEndField.OnChanged += x =>
                {
                    CharRange range = GetValue<CharRange>();
                    range.end = x;
                    SetValue(range);
                };

                return titleLayout;
            }

            /// <inheritdoc/>
            internal protected override bool Refresh(out bool rebuildGUI)
            {
                rebuildGUI = false;

                bool anythingModified = false;

                CharRange newValue = GetValue<CharRange>();
                if (rangeStartField.Value != newValue.start)
                {
                    rangeStartField.Value = newValue.start;
                    anythingModified = true;
                }

                if (rangeEndField.Value != newValue.end)
                {
                    rangeEndField.Value = newValue.end;
                    anythingModified = true;
                }

                return anythingModified;
            }
        }
    }
}
