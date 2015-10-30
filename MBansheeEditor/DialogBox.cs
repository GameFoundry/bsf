using System;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Displays a modal window containing a title, a message and a set of buttons. Allows an easy way to query and inform
    /// the user.
    /// </summary>
    public class DialogBox : ModalWindow
    {
        /// <summary>
        /// Type of the dialog box, determines what buttons to show.
        /// </summary>
        public enum Type
        {
            OK,
            OKCancel,
            YesNo,
            YesNoCancel,
            RetryCancel,
            RetryAbortIgnore,
            TryCancelContinue
        }

        /// <summary>
        /// Type of button that was pressed when a dialog box was closed.
        /// </summary>
        public enum ResultType
        {
            Yes,
            No,
            OK,
            Cancel,
            Retry,
            Abort,
            Ignore,
            Try,
            Continue,
            None
        }

        private Action<ResultType> resultCallback;
        private Type type;

        private GUILabel messageLabel;
        private ResultType result = ResultType.None;

        /// <summary>
        /// Button that was pressed when the dialog box was closed. Only valid after the user closes the dialog box.
        /// </summary>
        public ResultType Result
        {
            get { return result; }
        }

        /// <summary>
        /// Opens a new dialog box.
        /// </summary>
        /// <param name="title">Text to display in the title bar.</param>
        /// <param name="message">Message to display in the dialog box.</param>
        /// <param name="type">Type of dialog box that determines what buttons to display.</param>
        /// <param name="resultCallback">Callback to trigger when the user clicks on a dialog box button.</param>
        /// <returns>Instance of the dialog box window.</returns>
        public static DialogBox Open(LocString title, LocString message, Type type, Action<ResultType> resultCallback = null)
        {
            return new DialogBox(title, message, type, resultCallback);
        }

        /// <summary>
        /// Constructs the dialog box.
        /// </summary>
        /// <param name="title">Text to display in the title bar.</param>
        /// <param name="message">Message to display in the dialog box.</param>
        /// <param name="type">Type of dialog box that determines what buttons to display.</param>
        /// <param name="resultCallback">Callback to trigger when the user clicks on a dialog box button.</param>
        protected DialogBox(LocString title, LocString message, Type type, Action<ResultType> resultCallback)
            : base(false)
        {
            this.resultCallback = resultCallback;
            this.type = type;

            SetupGUI();

            Title = title;
            messageLabel.SetContent(message);

            Width = 280;
            Height = messageLabel.Bounds.height + 60;
        }

        private void OnEditorUpdate()
        {
            if (Input.IsButtonDown(ButtonCode.Return))
            {
                switch (type)
                {
                    case Type.OK:
                    case Type.OKCancel:
                        ButtonClicked(ResultType.OK);
                        break;
                    case Type.RetryAbortIgnore:
                    case Type.RetryCancel:
                        ButtonClicked(ResultType.Retry);
                        break;
                    case Type.TryCancelContinue:
                        ButtonClicked(ResultType.Try);
                        break;
                    case Type.YesNo:
                    case Type.YesNoCancel:
                        ButtonClicked(ResultType.Yes); 
                        break;
                }
            }

            if (Input.IsButtonDown(ButtonCode.Escape))
            {
                switch (type)
                {
                    case Type.OK:
                        ButtonClicked(ResultType.OK);
                        break;
                    case Type.RetryAbortIgnore:
                        ButtonClicked(ResultType.Ignore);
                        break;
                    case Type.OKCancel:
                    case Type.RetryCancel:
                    case Type.YesNoCancel:
                    case Type.TryCancelContinue:
                        ButtonClicked(ResultType.Cancel);
                        break;
                    case Type.YesNo:
                        ButtonClicked(ResultType.No);
                        break;
                }
            }
        }

        /// <summary>
        /// Creates all of the GUI elements required for the specified type of dialog box.
        /// </summary>
        private void SetupGUI()
        {
            messageLabel = new GUILabel("", EditorStyles.MultiLineLabel,
                GUIOption.FixedWidth(260), GUIOption.FlexibleHeight(0, 600));

            GUILayoutY layoutY = GUI.AddLayoutY();

            layoutY.AddSpace(10);
            GUILayoutX messageLayout = layoutY.AddLayoutX();
            messageLayout.AddFlexibleSpace();
            messageLayout.AddElement(messageLabel);
            messageLayout.AddFlexibleSpace();

            layoutY.AddSpace(10);

            GUILayoutX btnLayout = layoutY.AddLayoutX();
            btnLayout.AddFlexibleSpace();

            switch (type)
            {
                case Type.OK:
                    {
                        GUIButton okBtn = new GUIButton(new LocEdString("OK"));
                        okBtn.OnClick += () => ButtonClicked(ResultType.OK);

                        btnLayout.AddElement(okBtn);
                    }
                    break;
                case Type.OKCancel:
                    {
                        GUIButton okBtn = new GUIButton(new LocEdString("OK"));
                        okBtn.OnClick += () => ButtonClicked(ResultType.OK);

                        GUIButton cancelBtn = new GUIButton(new LocEdString("Cancel"));
                        cancelBtn.OnClick += () => ButtonClicked(ResultType.Cancel);

                        btnLayout.AddElement(okBtn);
                        btnLayout.AddSpace(20);
                        btnLayout.AddElement(cancelBtn);
                    }
                    break;
                case Type.RetryAbortIgnore:
                    {
                        GUIButton retryBtn = new GUIButton(new LocEdString("Retry"));
                        retryBtn.OnClick += () => ButtonClicked(ResultType.Retry);

                        GUIButton abortBtn = new GUIButton(new LocEdString("Abort"));
                        abortBtn.OnClick += () => ButtonClicked(ResultType.Abort);

                        GUIButton ignoreBtn = new GUIButton(new LocEdString("Ignore"));
                        ignoreBtn.OnClick += () => ButtonClicked(ResultType.Ignore);

                        btnLayout.AddElement(retryBtn);
                        btnLayout.AddSpace(20);
                        btnLayout.AddElement(abortBtn);
                        btnLayout.AddSpace(20);
                        btnLayout.AddElement(ignoreBtn);
                    }
                    break;
                case Type.RetryCancel:
                    {
                        GUIButton retryBtn = new GUIButton(new LocEdString("Retry"));
                        retryBtn.OnClick += () => ButtonClicked(ResultType.Retry);

                        GUIButton cancelBtn = new GUIButton(new LocEdString("Cancel"));
                        cancelBtn.OnClick += () => ButtonClicked(ResultType.Cancel);

                        btnLayout.AddElement(retryBtn);
                        btnLayout.AddSpace(20);
                        btnLayout.AddElement(cancelBtn);
                    }
                    break;
                case Type.TryCancelContinue:
                    {
                        GUIButton tryBtn = new GUIButton(new LocEdString("Try"));
                        tryBtn.OnClick += () => ButtonClicked(ResultType.Try);

                        GUIButton cancelBtn = new GUIButton(new LocEdString("Cancel"));
                        cancelBtn.OnClick += () => ButtonClicked(ResultType.Cancel);

                        GUIButton continueBtn = new GUIButton(new LocEdString("Continue"));
                        continueBtn.OnClick += () => ButtonClicked(ResultType.Continue);

                        btnLayout.AddElement(tryBtn);
                        btnLayout.AddSpace(20);
                        btnLayout.AddElement(cancelBtn);
                        btnLayout.AddSpace(20);
                        btnLayout.AddElement(continueBtn);
                    }
                    break;
                case Type.YesNo:
                    {
                        GUIButton yesBtn = new GUIButton(new LocEdString("Yes"));
                        yesBtn.OnClick += () => ButtonClicked(ResultType.Yes);

                        GUIButton noBtn = new GUIButton(new LocEdString("No"));
                        noBtn.OnClick += () => ButtonClicked(ResultType.No);

                        btnLayout.AddElement(yesBtn);
                        btnLayout.AddSpace(20);
                        btnLayout.AddElement(noBtn);
                    }
                    break;
                case Type.YesNoCancel:
                    {
                        GUIButton yesBtn = new GUIButton(new LocEdString("Yes"));
                        yesBtn.OnClick += () => ButtonClicked(ResultType.Yes);

                        GUIButton noBtn = new GUIButton(new LocEdString("No"));
                        noBtn.OnClick += () => ButtonClicked(ResultType.No);

                        GUIButton cancelBtn = new GUIButton(new LocEdString("Cancel"));
                        cancelBtn.OnClick += () => ButtonClicked(ResultType.Cancel);

                        btnLayout.AddElement(yesBtn);
                        btnLayout.AddSpace(20);
                        btnLayout.AddElement(noBtn);
                        btnLayout.AddSpace(20);
                        btnLayout.AddElement(cancelBtn);
                    }
                    break;
            }

            btnLayout.AddFlexibleSpace();
            layoutY.AddFlexibleSpace();
        }

        /// <summary>
        /// Triggered when one of the dialog box buttons was clicked.
        /// </summary>
        /// <param name="result">Type of the button that was clicked.</param>
        private void ButtonClicked(ResultType result)
        {
            this.result = result;

            if (resultCallback != null)
                resultCallback(result);

            Close();
        }
    }
}
