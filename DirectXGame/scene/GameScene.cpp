#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include<myMath.h>

GameScene::GameScene() {}

GameScene::~GameScene() { 
	delete model_;
	delete modelBlock_;
	delete debugCamera_;
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
	textureHandle_ = TextureManager::Load("cube/cube.jpg");
	model_ = Model::Create();
	modelBlock_ = Model::Create();
	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	viewProjection_.Initialize();
	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_->Initialize(model_, textureHandle_, &viewProjection_);
	debugCamera_ = new DebugCamera(1280, 720);
	const uint32_t kNumBlockHorizontal = 20;
	const uint32_t kNumBlockVirtucal = 10;
	const float kBlockWidth = 2.0f;
	const float kBlockHeight = 2.0f;
	worldTransformBlocks_.resize(kNumBlockVirtucal);

	for (uint32_t i = 0; i < kNumBlockVirtucal; ++i) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
		
	}
	for (uint32_t i = 0; i < kNumBlockVirtucal; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			worldTransformBlocks_[i][j] = new WorldTransform();
			worldTransformBlocks_[i][j]->Initialize();
			worldTransformBlocks_[i][j]->translation_.x = kBlockWidth * j;
			worldTransformBlocks_[i][j]->translation_.y = kBlockHeight * i;
		}
	}
}

void GameScene::Update() {
#ifdef _DEBUG
	if (input_->TriggerKey(DIK_SPACE)) {
		if (isDebugcameraActive_ == true)
			isDebugcameraActive_ = false;
		else
			isDebugcameraActive_ = true;
	}
#endif

	// カメラ処理
	if (isDebugcameraActive_) {
		// デバッグカメラの更新
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		// ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	} else {
		// ビュープロジェクション行列の更新と転送
		viewProjection_.UpdateMatrix();
	}
	// 自キャラの更新
	player_->Update();

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
	// 3Dモデル描画
	model_->Draw(worldTransform_, viewProjection_, textureHandle_);
	// 自キャラの描画
		player_->Draw();
	
	for(std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_){
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			modelBlock_->Draw(*worldTransformBlock, viewProjection_);
		}
	}


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
