using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    public enum ScriptAssemblyType
    {
        Game, Editor
    }

    public struct CompileData
    {
        public string[] files;
        public string defines;
    }

    public class CompilerInstance
    {
        private Process process;
        private Thread readErrorsThread;

        private List<CompilerMessage> errors = new List<CompilerMessage>();
        private List<CompilerMessage> warnings = new List<CompilerMessage>();

        private Regex messageParseRegex = new Regex(@"\s*(?<file>.*)\(\s*(?<line>\d+)\s*,\s*(?<column>\d+)\s*\)\s*:\s*(?<type>warning|error)\s+(.*):\s*(?<message>.*)");

        internal CompilerInstance(string[] files, string defines, string[] assemblyFolders, string[] assemblies,
            bool debugBuild, string outputFile)
        {
            ProcessStartInfo procStartInfo = new ProcessStartInfo();
            StringBuilder argumentsBuilder = new StringBuilder();

            if (!string.IsNullOrEmpty(defines))
                argumentsBuilder.Append(" -d:" + defines);

            if (assemblyFolders != null && assemblyFolders.Length > 0)
            {
                argumentsBuilder.Append(" -lib:");

                for (int i = 0; i < assemblyFolders.Length - 1; i++)
                    argumentsBuilder.Append(assemblyFolders[i] + ",");

                argumentsBuilder.Append(assemblyFolders[assemblyFolders.Length - 1]);
            }

            if (assemblies != null && assemblies.Length > 0)
            {
                argumentsBuilder.Append(" -r:");

                for (int i = 0; i < assemblies.Length - 1; i++)
                    argumentsBuilder.Append(assemblies[i] + ",");

                argumentsBuilder.Append(assemblies[assemblies.Length - 1]);
            }

            if (debugBuild)
                argumentsBuilder.Append(" -debug+ -o-");
            else
                argumentsBuilder.Append(" -debug- -o+");

            argumentsBuilder.Append(" -target:library -out:" + outputFile);

            for (int i = 0; i < files.Length; i++)
                argumentsBuilder.Append(" \"" + files[i] + "\"");

            procStartInfo.Arguments = argumentsBuilder.ToString();
            procStartInfo.CreateNoWindow = true;
            procStartInfo.FileName = EditorApplication.CompilerPath;
            procStartInfo.RedirectStandardError = true;
            procStartInfo.RedirectStandardOutput = false;
            procStartInfo.UseShellExecute = false;
            procStartInfo.WorkingDirectory = EditorApplication.ProjectPath;

            process = new Process();
            process.StartInfo = procStartInfo;
            process.Start();

            readErrorsThread = new Thread(ReadErrorStream);
            readErrorsThread.Start();
        }

        private void ReadErrorStream()
        {
            while (true)
            {
                if (process == null || process.HasExited)
                    return;

                string line = process.StandardError.ReadLine();

                CompilerMessage message;
                if (TryParseCompilerMessage(line, out message))
                {
                    if (message.type == CompilerMessageType.Warning)
                    {
                        lock (warnings)
                            warnings.Add(message);
                    }
                    else if (message.type == CompilerMessageType.Error)
                    {
                        lock (errors)
                            errors.Add(message);
                    }
                }
            }
        }

        private bool TryParseCompilerMessage(string messageText, out CompilerMessage message)
        {
            message = new CompilerMessage();

            Match match = messageParseRegex.Match(messageText);
            if (!match.Success)
                return false;

            message.file = match.Groups["file"].Value;
            message.line = Int32.Parse(match.Groups["line"].Value);
            message.column = Int32.Parse(match.Groups["column"].Value);
            message.type = match.Groups["type"].Value == "error" ? CompilerMessageType.Error : CompilerMessageType.Warning;
            message.message = match.Groups["message"].Value;

            return true;
        }

        public bool IsDone
        {
            get { return process.HasExited && readErrorsThread.ThreadState == System.Threading.ThreadState.Stopped; }
        }

        public bool HasErrors
        {
            get { return IsDone && process.ExitCode != 0; }
        }

        public CompilerMessage[] WarningMessages
        {
            get
            {
                lock (warnings)
                {
                    return warnings.ToArray();
                }
            }
        }

        public CompilerMessage[] ErrorMessages
        {
            get
            {
                lock (errors)
                {
                    return errors.ToArray();
                }
            }
        }

        public void Dispose()
        {
            if (process == null)
                return;

            if (!process.HasExited)
            {
                process.Kill();
                process.WaitForExit();
            }

            process.Dispose();
        }
    }

    public static class ScriptCompiler
    {
        public static CompilerInstance CompileAsync(ScriptAssemblyType type, PlatformType platform, bool debug, string outputDir)
        {
            LibraryEntry[] scriptEntries = ProjectLibrary.Search("*", new ResourceType[] { ResourceType.ScriptCode });

            List<string> scriptFiles = new List<string>();
            for (int i = 0; i < scriptEntries.Length; i++)
            {
                if(scriptEntries[i].Type != LibraryEntryType.File)
                    continue;

                FileEntry fileEntry = (FileEntry)scriptEntries[i];

                ScriptCodeImportOptions io = (ScriptCodeImportOptions) fileEntry.Options;
                if (io.EditorScript && type == ScriptAssemblyType.Editor ||
                    !io.EditorScript && type == ScriptAssemblyType.Game)
                {
                    scriptFiles.Add(scriptEntries[i].Path);
                }
            }

            string[] assemblyFolders;
            string[] assemblies;
            string outputFile;

            string[] frameworkAssemblies = BuildManager.GetFrameworkAssemblies(platform);
            if (type == ScriptAssemblyType.Game)
            {
                assemblyFolders = new string[]
                {
                    EditorApplication.BuiltinAssemblyPath, 
                    EditorApplication.FrameworkAssemblyPath
                };

                assemblies = new string[frameworkAssemblies.Length + 1];
                assemblies[assemblies.Length - 1] = EditorApplication.EngineAssembly;

                outputFile = Path.Combine(outputDir, EditorApplication.ScriptGameAssembly);
            }
            else
            {
                assemblyFolders = new string[]
                {
                    EditorApplication.BuiltinAssemblyPath, 
                    EditorApplication.FrameworkAssemblyPath,
                    EditorApplication.ScriptAssemblyPath
                };

                assemblies = new string[frameworkAssemblies.Length + 3];
                assemblies[assemblies.Length - 1] = EditorApplication.EngineAssembly;
                assemblies[assemblies.Length - 2] = EditorApplication.EditorAssembly;
                assemblies[assemblies.Length - 3] = EditorApplication.ScriptGameAssembly;

                outputFile = Path.Combine(outputDir, EditorApplication.ScriptEditorAssembly);
            }

            Array.Copy(frameworkAssemblies, assemblies, frameworkAssemblies.Length);

            string defines = BuildManager.GetDefines(platform);
            return new CompilerInstance(scriptFiles.ToArray(), defines, assemblyFolders, assemblies, debug, outputFile);
        }
    }

    public enum CompilerMessageType
    {
        Warning, Error
    }

    public struct CompilerMessage
    {
        public CompilerMessageType type;
        public string message;
        public string file;
        public int line;
        public int column;
    }
}
