#include <X11/Xlib.h>
#include <X11/xpm.h>

struct Toaster {
    int i;
    int x;
    int y;
    int moveDistance;
    int currentFrame;
};

struct Toast {
    int i;
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

void setToasterSpawnCoordinates(struct Toaster *toaster, int spawnWidth);

void setToastSpawnCoordinates(struct Toast *toast, int spawnWidth);

void spawnToasters(int spawnWidth, struct Toaster *toasters);

void spawnToasts(int spawnWidth, struct Toast *toasts);

void drawSprite(
        Display *display,
        GC graphicContext,
        Pixmap outputBuffer,
        XImage *sprite,
        Pixmap clipMask,
        int x,
        int y
);