#pragma once

namespace CustomizedSimpleGUI
{

	class Button
	{
	public:
		Button() = default;
		Button(const String label, const int32 font_size, const Vec2& pos, const Optional<double>& _width = unspecified, const bool enabled = true);
		bool isPushed() const;
		void update(const Optional<String> label = none);
		void draw() const;
		Button operator=(const Button& other)
		{
			if (this == &other) return *this;
			m_label = other.m_label;
			m_font = other.m_font;
			m_pos = other.m_pos;
			m_width = other.m_width;
			m_enabled = other.m_enabled;
			m_center = other.m_center;
			m_rect = other.m_rect;
			m_labelPos = other.m_labelPos;
			m_mouseOver = other.m_mouseOver;
			m_pushed = other.m_pushed;
			m_rrect = other.m_rrect;
			return *this;
		}
	private:
		String m_label;
		Font m_font;
		Vec2 m_pos;
		double m_width;
		bool m_enabled;
		Vec2 m_center;
		RectF m_rect;
		Vec2 m_labelPos;
		bool m_mouseOver;
		bool m_pushed;
		RoundRect m_rrect;
		void updateValues();
	};

}
