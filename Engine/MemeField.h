#pragma once
#include "Graphics.h"
#include "Vei2.h"
#include "SpriteCodex.h"
#include "RectI.h"
#include "Colors.h"
#include "Sound.h"

class MemeField
{
public:
	enum class State : char
	{
		Fucked,
		Winrar,
		Memeing
	};
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
		int MemeType() const;
		void SetNeighbourMemeCount(int nMemes);
		void Draw(const Vei2 screenPos, MemeField::State state, Graphics& gfx) const;
		void Reveal();
		void ToggleFlag();
		bool IsRevealed() const;
		bool IsFlagged() const;
		bool HasNeighbourMemes() const;
	private:
		State state = State::Hidden;
		bool hasMeme = false;
		int memeType = -1;
		int nNeighbourMemes = -1;
	};
public:
	MemeField(const Vei2& center, int width, int height);
	RectI GetRect() const;
	void Draw(Graphics& gfx) const;
	void OnRevealClick(const Vei2& screenPos);
	void OnFlagClick(const Vei2& screenPos);
	State GetState() const;
	void DestroyField();
private:
	void RevealTile(const Vei2& gridPos);
	Tile& TileAt(const Vei2& gridPos);
	const Tile& TileAt(const Vei2& gridPos) const;
	Vei2& ScreenToGrid(const Vei2& screenPos);
	int CountNeighbourMemes(const Vei2& gridPos);
	bool GameIsWon() const;
private:
	const int width;
	const int height;
	static constexpr Color fieldColor = Colors::MakeRGB(192, 192, 192);
	const int memesCount = (width*height)/10;
	Sound gameOverSound = Sound(L"Sounds/spayed.wav");
	Vei2 topLeft;
	State state = State::Memeing;
	Tile* field = nullptr;
};