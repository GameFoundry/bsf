using System;
using BansheeEngine;

namespace BansheeEditor
{
    public class ProgressBar : ModalWindow
    {
        private static ProgressBar instance;

        private GUIProgressBar progressBar;
        private LocString message;

        public float Percent
        {
            set { progressBar.Percent = value; }
            get { return progressBar.Percent; }
        }

        public static void Show(LocString title, float percent)
        {
            Show(title, "", percent);
        }

        public static void Show(LocString title, LocString message, float percent)
        {
            if (instance == null)
                instance = new ProgressBar();

            instance.Width = 250;
            instance.Height = 75;

            instance.Title = title;
            instance.Percent = percent;
            instance.message = message;
        }

        public static void Hide()
        {
            if (instance != null)
                instance.Close();

            instance = null;
        }

        protected ProgressBar()
            :base(false)
        { }

        private void OnInitialize()
        {
            progressBar = new GUIProgressBar();

            GUILayoutY layoutY = GUI.layout.AddLayoutY();

            layoutY.AddFlexibleSpace();
            GUILayoutX messageLayout = layoutY.AddLayoutX();
            messageLayout.AddFlexibleSpace();
            messageLayout.AddElement(new GUILabel(message));
            messageLayout.AddFlexibleSpace();

            layoutY.AddSpace(10);

            GUILayoutX barLayout = layoutY.AddLayoutX();
            barLayout.AddSpace(10);
            barLayout.AddElement(progressBar);
            barLayout.AddSpace(10);

            layoutY.AddFlexibleSpace();
        }
    }
}
