// EXPLANATION:
// Helps handle game input

#ifndef ASTEROIDS_INPUT_HEADER_GUARD
#define ASTEROIDS_INPUT_HEADER_GUARD

#include "raylib.h"

// Macros
// ----------------------------------------------------------------------------
#define INPUT_ACTIONS_COUNT 16 // Maximum number of game actions, e.g. confirm, pause, move up
#define INPUT_MAX_MAPS 24 // Maximum number of inputs that can be mapped to an action
#define INPUT_MAX_TOUCH_POINTS 8

// These are needed because MOUSE_LEFT_BUTTON is 0, which is the default null mapping value
#define INPUT_MOUSE_LEFT_BUTTON 7
#define INPUT_MOUSE_NULL 8

// Types and Structures
// ----------------------------------------------------------------------------
typedef enum InputAction {
    INPUT_ACTION_FULLSCREEN,
    INPUT_ACTION_CONFIRM,
    INPUT_ACTION_CANCEL,
    INPUT_ACTION_MENU_UP,
    INPUT_ACTION_MENU_DOWN,
    INPUT_ACTION_PAUSE,

    INPUT_ACTION_LEFT,
    INPUT_ACTION_RIGHT,
    INPUT_ACTION_THRUST,
    INPUT_ACTION_SHOOT,
} InputAction;

typedef struct InputActionsGlobal {
    bool fullscreen;
    bool confirm;
    bool cancel;
    bool moveUp;
    bool moveDown;
    bool pause;
} InputActionsGlobal;

typedef struct InputActionsPlayer {
    bool rotateLeft;
    bool rotateRight;
    bool thrust;
    bool shoot;
    bool thrustMouse;
    bool shootMouse;
} InputActionsPlayer;

typedef struct InputMouseState {
    Vector2 position;
    Vector2 delta;
    bool moved;
    bool tapped; // works for mouse click or touch point
    bool leftPressed;
    bool leftDown;
    bool rightPressed;
    bool rightDown;
} InputMouseState;

typedef struct TouchPoint {
    Vector2 position;
    bool isActive;
    bool pressedPreviousFrame;
    bool pressedCurrentFrame;
    int id;
    int currentButton; // TODO this should probably just be a bool
                       // ... actually the touch screen input UI stuff just needs a rewrite
} TouchPoint;

typedef struct InputState {
    InputActionsGlobal actions; // keeps track of user input for current frame
    InputActionsPlayer player;
    InputMouseState mouse;
    KeyboardKey keyMaps[INPUT_ACTIONS_COUNT][INPUT_MAX_MAPS]; // keyboard mappings for input actions
    MouseButton mouseMaps[INPUT_ACTIONS_COUNT][INPUT_MAX_MAPS]; // mouse mappings
    // Gesture gestureMaps[INPUT_ACTIONS_COUNT][INPUT_MAX_MAPS]; // gesture mappings
    // GamepadButton gamepadButtonMaps[INPUT_ACTIONS_COUNT][INPUT_MAX_MAPS];

    bool touchButtonPressed[INPUT_ACTIONS_COUNT]; // tracks touch screen input buttons
    TouchPoint touchPoints[INPUT_MAX_TOUCH_POINTS];
    int touchCount;
    bool touchMode; // enabled when touch points are detected, disabled by any non-touch input
    bool anyKeyPressed;
} InputState;

extern InputState input;

// Prototypes
// ----------------------------------------------------------------------------

// Per-Frame
void ProcessUserInput(void); // Process all user inputs for the current frame
void ProcessVirtualGamepad(void); // Process touch screen input buttons
void CancelUserInput(void); // Cancel all user inputs for the current frame

// Input Actions
void InitDefaultInputControls(void); // Sets the default key mapping control scheme
bool IsInputKeyModifier(KeyboardKey key);
bool IsInputActionPressed(InputAction action);
bool IsInputActionMousePressed(InputAction action);
bool IsInputActionDown(InputAction action);
bool IsInputActionMouseDown(InputAction action);

// Touch / Virtual Input
void SetTouchInput(InputAction action, bool isButtonPressed);
void SetTouchPointButton(int index, int buttonIdx); // Set a touch point's current button id (currently used for touch screen analog stick, which probably needs a redesign/rewrite)
bool IsTouchPointTapped(int index); // Check if a touch point was tapped (works like IsKeyPressed)
bool IsTouchingButton(int index, int buttonId); // Check if a touch point is pressing a specific button
bool IsTouchingAnyButton(int index); // Check if a touch point is pressing any button
int CheckCollisionTouchCircle(Vector2 center, float radius); // Check if any touch points are within a circle, returns index to touch point or -1
int CheckCollisionTouchRec(Rectangle rec); // Check if any touch points are within a rectangle, returns index to touch point or -1

// Gamepad
// int CheckAvailableGamepads(void);

#endif // ASTEROIDS_INPUT_HEADER_GUARD
