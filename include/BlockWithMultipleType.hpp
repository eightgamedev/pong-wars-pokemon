# pragma once
# include "Type.hpp"

class BlockWithMultipleType
{
public:
	BlockWithMultipleType() = default;
	BlockWithMultipleType(const Vec2& pos, Types types, const Vec2& size)
		: m_types(types)
		, m_rect(pos, size)
	{
		m_colors.push_back(typeColors.at(m_types.type1));
		m_colors.push_back(typeColors.at(m_types.type2));
	}

	void draw() const
	{
		Color outerColor = m_colors[0];
		Color innerColor = m_colors[1];

		m_rect.draw(outerColor).stretched(-3).draw(innerColor);
	}

	const RectF& getRect() const
	{
		return m_rect;
	}

	Types getTypes() const
	{
		return m_types;
	}

	void setType(Types types)
	{
		m_types = types;
		m_colors.clear();
		m_colors.push_back(typeColors.at(types.type1));
		m_colors.push_back(typeColors.at(types.type2));
	}

	bool hasSingleType() const
	{
		return m_types.type1 == m_types.type2;
	}

private:
	RectF m_rect;
	Types m_types;
	Array<Color> m_colors;
};
