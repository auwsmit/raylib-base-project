// EXPLANATION:
// Helps handle game input

#include "input.h"
#include "raymath.h"
#include "config.h"
#include "ui.h"
#include "game.h"

InputActionMaps inputMaps;

void InitDefaultInputSettings(void)
{
    InputState defaults = {
        // Gamepad settings
        .gamepad.leftStickDeadzone = 0.25f,
        .gamepad.rightStickDeadzone = 0.25f,
        .gamepad.leftTriggerDeadzone = -0.9f,
        .gamepad.rightTriggerDeadzone = -0.9f,
    };

    // Set touch point button ids
    for (unsigned int i = 0; i < INPUT_MAX_TOUCH_POINTS; i++)
        defaults.touchPoints[i].currentButton = -1;

    // Setup input action control mapping
    inputMaps = (InputActionMaps){
        // Global controls
        .gamepadButton[INPUT_ACTION_FULLSCREEN] = { GAMEPAD_BUTTON_SELECT },
        .key[INPUT_ACTION_FULLSCREEN] = {
            KEY_LEFT_ALT, KEY_ENTER,
            KEY_RIGHT_ALT, KEY_ENTER,
            KEY_LEFT_SHIFT, KEY_F,
            KEY_RIGHT_SHIFT, KEY_F,
            KEY_F11,
        },
        .key[INPUT_ACTION_DEBUG] = { KEY_F3 },

        // Menu controls
        .gamepadButton[INPUT_ACTION_CONFIRM] =   { GAMEPAD_BUTTON_SOUTH },
        .gamepadButton[INPUT_ACTION_CANCEL] =    { GAMEPAD_BUTTON_EAST },
        .gamepadButton[INPUT_ACTION_MENU_UP] =   { GAMEPAD_DPAD_UP },
        .gamepadButton[INPUT_ACTION_MENU_DOWN] = { GAMEPAD_DPAD_DOWN },
        .gamepadAxis[INPUT_ACTION_MENU_UP] =   { GAMEPAD_AXIS_LEFT_Y, -INPUT_ANALOG_MENU_DEADZONE },
        .gamepadAxis[INPUT_ACTION_MENU_DOWN] = { GAMEPAD_AXIS_LEFT_Y, INPUT_ANALOG_MENU_DEADZONE },
        .key[INPUT_ACTION_CONFIRM] =   { KEY_ENTER, KEY_SPACE },
        .key[INPUT_ACTION_CANCEL] =    { KEY_ESCAPE, KEY_BACKSPACE, },
        .key[INPUT_ACTION_MENU_UP] =   { KEY_W, KEY_UP },
        .key[INPUT_ACTION_MENU_DOWN] = { KEY_S, KEY_DOWN },

        // Player 1 controls
        .gamepadButton[INPUT_ACTION_PAUSE] = { GAMEPAD_BUTTON_START },
        .gamepadButton[INPUT_ACTION_LEFT] =   { GAMEPAD_DPAD_LEFT },
        .gamepadButton[INPUT_ACTION_RIGHT] =  { GAMEPAD_DPAD_RIGHT },
        .gamepadButton[INPUT_ACTION_THRUST] = { GAMEPAD_BUTTON_SOUTH, GAMEPAD_BUTTON_L1 },
        .gamepadButton[INPUT_ACTION_SHOOT] =  { GAMEPAD_BUTTON_WEST, GAMEPAD_BUTTON_R1 },
        .gamepadAxis[INPUT_ACTION_THRUST] =   { GAMEPAD_AXIS_LEFT_TRIGGER, INPUT_TRIGGER_BUTTON_DEADZONE },
        .gamepadAxis[INPUT_ACTION_SHOOT] =    { GAMEPAD_AXIS_RIGHT_TRIGGER, INPUT_TRIGGER_BUTTON_DEADZONE },
        .key[INPUT_ACTION_PAUSE] =           { KEY_P },
        .key[INPUT_ACTION_LEFT] =   { KEY_A, KEY_LEFT, },
        .key[INPUT_ACTION_RIGHT] =  { KEY_D, KEY_RIGHT, },
        .key[INPUT_ACTION_THRUST] = { KEY_W, KEY_UP, },
        .key[INPUT_ACTION_SHOOT] =  { KEY_SPACE },
        .mouse[INPUT_ACTION_THRUST] = { MOUSE_RIGHT_BUTTON },
        .mouse[INPUT_ACTION_SHOOT] =  { INPUT_MOUSE_LEFT_BUTTON },
    };

    input = defaults;
}

void ProcessUserInput(void)
{
    input.anyKeyPressed = (GetKeyPressed() != 0);

    // Check input mappings
    input.global.fullscreen =  IsInputActionPressed(INPUT_ACTION_FULLSCREEN);
    input.global.debug =       IsInputActionPressed(INPUT_ACTION_DEBUG);
    if (ui.currentMenu != UI_MENU_NONE)
    {
        input.menu.confirm =     IsInputActionPressed(INPUT_ACTION_CONFIRM);
        input.menu.cancel =      IsInputActionPressed(INPUT_ACTION_CANCEL);
        input.menu.moveUp =      IsInputActionDown(INPUT_ACTION_MENU_UP);
        input.menu.moveDown =    IsInputActionDown(INPUT_ACTION_MENU_DOWN);
    }
    if (game.currentScreen == SCREEN_GAMEPLAY)
    {
        input.player.pause =       IsInputActionPressed(INPUT_ACTION_PAUSE);
        input.player.rotateLeft =  IsInputActionDown(INPUT_ACTION_LEFT);
        input.player.rotateRight = IsInputActionDown(INPUT_ACTION_RIGHT);
        input.player.thrust =      IsInputActionDown(INPUT_ACTION_THRUST);
        input.player.shoot =       IsInputActionDown(INPUT_ACTION_SHOOT);
        input.player.thrustMouse = IsInputActionMouseDown(INPUT_ACTION_THRUST);
        input.player.shootMouse =  IsInputActionMouseDown(INPUT_ACTION_SHOOT);
    }

    // Update mouse
    input.mouse.position =     GetScreenToWorld2D(GetMousePosition(), game.camera);
    input.mouse.delta =        GetMouseDelta();
    input.mouse.moved =        (Vector2Length(input.mouse.delta) != 0);
    input.mouse.tapped =       IsGestureDetected(GESTURE_TAP);
    input.mouse.leftPressed =  IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    input.mouse.leftDown =     IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    input.mouse.rightPressed = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
    input.mouse.rightDown =    IsMouseButtonDown(MOUSE_BUTTON_RIGHT);

    // Detect gamepad
    input.gamepad.available = IsGamepadAvailable(input.gamepadId);
    if (input.gamepad.available)
    {
        input.gamepadButtonPressed = GetGamepadButtonPressed();
        input.anyGamepadButtonPressed = IsGamepadButtonPressed(input.gamepadId, input.gamepadButtonPressed);
        input.gamepad.leftStickX = GetGamepadAxisMovement(input.gamepadId, GAMEPAD_AXIS_LEFT_X);
        input.gamepad.leftStickY = GetGamepadAxisMovement(input.gamepadId, GAMEPAD_AXIS_LEFT_Y);
        input.gamepad.rightStickX = GetGamepadAxisMovement(input.gamepadId, GAMEPAD_AXIS_RIGHT_X);
        input.gamepad.rightStickY = GetGamepadAxisMovement(input.gamepadId, GAMEPAD_AXIS_RIGHT_Y);
        input.gamepad.leftTrigger = GetGamepadAxisMovement(input.gamepadId, GAMEPAD_AXIS_LEFT_TRIGGER);
        input.gamepad.rightTrigger = GetGamepadAxisMovement(input.gamepadId, GAMEPAD_AXIS_RIGHT_TRIGGER);

        for (int i = 0; i < INPUT_ACTIONS_COUNT; i++)
        {
            if (inputMaps.gamepadAxis[i].axis == 0)
                continue;
            input.gamepadAxisPressedPreviousFrame[i] = input.gamepadAxisPressedCurrentFrame[i];
            input.gamepadAxisPressedCurrentFrame[i] = IsInputActionAxisDown(i);
        }
    }

    input.anyInputPressed = (input.mouse.tapped || input.anyGamepadButtonPressed || input.anyKeyPressed);

    // Detect touch mode
    int tCount = GetTouchPointCount();
    input.touchCount = tCount;

    if (input.touchCount == 0)
    {
        if (input.mouse.leftDown || input.mouse.rightDown || input.anyKeyPressed)
            input.touchMode = false;
    }
    else input.touchMode = true;

    if (!input.touchMode) return;

    // Update touch points
    if (tCount > INPUT_MAX_TOUCH_POINTS)
        tCount = INPUT_MAX_TOUCH_POINTS;
    for (int i = 0; i < tCount; i++)
    {
        Vector2 touchPosition = GetScreenToWorld2D(GetTouchPosition(i), game.camera);
        TouchPoint *touchPoint = &input.touchPoints[i];
        touchPoint->position = touchPosition;
        touchPoint->pressedPreviousFrame = touchPoint->pressedCurrentFrame;
        touchPoint->pressedCurrentFrame = true;
        touchPoint->currentButton = -1;
        touchPoint->id = GetTouchPointId(i);
    }
    for (int i = tCount; i < INPUT_MAX_TOUCH_POINTS; i++)
        input.touchPoints[i].pressedCurrentFrame = false;

    // Update touch input buttons
    ProcessVirtualGamepad();
}

void ProcessVirtualGamepad(void)
{
    UpdateUiTouchInput(&ui.shoot);
    UpdateUiTouchInput(&ui.fly);
    UpdateUiAnalogStick(&ui.stick);
}

void CancelUserInput(void)
{
    input.global = (InputActionsGlobal){ 0 };
    input.menu = (InputActionsMenu){ 0 };
    input.player = (InputActionsPlayer){ 0 };
}

// Input Actions
// ----------------------------------------------------------------------------
bool IsInputKeyModifier(KeyboardKey key)
{
    if (key == KEY_LEFT_ALT || key == KEY_RIGHT_ALT ||
        key == KEY_LEFT_SHIFT || key == KEY_RIGHT_SHIFT ||
        key == KEY_LEFT_CONTROL || key == KEY_RIGHT_CONTROL)
        return true;
    return false;
}

bool IsInputActionDown(InputAction action)
{
    // Check touch screen button
    if (input.touchButtonDown[action] == true)
        return true;

    // Check controller buttons
    if (input.gamepad.available)
    {
        GamepadButton *buttons = inputMaps.gamepadButton[action];
        for (unsigned int i = 0; i < INPUT_MAX_MAPS && buttons[i] != 0; i++)
        {
            GamepadButton button = buttons[i];
            if (IsGamepadButtonDown(input.gamepadId, button))
                return true;
        }
        if (IsInputActionAxisDown(action))
            return true;
    }

    // Check potential key combinations
    KeyboardKey* keys = inputMaps.key[action];
    for (unsigned int i = 0; i < INPUT_MAX_MAPS && keys[i] != 0; i++)
    {
        KeyboardKey key = keys[i];

        // Check modifier plus next key (only 1 modifier for now)
        if (IsInputKeyModifier(key))
        {
            if ((i + 1 < INPUT_MAX_MAPS) && (keys[i + 1] != 0) &&
                (!IsInputKeyModifier(keys[i + 1])))
            {
                if (IsKeyDown(key) && IsKeyDown(keys[i + 1]))
                    return true;
                i++;
            }
            // Check just the modifier by itself
            else if (IsKeyDown(key))
                return true;
        }

        else if (IsKeyDown(key))
            return true;
    }

    // Check mouse buttons
    if (IsInputActionMouseDown(action))
        return true;

    return false;
}

bool IsInputActionAxisDown(InputAction action)
{
    // sticks and triggers
    GamepadAxis axis = inputMaps.gamepadAxis[action].axis;
    float deadzone = inputMaps.gamepadAxis[action].deadzone;
    float axisValue = 0.0f;
    if (axis == INPUT_GAMEPAD_AXIS_LEFT_X)
        axis = GAMEPAD_AXIS_LEFT_X;
    switch (axis)
    {
        case GAMEPAD_AXIS_LEFT_X: axisValue = input.gamepad.leftStickX;
                                  break;
        case GAMEPAD_AXIS_LEFT_Y: axisValue = input.gamepad.leftStickY;
                                  break;
        case GAMEPAD_AXIS_RIGHT_X: axisValue = input.gamepad.rightStickX;
                                   break;
        case GAMEPAD_AXIS_RIGHT_Y: axisValue = input.gamepad.rightStickY;
                                   break;
        case GAMEPAD_AXIS_LEFT_TRIGGER: axisValue = input.gamepad.leftTrigger;
                                        break;
        case GAMEPAD_AXIS_RIGHT_TRIGGER: axisValue = input.gamepad.rightTrigger;
                                         break;
        default: break;
    }
    bool negativeTrigger = ((deadzone < 0) && (axisValue < deadzone));
    bool positiveTrigger = ((deadzone > 0) && (axisValue > deadzone));
    if (negativeTrigger || positiveTrigger)
        return true;

    return false;
}

bool IsInputActionMouseDown(InputAction action)
{
    MouseButton* mb = inputMaps.mouse[action];
    for (unsigned int i = 0; i < INPUT_MAX_MAPS && mb[i] != 0; i++)
    {
        MouseButton button = mb[i];
        if (button == INPUT_MOUSE_LEFT_BUTTON)
            button = MOUSE_LEFT_BUTTON;
        if (IsMouseButtonDown(button))
            return true;
    }

    return false;
}

bool IsInputActionPressed(InputAction action)
{
    // Check touch screen button
    if (input.touchButtonPressed[action] == true)
        return true;

    // Check controller input
    if (input.gamepad.available)
    {
        // buttons
        GamepadButton *buttons = inputMaps.gamepadButton[action];
        for (unsigned int i = 0; i < INPUT_MAX_MAPS && buttons[i] != 0; i++)
        {
            GamepadButton button = buttons[i];
            if (IsGamepadButtonPressed(input.gamepadId, button))
                return true;
        }
        if (IsInputActionAxisPressed(action))
            return true;
    }

    // Check potential key combinations
    KeyboardKey* keys = inputMaps.key[action];
    for (unsigned int i = 0; i < INPUT_MAX_MAPS && keys[i] != 0; i++)
    {
        KeyboardKey key = keys[i];

        // Check modifier plus next key (only 1 modifier for now)
        if (IsInputKeyModifier(key))
        {
            if ((i + 1 < INPUT_MAX_MAPS) && (keys[i + 1] != 0) &&
                (!IsInputKeyModifier(keys[i + 1])))
            {
                if (IsKeyDown(key) && IsKeyPressed(keys[i + 1]))
                    return true;
                i++; // Skip the next key
            }
            // Check just the modifier by itself
            else if (IsKeyPressed(key))
                return true;
        }

        // Check a single key
        else if (IsKeyPressed(key))
            return true;
    }

    // Check mouse buttons
    if (IsInputActionMousePressed(action))
        return true;

    return false;
}

bool IsInputActionAxisPressed(InputAction action)
{
    if (!input.gamepadAxisPressedPreviousFrame[action] &&
        input.gamepadAxisPressedCurrentFrame[action])
        return true;

    return false;
}

bool IsInputActionMousePressed(InputAction action)
{
    MouseButton* mb = inputMaps.mouse[action];
    for (unsigned int i = 0; i < INPUT_MAX_MAPS && mb[i] != 0; i++)
    {
        MouseButton button = mb[i];
        if (button == INPUT_MOUSE_LEFT_BUTTON)
            button = MOUSE_LEFT_BUTTON;
        if (IsMouseButtonPressed(button))
            return true;
    }

    return false;
}

// Touch / Virtual Input
// ----------------------------------------------------------------------------
void SetTouchInputAction(InputAction action, bool isButtonDown)
{
    if (!input.touchButtonDown[action])
        input.touchButtonPressed[action] = true;
    input.touchButtonDown[action] = isButtonDown;
}

void SetTouchPointButton(int index, int buttonIdx)
{
    input.touchPoints[index].currentButton = buttonIdx;
}

bool IsTouchPointTapped(int index)
{
    return (input.touchPoints[index].pressedCurrentFrame &&
            !input.touchPoints[index].pressedPreviousFrame);
}

bool IsTouchingButton(int index, int buttonId)
{
    return input.touchPoints[index].currentButton != buttonId;
}

bool IsTouchingAnyButton(int index)
{
    return IsTouchingButton(index, -1);
}

int CheckCollisionTouchCircle(Vector2 center, float radius)
{
    for (int i = 0; i < input.touchCount; ++i)
    {
        if (CheckCollisionPointCircle(input.touchPoints[i].position, center, radius))
            return i;
    }

    return -1;
}

int CheckCollisionTouchRec(Rectangle rec)
{
    for (int i = 0; i < input.touchCount; ++i)
    {
        if (CheckCollisionPointRec(input.touchPoints[i].position, rec))
            return i;
    }

    return -1;
}
