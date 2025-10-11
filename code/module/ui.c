// EXPLANATION:
// For managing the user interface
// See ui.h for more documentation/descriptions

#include "ui.h"

#include "raylib.h"
#include "raymath.h"

#include "config.h"
#include "input.h"
#include "game.h"

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))

// Initialize
// ----------------------------------------------------------------------------

void InitUiState(void)
{
    UiState defaults = {
        .currentMenu = UI_MENU_TITLE,
        .selectedId = UI_BID_START,
        .firstFrame = true,
    };

    // Title menu
    // ----------------------------------------------------------------------------
    defaults.title[0] = InitUiTitle("Asteroids");
    defaults.title[1] = InitUiTitle("Remake");

    UiMenu *titleMenu = &defaults.menus[UI_MENU_TITLE];

    float startPosX = VIRTUAL_WIDTH/2 - (float)MeasureText("Start", UI_TITLE_BUTTON_SIZE)/2;
    float startPosY = defaults.title[1].position.y + defaults.title[1].fontSize;
    startPosY += UI_TITLE_SPACING;
    CreateUiMenuButton("Start", titleMenu, startPosX, startPosY, UI_TITLE_BUTTON_SIZE);
#if !defined(PLATFORM_WEB)
    CreateUiMenuButtonRelative("Exit", titleMenu, UI_BUTTON_SPACING, UI_TITLE_BUTTON_SIZE);
#endif

    // Pause menu
    // ----------------------------------------------------------------------------
    UiMenu *pauseMenu = &defaults.menus[UI_MENU_PAUSE];
    int resumeTextLength = MeasureText("Resume", UI_FONT_SIZE_EDGE);
    float resumePosX = (float)(VIRTUAL_WIDTH - resumeTextLength)/2;
    float resumePosY = (float)VIRTUAL_HEIGHT/3 + UI_FONT_SIZE_EDGE + UI_BUTTON_SPACING*2;
    CreateUiMenuButton("Resume", pauseMenu, resumePosX, resumePosY, UI_FONT_SIZE_EDGE);
    CreateUiMenuButtonRelative("Back to Title", pauseMenu, UI_BUTTON_SPACING, UI_FONT_SIZE_EDGE);
#if !defined(PLATFORM_WEB)
    CreateUiMenuButtonRelative("Exit Game", pauseMenu, UI_BUTTON_SPACING, UI_FONT_SIZE_EDGE);
#endif

    // Touch input buttons (virtual gamepad)
    // ----------------------------------------------------------------------------
    const int touchInputPadding = UI_EDGE_PADDING*2;

    // Pause button
    int pauseTextLength = MeasureText("Pause", UI_FONT_SIZE_EDGE*0.75f);
    float pausePosX = (float)(VIRTUAL_WIDTH - pauseTextLength)/2.5f;
    float pausePosY = (float)(VIRTUAL_HEIGHT - UI_FONT_SIZE_EDGE - touchInputPadding - UI_BUTTON_PADDING);
    defaults.gamepad.pause = InitUiButton("Pause", UI_BID_PAUSE, pausePosX, pausePosY, UI_FONT_SIZE_EDGE*0.75f);

    // Thrust button
    int flyTextLength = MeasureText("Thrust", UI_FONT_SIZE_EDGE);
    float flyPosX = (float)(VIRTUAL_WIDTH - flyTextLength - touchInputPadding - UI_BUTTON_PADDING);
    float flyPosY = (float)(VIRTUAL_HEIGHT - UI_FONT_SIZE_EDGE - touchInputPadding - UI_BUTTON_PADDING);
    defaults.gamepad.fly = InitUiButton("Thrust", UI_BID_THRUST, flyPosX, flyPosY, UI_FONT_SIZE_EDGE);

    // Shoot button
    int shootTextLength = MeasureText("Shoot", UI_FONT_SIZE_EDGE);
    float shootPosX = (float)(flyPosX - shootTextLength - touchInputPadding - UI_BUTTON_PADDING);
    float shootPosY = (float)(VIRTUAL_HEIGHT - UI_FONT_SIZE_EDGE - touchInputPadding - UI_BUTTON_PADDING);
    defaults.gamepad.shoot = InitUiButton("Shoot", UI_BID_SHOOT, shootPosX, shootPosY, UI_FONT_SIZE_EDGE);

    // Analog stick
    defaults.gamepad.stick.centerPos.x = UI_STICK_RADIUS + touchInputPadding;
    defaults.gamepad.stick.centerPos.y = VIRTUAL_HEIGHT - UI_STICK_RADIUS - touchInputPadding;
    defaults.gamepad.stick.stickPos = defaults.gamepad.stick.centerPos;
    defaults.gamepad.stick.centerRadius = UI_STICK_RADIUS;
    defaults.gamepad.stick.stickRadius = UI_STICK_RADIUS/2;
    defaults.gamepad.stick.lastTouchId = -1;

    ui = defaults;
}

UiButton InitUiTitle(char *text)
{
    int fontSize = UI_TITLE_SIZE;
    int textWidth = MeasureText(text, fontSize);
    float titlePosX = (VIRTUAL_WIDTH - (float)textWidth)/2;
    float titlePosY = UI_TITLE_TOP_PADDING;
#if defined(PLATFORM_WEB) // different spacing for web
    titlePosY += UI_TITLE_BUTTON_SIZE;
#endif

    static bool previousTitleLine = false;
    if (previousTitleLine)
        titlePosY += UI_TITLE_SIZE + 10;

    UiButton title = InitUiButton(text, -1, titlePosX, titlePosY, fontSize);
    previousTitleLine = true;

    return title;
}

UiButton InitUiButton(char *text, int buttonId, float textPosX, float textPosY, int fontSize)
{
    UiButton button = {
        .text = text,
        .buttonId = buttonId,
        .fontSize = fontSize,
        .position = { textPosX, textPosY },
        .color = RAYWHITE
    };

    return button;
}

UiButton *CreateUiMenuButton(char *text, UiMenu *menu, float textPosX, float textPosY, int fontSize)
{
    UiButton button = InitUiButton(text, menu->buttonCount, textPosX, textPosY, fontSize);
    menu->buttonCount++;
    menu->buttons = MemRealloc(menu->buttons, menu->buttonCount*sizeof(UiButton));
    menu->buttons[menu->buttonCount - 1] = button;

    return &menu->buttons[menu->buttonCount - 1];
}

UiButton *CreateUiMenuButtonRelative(char* text, UiMenu *menu, float offsetY, int fontSize)
{
    UiButton *originButton = &menu->buttons[menu->buttonCount - 1];
    float originWidth = (float)MeasureText(originButton->text, originButton->fontSize);
    float originPosX = (originButton->position.x + originWidth/2);
    float textPosX = originPosX - MeasureText(text, fontSize)/2;
    float textPosY = originButton->position.y + originButton->fontSize;

    return CreateUiMenuButton(text, menu, textPosX, textPosY + offsetY, fontSize);
}

void FreeUiState(void)
{
    for (unsigned int i = 0; i < ARRAY_SIZE(ui.menus); i++)
        MemFree(ui.menus[i].buttons);
}

// Update / User Input
// ----------------------------------------------------------------------------

void UpdateUiFrame(void)
{
    if (input.global.debug)
        game.debugMode = !game.debugMode;

    // Update title menu
    if (ui.currentMenu != UI_MENU_NONE)
    {
        // Cancel/Back to main title menu
        if (input.menu.cancel &&
            ui.currentMenu != UI_MENU_TITLE &&
            ui.currentMenu != UI_MENU_PAUSE)
        {
            ChangeUiMenu(UI_MENU_TITLE);
            PlaySound(game.sounds.menu);
        }

        // Input for menu selection and movement
        UiButton *selectedButton = &ui.menus[ui.currentMenu].buttons[ui.selectedId];
        UpdateUiButtonSelect(selectedButton);
        UpdateUiMenuTraverse();
    }

    // Update text fade animation
    static float fadeLength = 1.5f; // Fade in and out at this rate in seconds
    static bool fadingOut = false;
    float fadeIncrement = (1.0f/fadeLength)*game.frameTime;

    if (ui.textFade >= 1.0f)
        fadingOut = true;
    else if (ui.textFade <= 0.0f)
        fadingOut = false;
    if (fadingOut)
        fadeIncrement *= -1;

    ui.textFade += fadeIncrement;
}

void UpdateUiMenuTraverse(void)
{
    if (ui.currentMenu == UI_MENU_NONE)
        return;
    UiMenu *menu = &ui.menus[ui.currentMenu];

    UiTitleMenuId prevId = ui.selectedId; // used to play beep

    // Move cursor via mouse
    if (input.mouse.moved || (ui.firstFrame && ui.lastSelectWithMouse))
    {
        for (unsigned int i = 0; i < menu->buttonCount; i++)
        {
            UiButton *currentButton = 0;
            currentButton = &menu->buttons[i];

            if (IsMouseWithinUiButton(currentButton))
            {
                ui.selectedId = i;
                ui.autoScroll = false;
                ui.lastSelectWithMouse = true;
            }
        }
    }

    // Move cursor via input actions
    const float rapidFireIntervalTime = 0.6f;
    bool isInputUp = input.menu.moveUp;
    bool isInputDown = input.menu.moveDown;
    bool initialKeyPress = (!ui.autoScroll && ui.keyHeldTime == 0);
    bool heldLongEnoughToRepeat = (ui.autoScroll && ui.keyHeldTime >= 0.1f);
    if (initialKeyPress || heldLongEnoughToRepeat)
    {
        if (isInputUp)
        {
            if (ui.selectedId > 0)
                ui.selectedId--;
            else
                ui.selectedId = menu->buttonCount - 1;
            ui.keyHeldTime = 0;
            ui.lastSelectWithMouse = false;
        }
        if (isInputDown)
        {
            if ((unsigned int)ui.selectedId < menu->buttonCount - 1)
                ui.selectedId++;
            else
                ui.selectedId = 0;
            ui.keyHeldTime = 0.0f;
            ui.lastSelectWithMouse = false;
        }
    }

    // Update auto-scroll timer when movement input is held
    if (isInputUp || isInputDown)
    {
        ui.keyHeldTime += game.frameTime;
        if (ui.keyHeldTime >= rapidFireIntervalTime)
        {
            ui.autoScroll = true;
        }
    }
    else
    {
        ui.keyHeldTime = 0;
        ui.autoScroll = false;
    }

    // Play sound when cursor moved
    if (ui.selectedId != prevId && !ui.firstFrame && !input.touchMode)
        PlaySound(game.sounds.menu);

    ui.firstFrame = false;
}


// void UpdateUiButtonMouseHover(UiButton *button) // Disabled+Unused
// {
//     if (!input.mouse.moved) return;

//     if (IsMouseWithinUiButton(button))
//     {
//         // if (!button->mouseHovered)
//         //     PlaySound(game.sounds.menu);
//         button->mouseHovered = true;
//         ui.mouseInUse = true;
//     }
//     else
//     {
//         button->mouseHovered = false;
//         ui.mouseInUse = false;
//     }
// }

void UpdateUiButtonSelect(UiButton *button)
{
    int touchIdx = IsTouchWithinUiButton(button);

    bool buttonTapped = ((touchIdx != -1) && IsTouchPointTapped(touchIdx));
    bool buttonClicked = (input.mouse.leftPressed && IsMouseWithinUiButton(button));
    if (!buttonTapped && !buttonClicked)
        button->clicked = false;

    // Select pause button
    // if (ui.currentMenu == UI_MENU_NONE && (buttonTapped || buttonClicked))
    // {
    //     if (button->buttonId == UI_BID_PAUSE)
    //     {
    //         ChangeUiMenu(UI_MENU_PAUSE);
    //         PlaySound(game.sounds.menu);
    //         button->clicked = true;
    //     }
    // }

    // else if (input.menu.confirm ||
    //     (IsGestureDetected(GESTURE_TAP) &&
    //      (!IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && IsMouseWithinUiButton(button))))

    // Select a menu button
    if (input.menu.confirm || buttonTapped || buttonClicked)
    {
        if (ui.currentMenu == UI_MENU_NONE && !game.isPaused)
            return; // not a menu

        if (ui.currentMenu == UI_MENU_PAUSE && !ui.firstFrame)
        {
            if (ui.selectedId == UI_BID_RESUME)
            {
                game.isPaused = false;
                game.resumeInputCooldown = true;
                ui.currentMenu = UI_MENU_NONE;
            }
            else if (ui.selectedId == UI_BID_BACKTOTITLE)
            {
                ChangeUiMenu(UI_MENU_TITLE);
            }
            else if (ui.selectedId == UI_BID_PAUSE_EXIT)
                game.gameShouldExit = true;
        }

        else if (ui.currentMenu == UI_MENU_TITLE)
        {
            if (ui.selectedId == UI_BID_TITLE_EXIT)
                game.gameShouldExit = true;
            else if (ui.selectedId == UI_BID_START)
                ChangeUiMenu(UI_MENU_NONE);
        }

        PlaySound(game.sounds.menu);
    }
}

void UpdateUiTouchInput(UiButton *button)
{
    InputAction buttonInputAction;
    if (button->buttonId == UI_BID_PAUSE)
        buttonInputAction = INPUT_ACTION_PAUSE;
    else if (button->buttonId == UI_BID_SHOOT)
        buttonInputAction = INPUT_ACTION_SHOOT;
    else if (button->buttonId == UI_BID_THRUST)
        buttonInputAction = INPUT_ACTION_THRUST;
    else return;

    int touchIdx = IsTouchWithinUiButton(button);
    bool isButtonTapped = (touchIdx != -1);
    if (isButtonTapped)
        SetTouchPointButton(touchIdx, button->buttonId);

    SetTouchInputAction(buttonInputAction, isButtonTapped);
    button->clicked = isButtonTapped;
}

void UpdateUiAnalogStick(UiAnalogStick *stick)
{
    int touchIdx;
    if ((stick->lastTouchId != -1) &&
        (input.touchPoints[stick->lastTouchId].pressedCurrentFrame))
        touchIdx = stick->lastTouchId;
    else
        touchIdx = CheckCollisionTouchCircle(stick->centerPos, stick->centerRadius);

    // not touching analog stick
    if (touchIdx == -1 || input.touchCount == 0 || IsTouchingAnyButton(touchIdx))
    {
        stick->stickPos = stick->centerPos;
        stick->isActive = false;
        stick->lastTouchId = -1;
        return;
    }

    // is touching analog stick
    stick->lastTouchId = input.touchPoints[touchIdx].id;
    Vector2 touchPos = GetScreenToWorld2D(GetTouchPosition(touchIdx), game.camera);

    bool isTouchWithinStick =
        CheckCollisionPointCircle(touchPos, stick->centerPos, stick->centerRadius);
    if (isTouchWithinStick)
    {
        stick->stickPos = touchPos;
        stick->isActive = true;
    }
    else if (stick->isActive) // calculate position for edge of stick
    {
        Vector2 direction = Vector2Subtract(touchPos, stick->centerPos);
        float distance = Vector2Length(direction);
        if (distance > stick->centerRadius)
            direction = Vector2Scale(direction, stick->centerRadius / distance);
        stick->stickPos = Vector2Add(stick->centerPos, direction);
    }
}

void ChangeUiMenu(UiMenuState newMenu)
{
    if (newMenu == UI_MENU_TITLE)
    {
        // Reset game state if returning from gameplay
        if (game.currentScreen == SCREEN_GAMEPLAY)
            InitGameState(SCREEN_TITLE);

        ui.selectedId = UI_BID_START;
    }

    else if (newMenu == UI_MENU_PAUSE)
    {
        game.isPaused = true;
        ui.selectedId = UI_BID_RESUME;
        // ui.gamepad.pause.clicked = false;
        // input.player.pause = false;
        // ui.pause.mouseHovered = false;
    }

    else if (newMenu == UI_MENU_NONE)
    {
        game.currentScreen = SCREEN_GAMEPLAY;
        InitNewLevel(game.currentLevel);
    }

    ui.currentMenu = newMenu;
    ui.firstFrame = true;
}

bool IsMouseWithinUiButton(UiButton *button)
{
    Vector2 mousePos = input.mouse.position;

    int padding = UI_BUTTON_PADDING; // extra clickable area around the text
    int buttonWidth = MeasureText(button->text, button->fontSize);
    if ((mousePos.x >= button->position.x - padding) &&
        (mousePos.x <= button->position.x + buttonWidth + padding) &&
        (mousePos.y >= button->position.y - padding) &&
        (mousePos.y <= button->position.y + button->fontSize + padding))
        return true;
    else
        return false;
}

int IsTouchWithinUiButton(UiButton *button)
{
    int buttonWidth = MeasureText(button->text, button->fontSize);
    Rectangle buttonRect = {
       (float)button->position.x - UI_BUTTON_PADDING,
       (float)button->position.y,
       (float)buttonWidth + UI_BUTTON_PADDING*2,
       (float)button->fontSize + UI_BUTTON_PADDING*2, };

    return CheckCollisionTouchRec(buttonRect);
}

// Draw
// ----------------------------------------------------------------------------

void DrawUiFrame(void)
{
    // Title text and background
    // ----------------------------------------------------------------------------
    if (game.currentScreen == SCREEN_TITLE)
    {
        // Draw stars
        for (unsigned int i = 0; i < STAR_AMOUNT; i++)
            DrawCircleV(game.stars[i], 1.0f, RAYWHITE);

        // Draw title text
        for (unsigned int i = 0; i < ARRAY_SIZE(ui.title); i++)
            DrawUiElement(&ui.title[i]);
    }

    if (game.isPaused)
    {
        char *text = "PAUSED";
        int textOffset = MeasureText(text, UI_FONT_SIZE_CENTER)/2;
        DrawText(text, VIRTUAL_WIDTH/2 - textOffset,
                 VIRTUAL_HEIGHT/3 - UI_FONT_SIZE_CENTER/2,
                 UI_FONT_SIZE_CENTER, Fade(RAYWHITE, ui.textFade));
    }

    // Draw menus and buttons
    // ----------------------------------------------------------------------------
    if (ui.currentMenu != UI_MENU_NONE) // Draw non-gameplay menu
    {
        UiButton *selectedButton = &ui.menus[ui.currentMenu].buttons[ui.selectedId];
        DrawUiCursor(selectedButton);

        UiMenu *menu = &ui.menus[ui.currentMenu];
        for (unsigned int i = 0; i < menu->buttonCount; i++)
            DrawUiElement(&menu->buttons[i]);
    }
    else if (game.currentScreen == SCREEN_GAMEPLAY && input.touchMode) // Touch controls
    {
        // Draw pause button
        DrawUiOutline(&ui.gamepad.pause);
        DrawUiElement(&ui.gamepad.pause);

        // Draw fly input button
        DrawUiOutline(&ui.gamepad.fly);
        DrawUiElement(&ui.gamepad.fly);

        // Draw fly input button
        DrawUiOutline(&ui.gamepad.shoot);
        DrawUiElement(&ui.gamepad.shoot);

        // Draw analog stick
        DrawUiAnalogStick(&ui.gamepad.stick);
    }

    // Gameplay UI
    // ----------------------------------------------------------------------------
    if (game.currentScreen == SCREEN_GAMEPLAY)
    {
        DrawLives();

        // Draw level indicator
        const char *levelText = TextFormat("Level %i", game.currentLevel);
        unsigned int textLength = MeasureText(levelText, UI_FONT_SIZE_EDGE);
        DrawText(levelText,
                 VIRTUAL_WIDTH - textLength - UI_EDGE_PADDING, UI_EDGE_PADDING,
                 UI_FONT_SIZE_EDGE, RAYWHITE);

        // 10 asteroids or less text
        int totalRocksRemaining = game.rockLimit - game.eliminatedCount;
        if (totalRocksRemaining <= 10)
        {
            const char* remainText = TextFormat("Remaining: %i", totalRocksRemaining);
            int textOffset = MeasureText(remainText, UI_FONT_SIZE_EDGE)/2;
            DrawText(remainText,
                     VIRTUAL_WIDTH/2 - textOffset, UI_EDGE_PADDING,
                     UI_FONT_SIZE_EDGE, RAYWHITE);
        }

        // Draw center text (pause, game over, etc)
        DrawCenterText();
    }

    // Debug:
    // TODO make toggleable hotkey for debug overlay
    if (!game.debugMode) return;
    Color touchColors[10] = { RED, BLUE, GREEN, YELLOW, ORANGE, PURPLE, BROWN, WHITE, GRAY, MAGENTA };
    for (int i = 0; i < input.touchCount; ++i)
    {
        DrawCircleV(input.touchPoints[i].position, 155.0f, touchColors[i]);
    }

    const int textSize = 30;
    int textY = 150;
    DrawText(TextFormat("%i touchCount", input.touchCount), 0, textY, textSize, RAYWHITE);
    textY += textSize;
    for (int i = 0; i < 8; i++)
    {
        DrawText(TextFormat("touch %i x,y: %.0f,%.0f id: %i", i+1, input.touchPoints[i].position.x, input.touchPoints[i].position.y, input.touchPoints[i].id), 0, textY, textSize, RAYWHITE);
        textY += textSize;
    }

    /*
    Vector2 mousePos = input.mouse.position;
    DrawText(TextFormat("mouse: %3.0f, %3.0f", mousePos.x, mousePos.y), 0, textY, textSize, RAYWHITE);
    textY += textSize;
    if (game.touchCount > 0)
    {
        for (int i = 0; i < game.touchCount; i++)
        {
            DrawText(TextFormat("touch %i: %3.0f, %3.0f", i, GetTouchPosition(i).x, GetTouchPosition(i).y), 0, textY, textSize, RAYWHITE);
            textY += textSize;
        }
    }
    DrawText(TextFormat("%2i rock total", game.rockLimit), 0, textY, textSize, RAYWHITE);
    textY += textSize;
    DrawText(TextFormat("%2i remaining", game.rockLimit - game.eliminatedCount), 0, textY, textSize, RAYWHITE);
    textY += textSize;
    DrawText(TextFormat("speed: %3.0f", Vector2Length(game.ship.velocity)), 0, textY, textSize, RAYWHITE);
    textY += textSize;
    */
}

void DrawUiElement(UiButton *button)
{
    DrawText(button->text, (int)button->position.x, (int)button->position.y,
             button->fontSize, RAYWHITE);
}

void DrawUiCursor(UiButton *selectedButton)
{
    float size = UI_CURSOR_SIZE;

    Vector2 selectPointPos; // the corner/vertice pointing towards the right
    Vector2 cursorOffset = (Vector2){-50.0f, (float)selectedButton->fontSize/2};
    selectPointPos = Vector2Add(selectedButton->position, cursorOffset);

    DrawTriangle(Vector2Add(selectPointPos, (Vector2){ -size*2, size }),
                 selectPointPos,
                 Vector2Add(selectPointPos, (Vector2){ -size*2, -size }),
                 RAYWHITE);
}

void DrawUiOutline(UiButton *selectedButton)
{
    int padding         = UI_BUTTON_PADDING;
    int outlineWidth    = 4;
    int buttonWidth     = MeasureText(selectedButton->text, selectedButton->fontSize);
    int buttonHeight    = selectedButton->fontSize;
    float buttonPosX    = selectedButton->position.x - padding;
    float buttonPosY    = selectedButton->position.y - padding;
    int highlightWidth  = buttonWidth + padding * 2;
    int highlightHeight = buttonHeight + padding * 2;

    Color boxColor = RAYWHITE;
    if (selectedButton->clicked)
        boxColor = ColorAlpha(boxColor, UI_TRANSPARENCY*3);
    else
        boxColor = ColorAlpha(boxColor, UI_TRANSPARENCY);

    // outline box
    DrawRectangleLinesEx((Rectangle){ buttonPosX, buttonPosY,
                         (float)highlightWidth, (float)highlightHeight },
                         (float)outlineWidth, RAYWHITE);
    // box around text
    DrawRectangle((int)buttonPosX + outlineWidth,
                  (int)buttonPosY + outlineWidth,
                  highlightWidth - outlineWidth*2,
                  highlightHeight - outlineWidth*2, boxColor);
}

void DrawUiAnalogStick(UiAnalogStick *stick)
{
    DrawCircleV(stick->centerPos, stick->centerRadius, ColorAlpha(RAYWHITE, UI_TRANSPARENCY));
    DrawRing(stick->centerPos, stick->centerRadius - 4, stick->centerRadius + 4,
             0, 360, 0, RAYWHITE);
    DrawCircleV(stick->stickPos, stick->stickRadius, GRAY);
}

void DrawLives(void)
{
    const char* text = "Lives: ";
    const int textWidth = MeasureText(text, UI_FONT_SIZE_EDGE);
    DrawText(text, UI_EDGE_PADDING, UI_EDGE_PADDING, UI_FONT_SIZE_EDGE, RAYWHITE);
    const float scale = UI_FONT_SIZE_EDGE*0.95f/game.ship.length;
    const float spacing = game.ship.width*scale/8;
    Vector2 lifeTriangle[3] = { 0 };

    for (unsigned int i = 0; i < 3; i++)
    {
        lifeTriangle[i] = Vector2Scale(game.shipTriangle[i], scale);
        lifeTriangle[i] = Vector2Rotate(lifeTriangle[i], DEG2RAD*30.0f);
        lifeTriangle[i].x += textWidth;
        lifeTriangle[i].x += UI_EDGE_PADDING - game.ship.width*scale/2 - spacing;
        lifeTriangle[i].y += UI_EDGE_PADDING + game.ship.length*scale/2;
    }

    for (unsigned int i = 0; i < game.lives; i++)
    {
        for (unsigned int j = 0; j < 3; j++)
        {
            lifeTriangle[j].x += spacing + game.ship.width*scale;
        }
        DrawTriangle(lifeTriangle[0], lifeTriangle[1], lifeTriangle[2], RAYWHITE);
    }
}

void DrawCenterText(void)
{
    bool centerText = false;
    const char *text;
    Color fadeColor = Fade(RAYWHITE, ui.textFade);

    if (game.isPaused)
    {
        centerText = false;
    }
    else if (game.lives <= 0)
    {
        text = "GAME OVER";
        centerText = true;
    }
    else if (game.levelFinished)
    {
        text = "ASTEROIDS CLEARED";
        centerText = true;
    }
    else if (game.newLevelTimer > EPSILON)
    {
        if (game.currentLevel == 1)
            text = "GAME START";
        else
            text = TextFormat("LEVEL %i", game.currentLevel);
        centerText = true;
    }
    else if (game.ship.isExploded)
    {
        if (game.ship.respawnTimer < SHIP_RESPAWN_TIME)
            text = "RESPAWNING...";
        else if (game.lives == 1)
            text = TextFormat("%i LIFE LEFT!", game.lives);
        else
            text = TextFormat("%i LIVES LEFT", game.lives);
        centerText = true;
    }

    if (game.messageTimer > EPSILON && !game.levelFinished && !game.ship.isExploded)
    {
        centerText = false;
        ui.textFade = 1.0f;
    }

    if (centerText)
    {
        int textOffset = MeasureText(text, UI_FONT_SIZE_CENTER)/2;
        DrawText(text, VIRTUAL_WIDTH/2 - textOffset,
                 VIRTUAL_HEIGHT/2 - UI_FONT_SIZE_CENTER/2,
                 UI_FONT_SIZE_CENTER, fadeColor);
    }
}
