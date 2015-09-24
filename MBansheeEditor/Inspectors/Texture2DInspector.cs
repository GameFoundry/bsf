using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Renders an inspector for the <see cref="Texture2D"/> resource.
    /// </summary>
    [CustomInspector(typeof(Texture2D))]
    internal class Texture2DInspector : Inspector
    {
        private bool isInitialized;
        private GUIEnumField formatField = new GUIEnumField(typeof(PixelFormat), new LocEdString("Format"));
        private GUIToggleField generateMipsField = new GUIToggleField(new LocEdString("Generate mipmaps"));
        private GUIIntField maximumMipsField = new GUIIntField(new LocEdString("Maximum mipmap level"));
        private GUIToggleField srgbField = new GUIToggleField(new LocEdString("Gamma space"));
        private GUIToggleField cpuReadableField = new GUIToggleField(new LocEdString("CPU readable"));
        private GUIButton reimportButton = new GUIButton(new LocEdString("Reimport"));

        private TextureImportOptions importOptions;

        /// <summary>
        /// Initializes required data the first time <see cref="Refresh"/> is called.
        /// </summary>
        private void Initialize()
        {
            if (referencedObject != null)
            {
                importOptions = GetImportOptions();

                formatField.OnSelectionChanged += x => importOptions.Format = (PixelFormat)x;
                generateMipsField.OnChanged += x => importOptions.GenerateMipmaps = x;
                maximumMipsField.OnChanged += x => importOptions.MaxMipmapLevel = x;
                srgbField.OnChanged += x => importOptions.IsSRGB = x;
                cpuReadableField.OnChanged += x => importOptions.CPUReadable = x;

                reimportButton.OnClick += TriggerReimport;

                layout.AddElement(formatField);
                layout.AddElement(generateMipsField);
                layout.AddElement(maximumMipsField);
                layout.AddElement(srgbField);
                layout.AddElement(cpuReadableField);
                layout.AddSpace(10);

                GUILayout reimportButtonLayout = layout.AddLayoutX();
                reimportButtonLayout.AddFlexibleSpace();
                reimportButtonLayout.AddElement(reimportButton);
            }

            isInitialized = true;
        }

        /// <summary>
        /// Retrieves import options for the texture we're currently inspecting.
        /// </summary>
        /// <returns>Texture import options object.</returns>
        private TextureImportOptions GetImportOptions()
        {
            Texture2D texture = referencedObject as Texture2D;
            TextureImportOptions output = null;

            if (texture != null)
            {
                LibraryEntry texEntry = ProjectLibrary.GetEntry(ProjectLibrary.GetPath(texture));
                if (texEntry != null && texEntry.Type == LibraryEntryType.File)
                {
                    FileEntry texFileEntry = (FileEntry) texEntry;
                    output = texFileEntry.Options as TextureImportOptions;
                }
            }

            if (output == null)
            {
                if (importOptions == null)
                    output = new TextureImportOptions();
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

        /// <inheritdoc/>
        internal override bool Refresh()
        {
            if (!isInitialized)
            {
                Initialize();
                isInitialized = true;
            }

            TextureImportOptions newImportOptions = GetImportOptions();

            bool anythingModified = false;

            if (formatField.Value != (ulong)newImportOptions.Format)
            {
                formatField.Value = (ulong)newImportOptions.Format;
                anythingModified = true;
            }

            if (generateMipsField.Value != newImportOptions.GenerateMipmaps)
            {
                generateMipsField.Value = newImportOptions.GenerateMipmaps;
                anythingModified = true;
            }

            if (maximumMipsField.Value != newImportOptions.MaxMipmapLevel)
            {
                maximumMipsField.Value = newImportOptions.MaxMipmapLevel;
                anythingModified = true;
            }

            if (srgbField.Value != newImportOptions.IsSRGB)
            {
                srgbField.Value = newImportOptions.IsSRGB;
                anythingModified = true;
            }

            if (cpuReadableField.Value != newImportOptions.CPUReadable)
            {
                cpuReadableField.Value = newImportOptions.CPUReadable;
                anythingModified = true;
            }

            if (anythingModified)
                importOptions = newImportOptions;

            return anythingModified;
        }
    }
}
