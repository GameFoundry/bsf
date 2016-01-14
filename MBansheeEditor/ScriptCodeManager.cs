using System.IO;
using System.Text;
using System.Text.RegularExpressions;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Handles various operations related to script code in the active project, like compilation and code editor syncing.
    /// </summary>
    public sealed class ScriptCodeManager
    {
        private bool isGameAssemblyDirty;
        private bool isEditorAssemblyDirty;
        private CompilerInstance compilerInstance;

        /// <summary>
        /// Constructs a new script code manager.
        /// </summary>
        internal ScriptCodeManager()
        {
            ProjectLibrary.OnEntryAdded += OnEntryAdded;
            ProjectLibrary.OnEntryRemoved += OnEntryRemoved;
            ProjectLibrary.OnEntryImported += OnEntryImported;
        }

        /// <summary>
        /// Triggers required compilation or code editor syncing if needed.
        /// </summary>
        internal void Update()
        {
            if (EditorApplication.HasFocus && CodeEditor.IsSolutionDirty)
                CodeEditor.SyncSolution();

            if (EditorApplication.IsStopped)
            {
                if (compilerInstance == null)
                {
                    if (EditorApplication.HasFocus)
                    {
                        string outputDir = EditorApplication.ScriptAssemblyPath;

                        if (isGameAssemblyDirty)
                        {
                            compilerInstance = ScriptCompiler.CompileAsync(
                                ScriptAssemblyType.Game, BuildManager.ActivePlatform, true, outputDir);

                            EditorApplication.SetStatusCompiling(true);
                            isGameAssemblyDirty = false;
                        }
                        else if (isEditorAssemblyDirty)
                        {
                            compilerInstance = ScriptCompiler.CompileAsync(
                                ScriptAssemblyType.Editor, BuildManager.ActivePlatform, true, outputDir);

                            EditorApplication.SetStatusCompiling(true);
                            isEditorAssemblyDirty = false;
                        }
                    }
                }
                else
                {
                    if (compilerInstance.IsDone)
                    {
                        Debug.Clear(DebugMessageType.CompilerWarning);
                        Debug.Clear(DebugMessageType.CompilerError);

                        LogWindow window = EditorWindow.GetWindow<LogWindow>();
                        if (window != null)
                            window.Refresh();

                        if (compilerInstance.HasErrors)
                        {
                            foreach (var msg in compilerInstance.WarningMessages)
                                Debug.LogMessage(FormMessage(msg), DebugMessageType.CompilerWarning);

                            foreach (var msg in compilerInstance.ErrorMessages)
                                Debug.LogMessage(FormMessage(msg), DebugMessageType.CompilerError);
                        }

                        compilerInstance.Dispose();
                        compilerInstance = null;

                        EditorApplication.SetStatusCompiling(false);
                        EditorApplication.ReloadAssemblies();
                    }
                }
            }
        }

        /// <summary>
        /// Triggered when a new resource is added to the project library.
        /// </summary>
        /// <param name="path">Path of the added resource, relative to the project's resource folder.</param>
        private void OnEntryAdded(string path)
        {
            if (IsCodeEditorFile(path))
                CodeEditor.MarkSolutionDirty();
        }

        /// <summary>
        /// Triggered when a resource is removed from the project library.
        /// </summary>
        /// <param name="path">Path of the removed resource, relative to the project's resource folder.</param>
        private void OnEntryRemoved(string path)
        {
            if (IsCodeEditorFile(path))
                CodeEditor.MarkSolutionDirty();
        }

        /// <summary>
        /// Triggered when a resource is (re)imported in the project library.
        /// </summary>
        /// <param name="path">Path of the imported resource, relative to the project's resource folder.</param>
        private void OnEntryImported(string path)
        {
            LibraryEntry entry = ProjectLibrary.GetEntry(path);
            if (entry == null || entry.Type != LibraryEntryType.File)
                return;

            FileEntry fileEntry = (FileEntry)entry;
            if (fileEntry.ResType != ResourceType.ScriptCode)
                return;

            ScriptCode codeFile = ProjectLibrary.Load<ScriptCode>(path);
            if(codeFile == null)
                return;

            if(codeFile.EditorScript)
                isEditorAssemblyDirty = true;
            else
                isGameAssemblyDirty = true;
        }

        /// <summary>
        /// Checks is the resource at the provided path a file relevant to the code editor.
        /// </summary>
        /// <param name="path">Path to the resource, absolute or relative to the project's resources folder.</param>
        /// <returns>True if the file is relevant to the code editor, false otherwise.</returns>
        private bool IsCodeEditorFile(string path)
        {
            LibraryEntry entry = ProjectLibrary.GetEntry(path);
            if (entry != null && entry.Type == LibraryEntryType.File)
            {
                FileEntry fileEntry = (FileEntry)entry;

                foreach (var codeType in CodeEditor.CodeTypes)
                {
                    if (fileEntry.ResType == codeType)
                        return true;
                }
            }

            return false;
        }

        /// <summary>
        /// Converts data reported by the compiler into a readable string.
        /// </summary>
        /// <param name="msg">Message data as reported by the compiler.</param>
        /// <returns>Readable message string.</returns>
        private string FormMessage(CompilerMessage msg)
        {
            StringBuilder sb = new StringBuilder();

            if (msg.type == CompilerMessageType.Error)
                sb.AppendLine("Compiler error: " + msg.message);
            else
                sb.AppendLine("Compiler warning: " + msg.message);

            sb.AppendLine("\tin " + msg.file + "[" + msg.line + ":" + msg.column + "]");

            return sb.ToString();
        }

        /// <summary>
        /// Parses a log message and outputs a data object with a separate message and callstack entries. If the message
        /// is not a valid compiler message null is returned.
        /// </summary>
        /// <param name="message">Message to parse.</param>
        /// <returns>Parsed log message or null if not a valid compiler message.</returns>
        public static ParsedLogEntry ParseCompilerMessage(string message)
        {
            // Note: If modifying FormMessage method make sure to update this one as well to match the formattting

            // Check for error
            Regex regex = new Regex(@"Compiler error: (.*)\n\tin (.*)\[(.*):.*\]");
            var match = regex.Match(message);

            // Check for warning
            if (!match.Success)
            {
                regex = new Regex(@"Compiler warning: (.*)\n\tin (.*)\[(.*):.*\]");
                match = regex.Match(message);
            }

            // No match
            if (!match.Success)
                return null;

            ParsedLogEntry entry = new ParsedLogEntry();
            entry.callstack = new CallStackEntry[1];

            entry.message = match.Groups[1].Value;

            CallStackEntry callstackEntry = new CallStackEntry();
            callstackEntry.method = "";
            callstackEntry.file = match.Groups[2].Value;
            int.TryParse(match.Groups[3].Value, out callstackEntry.line);

            entry.callstack[0] = callstackEntry;
            return entry;
        }
    }
}
