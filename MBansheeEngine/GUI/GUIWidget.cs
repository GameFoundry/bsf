namespace BansheeEngine
{
    /// <summary>
    /// Container of GUI elements that can be positioned in the scene, and can output the rendered GUI to a user defined
    /// camera.
    /// </summary>
    public class GUIWidget : Component
    {
        private NativeGUIWidget nativeGUIWidget;

        [SerializeField]
        private SerializableData serializableData = new SerializableData();

        /// <summary>
        /// Skin used for rendering all the GUI elements belonging to this widget.
        /// </summary>
        internal GUISkin Skin
        {
            get { return nativeGUIWidget.Skin; }
            set
            {
                serializableData.skin = value;
                nativeGUIWidget.Skin = value;
            }
        }

        /// <summary>
        /// Determines to which camera are the GUI elements belonong to this widget rendered. If null then they will be
        /// rendered on the main camera.
        /// </summary>
        internal Camera Camera
        {
            get { return nativeGUIWidget.Camera; }
            set
            {
                serializableData.camera = value;
                nativeGUIWidget.Camera = value;
            }
        }

        /// <summary>
        /// Container into which all GUI elements belonging to this widget should be placed.
        /// </summary>
        internal GUIPanel Panel
        {
            get { return nativeGUIWidget.Panel; }
        }

        private void OnReset()
        {
            if (nativeGUIWidget != null)
                nativeGUIWidget.Destroy();

            nativeGUIWidget = new NativeGUIWidget();

            // Restore saved values after reset
            nativeGUIWidget.Skin = serializableData.skin;
            nativeGUIWidget.Camera = serializableData.camera;
        }

        private void OnDestroy()
        {
            nativeGUIWidget.Destroy();
        }

        /// <summary>
        /// Holds all data the GUIWidget component needs to persist through serialization.
        /// </summary>
        [SerializeObject]
        private class SerializableData
        {
            public GUISkin skin;
            public Camera camera;
        }
    }
}
