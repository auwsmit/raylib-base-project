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

// Aliases (just a personal preference)
#define INPUT_FACE_TOP      GAMEPAD_BUTTON_RIGHT_FACE_UP
#define INPUT_FACE_BOTTOM   GAMEPAD_BUTTON_RIGHT_FACE_DOWN
#define INPUT_FACE_RIGHT    GAMEPAD_BUTTON_RIGHT_FACE_RIGHT
#define INPUT_FACE_LEFT     GAMEPAD_BUTTON_RIGHT_FACE_LEFT
#define INPUT_BUTTON_START  GAMEPAD_BUTTON_MIDDLE_RIGHT
#define INPUT_BUTTON_SELECT GAMEPAD_BUTTON_MIDDLE_LEFT
#define INPUT_BUTTON_HOME   GAMEPAD_BUTTON_MIDDLE
#define INPUT_BUTTON_L1     GAMEPAD_BUTTON_LEFT_TRIGGER_1
#define INPUT_BUTTON_L2     GAMEPAD_BUTTON_LEFT_TRIGGER_2
#define INPUT_BUTTON_R1     GAMEPAD_BUTTON_RIGHT_TRIGGER_1
#define INPUT_BUTTON_R2     GAMEPAD_BUTTON_RIGHT_TRIGGER_2
#define INPUT_DPAD_UP       GAMEPAD_BUTTON_LEFT_FACE_UP
#define INPUT_DPAD_DOWN     GAMEPAD_BUTTON_LEFT_FACE_DOWN
#define INPUT_DPAD_LEFT     GAMEPAD_BUTTON_LEFT_FACE_LEFT
#define INPUT_DPAD_RIGHT    GAMEPAD_BUTTON_LEFT_FACE_RIGHT

// Types and Structures
// ----------------------------------------------------------------------------
typedef enum InputAction {
    // global
    INPUT_ACTION_FULLSCREEN,
    INPUT_ACTION_DEBUG,

    // menu
    INPUT_ACTION_CONFIRM,
    INPUT_ACTION_CANCEL,
    INPUT_ACTION_MENU_UP,
    INPUT_ACTION_MENU_DOWN,

    // in-game
    INPUT_ACTION_PAUSE,

    // player
    INPUT_ACTION_LEFT,
    INPUT_ACTION_RIGHT,
    INPUT_ACTION_THRUST,
    INPUT_ACTION_SHOOT,
} InputAction;

typedef struct InputActionsState {
    // global
    bool fullscreen;
    bool debug;

    // menu
    bool confirm;
    bool cancel;
    bool moveUp;
    bool moveDown;

    // in-game
    bool pause;
} InputActionsState;

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

typedef struct InputGamepadState {
    float leftStickDeadzone;
    float rightStickDeadzone;
    float leftTriggerDeadzone;
    float rightTriggerDeadzone;
    float leftStickX;
    float leftStickY;
    float rightStickX;
    float rightStickY;
    float leftTrigger;
    float rightTrigger;
    bool available;
} InputGamepadState;

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
    // tracks input actions for current frame
    InputActionsState actions;
    InputActionsPlayer player;

    // generic input info
    InputMouseState mouse;
    InputGamepadState gamepad;
    TouchPoint touchPoints[INPUT_MAX_TOUCH_POINTS];

    // mappings for input actions
    KeyboardKey keyMaps[INPUT_ACTIONS_COUNT][INPUT_MAX_MAPS];
    MouseButton mouseMaps[INPUT_ACTIONS_COUNT][4];
    GamepadButton gamepadMaps[INPUT_ACTIONS_COUNT][INPUT_MAX_MAPS];
    // GamepadButton gamepadAxisMaps[INPUT_ACTIONS_COUNT][2];

    bool touchButtonDown[INPUT_ACTIONS_COUNT]; // for touch screen input buttons
    bool touchButtonPressed[INPUT_ACTIONS_COUNT];
    int gamepadId;
    int touchCount;
    int gamepadButtonPressed;
    bool anyGamepadButtonPressed;
    bool anyKeyPressed;
    bool anyInputPressed;
    bool touchMode; // enabled when touch points are detected, disabled by any non-touch input
} InputState;

extern InputState input;

// Prototypes
// ----------------------------------------------------------------------------

// Primary
void InitDefaultInputSettings(void); // Sets the default key mapping control scheme
void ProcessUserInput(void); // Process all user inputs for the current frame
void ProcessVirtualGamepad(void); // Process touch screen input buttons
void CancelUserInput(void); // Cancel all user inputs for the current frame

// Input Actions
bool IsInputKeyModifier(KeyboardKey key);
bool IsInputActionPressed(InputAction action);
bool IsInputActionMousePressed(InputAction action);
bool IsInputActionDown(InputAction action);
bool IsInputActionMouseDown(InputAction action);

// Touch / Virtual Input
void SetTouchInputAction(InputAction action, bool isButtonPressed);
void SetTouchPointButton(int index, int buttonIdx); // Set a touch point's current button id (currently used for touch screen analog stick, which probably needs a redesign/rewrite)
bool IsTouchPointTapped(int index); // Check if a touch point was tapped (works like IsKeyPressed)
bool IsTouchingButton(int index, int buttonId); // Check if a touch point is pressing a specific button
bool IsTouchingAnyButton(int index); // Check if a touch point is pressing any button
int CheckCollisionTouchCircle(Vector2 center, float radius); // Check if any touch points are within a circle, returns index to touch point or -1
int CheckCollisionTouchRec(Rectangle rec); // Check if any touch points are within a rectangle, returns index to touch point or -1

#endif // ASTEROIDS_INPUT_HEADER_GUARD
