using System;
using BansheeEngine;

namespace BansheeEditor
{
    public class DialogBox : ModalWindow
    {
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
        private bool constructed;

        public ResultType Result
        {
            get { return result; }
        }

        public static DialogBox Open(LocString title, LocString message, Type type, Action<ResultType> resultCallback = null)
        {
            return new DialogBox(title, message, type, resultCallback);
        }

        protected DialogBox(LocString title, LocString message, Type type, Action<ResultType> resultCallback)
            : base(false)
        {
            this.resultCallback = resultCallback;
            this.type = type;

            constructed = true;
            SetupGUI();

            Title = title;
            messageLabel.SetContent(message);

            Width = 280;
            Height = messageLabel.Bounds.height + 60;
        }

        private void OnInitialize()
        {
            if (constructed)
                SetupGUI();
        }

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

        private void ButtonClicked(ResultType result)
        {
            this.result = result;

            if (resultCallback != null)
                resultCallback(result);

            Close();
        }
    }
}
