//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using BansheeEngine;

namespace BansheeEditor
{
    /** @addtogroup Windows
     *  @{
     */

    /// <summary>
    /// A modal window that displays a progress bar.
    /// </summary>
    public class ProgressBar : ModalWindow
    {
        private static ProgressBar instance;

        private GUIProgressBar progressBar;
        private GUILabel messageLabel;

        private LocString message;
        private float percent = 0.0f;

        /// <summary>
        /// Determines the message displayed above the progress bar.
        /// </summary>
        public LocString Message
        {
            set { messageLabel.SetContent(value); message = value; }
            get { return message; }
        }

        /// <summary>
        /// Determines size of the filled portion of the progress bar, in range [0, 1].
        /// </summary>
        public float Percent
        {
            set { progressBar.Percent = value; percent = value; }
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
            {
                instance = new ProgressBar(title, message, percent);
                instance.BuildGUI();
            }
            else
            {
                instance.Title = title;
                instance.Message = message;
                instance.Percent = percent;
            }
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
        /// Creates a new progress bar window.
        /// </summary>
        /// <param name="title">Text to show in the title.</param>
        /// <param name="message">Message to show above the progress bar.</param>
        /// <param name="percent">Initial value to show the progress as, in range [0, 1].</param>
        protected ProgressBar(LocString title, LocString message, float percent)
            : base(false)
        {
            Width = 350;
            Height = 75;

            Title = title;

            this.message = message;
            this.percent = percent;
        }

        private void BuildGUI()
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

            Percent = percent;
            messageLabel.SetContent(message);
        }
    }

    /** @} */
}
