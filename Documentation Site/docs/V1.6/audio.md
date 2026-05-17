
## Audio (winsimple_audio.hpp)

### Class Wav
Low‑level wrapper around the Windows MCI (Media Control Interface). Supports loading, playback, volume, seeking, and status queries for audio files.

* `Wav(std::string path = "", int channel = 0, bool blocking = true)` – constructor; stores path, channel index, and blocking flag.
* `~Wav()` – closes the MCI device if it was opened.
* `bool open(std::string m_path, int m_channel, bool m_blocking = true)` – opens an audio file on a specific MCI channel; returns success.
* `void play()` – starts playback of the opened file.
* `void stop()` – pauses playback on the current channel.
* `static void stop(int m_channel)` – pauses playback on the given channel.
* `bool setVolume(int percent)` – sets playback volume (0‑100); returns success.
* `int getVolume()` – returns the current volume as a percentage.
* `bool setProgress(long seconds)` – seeks to a time position in seconds; returns success.
* `long getProgress()` – returns the current playback position in seconds.
* `long getLength()` – returns the total length of the audio file in seconds.
* `bool isFinished()` – returns `true` if playback has reached the end.
* `static int getFreeChannel()` – finds and returns an available MCI channel index (0‑99), or -1 if none free.
* `static std::string getChannelStatus(int m_channel)` – returns the status string for a channel (`"playing"`, `"stopped"`, etc.) or `"error"`.
* `static bool PlayFree(std::string m_path, int m_channel, bool m_blocking = false)` – opens and immediately plays a file on a channel without requiring prior `open()`.

### Example
```
ws::Wav::PlayFree("audio.wav",0,true);

//OR

ws::Wav wav;
wav.open("audio.wav",0,false);
wav.play();
wav.setVolume(50);

//later...
if(wav.isFinished())
	std::cout << "Finished playing the sound" << std::endl;
```