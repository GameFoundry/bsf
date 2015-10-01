using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Helper layout object that wraps a standard <see cref="GUILayout"/>, used primarily by <see cref="InspectableField"/>
    /// implementations keep track of their own GUI elements in a layout.
    /// </summary>
    public class InspectableFieldLayout
    {
        private GUILayoutY parentLayout;
        private List<GUIElement> elements = new List<GUIElement>();

        /// <summary>
        /// Creates a new inspectable field layout object.
        /// </summary>
        /// <param name="parentLayout">GUI layout object to wrap.</param>
        public InspectableFieldLayout(GUILayoutY parentLayout)
        {
            this.parentLayout = parentLayout;
        }

        /// <summary>
        /// Number of child elements in the inspectable layout.
        /// </summary>
        public int NumElements
        {
            get { return elements.Count; }
        }

        /// <summary>
        /// Adds a GUI element to the field layout.
        /// </summary>
        /// <param name="index">Index into the GUI layout at which the field layout start at.</param>
        /// <param name="element">GUI element to insert into the layout.</param>
        public void AddElement(int index, GUIElement element)
        {
            parentLayout.InsertElement(index + elements.Count, element);
            elements.Add(element);
        }

        /// <summary>
        /// Creates a new <see cref="GUILayoutX"/> and adds it to the field layout.
        /// </summary>
        /// <param name="index">Index into the GUI layout at which the field layout start at.</param>
        /// <returns>Newly created layout.</returns>
        public GUILayoutX AddLayoutX(int index)
        {
            GUILayoutX layout = parentLayout.InsertLayoutX(index + elements.Count);
            elements.Add(layout);

            return layout;
        }

        /// <summary>
        /// Creates a new <see cref="GUILayoutY"/> and adds it to the field layout.
        /// </summary>
        /// <param name="index">Index into the GUI layout at which the field layout start at.</param>
        /// <returns>Newly created layout.</returns>
        public GUILayoutY AddLayoutY(int index)
        {
            GUILayoutY layout = parentLayout.InsertLayoutY(index + elements.Count);
            elements.Add(layout);

            return layout;
        }

        /// <summary>
        /// Destroys all GUI elements contained in the field layout. Leaves other elements present in GUI layout intact.
        /// </summary>
        public void DestroyElements()
        {
            foreach (var element in elements)
            {
                element.Destroy();
            }

            elements.Clear();
        }
    }
}
