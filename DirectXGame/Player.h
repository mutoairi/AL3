#pragma once
#include "Model.h"
#include "WorldTransform.h"
class Player {
public:
	//初期化
	void Initialize(Model*model,uint32_t textureHandles);
	//更新
	void Update();

	//描画
	void Draw();

private:
	//ワールド変換データ
	WorldTransform worldTransform_;
	//モデル
	Model* model_ = nullptr;
	//テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	
};
