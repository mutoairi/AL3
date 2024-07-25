#pragma once
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <vector>

	enum class MapChipType { kBlank, kBlock };

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

class MapChipField {


	public:
	uint32_t GetNumBlockVirtical();

	uint32_t GetNumBlockHorizontal();
	

	void ResetMapChipData();

	void LoadMapChipCsv(const std::string& filePath);
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);
	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);
	private:
	//1ブロックサイズ
	static inline const float kBlockWidth = 1.0f;
    static inline const float kBlockHeight = 1.0f;
	//ブロックの個数
	static inline const int kNumBlockVirtical = 20;
	static inline const int kNumBlockHorizontal = 100;

	MapChipData mapChipData_;

};
