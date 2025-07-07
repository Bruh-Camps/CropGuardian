// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "CSV.h"
#include "Random.h"
#include "Game.h"

#include "BuildTowerHUD.h"
#include "MainHUD.h"
#include "SpatialHashing.h"
#include "StartGameHUD.h"
#include "Actors/Actor.h"
#include "Actors/Base.h"
#include "Actors/Block.h"
#include "Actors/Enemy/Slime.h"
#include "Actors/Tower/NormalTower.h"
#include "Actors/BuildSpot.h"
#include "Actors/EnemyPortal.h"
#include "UIElements/UIScreen.h"
#include "Components/DrawComponents/DrawComponent.h"
#include "Components/DrawComponents/DrawSpriteComponent.h"
#include "Components/DrawComponents/DrawPolygonComponent.h"
#include "Components/ColliderComponents/AABBColliderComponent.h"

Game::Game(int windowWidth, int windowHeight)
        :mWindow(nullptr)
        ,mRenderer(nullptr)
        ,mTicksCount(0)
        ,mIsRunning(true)
        ,mWindowWidth(windowWidth)
        ,mWindowHeight(windowHeight)
        ,mMainHUD(nullptr)
        ,mBuildTowerHUD(nullptr)
        ,mStartGameHUD(nullptr)
        ,mBackgroundColor(0, 0, 0)
        ,mModColor(255, 255, 255)
        ,mCameraPos(Vector2::Zero)
        ,mAudio(nullptr)
        ,mGameTimer(0.0f)
        ,mLevelTimer(0)
        ,mLevelCoins(0)
        ,mSceneManagerTimer(0.0f)
        ,mSceneManagerState(SceneManagerState::None)
        ,mGameScene(GameScene::MainMenu)
        ,mNextScene(GameScene::MainMenu)
        ,mBackgroundTexture(nullptr)
        ,mBackgroundSize(Vector2::Zero)
        ,mBackgroundPosition(Vector2::Zero)
{

}

bool Game::Initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow("Crop Guardian", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mWindowWidth, mWindowHeight, 0);
    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!mRenderer)
    {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return false;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0)
    {
        SDL_Log("Unable to initialize SDL_image: %s", SDL_GetError());
        return false;
    }

    // Initialize SDL_ttf
    if (TTF_Init() != 0)
    {
        SDL_Log("Failed to initialize SDL_ttf");
        return false;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
    {
        SDL_Log("Failed to initialize SDL_mixer - error: %s" , Mix_GetError());
        SDL_Log("Audio driver: %s", SDL_GetAudioDeviceName(0, 0));
        return false;
    }


    // Start random number generator
    Random::Init();

    mAudio = new AudioSystem(8);


    mSpatialHashing = new SpatialHashing(TILE_SIZE * 4.0f,
                                         LEVEL_WIDTH * TILE_SIZE,
                                         LEVEL_HEIGHT * TILE_SIZE);
    mTicksCount = SDL_GetTicks();

    // Init all game actors
    SetGameScene(GameScene::MainMenu);

    // Initially, change scene to MainMenu
    //ChangeScene();

    return true;
}

void Game::SetGameScene(Game::GameScene scene, float transitionTime)
{
    if (mSceneManagerState == SceneManagerState::None) {
        if (scene == GameScene::MainMenu || scene == GameScene::CornFieldsMap || scene == GameScene::Map2 || scene == GameScene::GameOver || scene == GameScene::Victory) {
            mNextScene = scene;
            mSceneManagerState = SceneManagerState::Entering;
            mSceneManagerTimer = transitionTime;
        } else {
            SDL_Log("Invalid scene");
            return;
        }
    } else {
        SDL_Log("SceneManager state is not None");
        return;
    }

}

void Game::ResetGameScene(float transitionTime)
{
    SetGameScene(mGameScene, transitionTime);
}

void Game::ChangeScene()
{
    // Unload current Scene
    UnloadScene();

    // Reset camera position
    mCameraPos.Set(0.0f, 0.0f);

    // Reset game timer
    mGameTimer = 0.0f;

    // Reset gameplay state
    mGamePlayState = GamePlayState::Playing;

    // Reset scene manager state
    mSpatialHashing = new SpatialHashing(TILE_SIZE * 4.0f, LEVEL_WIDTH * TILE_SIZE, LEVEL_HEIGHT * TILE_SIZE);

    mGameScene = mNextScene;

    // Scene Manager FSM: using if/else instead of switch
    if (mNextScene == GameScene::MainMenu)
    {
        // Set background color
        mBackgroundColor.Set(166.0f, 176.0f, 79.0f);

        // Initialize main menu actors
        LoadMainMenu();
    }
    else if (mNextScene == GameScene::CornFieldsMap)
    {
        mMainHUD = new MainHUD(this, "../Assets/Fonts/SMB.ttf", mRenderer);
        mMainHUD->SetLives(5, 5);

        // Seta o próximo nível e cria o portal
        mCurrentLevel = 0;
        SetupLevelProgression();
        StartNextLevel();

        // Zera as moedas e o ajusta o tempo (em segundos) entre as ondas
        mLevelTimer = 0;
        mLevelCoins = 0;

        mBuildTowerHUD = new BuildTowerHUD(this, "../Assets/Fonts/SMB.ttf", mRenderer);

        mStartGameHUD = new StartGameHUD(this, "../Assets/Fonts/SMB.ttf", mRenderer);
        TogglePause();

        mMusicHandle = mAudio->PlaySound("MusicMain.mp3", true);

        // Set background color
        mBackgroundColor.Set(166.0f, 176.0f, 79.0f);

        // Initialize actors
        LoadLevel("../Assets/Levels/level1_path.csv", "../Assets/Levels/level1_scenary.csv", LEVEL_WIDTH, LEVEL_HEIGHT);

        // Base a ser defendida (o número de vidas deve condizer com o HUD)
        auto base = new Base(this, 5, 200);
        base->SetPosition(Vector2(39 * TILE_SIZE, (12 * TILE_SIZE - TILE_SIZE/2)));
        mCurrentBase = base;

    }
    else if (mNextScene == GameScene::Map2) {
        // TODO: Map 2
    }else if (mNextScene == GameScene::GameOver) {
        // Set background color
        mGamePlayState = GamePlayState::GameOver;

        mBackgroundColor.Set(166.0f, 176.0f, 79.0f);

        LoadGameOverScreen();
    }else if (mNextScene == GameScene::Victory) {
        // Set background color
        mGamePlayState = GamePlayState::Victory;

        mBackgroundColor.Set(166.0f, 176.0f, 79.0f);

        LoadVictoryScreen();
    }

}

//cria uma tela de UI e adicionar os elementos do menu principal.
void Game::LoadMainMenu()
{
    auto mainMenu = new UIScreen(this, "../Assets/Fonts/Old_School_Adventures.ttf", mRenderer);

    //mainMenu->AddText("CROP GUARDIAN", Vector2(170.0f, 50.0f), Vector2(300.0f, 30.0f), 48 /*estava 60*/);

    const Vector2 titleSize = Vector2(mWindowWidth, mWindowHeight);
    const Vector2 titlePos = Vector2(0.0f, 0.0f);
    mainMenu->AddImage("../Assets/Sprites/Logo.png", titlePos, titleSize);

    auto button1 = mainMenu->AddImageButton(
        "../Assets/Sprites/Buttons/play_button.png",
        Vector2(mWindowWidth / 2.0f - 353.0f, mWindowHeight - 180.0f),
        Vector2(288.0f, 96.0f),
        [this]() {
            PlaySound("menu_selected.wav", false);
            SetGameScene(GameScene::CornFieldsMap);
        }
    );

    auto button2 = mainMenu->AddImageButton(
        "../Assets/Sprites/Buttons/exit_button.png",
        Vector2(mWindowWidth/2.0f - 45.0f, mWindowHeight - 180.0f),
        Vector2(288.0f, 96.0f),
        [this](){
            Quit();
        }
    );
}

void Game::LoadGameOverScreen()
{
    // Para música anterior
    if (mAudio) {
        mAudio->StopSound(mMusicHandle);
        mMusicHandle = mAudio->PlaySound("GameOverTheme.mp3", false);
    }

    auto gameOverMenu = new UIScreen(this, "../Assets/Fonts/Old_School_Adventures.ttf", mRenderer);

    const Vector2 titleSize = Vector2(mWindowWidth, mWindowHeight);
    const Vector2 titlePos = Vector2(0.0f, 0.0f);
    gameOverMenu->AddImage("../Assets/Sprites/GameOver.png", titlePos, titleSize);

    gameOverMenu->AddImageButton(
        "../Assets/Sprites/Buttons/menu_button.png",
        Vector2(mWindowWidth / 2.0f - 328.0f, 4.0f * (mWindowHeight / 5.0f)),
        Vector2(288.0f, 96.0f),
        [this]() {
            if (mSceneManagerState == SceneManagerState::None) {
                PlaySound("menu_selected.wav", false);
                SetGameScene(GameScene::MainMenu);
            }
        }
    );

    gameOverMenu->AddImageButton(
        "../Assets/Sprites/Buttons/exit_button.png",
        Vector2(mWindowWidth / 2.0f, 4.0f * (mWindowHeight / 5.0f)),
        Vector2(288.0f, 96.0f),
        [this]() {
            Quit();
        }
    );
}

void Game::LoadVictoryScreen()
{
    // Para música anterior
    if (mAudio) {
        mAudio->StopSound(mMusicHandle);
        mMusicHandle = mAudio->PlaySound("Victory.wav", false);
    }

    auto victoryMenu = new UIScreen(this, "../Assets/Fonts/Old_School_Adventures.ttf", mRenderer);

    const Vector2 titleSize = Vector2(mWindowWidth, mWindowHeight);
    const Vector2 titlePos = Vector2(0.0f, 0.0f);
    victoryMenu->AddImage("../Assets/Sprites/Victory.png", titlePos, titleSize);

    // Adiciona botão para voltar ao menu
    victoryMenu->AddImageButton(
        "../Assets/Sprites/Buttons/menu_button.png",
        Vector2(mWindowWidth / 2.0f - 328.0f, 4.0f * (mWindowHeight / 5.0f)),
        Vector2(288.0f, 96.0f),
        [this]() {
            if (mSceneManagerState == SceneManagerState::None) {
                PlaySound("menu_selected.wav", false);
                SetGameScene(GameScene::MainMenu);
            }
        }
    );

    victoryMenu->AddImageButton(
        "../Assets/Sprites/Buttons/exit_button.png",
        Vector2(mWindowWidth / 2.0f, 4.0f * (mWindowHeight / 5.0f)),
        Vector2(288.0f, 96.0f),
        [this]() {
            Quit();
        }
    );
}

void Game::LoadLevel(const std::string& levelPath, const std::string& scenaryPath, const int levelWidth, const int levelHeight)
{
    int **mLevelData = ReadLevelData(levelPath, levelWidth, levelHeight);

    int **mLevelScenary = ReadLevelData(scenaryPath, levelWidth, levelHeight);

    if (!mLevelData) {
        SDL_Log("Failed to load level data");
        return;
    }

    // Instantiate level actors
    BuildLevel(mLevelData, mLevelScenary, levelWidth, levelHeight);
}

void Game::BuildLevel(int** levelData, int** levelScenary, int width, int height)
{
    mLevelData = levelData;

    const std::map<int, const std::string> tileMap = {
        {13, "../Assets/Sprites/Blocks/grass.png"},
        {6,  "../Assets/Sprites/Blocks/path_horizontal.png"},
        {3,  "../Assets/Sprites/Blocks/path_vertical.png"},
        {12, "../Assets/Sprites/Blocks/botton_right.png"},
        {10, "../Assets/Sprites/Blocks/top_right.png"},
        {2,  "../Assets/Sprites/Blocks/botton_left.png"},
        {17, "../Assets/Sprites/Blocks/top_left.png"}
    };

    const std::map<int, const std::string> scenaryMap = {
        {0,  "../Assets/Levels/Scenario/12.png"},
        {1,  "../Assets/Levels/Scenario/13.png"},
        {2,  "../Assets/Levels/Scenario/15.png"},
        {3,  "../Assets/Levels/Scenario/Autumn_bush1.png"},
        {4,  "../Assets/Levels/Scenario/Blue-green_crystal_light_shadow3.png"},
        {5,  "../Assets/Levels/Scenario/Bush_blue_flowers1.png"},
        {6,  "../Assets/Levels/Scenario/Bush_orange_flowers1.png"},
        {7,  "../Assets/Levels/Scenario/Bush_pink_flowers1.png"},
        {8,  "../Assets/Levels/Scenario/Bush_red_flowers1.png"},
        {9,  "../Assets/Levels/Scenario/Bush_simple1_1.png"},
        {10, "../Assets/Levels/Scenario/Cactus2_1.png"},
        {11, "../Assets/Levels/Scenario/corn.png"},
        {12, "../Assets/Levels/Scenario/Fern1_1.png"},
        {13, "../Assets/Levels/Scenario/Fern2_1.png"},
        {14, "../Assets/Levels/Scenario/mushroom1_light_shadow3.png"},
        {15, "../Assets/Levels/Scenario/mushroom4_light_shadow3.png"},
        {16, "../Assets/Levels/Scenario/Orange_mushrooms2_grass_shadow.png"},
        {17, "../Assets/Levels/Scenario/Snow_bush1.png"},
        {18, "../Assets/Levels/Scenario/Stone_pyramid2_grass_shadow.png"},
        {19, "../Assets/Levels/Scenario/Stone_pyramid3_grass_shadow.png"},
        {20, "../Assets/Levels/Scenario/Stone_pyramid3_ground_shadow.png"},
        {21, "../Assets/Levels/Scenario/Tree1.png"},
        {22, "../Assets/Levels/Scenario/6.png"},
        {23, "../Assets/Levels/Scenario/5.png"}
    };

    std::vector<std::vector<bool>> consumed(height, std::vector<bool>(width, false));

    // --- Primeiro loop: terreno e blocos sólidos ---
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            if (consumed[y][x]) continue;

            int tile = levelData[y][x];

            if (tile == 100000)
            {
                // TODO: Portal de inimigos
                continue;
            }

            // Verifica formação de BuildSpot (bloco 2x2)
            if (tile == TILE_PLACE_TL &&
                x + 1 < width && y + 1 < height &&
                levelData[y][x + 1] == TILE_PLACE_TR &&
                levelData[y + 1][x] == TILE_PLACE_BL &&
                levelData[y + 1][x + 1] == TILE_PLACE_BR)
            {
                BuildSpot* spot = new BuildSpot(this, Vector2(2 * TILE_SIZE, 2 * TILE_SIZE));
                spot->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
                consumed[y][x] = consumed[y][x + 1] = consumed[y + 1][x] = consumed[y + 1][x + 1] = true;
                continue;
            }

            // Tile comum (grama, estrada, curva, etc)
            auto it = tileMap.find(tile);
            if (it != tileMap.end())
            {
                Block* block = new Block(this, it->second, Game::TILE_SIZE, Game::TILE_SIZE, true, 1); // camada 1, sólido
                block->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
            }
        }
    }

    // --- Segundo loop: objetos de cenário decorativos (camada 2) ---
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int scenaryId = levelScenary[y][x];
            auto it = scenaryMap.find(scenaryId);
            if (it != scenaryMap.end())
            {
                auto [imgW, imgH] = GetImageSize(mRenderer, it->second);
                Block* decor = new Block(this, it->second, imgW, imgH, false, 2); // camada 2, não sólido

                decor->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
            }
        }
    }
}


int **Game::ReadLevelData(const std::string& fileName, int width, int height)
{
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        SDL_Log("Failed to load paths: %s", fileName.c_str());
        return nullptr;
    }

    // Create a 2D array of size width and height to store the level data
    int** levelData = new int*[height];
    for (int i = 0; i < height; ++i)
    {
        levelData[i] = new int[width];
    }

    // Read the file line by line
    int row = 0;

    std::string line;
    while (!file.eof())
    {
        std::getline(file, line);
        if(!line.empty())
        {
            auto tiles = CSVHelper::Split(line);

            if (tiles.size() != width) {
                SDL_Log("Invalid level data");
                return nullptr;
            }

            for (int i = 0; i < width; ++i) {
                levelData[row][i] = tiles[i];
            }
        }

        ++row;
    }

    // Close the file
    file.close();

    return levelData;
}

int Game::GetTileAt(int x, int y) const
{
    if (x >= 0 && x < LEVEL_WIDTH && y >= 0 && y < LEVEL_HEIGHT)
    {
        return mLevelData[y+1][x+1]; // y antes porque é uma matriz de linhas
    }
    return -1; // valor inválido para indicar erro
}

std::pair<int, int> Game::GetImageSize(SDL_Renderer* renderer, const std::string& path)
{
    SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
    if (!texture)
    {
        SDL_Log("Erro ao carregar textura %s: %s", path.c_str(), SDL_GetError());
        return { 32, 32 }; // fallback
    }

    int w, h;
    SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);
    SDL_DestroyTexture(texture); // evita vazamento de memória

    return { w, h };
}

void Game::RunLoop()
{
    while (mIsRunning)
    {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                Quit();
                break;
            case SDL_KEYDOWN:
                // Handle key press for UI screens
                if (!mUIStack.empty()) {
                    mUIStack.back()->HandleKeyPress(event.key.keysym.sym);
                }

                HandleKeyPressActors(event.key.keysym.sym, event.key.repeat == 0);

                // Check if the Return key has been pressed to pause/unpause the game
                if (event.key.keysym.sym == SDLK_RETURN)
                {
                    TogglePause();
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    int mouseX = event.button.x;
                    int mouseY = event.button.y;

                    if (mMainHUD) {
                        mMainHUD->ProcessMouseClick(mouseX, mouseY);
                    }

                    if (mBuildTowerHUD && mBuildTowerHUD->isVisible()) {
                        mBuildTowerHUD->ProcessMouseClick(mouseX, mouseY);
                    }

                    if (!mUIStack.empty() && !mMainHUD && !mBuildTowerHUD) {
                        mUIStack.back()->ProcessMouseClick(mouseX, mouseY);
                    }

                    for (auto* a : mSpatialHashing->QueryOnCamera(
                                       mCameraPos, mWindowWidth, mWindowHeight))
                    {
                        if (auto* bs = dynamic_cast<BuildSpot*>(a))
                            if (bs->ContainsScreenPoint(mouseX, mouseY)) {
                                bs->OnMouseClick();
                                break;
                            }
                    }
                }
            break;
        }
    }

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    if (!mUIStack.empty()) {
        mUIStack.back()->ProcessMouseHover(mouseX, mouseY);
    }

    ProcessMouseHover();

    ProcessInputActors();
}

void Game::ProcessMouseHover() {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    BuildSpot* newHoveredSpot = nullptr;
    for (auto* a : mSpatialHashing->QueryOnCamera(mCameraPos, mWindowWidth, mWindowHeight))
    {
        if (auto* bs = dynamic_cast<BuildSpot*>(a))
        {
            if (bs->ContainsScreenPoint(mouseX, mouseY))
            {
                newHoveredSpot = bs;
                break;
            }
        }
    }

    if (mHoveredSpot && mHoveredSpot != newHoveredSpot)
    {
        mHoveredSpot->OnHoverExit();
    }

    if (newHoveredSpot && newHoveredSpot != mHoveredSpot)
    {
        newHoveredSpot->OnHoverEnter();
    }

    mHoveredSpot = newHoveredSpot;
}

void Game::ProcessInputActors()
{
    if(mGamePlayState == GamePlayState::Playing || mGamePlayState == GamePlayState::WaitingNextWave)
    {
        // Get actors on camera
        std::vector<Actor*> actorsOnCamera =
                mSpatialHashing->QueryOnCamera(mCameraPos,mWindowWidth,mWindowHeight);

        const Uint8* state = SDL_GetKeyboardState(nullptr);

        bool isMarioOnCamera = false;
        for (auto actor: actorsOnCamera)
        {
            actor->ProcessInput(state);

        }
    }
}

void Game::HandleKeyPressActors(const int key, const bool isPressed)
{
    if(mGamePlayState == GamePlayState::Playing)
    {
        // Get actors on camera
        std::vector<Actor*> actorsOnCamera =
                mSpatialHashing->QueryOnCamera(mCameraPos,mWindowWidth,mWindowHeight);

        // Handle key press for actors
        bool isMarioOnCamera = false;
        for (auto actor: actorsOnCamera) {
            actor->HandleKeyPress(key, isPressed);

        }

    }

}

void Game::TogglePause()
{
    if (mGameScene != GameScene::MainMenu)
    {
        if (mGamePlayState == GamePlayState::Playing || mGamePlayState == GamePlayState::WaitingNextWave)
        {
            mGamePlayState = GamePlayState::Paused;

            mMainHUD->UpdatePauseButtonIcon(true);
            mStartGameHUD->Show();

            if (mMusicHandle.IsValid()) {
                mAudio->PauseSound(mMusicHandle);
            }
            mAudio->PlaySound("pause.mp3");
        }
        else if (mGamePlayState == GamePlayState::Paused || mGamePlayState == GamePlayState::WaitingNextWave)
        {
            mGamePlayState = GamePlayState::Playing;

            mMainHUD->UpdatePauseButtonIcon(false);
            mStartGameHUD->Hide();

            mAudio->ResumeSound(mMusicHandle);
            mAudio->PlaySound("pause.mp3");
        }
    }

}

void Game::UpdateGame()
{
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));

    float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
    if (deltaTime > 0.05f)
    {
        deltaTime = 0.05f;
    }

    mTicksCount = SDL_GetTicks();

    if(mGamePlayState != GamePlayState::Paused && mGamePlayState != GamePlayState::GameOver)
    {
        // Reinsert all actors and pending actors
        UpdateActors(deltaTime);
    }

    // Reinsert audio system
    mAudio->Update(deltaTime);

    // Reinsert UI screens
    for (auto ui : mUIStack) {
        if (ui->GetState() == UIScreen::UIState::Active) {
            ui->Update(deltaTime);
        }
    }

    // Delete any UIElements that are closed
    auto iter = mUIStack.begin();
    while (iter != mUIStack.end()) {
        if ((*iter)->GetState() == UIScreen::UIState::Closing) {
            delete *iter;
            iter = mUIStack.erase(iter);
        } else {
            ++iter;
        }
    }

    // ---------------------
    // Game Specific Updates
    // ---------------------

    UpdateSceneManager(deltaTime);

    if (mGameScene == GameScene::CornFieldsMap && mGamePlayState == GamePlayState::WaitingNextWave && mMainHUD) {
        UpdateLevelTime(deltaTime);
    }

    if (mGameScene == GameScene::CornFieldsMap && mCurrentPortal && mCurrentPortal->AreAllWavesFinished() && mEnemyCount == 0 && mSceneManagerState == SceneManagerState::None) {
        StartNextLevel();
    }
}

void Game::UpdateSceneManager(float deltaTime)
{
    if (mSceneManagerState == SceneManagerState::Entering) {
        mSceneManagerTimer -= deltaTime;
        if (mSceneManagerTimer <= 0) {
            mSceneManagerTimer = 0.0f; // Garante que não seja negativo para cálculo do alfa
            ChangeScene(); // Troca a cena quando a tela está totalmente preta
            mSceneManagerTimer = TRANSITION_TIME;
            mSceneManagerState = SceneManagerState::Active; // Inicia a fase de Fade-In
        }

    }

    if (mSceneManagerState == SceneManagerState::Active) { // Fase de Fade-In
        mSceneManagerTimer -= deltaTime;
        if (mSceneManagerTimer <= 0) {
            mSceneManagerTimer = 0.0f; // Garante que não seja negativo
            mSceneManagerState = SceneManagerState::None; // Transição concluída
        }
    }
}

// Quando estiver esperando por outra onda o timer do level começa a decrescer
void Game::UpdateLevelTime(float deltaTime)
{
    mGameTimer += deltaTime;
    if (mGameTimer >= 1.0f) {
        mGameTimer = 0;
        mLevelTimer++;
        if (mMainHUD) {
            mMainHUD->SetTime(mLevelTimer);
        }
    }
}

void Game::UpdateLevelCoins() {
    if (mMainHUD != nullptr) {
        mMainHUD->SetCoinCount(mLevelCoins);
    }
}

void Game::UpdateCamera() {}

void Game::UpdateActors(float deltaTime)
{
    // Get actors on camera
    std::vector<Actor*> actorsOnCamera =
        mSpatialHashing->QueryOnCamera(mCameraPos,mWindowWidth,mWindowHeight);

    for (auto actor : actorsOnCamera) //Só está fazendo update dos actors on camera
    {

        actor->Update(deltaTime);

        if (actor->GetState() == ActorState::Destroy)
        {
            mSpatialHashing->Remove(actor);
            delete actor;
        }

    }
}

void Game::AddActor(Actor* actor)
{
    mSpatialHashing->Insert(actor);
}

void Game::RemoveActor(Actor* actor)
{
    mSpatialHashing->Remove(actor);
}
void Game::Reinsert(Actor* actor)
{
    mSpatialHashing->Reinsert(actor);
}

std::vector<Actor *> Game::GetNearbyActors(const Vector2& position, const int range)
{
    return mSpatialHashing->Query(position, range);
}

std::vector<AABBColliderComponent *> Game::GetNearbyColliders(const Vector2& position, const int range)
{
    return mSpatialHashing->QueryColliders(position, range);
}

void Game::GenerateOutput()
{
    // Clear frame with background color
    SDL_SetRenderDrawColor(mRenderer, mBackgroundColor.x, mBackgroundColor.y, mBackgroundColor.z, 255);

    // Clear back buffer
    SDL_RenderClear(mRenderer);

    // Draw background texture considering camera position
    if (mBackgroundTexture)
    {
        SDL_Rect dstRect = { static_cast<int>(mBackgroundPosition.x - mCameraPos.x),
                             static_cast<int>(mBackgroundPosition.y - mCameraPos.y),
                             static_cast<int>(mBackgroundSize.x),
                             static_cast<int>(mBackgroundSize.y) };

        SDL_RenderCopy(mRenderer, mBackgroundTexture, nullptr, &dstRect);
    }

    // Get actors on camera
    std::vector<Actor*> actorsOnCamera =
            mSpatialHashing->QueryOnCamera(mCameraPos,mWindowWidth,mWindowHeight);

    // Get list of drawables in draw order
    std::vector<DrawComponent*> drawables;

    for (auto actor : actorsOnCamera)
    {
        auto drawable = actor->GetComponent<DrawComponent>();
        if (drawable && drawable->IsVisible())
        {
            drawables.emplace_back(drawable);
        }
    }

    // Sort drawables by draw order
    std::sort(drawables.begin(), drawables.end(),
              [](const DrawComponent* a, const DrawComponent* b) {
                  return a->GetDrawOrder() < b->GetDrawOrder();
              });

    // Draw all drawables
    for (auto drawable : drawables)
    {
        drawable->Draw(mRenderer, mModColor);
    }

    // Draw all UI screens
    for (auto ui :mUIStack)
    {
        ui->Draw(mRenderer);
    }

    if (mSceneManagerState == SceneManagerState::Entering || mSceneManagerState == SceneManagerState::Active) {

        // Habilita o blending para transparência alfa
        SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);

        float alphaNormalized = 0.0f;
        // TRANSITION_TIME deve ser a duração de uma fase do fade (ex: 0.5 segundos)
        // mSceneManagerTimer conta de TRANSITION_TIME para 0 em cada fase.

        if (mSceneManagerState == SceneManagerState::Entering) // Fade-Out: Alfa de 0 para 1 (transparente para opaco)
        {
            // Se TRANSITION_TIME for 0, evite divisão por zero
            if (TRANSITION_TIME > 0.0f) {
                alphaNormalized = 1.0f - (mSceneManagerTimer / TRANSITION_TIME);
            } else {
                alphaNormalized = 1.0f; // Instantaneamente opaco se tempo de transição for zero
            }
        }
        else // SceneManagerState::Active - Fade-In: Alfa de 1 para 0 (opaco para transparente)
        {
            // Se TRANSITION_TIME for 0, evite divisão por zero
            if (TRANSITION_TIME > 0.0f) {
                alphaNormalized = mSceneManagerTimer / TRANSITION_TIME;
            } else {
                alphaNormalized = 0.0f; // Instantaneamente transparente se tempo de transição for zero
            }
        }

        // Garante que o alfa normalizado esteja entre 0.0 e 1.0
        alphaNormalized = std::max(0.0f, std::min(1.0f, alphaNormalized));
        Uint8 alpha = static_cast<Uint8>(alphaNormalized * 255.0f);

        SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, alpha);

        SDL_Rect fullScreenRect;
        fullScreenRect.x = 0;
        fullScreenRect.y = 0;
        fullScreenRect.w = mWindowWidth;
        fullScreenRect.h = mWindowHeight;

        SDL_RenderFillRect(mRenderer, &fullScreenRect);
    }

    // Swap front buffer and back buffer
    SDL_RenderPresent(mRenderer);
}

void Game::SetBackgroundImage(const std::string& texturePath, const Vector2 &position, const Vector2 &size)
{
    if (mBackgroundTexture) {
        SDL_DestroyTexture(mBackgroundTexture);
        mBackgroundTexture = nullptr;
    }

    // Load background texture
    mBackgroundTexture = LoadTexture(texturePath);
    if (!mBackgroundTexture) {
        SDL_Log("Failed to load background texture: %s", texturePath.c_str());
    }

    // Set background position
    mBackgroundPosition.Set(position.x, position.y);

    // Set background size
    mBackgroundSize.Set(size.x, size.y);
}

SDL_Texture* Game::LoadTexture(const std::string& texturePath)
{
    SDL_Surface* surface = IMG_Load(texturePath.c_str());

    if (!surface) {
        SDL_Log("Failed to load image: %s", IMG_GetError());
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(mRenderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
        return nullptr;
    }

    return texture;
}


UIFont* Game::LoadFont(const std::string& fileName)
{
    if (mFonts.find(fileName) != mFonts.end()) {
        return mFonts[fileName];
    }

    UIFont* font = new UIFont(mRenderer);
    if (font->Load(fileName)) {
        mFonts[fileName] = font;
        return font;
    }

    font->Unload();
    delete font;
    return nullptr;
}

void Game::UnloadScene()
{
    // Delete actors
    if (mSpatialHashing) {
        delete mSpatialHashing;
        mSpatialHashing = nullptr;
    }

    // Delete UI screens
    for (auto ui : mUIStack) {
        delete ui;
    }
    mUIStack.clear();

    // Delete background texture
    if (mBackgroundTexture) {
        SDL_DestroyTexture(mBackgroundTexture);
        mBackgroundTexture = nullptr;
    }

    // Desaloca os dados do nível
    if (mLevelData) {
        for (int i = 0; i < LEVEL_HEIGHT; ++i) {
            delete[] mLevelData[i];
        }
        delete[] mLevelData;
        mLevelData = nullptr;
    }

    mMainHUD = nullptr;
    mBuildTowerHUD = nullptr;
    mStartGameHUD = nullptr;

    mCurrentBase = nullptr;
    mCurrentPortal = nullptr;
    mHoveredSpot = nullptr;

    mEnemyCount = 0;

    mLevelProgression.clear();
}

void Game::Shutdown()
{
    UnloadScene();

    for (auto font : mFonts) {
        font.second->Unload();
        delete font.second;
    }
    mFonts.clear();

    delete mAudio;
    mAudio = nullptr;

    Mix_CloseAudio();

    Mix_Quit();
    TTF_Quit();
    IMG_Quit();

    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void Game::PlaySound(const std::string& soundName, bool looping)
{
    if (mAudio)
    {
        mAudio->PlaySound(soundName, looping);
    }
}

void Game::SetupLevelProgression() {
    // Nível 1: 3 waves, 10s de espera
    mLevelProgression.push_back({3, 7.0f, 5.0f});

    // Nível 2: 5 waves, 8s de espera
    mLevelProgression.push_back({5, 5.0f, 5.0f});

    // Nível 3: 7 waves, 6s de espera
    mLevelProgression.push_back({7, 3.0f, 3.0f});
}

void Game::StartNextLevel() {
    mCurrentLevel++;

    if (mCurrentLevel > mLevelProgression.size() && mGameScene != GameScene::Victory) {
        SetGameScene(GameScene::Victory, 1.0f);
        return;
    }

    const LevelDefinition& currentDef = mLevelProgression[mCurrentLevel - 1];

    // Cria um novo portal para o nível
    // Remove o portal antigo se ele existir
    if (mCurrentPortal) {
        mCurrentPortal->SetState(ActorState::Destroy);
    }
    mCurrentPortal = new EnemyPortal(this, currentDef.numberOfWaves, currentDef.timeBetweenWaves, currentDef.initialDelay);
    mCurrentPortal->SetPosition(Vector2(0, (17 * TILE_SIZE + TILE_SIZE/2 - 4)));

    // Atualiza o HUD
    if (mMainHUD) {
        mMainHUD->SetLevel(mCurrentLevel);
    }
}

int Game::GetEnemiesPerWaveForCurrentLevel() const
{
    // Adiciona 2 inimigos na wave a cada fase
    return 3 + (mCurrentLevel - 1) * 2;
}

float Game::GetEnemySpawnIntervalForCurrentLevel() const
{
    // O intervalo de spawn diminui em 0.2s, com um mínimo de 0.5s
    return std::max(0.5f, 2.0f - (mCurrentLevel - 1) * 0.2f);
}

float Game::GetBeeSpawnChanceForCurrentLevel() const
{
    // A chance de uma abelha aparecer aumenta 10% a cada fase, com um máximo de 60%. Inicia em 30%
    return std::min(0.6f, 0.20f + (mCurrentLevel - 1) * 0.10f);
}