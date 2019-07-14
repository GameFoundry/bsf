---
title: Playing audio
---

To play an audio clip create an @bs::CAudioSource component.

~~~~~~~~~~~~~{.cpp}
HSceneObject audioSourceSO = SceneObject::create("Audio source");
HAudioSource audioSource = audioSourceSO->addComponent<CAudioSource>();
~~~~~~~~~~~~~

Each audio source can have a single **AudioClip** associated with it. We can attach the clip to the source by calling @bs::CAudioSource.

~~~~~~~~~~~~~{.cpp}
HAudioClip clip = gImporter().import<AudioClip>("myAudioClip.ogg");

audioSource->setClip(clip);
~~~~~~~~~~~~~

Once clip has been assigned you can control playback through these methods:
 - @bs::CAudioSource::play - Starts playing the assigned audio clip. If playback is currently stopped it starts playback from the clip beginning. If paused the playback is resumed from the point it was paused.
 - @bs::CAudioSource::stop - Stops playing the assigned audio clip.
 - @bs::CAudioSource::pause - Pauses playing the assigned audio clip, allowing you to later resume it with a call to **CAudioSource::play()**.
 
~~~~~~~~~~~~~{.cpp}
audioSource->play();
~~~~~~~~~~~~~

You can also control playback by enabling/disabling the scene object the component is attached to. Disabling the scene object will stop any playback.

~~~~~~~~~~~~~{.cpp}
// Stops playback on any audio source components attached to the scene object
audioSourceSO->setActive(false);
~~~~~~~~~~~~~

When enabling a scene object, you can tell the audio source component to automatically start playback of its assigned clip by enabling the option through @bs::CAudioSource::setPlayOnStart.

~~~~~~~~~~~~~{.cpp}
// When scene object this component is attached to, is enabled, playback will start automatically. Otherwise the user is required to call play() manually.
audioSource->setPlayOnStart(true);

// Starts playback
audioSourceSO->setActive(true);
~~~~~~~~~~~~~

This is all you need to do in order to play basic audio clips. But lets investigate a few more options that let you control the audio in more detail.

# Volume
Volume of the audio source can be controlled through @bs::CAudioSource::setVolume. The provided value ranges from 0 to 1.

~~~~~~~~~~~~~{.cpp}
// Set at half volume
audioSource->setVolume(0.5f);
~~~~~~~~~~~~~

# Pitch
Pitch can be controlled through @bs::CAudioSource::setPitch by providing a pitch multiplier. Values larger than 1 yield a higher pitch, while values smaller than 1 yield a lower pitch.

~~~~~~~~~~~~~{.cpp}
// Increase pitch 100%
audioSource->setPitch(2.0f);

// Decrease pitch by 50% (from default)
audioSource->setPitch(0.5f);
~~~~~~~~~~~~~

# Seeking
You can seek to a specific position within the currently assigned audio clip by calling @bs::CAudioSource::setTime. It accepts a time in seconds. If clip is currently playing, the playback will skip to the provided time. If clip is currently paused, the clip will resume from the provided time the next time **CAudioSource::play()** is called.

~~~~~~~~~~~~~{.cpp}
// Seek to 30 seconds
audioSource->setTime(30.0f);
~~~~~~~~~~~~~

# Loop
By default when the playback reaches the end of the current audio clip, the playback will end. You can ensure the playback loops instead by calling @bs::CAudioSource::setIsLooping.

~~~~~~~~~~~~~{.cpp}
// When playback reaches the end, loop back to start
audioSource->setIsLooping(true);
~~~~~~~~~~~~~

# 3D sounds
If an **AudioClip** has been marked as 3D sound (as described in the previous chapter), then sound playback will be influenced by the position and/or velocity of the scene object the **AudioSource** component is attached to. This means such sounds will sound differently depending on their distance from the listener (among other properties). This ensures the sounds feel realistic as the player walks around the level (i.e. sounding quieter when far away, or using surround to project the sound behind the player).

3D sounds only work if there is a listener defined in the scene. 

## Listener
Listener provides a reference point used for 3D sound effects. It is represented with a @bs::CAudioListener component. It requires no additional properties aside from being present in the scene.

~~~~~~~~~~~~~{.cpp}
HSceneObject audioListenerSO = SceneObject::create("Audio listener");
audioListenerSO->addComponent<CAudioListener>();
~~~~~~~~~~~~~

Normally you want to attach this component to a scene object representing your player, or the player's camera.

## Attenuation
Attenuation determines how quickly does audio volume drop off as the listener moves further from the audio source. Change it through @bs::CAudioSource::setAttenuation. Higher values means the sound attenuates more quickly with distance.

You can further control attenuation by setting @bs::CAudioSource::setMinDistance. If the listener is closer to the source than minimum distance, then the audio will play at full volume. This ensures that the effect of attenuation can be avoided when the listener is close to the source.

~~~~~~~~~~~~~{.cpp}
// Normal attenuation
audioSource->setAttenuation(1.0f);

// Attenuation starts at distance of 2 units
setAttenuation->setMinDistance(2.0f);
~~~~~~~~~~~~~

# Global controls
Audio options can also be controlled globally though the @bs::Audio system, accessible though @bs::gAudio. 

You can change the audio volume globally by calling @bs::Audio::setVolume.
~~~~~~~~~~~~~{.cpp}
// Mute all sounds
gAudio().setVolume(0.0f);
~~~~~~~~~~~~~

You can also pause/unpause all sounds globally by calling @bs::Audio::setPaused.
~~~~~~~~~~~~~{.cpp}
// Pause all sounds
gAudio().setPaused(true);
~~~~~~~~~~~~~

# Direct clip playback
Sometimes you do not need all the features offered by **AudioSource** component, and just want to play a sound with no extra options. In that case you can call @bs::Audio::play and provide it with an **AudioClip** to play. The clip will play at the provided volume & position. It will stop once it ends (no looping), and you won't have any control over its playback once it starts. This is useful for short one-shot effects.

~~~~~~~~~~~~~{.cpp}
gAudio().play(clip);
~~~~~~~~~~~~~

# Device enumeration & switching
If user has multiple audio devices, you can use the **Audio** system to enumerate through them and switch which device you wish to output to.

To retrieve a list of all devices call @bs::Audio::getAllDevices. This will return a set of @bs::AudioDevice objects, which contain unique names for all the output devices.

To switch the active device call @bs::Audio::setActiveDevice.

To retrieve the default audio device (the one user selected in the operating system), call @bs::Audio::getDefaultDevice.

~~~~~~~~~~~~~{.cpp}
// Enumerate devices and choose the second one available if present
Vector<AudioDevice> devices = gAudio().getAllDevices();
if(devices.size() > 1)
	gAudio().setActiveDevice(devices[1]);
~~~~~~~~~~~~~
