# pragma once
# include "Common.hpp"

void updateBlockCountHistory(const Type type, const double time, const int32 countChange, Array<Array<std::pair<double, size_t>>>& blockCountsHistory);
void updateBlock(Block& block, const Ball& ball, const double time, const int32 change, HashTable<Type, size_t>& blockCounts, Array<Array<std::pair<double, size_t>>>& blockCountsHistory);
void checkCollisionAndBounce(Ball& ball, Grid<Block>& blocks, HashTable<Type, size_t>& blockCounts, Array<Array<std::pair<double, size_t>>>& blockCountsHistory, Size GridSize,
	Vec2 BlockSize);
void adjustBallCounts(Array<Ball>& balls, const Grid<Block>& blocks, HashTable<Type, size_t>& ballCounts, const HashTable<Type, size_t>& blockCounts);
void drawGraph(const Array<Array<std::pair<double, size_t>>>& blockCountsHistory);

class SingleTypeWar : public App::Scene
{
public:
	SingleTypeWar(const InitData& init);

	void update() override;
	void draw() const override;
private:
	const Vec2 BlockSize{ 15, 15 };
	const Size GridSize{ 60, 60 };
	const Size FieldSize = GridSize * BlockSize.asPoint();
	const Size gridSectionSize{ GridSize.x / 3, GridSize.y / 6 };

	const double WallThickness = 20;
	const Polygon Wall = getRectFrame(RectF{ FieldSize }.stretched(WallThickness), WallThickness);

	HashTable<Type, size_t> ballCounts;
	HashTable<Type, size_t> blockCounts;
	Array<Array<std::pair<double, size_t>>> blockCountsHistory{ static_cast<size_t>(Type::Count) };

	const size_t typeCount{ static_cast<size_t>(Type::Count) };
	Array<Ball> balls;
	Grid<Block> blocks{ GridSize };

	// 描画位置と大きさを設定
	Mat3x2 matTitle = Mat3x2::Translate(1000, 0);
	Mat3x2 matField = Mat3x2::Translate(WallThickness, WallThickness);
	Mat3x2 matForPlot = Mat3x2::Scale(0.4, 0.4).translated(1000, 200);
	Mat3x2 matForRanking = Mat3x2::Translate(1730, 150);
};




