#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../img/toast.xpm"
#include "../img/toaster.xpm"
#include "vroot.h"
#include "flying-toasters.h"

#define TOASTER_SPRITE_COUNT 4
#define TOASTER_COUNT 10
#define TOAST_COUNT 6
#define SPRITE_SIZE 64

int main(int argc, char *argv[]) {
    Display *display;
    Window window;
    GC graphicContext;
    XWindowAttributes windowAttributes;
    Pixmap outputBuffer;

    struct Toaster toasters[TOASTER_COUNT];
    XImage *toasterSprites[TOASTER_SPRITE_COUNT];
    Pixmap toasterClipMasks[TOASTER_SPRITE_COUNT];

    struct Toast toasts[TOAST_COUNT];
    XImage *toastSprite;
    Pixmap toastClipMask;

    display = XOpenDisplay(getenv("DISPLAY"));
    window = createWindow(display, !(argc > 1 && strcmp(argv[1], "-windowed") == 0));
    graphicContext = XCreateGC(display, window, 0, NULL);

    XGetWindowAttributes(display, window, &windowAttributes);
    outputBuffer = XCreatePixmap(
            display,
            window,
            windowAttributes.width,
            windowAttributes.height,
            windowAttributes.depth
    );

    int spawnWidth = windowAttributes.width + windowAttributes.height;
    loadSprites(display, window, toasterSprites, toasterClipMasks, &toastSprite, &toastClipMask);
    spawnToasters(spawnWidth, toasters);
    spawnToasts(spawnWidth, toasts);

    int frameCounter = 0;
    int newX, newY, i, j;

    while (1) {
        XSetForeground(display, graphicContext, BlackPixelOfScreen(DefaultScreenOfDisplay(display)));
        XFillRectangle(display, outputBuffer, graphicContext, 0, 0, windowAttributes.width, windowAttributes.height);

        frameCounter = (frameCounter + 1) % 256;
        for (i = 0; i < TOAST_COUNT; i++) {
            if (isScrolledToScreen(toasts[i].x, toasts[i].y, windowAttributes.width)) {
                drawSprite(display, graphicContext, outputBuffer, toastSprite, toastClipMask, toasts[i].x, toasts[i].y);
            }

            newX = toasts[i].x - toasts[i].moveDistance;
            newY = toasts[i].y + toasts[i].moveDistance;
            if (isScrolledOutOfScreen(newX, newY, windowAttributes.height)) {
                setToastSpawnCoordinates(&toasts[i], spawnWidth);
                continue;
            }

            for (j = 0; j < TOAST_COUNT; j++) {
                if ((i != j) && hasSpriteCollision(toasts[j].x, toasts[j].y, newX, newY, 5)) {
                    newX = toasts[i].x - toasts[j].moveDistance;
                    newY = toasts[i].y + toasts[j].moveDistance;
                }
            }

            toasts[i].x = newX;
            toasts[i].y = newY;
        }

        for (i = 0; i < TOASTER_COUNT; i++) {
            if (isScrolledToScreen(toasters[i].x, toasters[i].y, windowAttributes.width)) {
                drawSprite(
                        display,
                        graphicContext,
                        outputBuffer,
                        toasterSprites[toasters[i].currentFrame],
                        toasterClipMasks[toasters[i].currentFrame],
                        toasters[i].x,
                        toasters[i].y
                );
            }

            newX = toasters[i].x - toasters[i].moveDistance;
            newY = toasters[i].y + toasters[i].moveDistance;
            if (isScrolledOutOfScreen(newX, newY, windowAttributes.height)) {
                setToasterSpawnCoordinates(&toasters[i], spawnWidth);
                continue;
            }

            for (j = 0; j < TOASTER_COUNT; j++) {
                if ((i != j) && hasSpriteCollision(toasters[j].x, toasters[j].y, newX, newY, 0)) {
                    if (toasters[i].x <= toasters[j].x + SPRITE_SIZE) {
                        newY = toasters[i].y + toasters[j].moveDistance;
                    } else {
                        newX = toasters[i].x - toasters[j].moveDistance;
                    }
                    break;
                }
            }

            toasters[i].x = newX;
            toasters[i].y = newY;

            if (frameCounter % (10 - toasters[i].moveDistance) == 0) {
                toasters[i].currentFrame = (toasters[i].currentFrame + 1) % TOASTER_SPRITE_COUNT;
            }
        }

        XCopyArea(
                display,
                outputBuffer,
                window,
                graphicContext,
                0,
                0,
                windowAttributes.width,
                windowAttributes.height,
                0,
                0
        );
        XFlush(display);

        usleep(1000000 / 60); // 60 fps
    }

    XCloseDisplay(display);

    return 0;
}

int hasSpriteCollision(int x1, int y1, int x2, int y2, int gap) {
    return (x1 < x2 + SPRITE_SIZE + gap) && (x2 < x1 + SPRITE_SIZE + gap) &&
           (y1 < y2 + SPRITE_SIZE + gap) && (y2 < y1 + SPRITE_SIZE + gap);
}

int isScrolledToScreen(int x, int y, int screenWidth) {
    return (y + SPRITE_SIZE > 0) && (x + SPRITE_SIZE > 0) && (x < screenWidth);
}

int isScrolledOutOfScreen(int x, int y, int screenHeight) {
    return (x <= -SPRITE_SIZE) || (y >= screenHeight);
}

Window createWindow(Display *display, int isInRoot) {
    Window window;
    XEvent e;

    window = DefaultRootWindow(display);
    if (!isInRoot) {
        window = XCreateSimpleWindow(
                display,
                window,
                200,
                200,
                1024,
                768,
                1,
                BlackPixel(display, DefaultScreen(display)),
                BlackPixel(display, DefaultScreen(display))
        );
        XStoreName(display, window, "flying-toasters");
        XSelectInput(display, window, StructureNotifyMask);
        XMapRaised(display, window);
        do {
            XWindowEvent(display, window, StructureNotifyMask, &e);
        } while (e.type != MapNotify);
    }

    return window;
}

void loadSprites(
        Display *display,
        Window window,
        XImage **toasterSprites,
        Pixmap *toasterClipMasks,
        XImage **toastSprite,
        Pixmap *toastClipMask
) {
    XImage *clipMask = NULL;

    for (int i = 0; i < TOASTER_SPRITE_COUNT; i++) {
        XpmCreateImageFromData(display, toasterXpm[i], &toasterSprites[i], &clipMask, NULL);
        toasterClipMasks[i] = XCreatePixmap(display, window, clipMask->width, clipMask->height, clipMask->depth);
        XPutImage(
                display,
                toasterClipMasks[i],
                XCreateGC(display, toasterClipMasks[i], 0, NULL),
                clipMask,
                0,
                0,
                0,
                0,
                clipMask->width,
                clipMask->height
        );
        clipMask = NULL;
    }

    XpmCreateImageFromData(display, toastXpm, toastSprite, &clipMask, NULL);
    *toastClipMask = XCreatePixmap(display, window, clipMask->width, clipMask->height, clipMask->depth);
    XPutImage(
            display,
            *toastClipMask,
            XCreateGC(display, *toastClipMask, 0, NULL),
            clipMask,
            0,
            0,
            0,
            0,
            clipMask->width,
            clipMask->height
    );
    clipMask = NULL;
}

void setToasterSpawnCoordinates(struct Toaster *toaster, int spawnWidth) {
    int yPosition = 5 + (int) random() % 3;
    int cellWidth = spawnWidth / TOASTER_COUNT;
    toaster->x = (toaster->i + yPosition - 1) * cellWidth + (cellWidth - SPRITE_SIZE) / 2;
    toaster->y = -SPRITE_SIZE * yPosition;
}

void setToastSpawnCoordinates(struct Toast *toast, int spawnWidth) {
    int yPosition = 1 + (int) random() % 3;
    int cellWidth = spawnWidth / TOAST_COUNT;
    toast->x = (toast->i + yPosition - 1) * cellWidth + (cellWidth - SPRITE_SIZE) / 2;
    toast->y = -SPRITE_SIZE * yPosition;
}

void spawnToasters(int spawnWidth, struct Toaster *toasters) {
    for (int i = 0; i < TOASTER_COUNT; i++) {
        toasters[i].i = i;
        toasters[i].moveDistance = 1 + (int) random() % 4;
        toasters[i].currentFrame = (int) random() % TOASTER_SPRITE_COUNT;
        setToasterSpawnCoordinates(&toasters[i], spawnWidth);
    }
}

void spawnToasts(int spawnWidth, struct Toast *toasts) {
    for (int i = 0; i < TOAST_COUNT; i++) {
        toasts[i].i = i;
        toasts[i].moveDistance = 1 + (int) random() % 3;
        setToastSpawnCoordinates(&toasts[i], spawnWidth);
    }
}

void drawSprite(
        Display *display,
        GC graphicContext,
        Pixmap outputBuffer,
        XImage *sprite,
        Pixmap clipMask,
        int x,
        int y
) {
    XSetClipMask(display, graphicContext, clipMask);
    XSetClipOrigin(display, graphicContext, x, y);
    XPutImage(
            display,
            outputBuffer,
            graphicContext,
            sprite,
            0,
            0,
            x,
            y,
            SPRITE_SIZE,
            SPRITE_SIZE
    );
    XSetClipMask(display, graphicContext, None);
}
