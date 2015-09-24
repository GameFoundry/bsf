using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Displays in-game viewport in the editor.
    /// </summary>
    public class GameWindow : EditorWindow // TODO - Dummy class, unfinished
    {
        /// <summary>
        /// Opens the game window.
        /// </summary>
        [MenuItem("Windows/Game", ButtonModifier.CtrlAlt, ButtonCode.G, 6000)]
        private static void OpenGameWindow()
        {
            OpenWindow<GameWindow>();
        }

        /// <summary>
        /// Starts execution of the game in the game window.
        /// </summary>
        [MenuItem("Tools/Play", 9300)]
        [ToolbarItem("Play", ToolbarIcon.Play, "", 1800, true)]
        private static void Play()
        {
            // TODO - Not implemented
        }

        /// <summary>
        /// Pauses the execution of the game on the current frame.
        /// </summary>
        [MenuItem("Tools/Pause", 9299)]
        [ToolbarItem("Pause", ToolbarIcon.Pause, "", 1799)]
        private static void Pause()
        {
            // TODO - Not implemented
        }

        /// <summary>
        /// Moves the execution of the game by one frame forward.
        /// </summary>
        [MenuItem("Tools/Step", 9298)]
        [ToolbarItem("Step", ToolbarIcon.Step, "", 1798)]
        private static void Step()
        {
            // TODO - Not implemented
        }

        /// <inheritdoc/>
        protected override LocString GetDisplayName()
        {
            return new LocEdString("Game");
        }
    }
}
