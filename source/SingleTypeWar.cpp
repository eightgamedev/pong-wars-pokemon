# include "SingleTypeWar.hpp"

SingleTypeWar::SingleTypeWar(const InitData& init)
	: IScene(init)
{

	for (size_t type = 0; type < typeCount; ++type)
	{
		ballCounts[static_cast<Type>(type)] = 0;
		blockCounts[static_cast<Type>(type)] = 0;
	}

	for (size_t y = 0; y < GridSize.y; ++y)
	{
		for (size_t x = 0; x < GridSize.x; ++x)
		{
			const Point SectionIndex = Point{ x, y } / gridSectionSize;
			Type type = static_cast<Type>((SectionIndex.y * 3 + SectionIndex.x) % typeCount);
			blocks[y][x] = Block{ Vec2(x, y) * BlockSize, type, BlockSize };
			blockCounts[type]++;
		}
	}

	for (size_t i = 0; i < typeCount; ++i)
	{
		blockCountsHistory[i].push_back({ 0, blockCounts[static_cast<Type>(i)] });
	}

	const Vec2 blockSectionSize = { (BlockSize * GridSize / 3).x, (BlockSize * GridSize / 6).y };

	for (size_t i = 0; i < typeCount; ++i)
	{
		// セクションのインデックスを計算
		int32 sectionIndexX = i % 3;
		int32 sectionIndexY = i / 3;

		// セクションの中心を計算
		Vec2 sectionCenter = Vec2(sectionIndexX, sectionIndexY) * blockSectionSize + blockSectionSize / 2;

		// ボールを作成し、初期位置をセクションの中心に設定
		balls.emplace_back(static_cast<Type>(i), sectionCenter);
		ballCounts[static_cast<Type>(i)]++;
	}
}

void SingleTypeWar::update()
{
	for (auto& ball : balls)
	{
		ball.update(FieldSize);
		checkCollisionAndBounce(ball, blocks, blockCounts, blockCountsHistory);
	}

	adjustBallCounts(balls, blocks, ballCounts, blockCounts);
}

void SingleTypeWar::draw() const
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

		for (auto& ball : balls)
		{
			ball.draw();
		}

		Wall.draw(Palette::White);
	}

	{
		const Transformer2D transformerForRanking{ matForRanking, TransformCursor::Yes };

		Array<std::pair<Type, int32>> blockCountsRanking(blockCounts.begin(), blockCounts.end());
	
		// ブロック数が大きい順に並べ替え
		std::sort(blockCountsRanking.begin(), blockCountsRanking.end(),
			[](const std::pair<Type, int32>& a, const std::pair<Type, int32>& b) {
					return a.second > b.second;
			});
		// ランキングを描画
		for (size_t i = 0; i < blockCountsRanking.size(); ++i)
		{
			const Vec2 textPos = { 0, 25 * i };
			const Type type = blockCountsRanking[i].first;
			const String text = typeName.at(type) + U": ";
			FontAsset(U"Ranking")(text).draw(textPos, typeColors.at(type));
			FontAsset(U"Ranking")(blockCountsRanking[i].second).draw(Arg::topRight = textPos.movedBy(150, 0), typeColors.at(type));
		}
	}

	{
		const Transformer2D transformerForPlot{ matForPlot, TransformCursor::Yes };
		drawGraph(blockCountsHistory);
	}
}
