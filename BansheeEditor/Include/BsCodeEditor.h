#pragma once

#include "BsEditorPrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	class CodeEditor;
	class CodeEditorFactory;

	/**
	 * @brief	Contains data about a reference to a 
	 *			project in an external editor solution.
	 */
	struct BS_ED_EXPORT CodeProjectReference
	{
		WString name;
		Path path;
	};

	/**
	 * @brief	Contains data about a single project
	 *			in an external editor solution.
	 */
	struct BS_ED_EXPORT CodeProjectData
	{
		WString name;
		Vector<Path> codeFiles;
		Vector<Path> nonCodeFiles;
		WString defines;
		Vector<CodeProjectReference> assemblyReferences;
		Vector<CodeProjectReference> projectReferences;
	};

	/**
	 * @brief	Contains data about an external editor solution,
	 *			including all projects contained.
	 */
	struct BS_ED_EXPORT CodeSolutionData
	{
		WString name;
		Vector<CodeProjectData> projects;
	};

	/**
	 * @brief	Handles connectivity of the editor with external code editing tools.
	 *			The system provides methods for interacting with external tools but
	 *			the exact tool used depends on the currently active setting.
	 */
	class BS_ED_EXPORT CodeEditorManager : public Module<CodeEditorManager>
	{
	public:
		CodeEditorManager();
		~CodeEditorManager();

		/**
		 * @brief	Returns a list of all available code editors for this platform.
		 */
		const Vector<CodeEditorType>& getAvailableEditors() const { return mEditors; }

		/**
		 * @brief	Changes the active code editor. All further operations on this object will
		 *			be executed using this editor. If the specified editor is not valid for this
		 *			platform, no change will be made.
		 */
		void setActive(CodeEditorType editor);

		/**
		 * @brief	Opens a code file in the active external editor. 
		 *
		 * @param	path		Path to the code file to open, can be absolute or relative to project resources folder.
		 *						The file should be part of a solution in the active editor.
		 * @param	lineNumber	Line number to focus on once the file is opened. Might not be supported by all
		 *						editors.
		 */
		void openFile(const Path& path, UINT32 lineNumber) const;

		/**
		 * @brief	Synchronizes all code files and assemblies in the active project and updates 
		 *			the project solution for the active editor. Each project can only have one solution
		 *			per editor.
		 */
		void syncSolution() const;

	private:
		/**
		 * @brief	Returns the absolute path at which the external editor solution file should be stored.
		 */
		Path getSolutionPath() const;

		CodeEditor* mActiveEditor;
		Map<CodeEditorType, CodeEditorFactory*> mFactoryPerEditor;
		Vector<CodeEditorType> mEditors;
		Vector<CodeEditorFactory*> mFactories;
	};

	/**
	 * @brief	Interface that classes interacting with external code editors needs to implement.
	 *
	 * @see		CodeEditorManager
	 */
	class BS_ED_EXPORT CodeEditor
	{
	public:
		virtual ~CodeEditor() { }

		/**
		 * @copydoc	CodeEditorManager::openFile
		 */
		virtual void openFile(const Path& solutionPath, const Path& path, UINT32 lineNumber) const = 0;

		/**
		 * @copydoc	CodeEditorManager::syncSolution
		 */
		virtual void syncSolution(const CodeSolutionData& data, const Path& outputPath) const = 0;
	};

	/**
	 * @brief	Interface for factory that creates a specific implementation(s) of a code editor.
	 *
	 * @see		CodeEditor
	 */
	class BS_ED_EXPORT CodeEditorFactory
	{
	public:
		virtual ~CodeEditorFactory() { }

		/**
		 * @brief	Returns a list of code editors supported by this factory.
		 */
		virtual const Vector<CodeEditorType>& getAvailableEditors() const = 0;

		/**
		 * @brief	Creates a specific implementation of a code editor.
		 *
		 * @param	editor	Type of editor to create. Make sure to provide a valid value
		 *					returned by "getAvailableEditors".
		 */
		virtual CodeEditor* create(CodeEditorType editor) const = 0;
	};
}