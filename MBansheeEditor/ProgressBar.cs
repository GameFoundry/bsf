using System;
using BansheeEngine;

namespace BansheeEditor
{
    public class ProgressBar : ModalWindow
    {
        private GUIProgressBar progressBar;

        private void OnInitialize()
        {
            progressBar = new GUIProgressBar();
            progressBar.Percent = 0.5f; // DEBUG ONLY
        }
    }
}
