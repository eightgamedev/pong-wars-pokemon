# pragma once
# include "Common.hpp"
# include "BallWithMultipleType.hpp"
# include "BlockWithMultipleType.hpp"

class MultipleTypeWar : public App::Scene
{
public:
	MultipleTypeWar(const InitData& init);

	void update() override;
	void draw() const override;
private:
	const Vec2 BlockSize{ 10, 10 };
	const Size GridSize{ 72, 72 };
	const Size FieldSize = GridSize * BlockSize.asPoint();
	const size_t typeCount{ static_cast<size_t>(Type::Count) };
	const size_t typeCombinationCount = typeCount * (typeCount - 1) / 2;

	const Size gridSectionSize{ GridSize.x / typeCount, GridSize.y / typeCount };

	const double WallThickness = 20;
	const Polygon Wall = getRectFrame(RectF{ FieldSize }.stretched(WallThickness), WallThickness);

	HashTable<Types, size_t> ballCounts;
	HashTable<Types, size_t> blockCounts;
	Array<std::pair<double, HashTable<Types, size_t>>> blockCountsHistory;

	HashTable<Types, Array<BallWithMultipleType>> ballsByType;
	Grid<BlockWithMultipleType> blocks{ GridSize };

	// 描画位置と大きさを設定
	Mat3x2 matTitle = Mat3x2::Translate(1000, 0);
	Mat3x2 matField = Mat3x2::Scale(1.2, 1.2).translated(WallThickness, WallThickness);
	Mat3x2 matForPlot = Mat3x2::Scale(0.4, 0.4).translated(1000, 200);
	Mat3x2 matForRanking = Mat3x2::Scale(0.8, 0.8).Translate(1000, 200);
	Mat3x2 matForWorst = Mat3x2::Scale(0.8, 0.8).Translate(1400, 200);
};




