# pragma once
# include "Type.hpp"

class Block
{
public:
	Block() = default;
	Block(const Vec2& pos, Type type, const Vec2& size)
		: m_type(type)
		, m_rect(pos, size)
		, m_color(typeColors.at(type))
	{
	}

	void draw() const
	{
		m_rect.stretched(-1).draw(m_color);
	}

	const RectF& getRect() const
	{
		return m_rect;
	}

	Type getType() const
	{
		return m_type;
	}

	void setType(Type type)
	{
		m_type = type;
		m_color = typeColors.at(type);
	}

private:
	RectF m_rect;
	Type m_type;
	Color m_color;
};
