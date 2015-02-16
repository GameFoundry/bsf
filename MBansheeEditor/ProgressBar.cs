using System;
using BansheeEngine;

namespace BansheeEditor
{
    public class ProgressBar : ModalWindow
    {
        private static ProgressBar instance;

        private GUIProgressBar progressBar;

        public float Percent
        {
            set { progressBar.Percent = value; }
            get { return progressBar.Percent; }
        }

        public static void Show(LocString title, float percent)
        {
            if (instance == null)
                instance = new ProgressBar();

            instance.Width = 200;
            instance.Height = 100;

            instance.Title = title;
            instance.Percent = percent;
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

            GUI.layout.AddFlexibleSpace();
            GUI.layout.AddElement(progressBar);
            GUI.layout.AddFlexibleSpace();
        }
    }
}
