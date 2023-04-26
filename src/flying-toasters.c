#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "../img/toast.xpm"
#include "../img/toaster.xpm"
#include "vroot.h"
#include "flying-toasters.h"

#define TOASTER_SPRITE_COUNT 4
#define TOASTER_COUNT 10
#define TOAST_COUNT 6
#define SPRITE_SIZE 64
#define GRID_WIDTH 4
#define GRID_HEIGHT 4
#define MAX_TOASTER_SPEED 4
#define MAX_TOAST_SPEED 3
#define FPS 60

int main(int argc, char *argv[]) {
    srand(time(NULL));

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

    loadSprites(display, window, toasterSprites, toasterClipMasks, &toastSprite, &toastClipMask);

    int frameCounter = 0;
    int newX, newY, i, j;
    int *grid = initGrid();

    spawnToasters(toasters, windowAttributes.width, windowAttributes.height, grid);
    spawnToasts(toasts, windowAttributes.width, windowAttributes.height, grid);

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
                setToastSpawnCoordinates(&toasts[i], windowAttributes.width, windowAttributes.height);
                continue;
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
                setToasterSpawnCoordinates(&toasters[i], windowAttributes.width, windowAttributes.height);
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

        usleep(1000000 / FPS);
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
                1920,
                1080,
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

void setToasterSpawnCoordinates(struct Toaster *toaster, int screenWidth, int screenHeight) {
    int slotWidth = screenWidth / GRID_WIDTH;
    int slotHeight = screenHeight / GRID_HEIGHT;
    toaster->x = screenHeight + (toaster->slot % GRID_WIDTH) * slotWidth + (slotWidth - SPRITE_SIZE) / 2;
    toaster->y = -screenHeight + (toaster->slot / GRID_WIDTH) * slotHeight + (slotHeight - SPRITE_SIZE) / 2;
}

void setToastSpawnCoordinates(struct Toast *toast, int screenWidth, int screenHeight) {
    int slotWidth = screenWidth / GRID_WIDTH;
    int slotHeight = screenHeight / GRID_HEIGHT;
    toast->x = screenHeight + (toast->slot % GRID_WIDTH) * slotWidth + (slotWidth - SPRITE_SIZE) / 2;
    toast->y = -screenHeight + (toast->slot / GRID_WIDTH) * slotHeight + (slotHeight - SPRITE_SIZE) / 2;
}

void spawnToasters(struct Toaster *toasters, int screenWidth, int screenHeight, int *grid) {
    for (int i = 0; i < TOASTER_COUNT; i++) {
        toasters[i].slot = grid[i];
        toasters[i].moveDistance = 1 + rand() % MAX_TOASTER_SPEED;
        toasters[i].currentFrame = rand() % TOASTER_SPRITE_COUNT;
        setToasterSpawnCoordinates(&toasters[i], screenWidth, screenHeight);
    }
}

void spawnToasts(struct Toast *toasts, int screenWidth, int screenHeight, int *grid) {
    for (int i = 0; i < TOAST_COUNT; i++) {
        toasts[i].slot = grid[TOASTER_COUNT + i];
        toasts[i].moveDistance = 1 + rand() % MAX_TOAST_SPEED;
        setToastSpawnCoordinates(&toasts[i], screenWidth, screenHeight);
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

int *initGrid() {
    static int grid[TOASTER_COUNT + TOAST_COUNT];
    int i, j;

    for (i = 0; i < TOASTER_COUNT + TOAST_COUNT; i++) {
        grid[i] = i;
    }

    for (i = 0; i < TOASTER_COUNT + TOAST_COUNT - 1; i++) {
        j = i + rand() % (TOASTER_COUNT + TOAST_COUNT - i);
        int t = grid[j];
        grid[j] = grid[i];
        grid[i] = t;
    }

    return grid;
}
