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

        public ResultType Result
        {
            get { return result; }
        }

        public static DialogBox Open(LocString title, LocString message, Type type, Action<ResultType> resultCallback = null)
        {
            DialogBox instance = new DialogBox();

            instance.Width = 250;
            instance.Height = 150;

            instance.Title = title;
            instance.type = type;
            instance.resultCallback = resultCallback;

            return instance;
        }

        private void Initialize(LocString title, LocString message, Type type, Action<ResultType> resultCallback)
        {
            Width = 250;
            Height = 150;

            Title = title;
            messageLabel.SetContent(message);
            this.resultCallback = resultCallback;
            this.type = type;
        }

        protected DialogBox()
            : base(false)
        { }

        private void OnInitialize()
        {
            messageLabel = new GUILabel("");

            GUILayoutY layoutY = GUI.layout.AddLayoutY();

            layoutY.AddFlexibleSpace();
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
                    GUIButton okBtn = new GUIButton("OK");
                    okBtn.OnClick += () => ButtonClicked(ResultType.OK);

                    btnLayout.AddElement(okBtn);
                }
                    break;
                case Type.OKCancel:
                    {
                        GUIButton okBtn = new GUIButton("OK");
                        okBtn.OnClick += () => ButtonClicked(ResultType.OK);

                        GUIButton cancelBtn = new GUIButton("Cancel");
                        cancelBtn.OnClick += () => ButtonClicked(ResultType.Cancel);

                        btnLayout.AddElement(okBtn);
                        btnLayout.AddSpace(20);
                        btnLayout.AddElement(cancelBtn);
                    }
                    break;
                case Type.RetryAbortIgnore:
                    {
                        GUIButton retryBtn = new GUIButton("Retry");
                        retryBtn.OnClick += () => ButtonClicked(ResultType.Retry);

                        GUIButton abortBtn = new GUIButton("Abort");
                        abortBtn.OnClick += () => ButtonClicked(ResultType.Abort);

                        GUIButton ignoreBtn = new GUIButton("Ignore");
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
                        GUIButton retryBtn = new GUIButton("Retry");
                        retryBtn.OnClick += () => ButtonClicked(ResultType.Retry);

                        GUIButton cancelBtn = new GUIButton("Cancel");
                        cancelBtn.OnClick += () => ButtonClicked(ResultType.Cancel);

                        btnLayout.AddElement(retryBtn);
                        btnLayout.AddSpace(20);
                        btnLayout.AddElement(cancelBtn);
                    }
                    break;
                case Type.TryCancelContinue:
                    {
                        GUIButton tryBtn = new GUIButton("Try");
                        tryBtn.OnClick += () => ButtonClicked(ResultType.Try);

                        GUIButton cancelBtn = new GUIButton("Cancel");
                        cancelBtn.OnClick += () => ButtonClicked(ResultType.Cancel);

                        GUIButton continueBtn = new GUIButton("Continue");
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
                        GUIButton yesBtn = new GUIButton("Yes");
                        yesBtn.OnClick += () => ButtonClicked(ResultType.Yes);

                        GUIButton noBtn = new GUIButton("No");
                        noBtn.OnClick += () => ButtonClicked(ResultType.No);

                        btnLayout.AddElement(yesBtn);
                        btnLayout.AddSpace(20);
                        btnLayout.AddElement(noBtn);
                    }
                    break;
                case Type.YesNoCancel:
                    {
                        GUIButton yesBtn = new GUIButton("Yes");
                        yesBtn.OnClick += () => ButtonClicked(ResultType.Yes);

                        GUIButton noBtn = new GUIButton("No");
                        noBtn.OnClick += () => ButtonClicked(ResultType.No);

                        GUIButton cancelBtn = new GUIButton("Cancel");
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
        }
    }
}
