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
#define INPUT_ANALOG_MENU_DEADZONE 0.5f // Deadzone used for analog stick menu movement
#define INPUT_TRIGGER_BUTTON_DEADZONE 0.25f // Deadzone used when trigger is used as a button

// These are needed because MOUSE_LEFT_BUTTON is 0, which is the default non-mapped value
#define INPUT_MOUSE_LEFT_BUTTON 7
// Same as above, but for GAMEPAD_AXIS_LEFT_X
#define INPUT_GAMEPAD_AXIS_LEFT_X 6

// Aliases (just a personal preference)
#define GAMEPAD_BUTTON_NORTH  GAMEPAD_BUTTON_RIGHT_FACE_UP
#define GAMEPAD_BUTTON_SOUTH  GAMEPAD_BUTTON_RIGHT_FACE_DOWN
#define GAMEPAD_BUTTON_EAST   GAMEPAD_BUTTON_RIGHT_FACE_RIGHT
#define GAMEPAD_BUTTON_WEST   GAMEPAD_BUTTON_RIGHT_FACE_LEFT
#define GAMEPAD_BUTTON_START  GAMEPAD_BUTTON_MIDDLE_RIGHT
#define GAMEPAD_BUTTON_SELECT GAMEPAD_BUTTON_MIDDLE_LEFT
#define GAMEPAD_BUTTON_HOME   GAMEPAD_BUTTON_MIDDLE
#define GAMEPAD_BUTTON_L1     GAMEPAD_BUTTON_LEFT_TRIGGER_1
#define GAMEPAD_BUTTON_L2     GAMEPAD_BUTTON_LEFT_TRIGGER_2
#define GAMEPAD_BUTTON_R1     GAMEPAD_BUTTON_RIGHT_TRIGGER_1
#define GAMEPAD_BUTTON_R2     GAMEPAD_BUTTON_RIGHT_TRIGGER_2
#define GAMEPAD_DPAD_UP       GAMEPAD_BUTTON_LEFT_FACE_UP
#define GAMEPAD_DPAD_DOWN     GAMEPAD_BUTTON_LEFT_FACE_DOWN
#define GAMEPAD_DPAD_LEFT     GAMEPAD_BUTTON_LEFT_FACE_LEFT
#define GAMEPAD_DPAD_RIGHT    GAMEPAD_BUTTON_LEFT_FACE_RIGHT

// Types and Structures
// ----------------------------------------------------------------------------
typedef enum InputAction {
    // global
    INPUT_ACTION_FULLSCREEN = 1,
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

typedef struct InputActionsGlobal {
    // global
    bool fullscreen;
    bool debug;
} InputActionsGlobal;

typedef struct InputActionsMenu {
    bool confirm;
    bool cancel;
    bool moveUp;
    bool moveDown;
} InputActionsMenu;

typedef struct InputActionsPlayer {
    bool pause;
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

typedef struct GamepadAxisMap {
    GamepadAxis axis;
    float deadzone; // the threshold for axis to be 'pressed down' as a button
                    // used to map analog stick or trigger as buttons
} GamepadAxisMap;

typedef struct InputActionMaps {
    KeyboardKey key[INPUT_ACTIONS_COUNT][INPUT_MAX_MAPS];
    MouseButton mouse[INPUT_ACTIONS_COUNT][4];
    GamepadButton gamepadButton[INPUT_ACTIONS_COUNT][INPUT_MAX_MAPS];
    GamepadAxisMap gamepadAxis[INPUT_ACTIONS_COUNT];
} InputActionMaps;

typedef struct InputState {
    // tracks input actions for current frame
    InputActionsGlobal global;
    InputActionsMenu menu;
    InputActionsPlayer player;

    // generic input info
    InputMouseState mouse;
    InputGamepadState gamepad;
    TouchPoint touchPoints[INPUT_MAX_TOUCH_POINTS];

    int gamepadId;
    int gamepadButtonPressed;
    int touchCount;
    bool touchMode; // enabled when touch points are detected, disabled by any non-touch input
    bool touchButtonDown[INPUT_ACTIONS_COUNT];
    bool touchButtonPressed[INPUT_ACTIONS_COUNT];
    bool gamepadAxisPressedCurrentFrame[INPUT_ACTIONS_COUNT]; // for when an axis is mapped as a button
    bool gamepadAxisPressedPreviousFrame[INPUT_ACTIONS_COUNT];
    bool anyGamepadButtonPressed;
    bool anyKeyPressed;
    bool anyInputPressed;
} InputState;

extern InputState input;

// Prototypes
// ----------------------------------------------------------------------------

// Primary
void InitDefaultInputSettings(void); // Sets the default control settings and mappings
void ProcessUserInput(void); // Process all user inputs for the current frame
void ProcessVirtualGamepad(void); // Process touch screen input buttons
void CancelUserInput(void); // Cancel all user inputs for the current frame

// Input Actions
bool IsInputKeyModifier(KeyboardKey key);
bool IsInputActionDown(InputAction action);
bool IsInputActionAxisDown(InputAction action);
bool IsInputActionMouseDown(InputAction action);
bool IsInputActionPressed(InputAction action);
bool IsInputActionAxisPressed(InputAction action);
bool IsInputActionMousePressed(InputAction action);

// Touch / Virtual Input
void SetTouchInputAction(InputAction action, bool isButtonPressed);
void SetTouchPointButton(int index, int buttonIdx); // Set a touch point's current button id (currently used for touch screen analog stick, which probably needs a redesign/rewrite)
bool IsTouchPointTapped(int index); // Check if a touch point was tapped (works like IsKeyPressed)
bool IsTouchingButton(int index, int buttonId); // Check if a touch point is pressing a specific button
bool IsTouchingAnyButton(int index); // Check if a touch point is pressing any button
int CheckCollisionTouchCircle(Vector2 center, float radius); // Check if any touch points are within a circle, returns index to touch point or -1
int CheckCollisionTouchRec(Rectangle rec); // Check if any touch points are within a rectangle, returns index to touch point or -1

#endif // ASTEROIDS_INPUT_HEADER_GUARD
