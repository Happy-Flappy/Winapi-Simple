## Animation (winsimple_animation.hpp)


<iframe width="560" height="315" src="https://youtube.com/embed/upzc98Zgysc" frameborder="0" allowfullscreen></iframe>

### Class GIF
Represents an animated GIF. Loads frames from a file, supports manual frame management, playback control, and looping.

* `GIF()` – empty constructor.
* `~GIF()` – releases all frame textures and delay data.
* `bool loadFromFile(std::string path)` – loads an animated GIF from a file and decodes every frame; returns success.
* `void addFrame(ws::Texture &newFrame, double millisecondDelay)` – appends a single frame with its display duration in milliseconds.
* `ws::Texture& getTexture()` – returns a reference to the currently displayed frame’s texture.
* `ws::Texture* getFrame(int index)` – returns a pointer to the texture at the given frame index; `nullptr` if invalid.
* `double getFrameDelay(int index)` – returns the delay in milliseconds for the specified frame.
* `bool setFrameDelay(int index, double newDelay)` – changes the delay of an existing frame; returns success.
* `bool setFrame(int index, ws::Texture &newTexture)` – replaces the texture at the given frame index; returns success.
* `bool getLoop()` – returns whether the GIF loops when playback finishes.
* `void setLoop(bool trueFalse = true)` – enables or disables looping.
* `int getFrameCount()` – returns the total number of frames.
* `int getCurrentFrame()` – returns the index of the frame currently being shown.
* `void play()` – starts or resumes playback.
* `void pause()` – pauses playback at the current frame.
* `void stop()` – stops playback and resets to the first frame.
* `std::string getStatus()` – returns current playback state: `"playing"`, `"paused"`, or `"stopped"`.
* `ws::Texture& update()` – advances the animation based on elapsed time and returns the current texture. Call each frame.
* `std::string getPath()` – returns the file path from which the GIF was loaded.

### Example
```
ws::GIF gif;
gif.loadFromFile("animation.gif");
gif.play();
gif.setLoop(true);

//in a loop - gif updates and returns the current texture.
sprite.setTexture(gif.update(),true);

```


### Shift Animation
A simple sprite‑sheet region walker.

#### struct ShiftData
* `int currentframe` – current frame index.
* `float delay` – time per frame in seconds (default 0.15f).
* `bool ended` – true after the last frame has been displayed.
* `bool start` – internal flag; set to `true` initially.
* `std::vector<ws::IntRect> rect` – sequence of texture rectangles.
* `ws::Timer timer` – internal timer.
* `void add(int left, int top, int width, int height)` – appends a rectangle region.
* `void add(ws::IntRect r)` – appends an existing `IntRect`.

#### Function Shift
* `ws::IntRect Shift(ShiftData &shift)` – advances the shift animation by one step when enough time has elapsed and returns the current rectangle. Once the last frame is reached, `shift.ended` becomes `true` and the final rectangle is returned on subsequent calls.


### Example
```

ws::ShiftData walk;
walk.delay = 0.15;
walk.add({62*0,62*0,62,62});
walk.add({62*1,62*0,62,62});
walk.add({62*2,62*0,62,62});
walk.add({62*0,62*1,62,62});
walk.add({62*1,62*2,62,62});

//in a loop - Shift automatically handles time.
sprite.setTextureRect(ws::Shift(walk));


```