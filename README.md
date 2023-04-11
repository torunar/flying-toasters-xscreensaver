# Flying Toasters

Classic [After Dark](https://en.wikipedia.org/wiki/After_Dark_(software)) screensaver recreation for XScreensaver.

## Installation

1. Install XScreensaver. Check your distribution instructions on how to install it.
2. Download [flying-toasters](https://github.com/torunar/flying-toasters-xscreensaver/releases/latest) anywhere on your computer, for example `/usr/local/bin/flying-toasters`.
3. Add path to `flying-toasters` executable to the `programs` section in `~/.xscreensaver`. It should look like this:
    ```
    programs:                                                                     \
                                    maze -root                                  \n\
    - GL:                           superquadrics -root                         \n\
    ...
    - GL:                           sphereeversion -root                        \n\
                                    /usr/local/bin/bin/flying-toasters          \n\
    ```
4. Launch XScreensaver and select "Flying-toasters" as your screensaver. It has no settings.

## Building and running

Run `make all` from the source code directory.