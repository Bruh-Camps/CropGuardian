// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <SDL.h>
#include <vector>
#include <unordered_map>
#include "AudioSystem.h"
#include "Math.h"
#include "Actors/Base.h"

class StartGameHUD;
class MainHUD;
class BuildTowerHUD;

class Game
{
public:
    static const int LEVEL_WIDTH = 40;
    static const int LEVEL_HEIGHT = 23;
    static const int TILE_SIZE = 32;
    static const int SPAWN_DISTANCE = 700;
    static const int TRANSITION_TIME = 1;

    static const int TILE_PLACE_TL = 0;   // top-left  of a build spot
    static const int TILE_PLACE_TR = 1;   // top-right
    static const int TILE_PLACE_BL = 4;   // bottom-left
    static const int TILE_PLACE_BR = 5;   // bottom-right
    static const int BUILD_SPOT_W  = 2 * TILE_SIZE;
    static const int BUILD_SPOT_H  = 2 * TILE_SIZE;

    enum class GameScene
    {
        MainMenu,
        CornFieldsMap,
        Map2,
        GameOver,
        Victory
    };

    enum class SceneManagerState
    {
        None,
        Entering,
        Active,
        Exiting
    };

    enum class GamePlayState
    {
        Playing,
        Paused,
        WaitingNextWave,
        GameOver,
        LevelComplete,
        Leaving,
        Victory
    };

    struct LevelDefinition {
        int numberOfWaves;
        float timeBetweenWaves;
        float initialDelay;
    };

    Game(int windowWidth, int windowHeight);

    bool Initialize();
    void RunLoop();
    void Shutdown();
    void Quit() { mIsRunning = false; }

    // Actor functions
    void UpdateActors(float deltaTime);
    void AddActor(class Actor* actor);
    void RemoveActor(class Actor* actor);
    void ProcessInputActors();
    void HandleKeyPressActors(const int key, const bool isPressed);

    // Level functions
    void LoadMainMenu();
    void LoadGameOverScreen();
    void LoadVictoryScreen();
    void LoadLevel(const std::string& levelName, const int levelWidth, const int levelHeight);
    int GetTileAt(int x, int y) const;
    int GetLevelTime() const {return mLevelTimer;};
    MainHUD* GetMainHUD() const {return mMainHUD;};
    StartGameHUD* GetStartGameHUD() const {return mStartGameHUD;};

    std::vector<Actor *> GetNearbyActors(const Vector2& position, const int range = 1);
    std::vector<class AABBColliderComponent *> GetNearbyColliders(const Vector2& position, const int range = 2);

    void Reinsert(Actor* actor);

    // Camera functions
    Vector2& GetCameraPos() { return mCameraPos; };
    void SetCameraPos(const Vector2& position) { mCameraPos = position; };

    // Audio functions
    class AudioSystem* GetAudio() { return mAudio; }

    // UI functions
    void PushUI(class UIScreen* screen) { mUIStack.emplace_back(screen); }
    const std::vector<class UIScreen*>& GetUIStack() { return mUIStack; }

    // Window functions
    int GetWindowWidth() const { return mWindowWidth; }
    int GetWindowHeight() const { return mWindowHeight; }

    // Loading functions
    class UIFont* LoadFont(const std::string& fileName);
    SDL_Texture* LoadTexture(const std::string& texturePath);

    void SetGameScene(GameScene scene, float transitionTime = 1.0f);
    void ResetGameScene(float transitionTime = 1.0f);
    void UnloadScene();

    void SetBackgroundImage(const std::string& imagePath, const Vector2 &position = Vector2::Zero, const Vector2& size = Vector2::Zero);
    void TogglePause();

    // Game-specific
    Base* GetCurrentBase() const { return mCurrentBase; }

    void ProcessMouseHover();

    void SetGamePlayState(GamePlayState state) { mGamePlayState = state; }
    GamePlayState GetGamePlayState() const { return mGamePlayState; }

    BuildTowerHUD* GetBuildTowerHUD() const { return mBuildTowerHUD; }

    void PlaySound(const std::string& soundName, bool looping = false);

    void AdvanceToNextLevel();
    int GetCurrentLevel() const { return mCurrentLevel; }

    int GetEnemiesPerWaveForCurrentLevel() const;
    float GetEnemySpawnIntervalForCurrentLevel() const;
    float GetBeeSpawnChanceForCurrentLevel() const;

    void IncrementEnemyCount() { mEnemyCount++; }
    void DecrementEnemyCount() { mEnemyCount--; }

private:
    void ProcessInput();
    void UpdateGame();
    void UpdateCamera();
    void GenerateOutput();

    // Scene Manager
    void UpdateSceneManager(float deltaTime);
    void ChangeScene();
    SceneManagerState mSceneManagerState;
    float mSceneManagerTimer;

    // HUD functions
    void UpdateLevelTime(float deltaTime);
    void UpdateLevelCoins();

    // Load the level from a CSV file as a 2D array
    int **ReadLevelData(const std::string& fileName, int width, int height);
    void BuildLevel(int** levelData, int width, int height);
    int** mLevelData = nullptr;

    // Spatial Hashing for collision detection
    class SpatialHashing* mSpatialHashing;

    // All the UI elements
    std::vector<class UIScreen*> mUIStack;
    std::unordered_map<std::string, class UIFont*> mFonts;

    // SDL stuff
    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;
    AudioSystem* mAudio;

    // Window properties
    int mWindowWidth;
    int mWindowHeight;

    // Track elapsed time since game start
    Uint32 mTicksCount;

    // Track actors state
    bool mIsRunning;
    GamePlayState mGamePlayState;

    // Track level state
    GameScene mGameScene;
    GameScene mNextScene;

    // Background and camera
    Vector3 mBackgroundColor;
    Vector3 mModColor;
    Vector2 mCameraPos;

    // Game-specific
    Base* mCurrentBase;
    SoundHandle mMusicHandle;

    class BuildSpot* mHoveredSpot = nullptr;

    class MainHUD *mMainHUD;
    class BuildTowerHUD *mBuildTowerHUD;
    class StartGameHUD *mStartGameHUD;

    float mGameTimer;
    int mLevelTimer;
    int mLevelCoins;

    int mCurrentLevel = 0;
    std::vector<LevelDefinition> mLevelProgression;

    int mEnemyCount = 0;
    class EnemyPortal* mCurrentPortal = nullptr;

    void SetupLevelProgression();
    void StartNextLevel();

    SDL_Texture *mBackgroundTexture;
    Vector2 mBackgroundSize;
    Vector2 mBackgroundPosition;
};
