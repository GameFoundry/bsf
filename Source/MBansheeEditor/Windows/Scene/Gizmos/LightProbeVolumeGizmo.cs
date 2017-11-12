//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2017 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Collections.Generic;
using BansheeEngine;
using BansheeEditor;

namespace BansheeEditor
{
    /** @addtogroup Scene-Editor
     *  @{
     */

    /// <summary>
    /// Render individual nodes of a light probe volume and allows the user to select and move them.
    /// </summary>
    [CustomHandle(typeof(LightProbeVolume))]
    internal class LightProbeVolumeNodeHandles : Handle
    {
        /// <summary>
        /// Returns the handle of the currently selected node in the light probe volume. Is set to uint.MaxValue if no
        /// node is selected.
        /// </summary>
        internal static uint SelectedNode
        {
            get { return selectedNode; }
        }

        private LightProbeVolume volume;
        private List<HandleSliderSphere> nodeColliders = new List<HandleSliderSphere>();
        private LightProbeInfo[] probeInfos;

        private MoveHandle moveHandle;
        private static uint selectedNode = uint.MaxValue;

        /// <summary>
        /// Creates a new instance of the object.
        /// </summary>
        public LightProbeVolumeNodeHandles(LightProbeVolume volume)
        {
            this.volume = volume;
            selectedNode = uint.MaxValue;
        }

        /// <inheritdoc/>
        protected internal override void PreInput()
        {
            Matrix4 transform = volume.SceneObject.WorldTransform;
            probeInfos = volume.GetProbes();
            for (int i = 0; i < probeInfos.Length; i++)
            {
                if(i == nodeColliders.Count)
                    nodeColliders.Add(new HandleSliderSphere(this, 1.0f, false));

                Vector3 position = probeInfos[i].position;
                position = transform.MultiplyAffine(position);

                nodeColliders[i].Position = position;
            }

            while (nodeColliders.Count > probeInfos.Length)
            {
                nodeColliders[nodeColliders.Count - 1].Destroy();
                nodeColliders.RemoveAt(nodeColliders.Count - 1);
            }

            if (selectedNode != uint.MaxValue)
            {
                if(moveHandle == null)
                    moveHandle = new MoveHandle();

                Vector3 position = Vector3.Zero;
                for (int i = 0; i < probeInfos.Length; i++)
                {
                    if (probeInfos[i].handle == selectedNode)
                    {
                        position = transform.MultiplyAffine(probeInfos[i].position);
                        break;
                    }
                }

                moveHandle.Position = position;
                moveHandle.PreInput();
            }
        }

        /// <inheritdoc/>
        protected internal override void PostInput()
        {
            for(int i = 0; i < nodeColliders.Count; i++)
            {
                if (nodeColliders[i].State == HandleSlider.StateType.Active)
                {
                    selectedNode = probeInfos[i].handle;
                    break;
                }
            }

            if (selectedNode != uint.MaxValue && moveHandle != null)
            {
                moveHandle.PostInput();

                if (moveHandle.IsDragged())
                    volume.SetProbePosition(selectedNode, moveHandle.Position + moveHandle.Delta);
            }
        }

        /// <inheritdoc/>
        protected internal override void Draw()
        {
            HandleDrawing.Transform = Matrix4.TRS(volume.SceneObject.Position, volume.SceneObject.Rotation, Vector3.One);
            HandleDrawing.Color = Color.White;

            foreach (var entry in probeInfos)
                HandleDrawing.DrawSphere(entry.position, 1.0f);
        }
    }

    /** @} */
}
