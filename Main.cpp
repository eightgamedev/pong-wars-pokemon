# include "Common.hpp"
# include "Setting.hpp"
# include "SingleTypeWar.hpp"
# include "MultipleTypeWar.hpp"

void Main()
{
	const String applicationName = U"pong-wars-pokemon";
	Window::SetTitle(applicationName);
	Window::SetStyle(WindowStyle::Sizable);
	Window::Maximize();
	Scene::SetResizeMode(ResizeMode::Keep);
	Scene::Resize(1920, 1009);	// フルHDサイズからタスクバーの高さを引いたサイズ

	FontAsset::Register(U"Ranking", 20, Typeface::Bold);
	FontAsset::Register(U"Label", 30, Typeface::Bold);
	FontAsset::Register(U"Title", 60, Typeface::Bold);

	App manager;
	manager.add<Setting>(State::Setting)
		.add<SingleTypeWar>(State::SingleTypeWar)
		.add<MultipleTypeWar>(State::MultipleTypeWar);

	manager.init(State::Setting);

	while (System::Update())
	{
		if (not manager.update())
		{
			break;
		}
	}
}
