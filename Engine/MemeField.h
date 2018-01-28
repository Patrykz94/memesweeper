#pragma once
#include "Graphics.h"
#include "Vei2.h"
#include "SpriteCodex.h"
#include "RectI.h"
#include "Colors.h"
#include "Sound.h"

class MemeField
{
private:
	class Tile
	{
	public:
		enum class State : char
		{
			Hidden,
			Flagged,
			Revealed
		};
	public:
		void SpawnMeme();
		bool HasMeme() const;
		void SetNeighbourMemeCount(int nMemes);
		void Draw(const Vei2 screenPos, bool isFucked, Graphics& gfx) const;
		void Reveal();
		void ToggleFlag();
		bool IsRevealed() const;
		bool IsFlagged() const;
	private:
		State state = State::Hidden;
		bool hasMeme = false;
		int nNeighbourMemes = -1;
	};
public:
	MemeField(int nMemes);
	RectI GetRect() const;
	void Draw(Graphics& gfx) const;
	void OnRevealClick(const Vei2& screenPos);
	void OnFlagClick(const Vei2& screenPos);
	Sound gameOverSound = L"Sounds/spayed.wav";
	bool IsFucked() const;
	bool IsWinrar() const;
private:
	Tile& TileAt(const Vei2& gridPos);
	const Tile& TileAt(const Vei2& gridPos) const;
	Vei2& ScreenToGrid(const Vei2& screenPos);
	int CountNeighbourMemes(const Vei2& gridPos);
private:
	static constexpr int width = 20;
	static constexpr int height = 16;
	Vei2 fieldPosition = { Graphics::ScreenWidth / 2 - width * SpriteCodex::tileSize / 2, Graphics::ScreenHeight / 2 - height*SpriteCodex::tileSize / 2 };
	static constexpr Color fieldColor = Colors::MakeRGB(192, 192, 192);
	bool isFucked = false;
	Tile field[width * height];
	int memesCount;
};