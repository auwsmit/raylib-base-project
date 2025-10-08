// EXPLANATION:
// Helps handle game input

#include "input.h"
#include "raymath.h"
#include "config.h"
#include "ui.h"
#include "game.h"

void ProcessUserInput(void)
{
    input.anyKeyPressed = (GetKeyPressed() != 0);

    // Check input mappings
    input.actions.fullscreen =  IsInputActionPressed(INPUT_ACTION_FULLSCREEN);
    input.actions.confirm =     IsInputActionPressed(INPUT_ACTION_CONFIRM);
    input.actions.cancel =      IsInputActionPressed(INPUT_ACTION_CANCEL);
    input.actions.moveUp =      IsInputActionDown(INPUT_ACTION_MENU_UP);
    input.actions.moveDown =    IsInputActionDown(INPUT_ACTION_MENU_DOWN);
    input.actions.pause =       IsInputActionPressed(INPUT_ACTION_PAUSE);

    input.player.rotateLeft =  IsInputActionDown(INPUT_ACTION_LEFT);
    input.player.rotateRight = IsInputActionDown(INPUT_ACTION_RIGHT);
    input.player.thrust =      IsInputActionDown(INPUT_ACTION_THRUST);
    input.player.shoot =       IsInputActionDown(INPUT_ACTION_SHOOT);
    input.player.thrustMouse = IsInputActionMouseDown(INPUT_ACTION_THRUST);
    input.player.shootMouse =  IsInputActionMouseDown(INPUT_ACTION_SHOOT);

    // Update mouse
    input.mouse.position =     GetScreenToWorld2D(GetMousePosition(), game.camera);
    input.mouse.delta =        GetMouseDelta();
    input.mouse.moved =        (Vector2Length(input.mouse.delta) != 0);
    input.mouse.tapped =       IsGestureDetected(GESTURE_TAP);
    input.mouse.leftPressed =  IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    input.mouse.leftDown =     IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    input.mouse.rightPressed = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
    input.mouse.rightDown =    IsMouseButtonDown(MOUSE_BUTTON_RIGHT);

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
    input.actions = (InputActionsGlobal){ 0 };
    input.player = (InputActionsPlayer){ 0 };
}

// Input Actions
// ----------------------------------------------------------------------------
void InitDefaultInputControls(void)
{
    InputState defaultControls = {
        // Global across program
        .keyMaps[INPUT_ACTION_FULLSCREEN] = {
            KEY_LEFT_ALT, KEY_ENTER,
            KEY_RIGHT_ALT, KEY_ENTER,
            KEY_LEFT_SHIFT, KEY_F,
            KEY_RIGHT_SHIFT, KEY_F,
            KEY_F11,
        },

        // Menu and Game
        .keyMaps[INPUT_ACTION_CONFIRM] =   { KEY_ENTER, KEY_SPACE },
        .keyMaps[INPUT_ACTION_CANCEL] =    { KEY_ESCAPE, KEY_BACKSPACE, },
        .keyMaps[INPUT_ACTION_MENU_UP] =   { KEY_W, KEY_UP },
        .keyMaps[INPUT_ACTION_MENU_DOWN] = { KEY_S, KEY_DOWN },
        .keyMaps[INPUT_ACTION_PAUSE] =     { KEY_P },

        // Player 1 controls
        .keyMaps[INPUT_ACTION_LEFT] =      { KEY_A, KEY_LEFT, },
        .keyMaps[INPUT_ACTION_RIGHT] =     { KEY_D, KEY_RIGHT, },
        .keyMaps[INPUT_ACTION_THRUST] =    { KEY_W, KEY_UP, },
        .mouseMaps[INPUT_ACTION_THRUST] =  { MOUSE_RIGHT_BUTTON },
        .keyMaps[INPUT_ACTION_SHOOT] =     { KEY_SPACE },
        .mouseMaps[INPUT_ACTION_SHOOT] =   { INPUT_MOUSE_LEFT_BUTTON },
    };

    // Set touch point button ids (bad code. only used for touch screen analog stick, needs to be rewritten)
    for (unsigned int i = 0; i < INPUT_MAX_TOUCH_POINTS; ++i)
        input.touchPoints[i].currentButton = -1;

    input = defaultControls;
}

bool IsInputKeyModifier(KeyboardKey key)
{
    if (key == KEY_LEFT_ALT || key == KEY_RIGHT_ALT ||
        key == KEY_LEFT_SHIFT || key == KEY_RIGHT_SHIFT ||
        key == KEY_LEFT_CONTROL || key == KEY_RIGHT_CONTROL)
        return true;
    return false;
}

bool IsInputActionPressed(InputAction action)
{
    // Check touch screen button
    if (input.touchButtonPressed[action] == true)
        return true;

    // Check potential key combinations
    KeyboardKey* keys = input.keyMaps[action];
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

    // Check controller buttons
    // TODO

    return false;
}

bool IsInputActionMousePressed(InputAction action)
{
    MouseButton* mb = input.mouseMaps[action];
    for (unsigned int i = 0; i < INPUT_MAX_MAPS && mb[i] != 0; i++)
    {
        MouseButton button = mb[i];
        if (button == 0) button = INPUT_MOUSE_NULL;
        if (button == INPUT_MOUSE_LEFT_BUTTON)
            button = MOUSE_LEFT_BUTTON;
        if (IsMouseButtonPressed(button))
            return true;
    }

    return false;
}

bool IsInputActionDown(InputAction action)
{
    if (input.touchButtonPressed[action] == true)
        return true;

    // Check potential key combinations
    KeyboardKey* keys = input.keyMaps[action];
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

    // Check controller buttons
    // TODO

    return false;
}

bool IsInputActionMouseDown(InputAction action)
{
    MouseButton* mb = input.mouseMaps[action];
    for (unsigned int i = 0; i < INPUT_MAX_MAPS && mb[i] != 0; i++)
    {
        MouseButton button = mb[i];
        if (button == 0) button = INPUT_MOUSE_NULL;
        if (button == INPUT_MOUSE_LEFT_BUTTON)
            button = MOUSE_LEFT_BUTTON;
        if (IsMouseButtonDown(button))
            return true;
    }

    return false;
}

// Touch / Virtual Input
// ----------------------------------------------------------------------------
void SetTouchInput(InputAction action, bool isButtonPressed)
{
    input.touchButtonPressed[action] = isButtonPressed;
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

// Gamepad
// ----------------------------------------------------------------------------

// int CheckAvailableGamepads(void)
// {
//     // TODO
// }
