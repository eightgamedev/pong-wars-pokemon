# include "MultipleTypeWar.hpp"

void MultipleTypeWar::addBlockCounts(const double time)
{
	blockCountsHistory.push_back({ time, blockCounts });
}

void MultipleTypeWar::deleteBlockCounts(const double time)
{
	while (!blockCountsHistory.isEmpty() && blockCountsHistory.front().first < time - 30)
	{
		blockCountsHistory.pop_front();
	}
}

void MultipleTypeWar::updateBlockCountsHistory()
{
	static double lastUpdateTimeForHistory = 0.0;
	double currentTime = Scene::Time() - sceneStartTime;
	double interval = 0.5;

	if (currentTime - lastUpdateTimeForHistory >= interval)
	{
		addBlockCounts(currentTime);
		lastUpdateTimeForHistory = currentTime;
	}

	deleteBlockCounts(currentTime);
}

void MultipleTypeWar::updateBlockCountsRanking()
{
	static double lastUpdateTimeForRanking = 0.0;
	double currentTime = Scene::Time() - sceneStartTime;
	double interval = 0.5;

	if (currentTime - lastUpdateTimeForRanking >= interval)
	{
		blockCountsRanking = Array<std::pair<Types, int32>>(blockCounts.begin(), blockCounts.end());

		// ブロック数が大きい順に並べ替え
		std::sort(blockCountsRanking.begin(), blockCountsRanking.end(),
					[](const std::pair<Types, int32>& a, const std::pair<Types, int32>& b) {
						return a.second > b.second;
			});

		lastUpdateTimeForRanking = currentTime;
	}
}

void MultipleTypeWar::updateBlock(BlockWithMultipleType& block, const BallWithMultipleType& ball, double time)
{
	blockCounts.at(block.getTypes())--;
	block.setType(ball.getTypes());
	blockCounts.at(block.getTypes())++;
}

void MultipleTypeWar::checkCollisionAndBounce(BallWithMultipleType &ball)
{
	const Point gridPos = ball.getPos().asPoint() / BlockSize.asPoint();

	// ボールの位置を基に、調査するグリッドセルの範囲を決定
	int startX = Max(0, gridPos.x - 1);
	int endX = Min(GridSize.x - 1, gridPos.x + 1);
	int startY = Max(0, gridPos.y - 1);
	int endY = Min(GridSize.y - 1, gridPos.y + 1);

	for (int x = startX; x <= endX; ++x)
	{
		for (int y = startY; y <= endY; ++y)
		{
			auto& block = blocks[y][x];

			if (!ball.getCircle().intersects(block.getRect())) {
				continue;
			}

			if (ball.getTypes() == block.getTypes()) {
				continue;
			}


			double affinityFirst = TypeAffinityTable.at({ ball.getTypes().type1, block.getTypes().type1 }) + TypeAffinityTable.at({ ball.getTypes().type1, block.getTypes().type2 });
			double affinitySecond = TypeAffinityTable.at({ ball.getTypes().type1, block.getTypes().type2 }) + TypeAffinityTable.at({ ball.getTypes().type2, block.getTypes().type2 });
			double affinity = (affinityFirst + affinitySecond) / 2;
			if (ball.hasSingleType()) affinity /= 2;
			if (block.hasSingleType()) affinity /= 2;

			bool canAttack = RandomBool(affinity * 0.25);

			if (canAttack)
			{
				updateBlock(block, ball, Scene::Time());
			}

			ball.bounce(block.getRect());
		}
	}
}

void MultipleTypeWar::adjustBallCounts()
{
	// 各タイプのボールの数を調整
	for (const auto& pair : blockCounts)
	{
		Types types = pair.first;
		int32 blockCount = pair.second;
		// 調整後のボールの数
		int32 desiredBallCount = Max((blockCount / (gridSectionSize.x * gridSectionSize.y) * 2), 1);

		// ボールが足りない場合は同じタイプのボールの位置から追加
		if (ballCounts[types] < desiredBallCount)
		{
			Array<Vec2> ballPositions;
			for (auto& ball : ballsByType[types])
			{
				ballPositions.push_back(ball.getPos());
			}

			for (const auto& position : ballPositions)
			{
				ballsByType[types].push_back(BallWithMultipleType{ types, position });
				ballCounts[types]++;
				if (ballCounts[types] >= desiredBallCount)
				{
					break;
				}
			}
		}

		// ボールが多すぎる場合は削除
		else if (ballCounts[types] > desiredBallCount)
		{
			while (ballCounts[types] > desiredBallCount)
			{
				ballsByType[types].pop_back();
				ballCounts[types]--;
			}
		}
	}
}



void MultipleTypeWar::drawBlocks() const
{
	for (auto& block : blocks)
	{
		block.draw();
	}
}

void MultipleTypeWar::drawBalls() const
{
	for (auto& balls : ballsByType)
	{
		for (auto& ball : balls.second)
		{
			ball.draw();
		}
	}
}

void MultipleTypeWar::drawRankingBest() const
{
	// トップ20を描画
	size_t limit = Min(blockCountsRanking.size(), size_t(20));
	for (size_t i = 0; i < limit; ++i)
	{
		const Vec2 textPos = { 0, 25 * i };
		const Types types = blockCountsRanking[i].first;
		const Type type1 = types.type1;
		const Type type2 = types.type2;
		const String text1 = typeName.at(type1);
		const String text2 = typeName.at(type2);
		const double Occupancy = static_cast<double>(blockCounts.at(types)) / (GridSize.x * GridSize.y) * 100;
		FontAsset(U"Ranking")(text1).draw(textPos, typeColors.at(type1));
		FontAsset(U"Ranking")(text2).draw(Arg::topRight = textPos.movedBy(170, 0), typeColors.at(type2));
		DrawGradientText(FontAsset(U"Ranking"), Format(blockCountsRanking[i].second), textPos.movedBy(200, 0), typeColors.at(type1), typeColors.at(type2));
		DrawGradientText(FontAsset(U"Ranking"), U"({:.2f}%)"_fmt(Occupancy), textPos.movedBy(250, 0), typeColors.at(type1), typeColors.at(type2));
	}
}

void MultipleTypeWar::drawRankingWorst() const
{
	// ワースト20を描画
	size_t limit = Min(blockCountsRanking.size(), size_t(20));
	for (size_t i = 0; i < limit; ++i)
	{
		const Vec2 textPos = { 0, 25 * i };
		const Types types = blockCountsRanking[blockCountsRanking.size() - 1 - i].first;
		const Type type1 = types.type1;
		const Type type2 = types.type2;
		const String text1 = typeName.at(type1);
		const String text2 = typeName.at(type2);
		const double Occupancy = static_cast<double>(blockCounts.at(types)) / (GridSize.x * GridSize.y) * 100;
		FontAsset(U"Ranking")(text1).draw(textPos, typeColors.at(type1));
		FontAsset(U"Ranking")(text2).draw(Arg::topRight = textPos.movedBy(170, 0), typeColors.at(type2));
		DrawGradientText(FontAsset(U"Ranking"), Format(blockCountsRanking[blockCountsRanking.size() - 1 - i].second), textPos.movedBy(200, 0), typeColors.at(type1), typeColors.at(type2));
		DrawGradientText(FontAsset(U"Ranking"), U"({:.2f}%)"_fmt(Occupancy), textPos.movedBy(250, 0), typeColors.at(type1), typeColors.at(type2));
	}
}

void MultipleTypeWar::drawGraph() const
{
	size_t maxBlockCount = 0;
	for (const auto& history : blockCountsHistory) {
		for (const auto& pair : history.second) {
			if (pair.second > maxBlockCount) {
				maxBlockCount = pair.second;
			}
		}
	}

	const double timeRange = 30;
	double currentTime = Scene::Time() - sceneStartTime;
	double minTime = currentTime - timeRange;
	double maxTime = timeRange;

	// グラフのサイズとスケールを計算
	const double graphHeight = 1000.0;
	const double graphWidth = 1700.0;
	const double timeScale = graphWidth / maxTime;
	const double yScale = graphHeight / maxBlockCount;

	// グリッドの縦線を描画
	for (int32 i = 0; i <= maxTime; i += 10) {
		const double x = i * timeScale;
		const int32 labelValue = maxTime - i;
		Line(x, 0, x, graphHeight).draw(1, Palette::Gray);
		const String label = labelValue == 0 ? Format(labelValue) : U"-{}"_fmt(labelValue);
		FontAsset(U"Label")(label).draw(Arg::topCenter = Vec2(x, graphHeight + 10), Palette::White);
	}

	// x軸を描画
	Line(0, graphHeight, graphWidth, graphHeight).draw(1, Palette::White);

	// x軸のラベルを描画
	FontAsset(U"Label")(U"Time (s)").draw(Arg::topCenter = Vec2(graphWidth / 2, graphHeight + 40), Palette::White);

	// y軸を描画
	Line(0, 0, 0, graphHeight).draw(1, Palette::White);

	// y軸のラベルとグリッドの横線を描画
	// maxBlockCountの桁数を計算
	int digits = maxBlockCount > 0 ? static_cast<int>(std::log10(maxBlockCount)) + 1 : 1;

	// 桁数に応じてspaceを設定
	size_t space = std::pow(10, digits - 1);
	for (int32 i = 0; i <= maxBlockCount; i += space) { // maxBlockCountはブロックの最大数
		const double y = graphHeight - i * yScale;
		const String label = Format(i);
		FontAsset(U"Label")(label).draw(Arg::rightCenter = Vec2(-40, y), Palette::White);
		Line(-10, y, 0, y).draw(1, Palette::White);
		Line(0, y, graphWidth, y).draw(0.5, Palette::Gray);
	}

	// グラフを描画
	for (size_t i = 1; i < blockCountsHistory.size(); ++i) {
		const auto& previous = blockCountsHistory[i - 1];
		const auto& current = blockCountsHistory[i];

		for (const auto& pair : current.second) {
			const double x0 = (previous.first - minTime) * timeScale;
			const double y0 = graphHeight - previous.second.at(pair.first) * yScale;
			const double x1 = (current.first - minTime) * timeScale;
			const double y1 = graphHeight - pair.second * yScale;

			Line(x0, y0, x1, y1).draw(2, typeColors.at(pair.first.type1));
		}
	}

}

MultipleTypeWar::MultipleTypeWar(const InitData& init)
	: IScene(init)
{
	sceneStartTime = Scene::Time();
	for (size_t firstType = 0; firstType < typeCount; ++firstType)
	{
		for (size_t secondType = firstType; secondType < typeCount; ++secondType)
		{
			Types types{ static_cast<Type>(firstType), static_cast<Type>(secondType) };
			ballCounts[types] = 0;
			blockCounts[types] = 0;
		}
	}

	for (size_t y = 0; y < GridSize.y; ++y)
	{
		for (size_t x = 0; x < GridSize.x; ++x)
		{
			const Point SectionIndex = Point{ x, y } / gridSectionSize;
			Types types;
			if (SectionIndex.x > SectionIndex.y)
			{
				types = Types{ static_cast<Type>(SectionIndex.y % typeCount), static_cast<Type>(SectionIndex.x % typeCount) };
			}
			else
			{
				types = Types{ static_cast<Type>(SectionIndex.x % typeCount), static_cast<Type>(SectionIndex.y % typeCount) };
			}
			blocks[y][x] = BlockWithMultipleType{ Vec2(x, y) * BlockSize, types, BlockSize };
			blockCounts[types]++;
		}
	}

	const Vec2 blockSectionSize = Vec2{ GridSize } / typeCount * BlockSize;


	// 各タイプの組み合わせに対してボールを作成
	for (size_t y = 0; y < typeCount; ++y)
	{
		for (size_t x = 0; x < typeCount; ++x)
		{
			// セクションのインデックスを計算
			int32 sectionIndexX = x % typeCount;
			int32 sectionIndexY = y % typeCount;

			// セクションの中心を計算
			Vec2 sectionCenter = Vec2(sectionIndexX, sectionIndexY) * blockSectionSize + blockSectionSize / 2;

			Types types;

			if (sectionIndexX > sectionIndexY)
			{
				types = Types{ static_cast<Type>(sectionIndexY), static_cast<Type>(sectionIndexX) };
				ballCounts[Types{ static_cast<Type>(y), static_cast<Type>(x) }]++;
			}
			else
			{
				types = Types{ static_cast<Type>(sectionIndexX), static_cast<Type>(sectionIndexY) };
				ballCounts[Types{ static_cast<Type>(x), static_cast<Type>(y) }]++;
			}

			ballsByType[types].push_back(BallWithMultipleType{ types, sectionCenter });
		}
	}
}

void MultipleTypeWar::update()
{
	for (auto& balls : ballsByType)
	{
		for (auto& ball : balls.second)
		{
			ball.update(FieldSize);
			checkCollisionAndBounce(ball);
		}
	}

	adjustBallCounts();
	updateBlockCountsHistory();
	updateBlockCountsRanking();
}

void MultipleTypeWar::draw() const
{
	{
		const Transformer2D transformer{ matField, TransformCursor::Yes };

		drawBlocks();

		drawBalls();

		Wall.draw(Palette::White);
	}

	{
		const Transformer2D transformerForRankingBest{ matRankingBest, TransformCursor::Yes };
		drawRankingBest();
	}

	{
		const Transformer2D transformerForRankingWorst{ matRankingWorst, TransformCursor::Yes };
		// drawRankingWorst();
	}

	{
		const Transformer2D transformerForPlot{ matPlot, TransformCursor::Yes };
		drawGraph();
	}
}
