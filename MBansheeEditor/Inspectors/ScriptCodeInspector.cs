using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Renders an inspector for the <see cref="ScriptCode"/> resource.
    /// </summary>
    [CustomInspector(typeof (ScriptCode))]
    internal class ScriptCodeInspector : Inspector
    {
        private const int MAX_SHOWN_CHARACTERS = 3000;

        private GUILabel textLabel = new GUILabel("", EditorStyles.MultiLineLabel, GUIOption.FixedHeight(500));
        private GUITexture textBg = new GUITexture(null, EditorStyles.ScrollAreaBg);
        private GUIToggleField isEditorField = new GUIToggleField(new LocEdString("Is editor script"));

        private string shownText = "";

        /// <inheritdoc/>
        protected internal override void Initialize()
        {
            ScriptCode scriptCode = referencedObject as ScriptCode;
            if (scriptCode == null)
                return;

            isEditorField.OnChanged += x =>
            {
                scriptCode.EditorScript = x;
                EditorApplication.SetDirty(scriptCode);
            };

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

            layout.AddElement(isEditorField);
        }

        /// <inheritdoc/>
        protected internal override void Refresh()
        {
            ScriptCode scriptCode = referencedObject as ScriptCode;
            if (scriptCode == null)
                return;

            isEditorField.Value = scriptCode.EditorScript;

            string newText = scriptCode.Text;
            string newShownText = scriptCode.Text.Substring(0, MathEx.Min(newText.Length, MAX_SHOWN_CHARACTERS));

            if (newShownText != shownText)
            {
                textLabel.SetContent(newShownText);
                shownText = newShownText;
            }
        }
    }
}