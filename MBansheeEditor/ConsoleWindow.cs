using BansheeEngine;
using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace BansheeEditor
{
    /// <summary>
    /// Displays a list of log messages.
    /// </summary>
    public class ConsoleWindow : EditorWindow
    {
        /// <summary>
        /// Filter type that determines what kind of messages are shown in the console.
        /// </summary>
        [Flags]
        private enum EntryFilter
        {
            Info = 0x01, Warning = 0x02, Error = 0x04, All = Info | Warning | Error
        }

        private const int ENTRY_HEIGHT = 33;
        private static int sSelectedElementIdx = -1;

        private GUIListView<ConsoleGUIEntry, ConsoleEntryData> listView;
        private List<ConsoleEntryData> entries = new List<ConsoleEntryData>();
        private EntryFilter filter = EntryFilter.All;

        /// <summary>
        /// Opens the console window.
        /// </summary>
        [MenuItem("Windows/Console", ButtonModifier.CtrlAlt, ButtonCode.C, 6000)]
        private static void OpenConsoleWindow()
        {
            OpenWindow<ConsoleWindow>();
        }

        /// <inheritdoc/>
        protected override LocString GetDisplayName()
        {
            return new LocEdString("Console");
        }

        private void OnInitialize()
        {
            Width = 300;

            GUILayoutY layout = GUI.AddLayoutY();

            listView = new GUIListView<ConsoleGUIEntry, ConsoleEntryData>(Width, Height, ENTRY_HEIGHT, layout);

            Debug.OnAdded += OnEntryAdded;

            // TODO - Add buttons to filter info/warning/error
            // TODO - Add button to clear console
            // TODO - Add button that splits the window vertically and displays details about an entry + callstack
            // TODO - On entry double-click open VS at that line
            // TODO - On callstack entry double-click open VS at that line


            for (int i = 0; i < 10; i++)
                Debug.Log("DUMMY ENTRY #" + i);
        }

        private void OnEditorUpdate()
        {
            listView.Update();
        }

        private void OnDestroy()
        {
            Debug.OnAdded -= OnEntryAdded;
        }

        /// <inheritdoc/>
        protected override void WindowResized(int width, int height)
        {
            listView.SetSize(width, height);

            base.WindowResized(width, height);
        }

        /// <summary>
        /// Triggered when a new entry is added in the debug log.
        /// </summary>
        /// <param name="type">Type of the message.</param>
        /// <param name="message">Message string.</param>
        private void OnEntryAdded(DebugMessageType type, string message)
        {
            // Check if compiler message, otherwise parse it normally
            LogEntryData logEntry = ScriptCodeManager.ParseCompilerMessage(message);
            if (logEntry == null)
                logEntry = Debug.ParseLogMessage(message);

            ConsoleEntryData newEntry = new ConsoleEntryData();
            newEntry.type = type;
            newEntry.callstack = logEntry.callstack;
            newEntry.message = logEntry.message;

            entries.Add(newEntry);

            if (DoesFilterMatch(type))
                listView.AddEntry(newEntry);
        }

        /// <summary>
        /// Changes the filter that controls what type of messages are displayed in the console.
        /// </summary>
        /// <param name="filter">Flags that control which type of messages should be displayed.</param>
        private void SetFilter(EntryFilter filter)
        {
            if (this.filter == filter)
                return;

            listView.Clear();
            foreach (var entry in entries)
            {
                if (DoesFilterMatch(entry.type))
                    listView.AddEntry(entry);
            }

            this.filter = filter;
        }

        /// <summary>
        /// Checks if the currently active entry filter matches the provided type (i.e. the entry with the type should be
        /// displayed).
        /// </summary>
        /// <param name="type">Type of the entry to check.</param>
        /// <returns>True if the entry with the specified type should be displayed in the console.</returns>
        private bool DoesFilterMatch(DebugMessageType type)
        {
            switch (type)
            {
                case DebugMessageType.Info:
                    return filter.HasFlag(EntryFilter.Info);
                case DebugMessageType.Warning:
                    return filter.HasFlag(EntryFilter.Warning);
                case DebugMessageType.Error:
                    return filter.HasFlag(EntryFilter.Error);
            }

            return false;
        }

        /// <summary>
        /// Removes all entries from the console.
        /// </summary>
        private void Clear()
        {
            listView.Clear();
            entries.Clear();
        }

        /// <summary>
        /// Contains data for a single entry in the console.
        /// </summary>
        private class ConsoleEntryData : GUIListViewData
        {
            public DebugMessageType type;
            public string message;
            public CallStackEntry[] callstack;
        }

        /// <summary>
        /// Contains GUI elements used for displaying a single entry in the console.
        /// </summary>
        private class ConsoleGUIEntry : GUIListViewEntry<ConsoleEntryData>
        {
            private const int CALLER_LABEL_HEIGHT = 11;
            private const int MESSAGE_HEIGHT = ENTRY_HEIGHT - CALLER_LABEL_HEIGHT;
            private static readonly Color BG_COLOR = Color.DarkGray;
            private static readonly Color SELECTION_COLOR = Color.DarkCyan;

            private GUIPanel overlay;
            private GUIPanel main;
            private GUIPanel underlay;

            private GUILabel messageLabel;
            private GUILabel functionLabel;
            private GUITexture background;

            private int entryIdx;
            private string file;
            private int line;

            /// <inheritdoc/>
            public override void BuildGUI()
            {
                main = Layout.AddPanel(0, 1, 1, GUIOption.FixedHeight(ENTRY_HEIGHT));
                overlay = main.AddPanel(-1, 0, 0, GUIOption.FixedHeight(ENTRY_HEIGHT));
                underlay = main.AddPanel(1, 0, 0, GUIOption.FixedHeight(ENTRY_HEIGHT));

                GUILayoutY mainLayout = main.AddLayoutY();
                GUILayoutY overlayLayout = overlay.AddLayoutY();
                GUILayoutY underlayLayout = underlay.AddLayoutY();

                messageLabel = new GUILabel(new LocEdString(""), EditorStyles.MultiLineLabel, GUIOption.FixedHeight(MESSAGE_HEIGHT));
                functionLabel = new GUILabel(new LocEdString(""), GUIOption.FixedHeight(CALLER_LABEL_HEIGHT));

                mainLayout.AddElement(messageLabel);
                mainLayout.AddElement(functionLabel);

                background = new GUITexture(Builtin.WhiteTexture, GUIOption.FixedHeight(ENTRY_HEIGHT));
                underlayLayout.AddElement(background);

                GUIButton button = new GUIButton(new LocEdString(""), EditorStyles.Blank, GUIOption.FixedHeight(ENTRY_HEIGHT));
                overlayLayout.AddElement(button);

                button.OnClick += OnClicked;
                button.OnDoubleClick += OnDoubleClicked;
            }

            /// <inheritdoc/>
            public override void UpdateContents(int index, ConsoleEntryData data)
            {
                if (index != sSelectedElementIdx)
                {
                    if (index%2 != 0)
                    {
                        background.Visible = true;
                        background.SetTint(BG_COLOR);
                    }
                    else
                    {
                        background.Visible = false;
                    }
                }
                else
                {
                    background.Visible = true;
                    background.SetTint(SELECTION_COLOR);
                }

                messageLabel.SetContent(new LocEdString(data.message));

                string method = "";
                if (data.callstack != null && data.callstack.Length > 0)
                {
                    file = data.callstack[0].file;
                    line = data.callstack[0].line;

                    if (string.IsNullOrEmpty(data.callstack[0].method))
                        method = "\tat " + file + ":" + line;
                    else
                        method = "\t" + data.callstack[0].method + " at " + file + ":" + line;
                }
                else
                {
                    file = "";
                    line = 0;
                }

                functionLabel.SetContent(new LocEdString(method));

                entryIdx = index;
            }

            /// <summary>
            /// Triggered when the entry is selected.
            /// </summary>
            private void OnClicked()
            {
                sSelectedElementIdx = entryIdx;

                RefreshEntries();
            }

            /// <summary>
            /// Triggered when the entry is double-clicked.
            /// </summary>
            private void OnDoubleClicked()
            {
                if(!string.IsNullOrEmpty(file))
                    CodeEditor.OpenFile(file, line);
            }
        }
    }
}
