typedef enum {
    TEXT_MODE = 0,
    GRAPHICS_MODE = 1
} DisplayMode;

typedef enum {
    LARGE_T = 0,
    MEDIUM_T = 1,
    SMALL_T = 2,
    BACKGROUND_T = 3
} SpriteTypes;

typedef enum {
    SCREEN_WIDTH = 512,
    SCREEN_HEIGHT = 288
} ScreenDimensions;

typedef enum {
    TEXT_WIDTH = 64,
    TEXT_HEIGHT = 36
} TextDimensions;

typedef enum {
    A_KEY = 0x01,
    W_KEY = 0x02,
    X_KEY = 0x04,
    D_KEY = 0x08,
    U_KEY = 0x10,
    I_KEY = 0x20,
    J_KEY = 0x40,
    K_KEY = 0x80
} ControllerKeys;