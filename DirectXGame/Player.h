#pragma once
#define NOMINMAX
#include "Model.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include <numbers>
#include "Input.h"
#include <algorithm>
#include <cassert>
#include<vector>

enum class LRDirection {
	kRight,
	kLeft,
};
class Player {
public:
	// 初期化
	void Initialize(Model* model, ViewProjection* viewProjection,const Vector3&position);
	// 更新
	void Update();
	// 移動関数
	void Move();
	// 描画
	void Draw();
	const WorldTransform& GetWorldTransform() { return worldTransform_; };
	const Vector3& GetVelocity() const { return velocity_; }

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	//uint32_t textureHandle_ = 0u;
	// ビュープロジェクション
	ViewProjection* viewPlojection_ = nullptr;
	static inline const float kAcceleration = 0.1f;
	static inline const float kAttenuation = 0.05f;
	static inline const float kLimitRunSpeed = 0.5f;
	// 重力加速度(下方向)
	static inline const float kGravityAcceleration = 0.1f;
	// 最大落下速度(下方向)
	static inline const float kLimitFallSpeed = 1.0f;
	// ジャンプ初速(上方向)
	static inline const float kJumpAcceleration = 1.0f;
	LRDirection lrDirection_ = LRDirection::kRight;
	//旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	//旋回タイマー
	float turnTimer_ = 0.0f;
	//旋回時間
	static inline const float kTimeTurn = 0.3f;
	//接地状態フラグ
	bool onGround_ = true;
	
	Vector3 velocity_ = {};
};
