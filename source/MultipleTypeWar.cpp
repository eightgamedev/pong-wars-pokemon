# include "MultipleTypeWar.hpp"

void addBlockCounts(const double time, Array<std::pair<double, HashTable<Types, size_t>>>& blockCountsHistory, HashTable<Types, size_t>& blockCounts)
{
	blockCountsHistory.push_back({ time, blockCounts });
}

void deleteBlockCounts(const double time, Array<std::pair<double, HashTable<Types, size_t>>>& blockCountsHistory)
{
	while (!blockCountsHistory.isEmpty() && blockCountsHistory.front().first < time - 30)
	{
		blockCountsHistory.pop_front();
	}
}

void updateBlockCountHistoryForMultipleType(Array<std::pair<double, HashTable<Types, size_t>>>& blockCountsHistory, HashTable<Types, size_t>& blockCounts)
{
	static double lastUpdateTime = 0.0;
	double currentTime = Scene::Time();

	if (currentTime - lastUpdateTime >= 1.0) // 1秒以上経過したかチェック
	{
		addBlockCounts(currentTime, blockCountsHistory, blockCounts); // 1秒ごとにブロック数の履歴を更新
		lastUpdateTime = currentTime;
	}

	deleteBlockCounts(currentTime, blockCountsHistory); // 60秒以上前の履歴を削除
}

void updateBlockWithMultipleType(BlockWithMultipleType& block, const BallWithMultipleType& ball, double time, HashTable<Types, size_t>& blockCounts, Array<std::pair<double, HashTable<Types, size_t>>>& blockCountsHistory)
{
	blockCounts.at(block.getTypes())--;
	block.setType(ball.getTypes());
	blockCounts.at(block.getTypes())++;
}

void checkCollisionAndBounceForMultipleType(BallWithMultipleType &ball,
	Grid<BlockWithMultipleType>& blocks,
	HashTable<Types, size_t>& blockCounts,
	Array<std::pair<double, HashTable<Types, size_t>>>& blockCountsHistory,
	Size GridSize,
	Vec2 BlockSize
)
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
				updateBlockWithMultipleType(block, ball, Scene::Time(), blockCounts, blockCountsHistory);
			}

			ball.bounce(block.getRect());
		}
	}
}

void adjustBallCountsForMultipleType(HashTable<Types, Array<BallWithMultipleType>>& ballsByType, const Grid<BlockWithMultipleType>& blocks, HashTable<Types, size_t>& ballCounts, HashTable<Types, size_t>& blockCounts)
{
	// 各タイプのボールの数を調整
	for (const auto& pair : blockCounts)
	{
		Types types = pair.first;
		int32 blockCount = pair.second;
		// 調整後のボールの数
		int32 desiredBallCount = Max(blockCount / 20, 1);

		// ボールが足りない場合は同じタイプのボールの位置から追加
		while (ballCounts[types] < desiredBallCount)
		{
			for (auto& ball : ballsByType[types])
			{
				ballsByType[types].push_back(BallWithMultipleType{ types, ball.getCircle().center });
				ballCounts[types]++;
				if (ballCounts[types] >= desiredBallCount)
				{
					break;
				}
			}
		}

		// ボールが多すぎる場合は削除
		while (ballCounts[types] > desiredBallCount)
		{
			ballsByType[types].pop_back();
			ballCounts[types]--;
		}
	}
}

void drawGraphForMultipleType(const Array<std::pair<double, HashTable<Types, size_t>>>& blockCountsHistory)
{
	size_t maxBlockCount = 0;
	for (const auto& history : blockCountsHistory) {
		for (const auto& pair : history.second) {
			if (pair.second > maxBlockCount) {
				maxBlockCount = pair.second;
			}
		}
	}

	// 現在の時間を取得
	double currentTime = Scene::Time();

	// 時間の最大値
	double maxTime = 30;

	// 過去30秒以降の最小の時間を計算
	double minTime = std::max(0.0, currentTime - maxTime);

	// グラフのサイズとスケールを計算
	const double graphHeight = 1000.0;
	const double graphWidth = 1700.0;
	const double timeScale = graphWidth / maxTime;
	const double yScale = graphHeight / maxBlockCount; // maxBlockCountはブロックの最大数

	// グリッドの縦線を描画
	for (int32 i = 0; i <= maxTime; i += 10) {
		const double x = i * timeScale;
		Line(x, 0, x, graphHeight).draw(1, Palette::Gray);
		const String label = Format(i);
		FontAsset(U"Label")(label).draw(Arg::topCenter = Vec2(x, graphHeight + 10), Palette::White);
	}

	// x軸を描画
	Line(0, graphHeight, graphWidth, graphHeight).draw(1, Palette::White);

	// x軸のラベルを描画
	FontAsset(U"Label")(U"Time (s)").draw(Arg::topCenter = Vec2(graphWidth / 2, graphHeight + 40), Palette::White);

	// y軸を描画
	Line(0, 0, 0, graphHeight).draw(1, Palette::White);

	// y軸のラベルとグリッドの横線を描画
	for (int32 i = 0; i <= maxBlockCount; i += 10) { // maxBlockCountはブロックの最大数
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

	blockCountsHistory.push_back({ 0, blockCounts });

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
			}
			else
			{
				types = Types{ static_cast<Type>(sectionIndexX), static_cast<Type>(sectionIndexY) };
			}

			// ボールを作成し、初期位置をセクションの中心に設定
			ballsByType[types].push_back(BallWithMultipleType{ types, sectionCenter });
			ballCounts[Types{ static_cast<Type>(y), static_cast<Type>(x) }]++;
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
			checkCollisionAndBounceForMultipleType(ball, blocks, blockCounts, blockCountsHistory, GridSize, BlockSize);
		}
	}

	adjustBallCountsForMultipleType(ballsByType, blocks, ballCounts, blockCounts);
	updateBlockCountHistoryForMultipleType(blockCountsHistory, blockCounts);

}

void MultipleTypeWar::draw() const
{
	{
		const Transformer2D transformer{ matTitle, TransformCursor::Yes };

		FontAsset(U"Title")(U"Pong Wars Pokemon").draw(0, 0, Palette::White);
		FontAsset(U"Title")(U"@ito_eight").draw(0, 60, Palette::White);
	}

	{
		const Transformer2D transformer{ matField, TransformCursor::Yes };

		for (auto& block : blocks)
		{
			block.draw();
		}

		for (auto& balls : ballsByType)
		{
			for (auto& ball : balls.second)
			{
				ball.draw();
			}
		}

		Wall.draw(Palette::White);
	}


	Array<std::pair<Types, int32>> blockCountsRanking(blockCounts.begin(), blockCounts.end());

	// ブロック数が大きい順に並べ替え
	std::sort(blockCountsRanking.begin(), blockCountsRanking.end(),
		[](const std::pair<Types, int32>& a, const std::pair<Types, int32>& b) {
			return a.second > b.second;
		});

	{
		const Transformer2D transformerForRanking{ matForRanking, TransformCursor::Yes };

		// トップ20を描画
		for (size_t i = 0; i < 20; ++i)
		{
			const Vec2 textPos = { 0, 25 * i };
			const Types types = blockCountsRanking[i].first;
			const Type type1 = types.type1;
			const Type type2 = types.type2;
			const String text1 = typeName.at(type1);
			const String text2 = typeName.at(type2);
			FontAsset(U"Ranking")(text1).draw(textPos, typeColors.at(type1));
			FontAsset(U"Ranking")(text2).draw(Arg::topRight = textPos.movedBy(170, 0), typeColors.at(type2));
			// FontAsset(U"Ranking")(blockCountsRanking[i].second).draw(Arg::topRight = textPos.movedBy(250, 0), Arg::top = typeColors.at(type1), Arg::bottom = typeColors.at(type2));
			DrawGradientText(FontAsset(U"Ranking"), Format(blockCountsRanking[i].second), textPos.movedBy(250, 0), typeColors.at(type1), typeColors.at(type2));
		}
	}

	{
		const Transformer2D transformerForWorst{ matForWorst, TransformCursor::Yes };

		// ワースト20を描画
		for (size_t i = 0; i < 20; ++i)
		{
			const Vec2 textPos = { 0, 25 * i };
			const Types types = blockCountsRanking[blockCountsRanking.size() - 1 - i].first;
			const Type type1 = types.type1;
			const Type type2 = types.type2;
			const String text1 = typeName.at(type1);
			const String text2 = typeName.at(type2);
			FontAsset(U"Ranking")(text1).draw(textPos, typeColors.at(type1));
			FontAsset(U"Ranking")(text2).draw(Arg::topRight = textPos.movedBy(170, 0), typeColors.at(type2));
			// FontAsset(U"Ranking")(blockCountsRanking[blockCountsRanking.size() - 1 - i].second).draw(Arg::topRight = textPos.movedBy(250, 0), typeColors.at(type1));
			DrawGradientText(FontAsset(U"Ranking"), Format(blockCountsRanking[blockCountsRanking.size() - 1 - i].second), textPos.movedBy(250, 0), typeColors.at(type1), typeColors.at(type2));
		}
	}

	{
		const Transformer2D transformerForPlot{ matForPlot, TransformCursor::Yes };
		drawGraphForMultipleType(blockCountsHistory);
	}
}
