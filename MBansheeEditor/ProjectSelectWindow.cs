using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    internal sealed class ProjectSelectWindow : ModalWindow
    {
        private GUITextBox textBox;
        private GUIListBox listBox;

        public ProjectSelectWindow()
            :base(0, 0, 500, 200)
        {
            //GUILabel label = GUI.main.AddLabel("Test test");
            //label.SetContent("Test2");

            GUIScrollArea scrollArea = GUI.layout.AddScrollArea();

            GUIButton button = scrollArea.layout.AddButton("CLICK ME");
            button.OnClick += button_OnClick;

            textBox = scrollArea.layout.AddTextBox(true, GUIOption.FixedHeight(500), GUIOption.FixedWidth(100));
            textBox.text = "Supsup?";

            LocString[] listElems = new LocString[] { "ListElem1", "ListElem2", "ListElem3" };
            listBox = scrollArea.layout.AddListBox(listElems);
        }

        void button_OnClick()
        {
            textBox.text = "CLICKED!";

            LocString[] listElems = new LocString[] { "ListElem4", "ListElem5", "ListElem6" };
            listBox.SetElements(listElems);
        }
    }
}
