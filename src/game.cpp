#pragma warning(disable : 4996)
#pragma warning(disable : 6387)
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

enum Difficulty { EASY, MEDIUM, HARD };
Difficulty selectedDifficulty = MEDIUM;

struct Animdata
{
    Rectangle rec;

    Vector2 pos;

    int frame1;

    float UpdateTime;

    float runningTime;

    bool passed{};

};

bool isOnGround(Animdata data, int windowHeight) {
    return data.pos.y >= windowHeight - data.rec.height;
}

Animdata UpdateAnimData(Animdata data, float DeltaTime, int MaxFrame) {
    data.runningTime += DeltaTime;
    if (data.runningTime >= data.UpdateTime) {
        data.runningTime = 0;
        //START OVER AGAIN SO THE FRAMES UPDATE IT SELF
        data.rec.x = data.frame1 * data.rec.width;// accessing every frame in the scarfy frames  0 * العرض
        data.frame1++;
        if (data.frame1 > MaxFrame) {
            data.frame1 = 0;
        }
    }
    return data;
}

int main() {
    constexpr int gravity{ 1'000 };//px/s**2

    bool isinair{};

    bool showMenu = true;

    SetTargetFPS(60); //60 frame every second , every frame takes approxmently 1/60 seconds

    int doublejumbs{};
    constexpr int maxjumbs = 2;
    /*constexpr uses less RAM and makes the program faster because the value is known at compile time.
        The compiler doesn't need to access RAM at runtime, and the value doesn't need to be stored in RAM at all
    */

    int velocity{ 0 };
    //VERTICAL SPEED , IF SCARFY JUMPS IT DECREASES  

    int CoinsCollector{};

    int points{};
    bool raining{};

    constexpr int WindowDimensions[2] = { 800,600 };
    constexpr int NumOfCoins = 10;
    constexpr int NumOfNebulas = 5;

    //----------------------------------------------------------------------------------------------------------------------------------

    InitWindow(WindowDimensions[0], WindowDimensions[1], "〰️Dapper-Dasher〰️");

    SetExitKey(KEY_Q);

    //----------------------------------------------------------------------------------------------------------------------------------

    //SETTING SOUNDS&MUSIC

    InitAudioDevice();

    Sound jumpsound = LoadSound("C:/Users/Amr/OneDrive/Attachments/Scarfy/jumb.wav");

    Sound WinningSound = LoadSound("C:/Users/Amr/OneDrive/Attachments/Scarfy/winning.wav");

    Sound LosingSound = LoadSound("C:/Users/Amr/OneDrive/Attachments/Scarfy/lost.wav");
    bool hasPlayedLoseSound = false;

    Sound CoinSound = LoadSound("C:/Users/Amr/OneDrive/Attachments/Scarfy/402767__lilmati__retro-coin-03.wav");

    Sound clickSound = LoadSound("C:/Users/Amr/OneDrive/Attachments/Scarfy/414719__devern__mouse-click.wav");

    Sound StartGame = LoadSound("C:/Users/Amr/OneDrive/Attachments/Scarfy/653668__eilassfx__game-started.wav");

    Music GameMusic = LoadMusicStream("C:/Users/Amr/OneDrive/Attachments/Scarfy/gamesound.wav");
    bool hasPlayedGameMusic{};

    Music MenuMusic = LoadMusicStream("C:/Users/Amr/OneDrive/Attachments/Scarfy/480932__wi-photos__menu.wav");
    bool hasPlayedMenuMusic{};

    //----------------------------------------------------------------------------------------------------------------------------------

    // volume
    SetMusicVolume(MenuMusic, 0.2f);

    SetMusicVolume(GameMusic, 0.34f);

    SetSoundVolume(WinningSound, 0.8f);

    SetSoundVolume(LosingSound, 0.6f);

    SetSoundVolume(jumpsound, 0.3f);

    //----------------------------------------------------------------------------------------------------------------------------------

    Texture2D nebula = LoadTexture("C:/Users/Amr/OneDrive/Attachments/Scarfy/textures/12_nebula_spritesheet.png");

    Animdata nebulea[NumOfNebulas]{};

    for (int i = 0; i < NumOfNebulas; i++)
    {
        nebulea[i].rec.x = 0.0;

        nebulea[i].rec.y = 0.0;

        nebulea[i].rec.width = nebula.width / 8;

        nebulea[i].rec.height = nebula.height / 8;

        nebulea[i].pos.y = WindowDimensions[1] - nebula.height / 8;

        nebulea[i].runningTime = 0.0;

        nebulea[i].UpdateTime = 1.0 / 16.0; //needed time to change the nebula frame

        nebulea[i].pos.x = WindowDimensions[0] + (i + 1) * (GetRandomValue(300, 500) + GetRandomValue(250, 300));
    }

    //----------------------------------------------------------------------------------------------------------------------------------

    float bgx{};

    float mgx{};

    float fgx{};

    //----------------------------------------------------------------------------------------------------------------------------------

    Texture2D Coin = LoadTexture("C:/Users/Amr/OneDrive/Attachments/Scarfy/textures/Full-Coinss.png");

    Texture2D BackGround = LoadTexture("C:/Users/Amr/OneDrive/Attachments/Scarfy/textures/far-buildings.png");

    Texture2D scarfy = LoadTexture("C:/Users/Amr/OneDrive/Attachments/Scarfy/textures/scarfy.png");

    Texture2D Foreground = LoadTexture("C:/Users/Amr/OneDrive/Attachments/Scarfy/textures/foreground.png");

    Texture2D BackBuilding = LoadTexture("C:/Users/Amr/OneDrive/Attachments/Scarfy/textures/back-buildings.png");

    //----------------------------------------------------------------------------------------------------------------------------------

    Animdata COINS[NumOfCoins];

    for (int i = 0; i < NumOfCoins; i++)
    {
        COINS[i].rec.width = Coin.width / 8;

        COINS[i].rec.height = Coin.height;

        COINS[i].rec.x = 0;

        COINS[i].rec.y = 0;

        COINS[i].pos.y = GetRandomValue(WindowDimensions[1] - Coin.height - 300, WindowDimensions[1] - Coin.height - 100);

        COINS[i].passed = false;
        //// TO KNOW IF IT IS COLLECTED OR NOT.

        COINS[i].pos.x = WindowDimensions[0] + (i + 3) * (GetRandomValue(400, 500) + GetRandomValue(150, 300));    //كان i=0 يخلي اول هازارد قدام اللاعب على طول

        COINS[i].UpdateTime = 1.0 / 24.0;

        COINS[i].runningTime = 0;

        COINS[i].frame1 = 0;
    }

    //----------------------------------------------------------------------------------------------------------------------------------

    Animdata scarfydata;
    scarfydata.rec.width = scarfy.width / 6;
    scarfydata.rec.height = scarfy.height;
    scarfydata.rec.x = 0;
    scarfydata.rec.y = 0;
    scarfydata.pos.x = WindowDimensions[0] / 2 - scarfydata.rec.width;
    scarfydata.pos.y = WindowDimensions[1] - scarfydata.rec.height;
    scarfydata.frame1 = 0;
    scarfydata.UpdateTime = 1.0 / 12.0;
    scarfydata.runningTime = 0.0;

    //----------------------------------------------------------------------------------------------------------------------------------

    Image icon = LoadImage("C:/Users/Amr/OneDrive/Attachments/Scarfy/AF5r+U.png");
    SetWindowIcon(icon);
    UnloadImage(icon);

    //----------------------------------------------------------------------------------------------------------------------------------

    int nebvel{ -350 };
    float FinishLine{ nebulea[NumOfNebulas - 1].pos.x };
    bool collosion{};
    bool hasPlayedWinSound = false;
    bool lost{ 0 };
    bool won{};
    bool ExitButtonAppear{ 1 };
    int selectedLevel = 0;
    bool dropdownEditMode = false;

    Font SFont = LoadFont("C:/Users/Amr/OneDrive/Attachments/Scarfy/varsity_regular.ttf");
    const char* levelOptions = "Easy;Medium;Hard";

    GuiSetStyle(DEFAULT, TEXT_SIZE, 30);

    //----------------------------------------------------------------------------------------------------------------------------------

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(WHITE);
        float dt{ GetFrameTime() };//time btn every frame and the next one

        if (showMenu) {
            if (!hasPlayedMenuMusic) {
                PlayMusicStream(MenuMusic);
                hasPlayedMenuMusic = 1;
            }
            UpdateMusicStream(MenuMusic);

            bgx -= 20 * dt;

            mgx -= 40 * dt;

            fgx -= 80 * dt;

            if (bgx <= -BackGround.width * 2) {
                bgx = 0.0;
            }

            if (mgx <= -BackBuilding.width * 2) {
                mgx = 0.0;
            }

            if (fgx <= -Foreground.width * 2) {
                fgx = 0.0;
            }

            Vector2 bg1pos{ bgx,0.0 };

            DrawTextureEx(BackGround, bg1pos, 0.0, 3.13, WHITE);

            Vector2 bg2pos{ bgx + BackGround.width * 2,0.0 };

            DrawTextureEx(BackGround, bg2pos, 0.0, 3.13, WHITE);

            Vector2 mg1Pos{ mgx, 0.0 };

            DrawTextureEx(BackBuilding, mg1Pos, 0.0, 3.12, WHITE);

            Vector2 mg2Pos{ mgx + BackBuilding.width * 2, 0.0 };

            DrawTextureEx(BackBuilding, mg2Pos, 0.0, 3.12, WHITE);

            // draw the foreground
            Vector2 fg1Pos{ fgx, 0.0 };

            DrawTextureEx(Foreground, fg1Pos, 0.0, 3.12, WHITE);

            Vector2 fg2Pos{ fgx + Foreground.width * 2, 0.0 };

            DrawTextureEx(Foreground, fg2Pos, 0.0, 3.12, WHITE);

            ClearBackground({ 240, 220, 180, 255 });

            //----------------------------------------------------------------------------------------------------------------------------------

            // MENU

            GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(BLACK));
            GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(RED));
            GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(WHITE));
            if (GuiButton({ WindowDimensions[0] / 2 - 100, WindowDimensions[1] / 2 - 50, 200, 50 }, GuiIconText(ICON_STAR, "START"))) {
                showMenu = false;

                PlaySound(StartGame);
            }

            //----------------------------------------------------------------------------------------------------------------------------------

            GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(RED));
            GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(WHITE));
            GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(WHITE));
            if (GuiButton({ WindowDimensions[0] - 70, WindowDimensions[1] - 600, 70, 50 }, GuiIconText(ICON_EXIT, "X"))) {
                break;
            }

            //----------------------------------------------------------------------------------------------------------------------------------

            DrawTextEx(SFont, "SCARFY", { WindowDimensions[0] / 4 - 30, WindowDimensions[1] / 2 - 220 }, 150, 2, RED);

            //----------------------------------------------------------------------------------------------------------------------------------

            DrawText("Select Difficulty:", WindowDimensions[0] / 2 - 170, WindowDimensions[1] / 2 + 20, 20, PURPLE);

            GuiSetStyle(DROPDOWNBOX, BASE_COLOR_FOCUSED, ColorToInt(WHITE));

            if (GuiDropdownBox({ WindowDimensions[0] / 2 - 150, WindowDimensions[1] / 2 + 50, 300, 40 }, levelOptions, &selectedLevel, dropdownEditMode)) {
                dropdownEditMode = !dropdownEditMode;
                //OPEN AND CLOSES THE DROPDOWNBOX.     

                GuiSetStyle(DROPDOWNBOX, TEXT_COLOR_NORMAL, ColorToInt(BLUE));

                GuiSetStyle(DROPDOWNBOX, DROPDOWN_ITEMS_SPACING, 10);

                //----------------------------------------------------------------------------------------------------------------------------------

                if (selectedLevel == 0) {
                    GuiSetStyle(DROPDOWNBOX, TEXT_COLOR_NORMAL, ColorToInt(GREEN));
                    GuiSetStyle(DROPDOWNBOX, BASE_COLOR_NORMAL, ColorToInt(DARKGRAY));
                }
                else if (selectedLevel == 1) {
                    GuiSetStyle(DROPDOWNBOX, TEXT_COLOR_NORMAL, ColorToInt(BLUE));
                    GuiSetStyle(DROPDOWNBOX, BASE_COLOR_NORMAL, ColorToInt(YELLOW));
                }
                else if (selectedLevel == 2) {
                    GuiSetStyle(DROPDOWNBOX, TEXT_COLOR_NORMAL, ColorToInt(RED));
                    GuiSetStyle(DROPDOWNBOX, BASE_COLOR_NORMAL, ColorToInt(BLACK));
                }
                else {
                    GuiSetStyle(DROPDOWNBOX, BASE_COLOR_NORMAL, ColorToInt(DARKGRAY));
                }

                PlaySound(clickSound);
            }
        }

        //----------------------------------------------------------------------------------------------------------------------------------

        else {
            switch (selectedLevel)
            {
            case EASY:
                nebvel = -375;
                break;

            case MEDIUM:
                nebvel = -450;
                break;

            case HARD:
                nebvel = -525;
                break;
            }

            //----------------------------------------------------------------------------------------------------------------------------------

            if (!hasPlayedGameMusic)
            {
                PlayMusicStream(GameMusic);
                hasPlayedGameMusic = true;
            }

            //----------------------------------------------------------------------------------------------------------------------------------

            if (IsKeyPressed(KEY_ESCAPE)) {
                showMenu = 1;
            }

            UpdateMusicStream(GameMusic);
            /*"We had an issue where the music would stop playing when it ended,
            so I used music instead of sound and updated it every frame to let the library engine keep the music playing.
            */

            //----------------------------------------------------------------------------------------------------------------------------------

            bgx -= 20 * dt;

            mgx -= 40 * dt;

            fgx -= 80 * dt;

            //----------------------------------------------------------------------------------------------------------------------------------

            if (bgx <= -BackGround.width * 2) {
                bgx = 0.0;
            }

            if (mgx <= -BackBuilding.width * 2) {
                mgx = 0.0;
            }

            if (fgx <= -Foreground.width * 2) {
                fgx = 0.0;
            }

            Vector2 bg1pos{ bgx,0.0 };

            DrawTextureEx(BackGround, bg1pos, 0.0, 3.13, WHITE);

            Vector2 bg2pos{ bgx + BackGround.width * 2,0.0 };

            DrawTextureEx(BackGround, bg2pos, 0.0, 3.13, WHITE);

            Vector2 mg1Pos{ mgx, 0.0 };

            DrawTextureEx(BackBuilding, mg1Pos, 0.0, 3.12, WHITE);

            Vector2 mg2Pos{ mgx + BackBuilding.width * 2, 0.0 };

            DrawTextureEx(BackBuilding, mg2Pos, 0.0, 3.12, WHITE);

            // draw the foreground
            Vector2 fg1Pos{ fgx, 0.0 };

            DrawTextureEx(Foreground, fg1Pos, 0.0, 3.12, WHITE);

            Vector2 fg2Pos{ fgx + Foreground.width * 2, 0.0 };

            DrawTextureEx(Foreground, fg2Pos, 0.0, 3.12, WHITE);

            //----------------------------------------------------------------------------------------------------------------------------------

            //hanging scarfy frame

            scarfydata.runningTime += dt; //every frame , delta time changes, we sum it so we can determine when to change the scarfy frame 

            if (!isinair)//frame only changes when scarfy is not in the air (in the ground)
            {
                scarfydata = UpdateAnimData(scarfydata, dt, 5);
            }

            //----------------------------------------------------------------------------------------------------------------------------------

            for (int i = 0; i < NumOfNebulas; i++) {
                nebulea[i] = UpdateAnimData(nebulea[i], dt, 7);
            }

            //----------------------------------------------------------------------------------------------------------------------------------

            for (int i = 0; i < NumOfCoins; i++) {
                COINS[i] = UpdateAnimData(COINS[i], dt, 7);
                printf("Coin %d frame: %d, runningTime: %f, UpdateTime: %f\n", i, COINS[i].frame1, COINS[i].runningTime, COINS[i].UpdateTime);
            }

            //----------------------------------------------------------------------------------------------------------------------------------

            if (isOnGround(scarfydata, WindowDimensions[1])) {
                velocity = 0;

                isinair = false;

                doublejumbs = 0;
            }

            //----------------------------------------------------------------------------------------------------------------------------------

            else if (!isOnGround(scarfydata, WindowDimensions[1]))
            {
                velocity += gravity * dt;

                isinair = true;
            }

            //----------------------------------------------------------------------------------------------------------------------------------

            if (IsKeyPressed(KEY_SPACE) && doublejumbs < maxjumbs)
            {
                velocity = (doublejumbs == 0) ? -600 : -750;
                doublejumbs++;
                if (!lost)
                {
                    PlaySound(jumpsound);
                    // When the player lost and pressed the spacebar, the jump sound would still play — so I prevented that.
                }
            }

            //----------------------------------------------------------------------------------------------------------------------------------

            for (int i = 0; i < NumOfNebulas; i++) {
                nebulea[i].pos.x += nebvel * dt;

                if (!nebulea[i].passed && nebulea[i].pos.x + nebulea[i].rec.width < scarfydata.pos.x && !lost)
                {
                    points++;
                    nebulea[i].passed = true; // mark as passed
                }
            }

            //----------------------------------------------------------------------------------------------------------------------------------

            for (int i = 0; i < NumOfCoins; i++) {
                COINS[i].pos.x += nebvel * dt;
            }

            //----------------------------------------------------------------------------------------------------------------------------------

            for (Animdata nebula : nebulea) {
                float pad{ 25 };
                Rectangle nebrec{
                    nebula.pos.x + pad,
                    nebula.pos.y + pad,
                    nebula.rec.width - 2 * pad,
                    nebula.rec.height - 2 * pad
                };

                Rectangle scarfyrec{
                    scarfydata.pos.x + pad,
                    scarfydata.pos.y + pad,
                    scarfydata.rec.width - 2 * pad,
                    scarfydata.rec.height - 2 * pad
                };

                if (CheckCollisionRecs(nebrec, scarfyrec) && !lost) {
                    collosion = true;
                    lost = true;
                }
            }

            //----------------------------------------------------------------------------------------------------------------------------------

            for (auto& Coin : COINS) {
                float pad{ 5 };

                Rectangle scarfyrec{
                    scarfydata.pos.x + pad,
                    scarfydata.pos.y + pad,
                    scarfydata.rec.width - 2 * pad,
                    scarfydata.rec.height - 2 * pad
                };

                Rectangle coinRec{
                    Coin.pos.x + pad,
                    Coin.pos.y + pad,
                    Coin.rec.width - 2 * pad,
                    Coin.rec.height - 2 * pad
                };

                if (CheckCollisionRecs(scarfyrec, coinRec) && !lost && !Coin.passed) {
                    points += 2;

                    CoinsCollector++;

                    Coin.passed = true;

                    Coin.pos = Vector2{ -100, -100 };

                    PlaySound(CoinSound);
                }
            }
        }

        //----------------------------------------------------------------------------------------------------------------------------------

        scarfydata.pos.y += velocity * dt;

        FinishLine = nebulea[NumOfNebulas - 1].pos.x;

        //----------------------------------------------------------------------------------------------------------------------------------

        if (!showMenu) {
            DrawText(TextFormat("Score: %d/%d", points, NumOfNebulas), WindowDimensions[0] / 112, 30, 50, DARKBLUE);

            DrawText(TextFormat("Coins: %d", CoinsCollector), WindowDimensions[0] - 200, 30, 50, YELLOW);
        }

        //----------------------------------------------------------------------------------------------------------------------------------

        if (points >= NumOfNebulas / 2) {
            DrawCircle(GetRandomValue(0, WindowDimensions[0]), GetRandomValue(0, WindowDimensions[1]), 2.5, BLUE);
            raining = 1;
        }

        //----------------------------------------------------------------------------------------------------------------------------------

        if (lost || won) {
            if (won) {
                GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(BLUE));
            }
            else {
                GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(RED));
            }
            GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(BLACK));
            GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(BLACK));
            if (GuiButton({ WindowDimensions[0] / 2 - 100, WindowDimensions[1] / 2 + 90, 200, 50 }, "RESTART")) {
                //REINTALIZE THE VARIBULES
                lost = false;
                won = false;
                points = 0;
                CoinsCollector = 0;
                hasPlayedLoseSound = false;
                hasPlayedWinSound = false;
                scarfydata.pos.y = WindowDimensions[1] - scarfydata.rec.height;
                scarfydata.runningTime = 0.0;
                scarfydata.frame1 = 0;
                velocity = 0;
                doublejumbs = 0;
                isinair = false;

                for (int i = 0; i < NumOfNebulas; i++) {
                    nebulea[i].pos.x = WindowDimensions[0] + (i + 1) * (GetRandomValue(300, 500) + GetRandomValue(250, 300));
                    nebulea[i].passed = false;
                    nebulea[i].runningTime = 0.0;
                    nebulea[i].frame1 = 0;
                }

                for (int i = 0; i < NumOfCoins; i++) {
                    COINS[i].pos.x = WindowDimensions[0] + (i + 3) * (GetRandomValue(400, 500) + GetRandomValue(150, 300));
                    COINS[i].passed = false;
                    COINS[i].pos.y = GetRandomValue(WindowDimensions[1] - Coin.height - 300, WindowDimensions[1] - Coin.height - 100);
                    COINS[i].runningTime = 0.0;
                    COINS[i].frame1 = 0;
                }

                FinishLine = nebulea[NumOfNebulas - 1].pos.x;
            }
        }

        //----------------------------------------------------------------------------------------------------------------------------------

        if (collosion && lost)
        {
            // lose the game
            DrawText("Game Over!", WindowDimensions[0] / 4, WindowDimensions[1] / 2, 70, RED);
            if (!hasPlayedLoseSound) {
                PlaySound(LosingSound);
                hasPlayedLoseSound = true;
            }
        }
        else if (scarfydata.pos.x >= FinishLine)
        {
            DrawText("You Win!", WindowDimensions[0] / 4, WindowDimensions[1] / 2, 90, DARKBLUE);
            won = true;
            if (!hasPlayedWinSound)
            {
                PlaySound(WinningSound);
                hasPlayedWinSound = 1;
            }

            /* "I had an issue where the sound was glitching at the start because it was being reused every frame,
            which caused it to cut off before the condition was even checked.
           Now it plays as if it continues from where it left off, and I added a flag to make sure it doesn’t play again
            if it already started.
            */
        }

        //----------------------------------------------------------------------------------------------------------------------------------

        else
        {
            if (showMenu == 0) {
                for (int i = 0; i < NumOfNebulas; i++)
                {
                    // draw nebula
                    DrawTextureRec(nebula, nebulea[i].rec, nebulea[i].pos, WHITE);
                }

                //----------------------------------------------------------------------------------------------------------------------------------

                // draw scarfy
                DrawTextureRec(scarfy, scarfydata.rec, scarfydata.pos, WHITE);

                //----------------------------------------------------------------------------------------------------------------------------------

                for (int i = 0; i < NumOfCoins; i++) {
                    if (!COINS[i].passed && !lost) {
                        Rectangle sourceRec = COINS[i].rec;
                        Rectangle destRec = { COINS[i].pos.x, COINS[i].pos.y, COINS[i].rec.width, COINS[i].rec.height };
                        Vector2 origin = { COINS[i].rec.width / 2.0f, COINS[i].rec.height / 2.0f };
                        DrawTexturePro(Coin, sourceRec, destRec, origin, 180.0f, WHITE); //Coins direction
                    }
                }
            }
        }

        //----------------------------------------------------------------------------------------------------------------------------------

        // stop drawing
        EndDrawing();
    }

    //----------------------------------------------------------------------------------------------------------------------------------

    UnloadTexture(Coin);
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(BackGround);
    UnloadTexture(BackBuilding);
    UnloadTexture(Foreground);
    UnloadSound(jumpsound);
    UnloadSound(WinningSound);
    UnloadSound(LosingSound);
    UnloadSound(CoinSound);
    UnloadSound(clickSound);
    UnloadSound(StartGame);
    UnloadMusicStream(GameMusic);
    UnloadMusicStream(MenuMusic);
    UnloadFont(SFont);
    CloseAudioDevice();
    CloseWindow();
}