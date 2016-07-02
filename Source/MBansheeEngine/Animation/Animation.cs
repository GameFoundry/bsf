//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
    /** @addtogroup Animation
     *  @{
     */

    /// <summary>
    /// Determines how an animation clip behaves when it reaches the end.
    /// </summary>
    public enum AnimWrapMode // Note: Must match C++ enum AnimWrapMode
    {
        /// <summary>
        /// Loop around to the beginning/end when the last/first frame is reached.
        /// </summary>
        Loop,
        /// <summary>
        /// Clamp to end/beginning, keeping the last/first frame active.
        /// </summary>
	    Clamp
    }

    /// <summary>
    /// Determines what happens to other animation clips when a new clip starts playing.
    /// </summary>
    public enum AnimPlayMode // Note: Must match C++ enum AnimPlayMode
    {
        /// <summary>
        /// All other animation clips are stopped.
        /// </summary>
        StopAll,
        /// <summary>
        /// Only the clips within the current layer are stopped.
        /// </summary>
		StopLayer
    }

    /// <summary>
    /// Contains information about a currently playing animation clip.
    /// </summary>
    public struct AnimationClipState
    {
        /// <summary>
        /// Layer the clip is playing on. Multiple clips can be played simulatenously on different layers.
        /// </summary>
        public int layer;

        /// <summary>
        /// Current time the animation is playing from.
        /// </summary>
        public float time;

        /// <summary>
        /// Speed at which the animation is playing.
        /// </summary>
        public float speed;

        /// <summary>
        /// Determines how much of an influence does the clip have on the final pose.
        /// </summary>
        public float weight;

        /// <summary>
        /// Determines what happens to other animation clips when a new clip starts playing.
        /// </summary>
        public AnimWrapMode wrapMode;

        /// <summary>
        /// Initializes the state with default values.
        /// </summary>
        public void InitDefault()
        {
            speed = 1.0f;
            weight = 1.0f;
            wrapMode = AnimWrapMode.Loop;
        }
    }

    /// <summary>
    /// Handles animation playback. Takes one or multiple animation clips as input and evaluates them every animation update
    /// tick depending on set properties.The evaluated data is used by the core thread for skeletal animation, by the sim
    /// thread for updating attached scene objects and bones (if skeleton is attached), or the data is made available for
    /// manual queries in the case of generic animation.
    /// </summary>
    public class Animation : Component
    {
        private NativeAnimation _native;

        [SerializeField]
        private SerializableData serializableData = new SerializableData();

        /// <summary>
        /// Returns the non-component version of Animation that is wrapped by this component. 
        /// </summary>
        internal NativeAnimation Native
        {
            get { return _native; }
        }

        /// <summary>
        /// Determines the default clip to play as soon as the component is enabled. If more control over playing clips is
        /// needed use the <see cref="Play"/>, <see cref="Blend"/> and <see cref="CrossFade"/> methods to queue clips for
        /// playback manually, and <see cref="SetState"/> method for modify their states individually.
        /// </summary>
        public AnimationClip DefaultClip
        {
            get { return serializableData.defaultClip; }
            set
            {
                serializableData.defaultClip = value;

                if (value != null && _native != null)
                    _native.Play(value, 0, AnimPlayMode.StopLayer);
            }
        }

        /// <summary>
        /// Determines the wrap mode for all active animations. Wrap mode determines what happens when animation reaches the 
        /// first or last frame.
        /// <see cref="AnimWrapMode"/>
        /// </summary>
        public AnimWrapMode WrapMode
        {
            get { return serializableData.wrapMode; }
            set
            {
                serializableData.wrapMode = value;

                if (_native != null)
                    _native.WrapMode = value;
            }
        }

        /// <summary>
        /// Determines the speed for all animations. The default value is 1.0f. Use negative values to play-back in reverse.
        /// </summary>
        public float Speed
        {
            get { return serializableData.speed; }
            set
            {
                serializableData.speed = value;

                if (_native != null)
                    _native.Speed = value;
            }
        }

        /// <summary>
        /// Checks if any animation clips are currently playing.
        /// </summary>
        public bool IsPlaying
        {
            get
            {
                if (_native != null)
                    return _native.IsPlaying();

                return false;
            }
        }

        /// <summary>
        /// Plays the specified animation clip at the specified layer.
        /// </summary>
        /// <param name="clip">Clip to play.</param>
        /// <param name="layer">Layer to play the clip in. Multiple clips can be playing at once in separate layers.</param>
        /// <param name="playMode">Determines how are other playing animations handled when this animation starts.</param>
        public void Play(AnimationClip clip, int layer = 0, AnimPlayMode playMode = AnimPlayMode.StopLayer)
        {
            if(_native != null)
                _native.Play(clip, layer, playMode);
        }

        /// <summary>
        /// Blends the specified animation clip with other animation clips playing in the specified layer.
        /// </summary>
        /// <param name="clip">Clip to blend.</param>
        /// <param name="weight">Determines how much of an effect will the blended animation have on the final output.
        ///                      In range [0, 1]. All animation weights on the same layer will be normalized.</param>
        /// <param name="fadeLength">Applies the blend over a specified time period, increasing the weight as the time
        ///                          passes. Set to zero to blend immediately. In seconds.</param>
        /// <param name="layer">Layer to play the clip in. Multiple clips can be playing at once in separate layers.</param>
        public void Blend(AnimationClip clip, float weight = 1.0f, float fadeLength = 0.0f, int layer = 0)
        {
            if (_native != null)
                _native.Blend(clip, weight, fadeLength, layer);
        }

        /// <summary>
        /// Fades the specified animation clip in, while fading other playing animations out, over the specified time 
        /// period.
        /// </summary>
        /// <param name="clip">Clip to fade in.</param>
        /// <param name="fadeLength">Determines the time period over which the fade occurs. In seconds.</param>
        /// <param name="layer">Layer to play the clip in. Multiple clips can be playing at once in separate layers.</param>
        /// <param name="playMode">Determines should all other animations be faded out, or just the ones on the same layer.
        ///                        </param>
        public void CrossFade(AnimationClip clip, float fadeLength = 0.0f, int layer = 0, 
            AnimPlayMode playMode = AnimPlayMode.StopLayer)
        {
            if (_native != null)
                _native.CrossFade(clip, fadeLength, layer, playMode);
        }

        /// <summary>
        /// Stops playing all animations on the provided layer.
        /// </summary>
        /// <param name="layer">Layer on which to stop animations on.</param>
        public void Stop(int layer)
        {
            if (_native != null)
                _native.Stop(layer);
        }

        /// <summary>
        /// Stops playing all animations.
        /// </summary>
        public void StopAll()
        {
            if (_native != null)
                _native.StopAll();
        }

        /// <summary>
        /// Retrieves detailed information about a currently playing animation clip.
        /// </summary>
        /// <param name="clip">Clip to retrieve the information for.</param>
        /// <param name="state">Animation clip state containing the requested information. Only valid if the method returns
        ///                     true.</param>
        /// <returns>True if the state was found (animation clip is playing), false otherwise.</returns>
        public bool GetState(AnimationClip clip, out AnimationClipState state)
        {
            if (_native != null)
                return _native.GetState(clip, out state);

            state = new AnimationClipState();
            return false;
        }

        /// <summary>
        /// Changes the state of a playing animation clip. If animation clip is not currently playing the state change is
        /// ignored.
        /// </summary>
        /// <param name="clip">Clip to change the state for.</param>
        /// <param name="state">New state of the animation (e.g. changing the time for seeking).</param>
        public void SetState(AnimationClip clip, AnimationClipState state)
        {
            if (_native != null)
                _native.SetState(clip, state);
        }

        private void OnEnabled()
        {
            RestoreNative();
        }

        private void OnDisabled()
        {
            DestroyNative();
        }

        private void OnDestroy()
        {
            DestroyNative();
        }

        /// <summary>
        /// Creates the internal representation of the animation and restores the values saved by the component.
        /// </summary>
        private void RestoreNative()
        {
            if (_native != null)
                _native.Destroy();

            _native = new NativeAnimation();

            // Restore saved values after reset
            _native.WrapMode = serializableData.wrapMode;
            _native.Speed = serializableData.speed;

            if (serializableData.defaultClip != null)
                _native.Play(serializableData.defaultClip, 0, AnimPlayMode.StopLayer);

            Renderable renderable = SceneObject.GetComponent<Renderable>();
            if (renderable == null)
                return;

            NativeRenderable nativeRenderable = renderable.Native;
            if (nativeRenderable != null)
                nativeRenderable.Animation = _native;
        }

        /// <summary>
        /// Destroys the internal animation representation.
        /// </summary>
        private void DestroyNative()
        {
            Renderable renderableComponent = SceneObject.GetComponent<Renderable>();
            if (renderableComponent != null)
            {
                NativeRenderable renderable = renderableComponent.Native;

                if (renderable != null)
                    renderable.Animation = null;
            }

            if (_native != null)
            {
                _native.Destroy();
                _native = null;
            }
        }

        /// <summary>
        /// Holds all data the animation component needs to persist through serialization.
        /// </summary>
        [SerializeObject]
        private class SerializableData
        {
            public AnimationClip defaultClip;
            public AnimWrapMode wrapMode = AnimWrapMode.Loop;
            public float speed = 1.0f;
        }
    }

    /** @} */
}