#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <vector>
#include"DebugCamera.h"
#include"Player.h"
#include"Enemy.h"
#include"Skydome.h"
#include"MapChipField.h"
#include"aabb.h"
#include"CameraController.h"
#include"deathParticles.h"

// ゲームのフェーズ(型)
enum class Phase {
	kPlay,  // ゲームプレイ
	kDeath, // デス演出
};
/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();
	//すべての当たり判定を行う
	void CheckAllCollisions();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	//表示ブロックの生成
	void GenerateBlocks();

	void ChangePhase();

	// デスフラグのgetter
	bool IsFinished() const { return finished_; }


private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	//uint32_t textureHandle_ = 0;
	//モデル
	Model* model_ = nullptr;
	Model* modelEnemy_ = nullptr;
	Model* modelBlock_ = nullptr;
	Model* modelSkydome_ = nullptr;
	//ワールドトランスフォーム
	WorldTransform worldTransform_;
	//ビュープロジェクション
	ViewProjection viewProjection_;
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;
	bool isDebugcameraActive_ = false;
	DebugCamera* debugCamera_ = nullptr;
	Player* player_ = nullptr;
	std::list<Enemy*> enemies_;
	Skydome* skydome_ = nullptr;
	MapChipField* mapChipField_;
	deathParticles* deathParticles_ = nullptr;
	Model* modelParticles_ = nullptr;
    // ゲームの現在フェーズ(変数)
	Phase phase_;
	// 終了フラグ
	bool finished_ = false;
	//カメラコントローラー
	CameraController* camearaController_ = nullptr;
	CameraController::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
