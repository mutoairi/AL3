#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include<myMath.h>

GameScene::GameScene() {}

GameScene::~GameScene() { 
	delete model_;
	delete modelEnemy_;
	delete modelBlock_;
	delete modelSkydome_;
	delete mapChipField_;
	delete camearaController_;
	delete player_;
	delete deathParticles_;
	delete skydome_;
	delete debugCamera_;
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {

		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
}

void GameScene::Initialize() {
	
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	// ファイル名を指定してテクスチャを読み込む
	//textureHandle_ = TextureManager::Load("cube/cube.jpg");
	model_ = Model::CreateFromOBJ("player", true);
	modelEnemy_ = Model::CreateFromOBJ("enemy", true);
	modelBlock_ = Model::CreateFromOBJ("block", true);
	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	viewProjection_.Initialize();
	// マップチップ
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/map.csv");
	// 自キャラの生成
	player_ = new Player();
	// 自キャラの生成(モデル)
	model_ = Model::CreateFromOBJ("player", true);
	// 座標をマップチップ番号で指定
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(3, 1);
	// 自キャラの初期化
	player_->Initialize(model_, &viewProjection_, playerPosition);
	player_->SetMapChipField(mapChipField_);
	// パーティクルモデル
	modelParticles_ = Model::CreateFromOBJ("deathParticle", true);
	// 仮の生成
	deathParticles_ = new deathParticles;
	deathParticles_->Initialize(modelParticles_, &viewProjection_, playerPosition);
	// 敵の生成
	for (int32_t i = 0; i < 1; ++i) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(50 + i * 3, 18);
		newEnemy->Initialize(modelEnemy_, &viewProjection_, enemyPosition);
		enemies_.push_back(newEnemy);
	}
	
	//enemy_->SetMapChipField(mapChipField_);
	
	
	// 天球の生成
	skydome_ = new Skydome();
	// 天球3Dモデルの生成
	modelSkydome_ = Model::CreateFromOBJ("sphere", true);
	// 天球の初期化
	skydome_->Initialize(modelSkydome_, &viewProjection_);
	//ゲームプレイフェーズから開始
	phase_ = Phase::kPlay;
	//カメラコントローラー
	camearaController_ = new CameraController;
	camearaController_->Initialize();
	camearaController_->SetMovableArea(cameraArea);
	camearaController_->SetTarget(player_);
	camearaController_->Reset();

	// 全ての当たり判定を行う
	CheckAllCollisions();

	debugCamera_ = new DebugCamera(1280, 720);
	GenerateBlocks();
	
}

void GameScene::GenerateBlocks() {
     uint32_t kNumBlockHorizontal = mapChipField_->GetNumBlockHorizontal();
	 uint32_t kNumBlockVirtucal = mapChipField_->GetNumBlockVirtical();
	
	worldTransformBlocks_.resize(kNumBlockVirtucal);

	for (uint32_t i = 0; i < kNumBlockVirtucal; ++i) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}
	for (uint32_t i = 0; i < kNumBlockVirtucal; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {


			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j,i);
			}
			
			
			
		}
	}
}

void GameScene::CheckAllCollisions() {
	#pragma region 自キャラと敵の当たり判定
	//判定対象1と2の座標
	AABB aabb1, aabb2;
	//自キャラの座標
	aabb1 = player_->GetAABB();
	//自キャラと敵すべての当たり判定
	for (Enemy* enemy : enemies_) {
		aabb2 = enemy->GetAABB();
		//AABB同士の交差判定
		if (AABB::IsCollision(aabb1, aabb2)) {
		//自キャラの衝突時のコールバックを呼び出す
			player_->OnCollision(enemy);
		// 敵キャラの衝突時のコールバックを呼び出す
			enemy->OnCollision(player_);
		}
	}
	#pragma endregion

}

void GameScene::ChangePhase() {
	
		switch(phase_) {
		case Phase::kPlay:
			if (player_->IsDead()) {
				// 死亡演出フェーズに切り替え
				phase_ = Phase::kDeath;
				// 自キャラの座標を取得
				const Vector3& deathParticlesPosition = player_->GetWorldPosition();
				deathParticles_ = new deathParticles;
				deathParticles_->Initialize(modelParticles_, &viewProjection_, deathParticlesPosition);
			}
			break;
		case Phase::kDeath:
			// デス演出フェーズの処理
			if (deathParticles_ && deathParticles_->IsFinished()) {
				finished_ = true;
			}
			break;
	}
}

void GameScene::Update() {
	ChangePhase();
	// デバッグカメラの更新
	debugCamera_->Update();
#ifdef _DEBUG
	if (input_->TriggerKey(DIK_SPACE)) {
		if (isDebugcameraActive_ == true)
			isDebugcameraActive_ = false;
		else
			isDebugcameraActive_ = true;
	}
#endif
	switch (phase_) {
	case Phase::kPlay:
		// 全ての当たり判定を行う
		CheckAllCollisions();
		// 自キャラの更新
		player_->Update();
		// 敵の更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}
		// 天球の更新
		skydome_->Update();
		// カメラコントローラー
		camearaController_->Update();

		// カメラ処理
		if (isDebugcameraActive_) {
			// デバッグカメラの更新
			debugCamera_->Update();
			viewProjection_.matView = debugCamera_->GetViewProjection().matView;
			viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
			// ビュープロジェクション行列の転送
			viewProjection_.TransferMatrix();
		} else {

			viewProjection_.matView = camearaController_->GetViewProjection().matView;
			viewProjection_.matProjection = camearaController_->GetViewProjection().matProjection;

			// ビュープロジェクション行列の更新と転送
			viewProjection_.TransferMatrix();
		}
		
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {

			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock)
					continue;
				// アフィン変換の作成
				worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
				// 定数バッファに転送する
				worldTransformBlock->TransferMatrix();
			}
		}
		break;
	case Phase::kDeath:
		// カメラ処理
		if (isDebugcameraActive_) {
			// デバッグカメラの更新
			debugCamera_->Update();
			viewProjection_.matView = debugCamera_->GetViewProjection().matView;
			viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
			// ビュープロジェクション行列の転送
			viewProjection_.TransferMatrix();
		} else {

			viewProjection_.matView = camearaController_->GetViewProjection().matView;
			viewProjection_.matProjection = camearaController_->GetViewProjection().matProjection;

			// ビュープロジェクション行列の更新と転送
			viewProjection_.TransferMatrix();
		}
		// パーティクルの更新
		if (deathParticles_) {
			deathParticles_->Update();
		}
		// 敵の更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}
		// 天球の更新
		skydome_->Update();

		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {

			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock)
					continue;
				// アフィン変換の作成
				worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
				// 定数バッファに転送する
				worldTransformBlock->TransferMatrix();
			}
		}
		break;
	
	}
	
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// </summary>
	/// ここに背景スプライトの描画処理を追加できる

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	
	

	switch (phase_) {
	case Phase::kPlay: // 自キャラの描画
		player_->Draw();
		break;
	case Phase::kDeath:
		break;
	}

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			modelBlock_->Draw(*worldTransformBlock, viewProjection_);
		}
	}
	// 敵の描画
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}
	// パーティクル描画
	if (deathParticles_) {
		deathParticles_->Draw();
	}

	// 天球の描画
	skydome_->Draw();
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
