#include <X11/Xlib.h>
#include <X11/xpm.h>

struct Toaster {
    int slot;
    int x;
    int y;
    int moveDistance;
    int currentFrame;
};

struct Toast {
    int slot;
    int x;
    int y;
    int moveDistance;
};

int hasSpriteCollision(int x1, int y1, int x2, int y2, int gap);

int isScrolledToScreen(int x, int y, int screenWidth);

int isScrolledOutOfScreen(int x, int y, int screenHeight);

Window createWindow(Display *display, int isInRoot);

void loadSprites(
        Display *display,
        Window window,
        XImage **toasterSprites,
        Pixmap *toasterClipMasks,
        XImage **toastSprite,
        Pixmap *toastClipMask
);

void setToasterSpawnCoordinates(struct Toaster *toaster, int screenWidth, int screenHeight);

void setToastSpawnCoordinates(struct Toast *toast, int screenWidth, int screenHeight);

void spawnToasters(struct Toaster *toasters, int screenWidth, int screenHeight, int *grid);

void spawnToasts(struct Toast *toasts, int screenWidth, int screenHeight, int *grid);

void drawSprite(
        Display *display,
        GC graphicContext,
        Pixmap outputBuffer,
        XImage *sprite,
        Pixmap clipMask,
        int x,
        int y
);

int *initGrid();
