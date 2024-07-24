#include "WorldTransform.h"
#include"myMath.h"

void WorldTransform::UpdateMatirx() { matWorld_ = MakeAffineMatrix() }