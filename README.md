# Flying Toasters

Classic [After Dark](https://en.wikipedia.org/wiki/After_Dark_(software)) screensaver recreation for XScreensaver.

![image](https://user-images.githubusercontent.com/1062217/231195791-1b5be6d7-5461-4243-8199-2a7dc88458d4.png)

## Installation

1. Install XScreensaver. Check your distribution instructions on how to do it.
2. Download [flying-toasters](https://github.com/torunar/flying-toasters-xscreensaver/releases/latest) anywhere on your computer, for example `/usr/local/bin/flying-toasters`.
3. Add path to `flying-toasters` executable to the `programs` section in `~/.xscreensaver`. It should look like this:
    ```
    programs:                                                                     \
                                    maze -root                                  \n\
    - GL:                           superquadrics -root                         \n\
    ...
    - GL:                           sphereeversion -root                        \n\
                                    /usr/local/bin/flying-toasters              \n\
    ```
4. Launch XScreensaver and select "Flying-toasters" as your screensaver. It has no settings.

## Building locally

1. Install `gcc` and `libx11-dev` and `libxpm-dev` libraries.
2. Run `make build` from the source code directory to build from source.
3. Built screensaver will be put into the `bin` directory. Type `make run` to preview it in windowed mode.
