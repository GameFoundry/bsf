using System;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// A modal window that displays a progress bar.
    /// </summary>
    public class ProgressBar : ModalWindow
    {
        private static ProgressBar instance;

        private GUIProgressBar progressBar;
        private GUILabel messageLabel;

        /// <summary>
        /// Determines size of the filled portion of the progress bar, in range [0, 1].
        /// </summary>
        public float Percent
        {
            set { progressBar.Percent = value; }
            get { return progressBar.Percent; }
        }

        /// <summary>
        /// Shows a progress bar window.
        /// </summary>
        /// <param name="title">Text to show in the title.</param>
        /// <param name="percent">Initial value to show the progress as, in range [0, 1].</param>
        public static void Show(LocString title, float percent)
        {
            Show(title, "", percent);
        }

        /// <summary>
        /// Shows a progress bar window.
        /// </summary>
        /// <param name="title">Text to show in the title.</param>
        /// <param name="message">Message to show above the progress bar.</param>
        /// <param name="percent">Initial value to show the progress as, in range [0, 1].</param>
        public static void Show(LocString title, LocString message, float percent)
        {
            if (instance == null)
                instance = new ProgressBar();

            instance.Initialize(title, message, percent);
        }

        /// <summary>
        /// Initializes the progress bar window. Must be called after construction and before use.
        /// </summary>
        /// <param name="title">Text to show in the title.</param>
        /// <param name="message">Message to show above the progress bar.</param>
        /// <param name="percent">Initial value to show the progress as, in range [0, 1].</param>
        private void Initialize(LocString title, LocString message, float percent)
        {
            Width = 350;
            Height = 75;

            Title = title;
            Percent = percent;
            messageLabel.SetContent(message);
        }

        /// <summary>
        /// Hides the progress bar, closing the window.
        /// </summary>
        public static void Hide()
        {
            if (instance != null)
                instance.Close();

            instance = null;
        }

        /// <summary>
        /// Creates a new uninitialized progress bar.
        /// </summary>
        protected ProgressBar()
            :base(false)
        { }

        private void OnInitialize()
        {
            progressBar = new GUIProgressBar();
            messageLabel = new GUILabel("");

            GUILayoutY layoutY = GUI.AddLayoutY();

            layoutY.AddFlexibleSpace();
            GUILayoutX messageLayout = layoutY.AddLayoutX();
            messageLayout.AddFlexibleSpace();
            messageLayout.AddElement(messageLabel);
            messageLayout.AddFlexibleSpace();

            layoutY.AddSpace(10);

            GUILayoutX barLayout = layoutY.AddLayoutX();
            barLayout.AddSpace(30);
            barLayout.AddElement(progressBar);
            barLayout.AddSpace(30);

            layoutY.AddFlexibleSpace();
        }
    }
}
