using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Renders an inspector for the <see cref="PlainText"/> resource.
    /// </summary>
    [CustomInspector(typeof(PlainText))]
    internal class PlainTextInspector : Inspector
    {
        private const int MAX_SHOWN_CHARACTERS = 3000;

        private GUILabel textLabel = new GUILabel("", EditorStyles.MultiLineLabel, GUIOption.FixedHeight(500));
        private GUITexture textBg = new GUITexture(null, EditorStyles.ScrollAreaBg);
        private bool isInitialized;

        private string shownText = "";

        /// <inheritdoc/>
        internal override bool Refresh()
        {
            PlainText plainText = referencedObject as PlainText;
            if (plainText == null)
                return false;

            if (!isInitialized)
            {
                GUIPanel textPanel = layout.AddPanel();
                GUILayout textLayoutY = textPanel.AddLayoutY();
                textLayoutY.AddSpace(5);
                GUILayout textLayoutX = textLayoutY.AddLayoutX();
                textLayoutX.AddSpace(5);
                textLayoutX.AddElement(textLabel);
                textLayoutX.AddSpace(5);
                textLayoutY.AddSpace(5);

                GUIPanel textBgPanel = textPanel.AddPanel(1);
                textBgPanel.AddElement(textBg);

                isInitialized = true;
            }

            bool anythingModified = false;

            string newText = plainText.Text;
            string newShownText = plainText.Text.Substring(0, MathEx.Min(newText.Length, MAX_SHOWN_CHARACTERS));

            if (newShownText != shownText)
            {
                textLabel.SetContent(newShownText);
                shownText = newShownText;
                anythingModified = true;
            }

            return anythingModified;
        }
    }
}