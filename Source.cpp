#include "DirectX.h"
#include<fstream>
#include <iostream>
#include <sstream>

//World Generation
const int Width = 800;  
const int Height = 600;
const float framerate = 30;  
bool gameover;
Model2D background, base, bird, pipeUP[4], pipeDOWN[4];
int world_width = 800, 
world_height = 600;
int road_pos = 500;

//Score System
int distance = 0;
int highscore = 0;

//Bird Physics
int MaxHeight = 0;
float gravity = 2.5f;
float birdPosition = 0.0f;
float birdVelocity = 0.0f;
float birdLift = -20.0f;

//Flapping Cooldown
long cooling = 0;
const int cooldown = 75;

//Pipe Configuration
float pipeRate = 3.0f;
float pipePos[4] = { 800.0f, 1000.0f, 1200.0f, 1400.0f};

//Base Movement
float baseRate = 3.0f;
float baseSpeed = 0.0f;

//Interactions
Keyboard::State kb;
Mouse::ButtonStateTracker mousetracker;
std::unique_ptr<SoundEffect> flap;
std::unique_ptr<SoundEffect> point;
std::unique_ptr<SoundEffect> hit;
std::unique_ptr<SoundEffect> die;
std::unique_ptr<SpriteFont> spriteFont;

void UpdateBird()
{
    birdVelocity += gravity;
    bird.y += birdVelocity;
    kb = keyboard->GetState();

    //bird hits road
    if (bird.y > road_pos)
    {
        birdVelocity = 0;
        bird.y = road_pos;
    }

    if (bird.y < -1) {
        birdVelocity = 0;
        bird.y = 0;
    }

    if ((keyboard->GetState().Space || mouse->GetState().leftButton) && GetTickCount() - cooling > cooldown)
    {
        birdVelocity += birdLift;
        bird.y += birdVelocity;
        PlaySound(flap.get());
        cooling = GetTickCount();
    }
    if (CheckModel2DCollided(bird, base))
    {
        Game_End();
    }

}
void UpdatePipe()
{

    for (int i = 0; i < 4; i++) {
        pipePos[i] -= pipeRate;
        pipeUP[i].x = pipePos[i];
        pipeDOWN[i].x = pipeUP[i].x;

          pipeDOWN[i].y = pipeUP[i].y - 500;

        if (CheckModel2DCollided(bird, pipeUP[i]))
        {
            Game_End();
        }
        if (CheckModel2DCollided(bird, pipeDOWN[i]))
        {
            Game_End();
        }
        if (pipePos[i] < 0)
        {
            pipePos[i] = 800.0f;
            pipeUP[i].y = rand() % 225 + 200;
            pipeDOWN[i].y = pipeUP[i].y - 500;
            distance += 5;
            PlaySound(point.get());
        }
    }
}
void UpdateBase() 
{
    baseSpeed -= baseRate;
    base.x = baseSpeed;

    if (base.x == -48 || base.x == -49){
        baseSpeed= -1;
        base.x = -1;
        distance++;
    }
}
bool Game_Init(HWND hwnd)
{
    gameover = false;
    if (InitD3D(hwnd) == false)
        return false;
    if (InitInput(hwnd) == false)
        return false;
    if (InitSound() == false)
        return false;


    flap = LoadSound(L"wing.wav");
    point = LoadSound(L"point.wav");
    hit = LoadSound(L"hit.wav");
    die = LoadSound(L"die.wav");

    background = CreateModel2D(L"background.bmp");
    if (background.texture == NULL)
        return false;

    bird = CreateModel2D(L"birdDown.png");
    if (bird.texture == NULL)
        return false;

    for (int i = 0; i < 4; i++) {
        pipeUP[i] = CreateModel2D(L"pipe-up.png");
        if (pipeUP[i].texture == NULL)
            return false;
    }

    for (int i = 0; i < 4; i++) {
        pipeDOWN[i] = CreateModel2D(L"pipe-down.png");
        if (pipeDOWN[i].texture == NULL)
            return false;
    }

    base = CreateModel2D(L"longbase.bmp");
    if (base.texture == NULL)
        return false;

    try {
        spriteFont = std::make_unique<SpriteFont>(dev,
            L"times_new_roman.spritefont");
    }
    catch (std::runtime_error e)
    {
        MessageBox(NULL, L"Loading times_new_roman.spritefont error", L"Error", MB_OK | MB_ICONERROR);
            return false;
    }

    //Reading highscore
    std::fstream f(".\\score.txt");
    std::stringstream iss;
    iss << f.rdbuf();
    std::string fileString = iss.str();
    highscore = std::stoi(fileString);
    f.close();

    // initial positions
    base.y = road_pos;
    bird.x = 100; 
    bird.y = 250;

    // initial pipe
    for (int i = 0; i < 4; i++) {
        pipeUP[i].y = rand() % 225 + 200;
        pipeDOWN[i].x = pipeUP[i].x;
        pipeDOWN[i].y = pipeUP[i].y - 500;
    }

    return true;
}
void Game_Run()
{
    long static start = 0;
    float frame_interval = 1000.0 / framerate;
    if (GetTickCount() - start >= frame_interval)
    {
        //reset timing
        start = GetTickCount();
        ClearScreen();
        spriteBatch->Begin();
        UpdateBird();
        DrawModel2D(background);
        for (int i = 0; i < 4; i++) {
            DrawModel2D(pipeUP[i]);
            DrawModel2D(pipeDOWN[i]);
        }
        DrawModel2D(base);

        wchar_t s[80];
        swprintf(s, 80, L"Distance: %d    Highscore: %d", distance, highscore);
        spriteFont->DrawString(spriteBatch.get(), s, XMFLOAT2(50, 50), Colors::Red);

        UpdateBase();
        UpdatePipe();
        DrawModel2D(bird);
        spriteBatch->End();
        swapchain->Present(0, 0);
    }

}

void Game_End()
{
    PlaySound(hit.get());
    PlaySound(die.get());
    //Display score on messagebox
    std::wstring wstrDistance = std::to_wstring(distance);
    std::wstring wstrMessage = L"Distance Traveled: " + wstrDistance;
    LPCWSTR sw = wstrMessage.c_str();
    MessageBox(NULL, sw, L"Score", MB_OK);

    //Writing highscore
    if (distance > highscore) {

        std::ofstream outfile;
        outfile.open(".\\score.txt");
        outfile << distance;
        outfile.close();
    }

    bird.texture->Release();
    base.texture->Release();
    background.texture->Release();
    pipeDOWN[0].texture->Release();
    pipeUP[0].texture->Release();
    CleanD3D();
    exit(0);
}
