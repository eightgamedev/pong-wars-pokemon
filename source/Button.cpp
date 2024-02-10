#include "Button.hpp"
# include "GUIVariables.hpp"

namespace CustomizedSimpleGUI
{

	Button::Button(const String label, const int32 fontSize, const Vec2& pos, const Optional<double>& _width, const bool enabled)
		: m_label(label), m_pos(pos), m_enabled(enabled), m_mouseOver(false), m_pushed(false)
	{
		m_font = Font{ FontMethod::MSDF, fontSize, Typeface::CJK_Regular_JP };
		const auto dtext = m_font(m_label);
		const double labelWidth = Math::Ceil(dtext.region().w);
		m_width = _width.value_or_eval([&]() { return (labelWidth + 40); });
		m_center = { m_pos.x + m_width / 2.0, m_pos.y + m_font.height() / 2.0 };
		m_rect = { Arg::center = m_center, m_width, (double)m_font.height() };
		m_labelPos = { (m_rect.x + (m_width - labelWidth) / 2.0), (m_center.y - m_font.height() / 2.0 + FontYOffset) };
		m_rrect = m_rect.rounded(RoundSize);
	}

	void Button::updateValues()
	{
		const auto dtext = m_font(m_label);
		const double labelWidth = Math::Ceil(dtext.region().w);
		m_width = labelWidth + 40;
		m_center = { m_pos.x + m_width / 2.0, m_pos.y + m_font.height() / 2.0 };
		m_rect = { Arg::center = m_center, m_width, (double)m_font.height() };
		m_labelPos = { (m_rect.x + (m_width - labelWidth) / 2.0), (m_center.y - m_font.height() / 2.0 + FontYOffset) };
		m_rrect = m_rect.rounded(RoundSize);
	}

	bool Button::isPushed() const
	{
		return m_pushed;
	}

	void Button::update(const Optional<String> label)
	{
		if (label.has_value())
		{
			m_label = label.value();
			updateValues();
		}
		m_mouseOver = (m_enabled && m_rect.mouseOver());
		m_pushed = (m_mouseOver && Cursor::OnClientRect() && MouseL.down());
	}

	void Button::draw() const
	{
		if (m_enabled)
		{
			m_rrect.draw(m_mouseOver ? ButtonMouseOverColor : BackgroundColor);
		}
		else
		{
			m_rrect.draw(DisabledBackgroundColor);
		}

		if (!m_pushed)
		{
			m_rrect.drawFrame(1, 0, FrameColor);
		}

		m_font(m_label).draw(m_labelPos, GetTextColor(m_enabled));

		if (m_mouseOver)
		{
			Cursor::RequestStyle(CursorStyle::Hand);
		}
	}

}

