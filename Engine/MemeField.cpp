#include "MemeField.h"
#include <algorithm>
#include <assert.h>
#include <random>

MemeField::MemeField(int nMemes)
{
	assert(nMemes > 0 && nMemes < width * height);
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> xDist(0, width - 1);
	std::uniform_int_distribution<int> yDist(0, height - 1);
	
	for (int nSpawned = 0; nSpawned < nMemes; nSpawned++)
	{
		Vei2 spawnPos;
		do
		{
			spawnPos = { xDist(rng), yDist(rng) };
		}
		while (TileAt(spawnPos).HasMeme());
		TileAt(spawnPos).SpawnMeme();
	}
	for (Vei2 gridPos = { 0,0 }; gridPos.y < height; gridPos.y++)
	{
		for (gridPos.x = 0; gridPos.x < width; gridPos.x++)
		{
			TileAt(gridPos).SetNeighbourMemeCount(CountNeighbourMemes( gridPos ));
		}
	}

	memesCount = nMemes;
}

RectI MemeField::GetRect() const
{
	return RectI(fieldPosition.x, fieldPosition.x + width * SpriteCodex::tileSize, fieldPosition.y, fieldPosition.y + height * SpriteCodex::tileSize);
}

void MemeField::Draw(Graphics & gfx) const
{
	if (IsWinrar())
	{
		
		SpriteCodex::DrawWin({ Graphics::ScreenWidth / 2, Graphics::ScreenHeight / 2 }, gfx);
	}
	else
	{
		gfx.DrawRect(GetRect(), fieldColor);
		for (Vei2 gridPos = { 0,0 }; gridPos.y < height; gridPos.y++)
		{
			for (gridPos.x = 0; gridPos.x < width; gridPos.x++)
			{
				TileAt(gridPos).Draw(gridPos * SpriteCodex::tileSize + fieldPosition, isFucked, gfx);
			}
		}
	}
}

void MemeField::OnRevealClick(const Vei2& screenPos)
{
	if (!isFucked) {
		const Vei2 gridPos = ScreenToGrid(screenPos);
		assert(gridPos.x >= 0 && gridPos.x < width && gridPos.y >= 0 && gridPos.y < height);
		RevealTiles(gridPos);
	}
}

void MemeField::OnFlagClick(const Vei2 & screenPos)
{
	if (!isFucked) {
		const Vei2 gridPos = ScreenToGrid(screenPos);
		assert(gridPos.x >= 0 && gridPos.x < width && gridPos.y >= 0 && gridPos.y < height);
		Tile& tile = TileAt(gridPos);
		if (!tile.IsRevealed())
		{
			tile.ToggleFlag();
		}
	}
}

bool MemeField::IsFucked() const
{
	return isFucked;
}

bool MemeField::IsWinrar() const
{
	int countHidden = 0;
	for (Vei2 gridPos = { 0,0 }; gridPos.x < width; gridPos.x++)
	{
		for (gridPos.y = 0; gridPos.y < height; gridPos.y++)
		{
			if (!TileAt(gridPos).IsRevealed())
			{
				countHidden++;
			}
		}
	}
	if (countHidden > memesCount)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void MemeField::RevealTiles(const Vei2& gridPos)
{
	Tile& tile = TileAt(gridPos);
	if (!tile.IsRevealed() && !tile.IsFlagged())
	{
		tile.Reveal();
		if (tile.HasNeighbourMemes())
		{
			if (tile.HasMeme())
			{
				isFucked = true;
			}
		}
		else
		{
			const int xStart = std::max(0, gridPos.x - 1);
			const int yStart = std::max(0, gridPos.y - 1);
			const int xEnd = std::min(width - 1, gridPos.x + 1);
			const int yEnd = std::min(height - 1, gridPos.y + 1);

			for (Vei2 gridPos = { xStart,yStart }; gridPos.y <= yEnd; gridPos.y++)
			{
				for (gridPos.x = xStart; gridPos.x <= xEnd; gridPos.x++)
				{
					RevealTiles(gridPos);
				}
			}
		}
	}
}

MemeField::Tile & MemeField::TileAt(const Vei2 & gridPos)
{
	return field[gridPos.y * width + gridPos.x];
}

const MemeField::Tile & MemeField::TileAt(const Vei2 & gridPos) const
{
	return field[gridPos.y * width + gridPos.x];
}

Vei2& MemeField::ScreenToGrid(const Vei2 & screenPos)
{
	return (screenPos - fieldPosition) / SpriteCodex::tileSize;
}

int MemeField::CountNeighbourMemes(const Vei2 & gridPos)
{
	const int xStart = std::max(0, gridPos.x - 1);
	const int yStart = std::max(0, gridPos.y - 1);
	const int xEnd = std::min(width - 1, gridPos.x + 1);
	const int yEnd = std::min(height - 1, gridPos.y + 1);

	int count = 0;
	for (Vei2 gridPos = { xStart,yStart }; gridPos.y <= yEnd; gridPos.y++)
	{
		for (gridPos.x = xStart; gridPos.x <= xEnd; gridPos.x++)
		{
			if (TileAt(gridPos).HasMeme())
			{
				count++;
			}
		}
	}
	return count;
}

void MemeField::Tile::SpawnMeme()
{
	assert(!hasMeme);
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> meme(1, 3);

	hasMeme = true;
	memeType = meme(rng);
}

bool MemeField::Tile::HasMeme() const
{
	return hasMeme;
}

int MemeField::Tile::MemeType() const
{
	return memeType;
}

void MemeField::Tile::SetNeighbourMemeCount(int nMemes)
{
	assert(nNeighbourMemes == -1);
	nNeighbourMemes = nMemes;
}

void MemeField::Tile::Draw(const Vei2 screenPos, bool isFucked, Graphics & gfx) const
{
	if (!isFucked) {
		switch (state)
		{
		case State::Hidden:
			SpriteCodex::DrawTileButton(screenPos, gfx);
			break;
		case State::Flagged:
			SpriteCodex::DrawTileButton(screenPos, gfx);
			SpriteCodex::DrawTileFlag(screenPos, gfx);
			break;
		case State::Revealed:
			if (!HasMeme())
			{
				SpriteCodex::DrawTileNumber(screenPos, nNeighbourMemes, gfx);
			}
			else
			{
				SpriteCodex::DrawTilePepper(screenPos, MemeType(), gfx);
			}
			break;
		}
	}
	else // we are fucked (game over)
	{
		switch (state)
		{
		case State::Hidden:
			if (HasMeme())
			{
				SpriteCodex::DrawTilePepper(screenPos, MemeType(), gfx);
			}
			else
			{
				SpriteCodex::DrawTileButton(screenPos, gfx);
			}
			break;
		case State::Flagged:
			if (HasMeme())
			{
				SpriteCodex::DrawTilePepper(screenPos, MemeType(), gfx);
				SpriteCodex::DrawTileFlag(screenPos, gfx);
			}
			else
			{
				SpriteCodex::DrawTilePepper(screenPos, MemeType(), gfx);
				SpriteCodex::DrawTileCross(screenPos, gfx);
			}
			break;
		case State::Revealed:
			if (HasMeme())
			{
				SpriteCodex::DrawTileRed(screenPos, gfx);
				SpriteCodex::DrawTilePepper(screenPos, MemeType(), gfx);
			}
			else
			{
				SpriteCodex::DrawTileNumber(screenPos, nNeighbourMemes, gfx);
			}
			break;
		}
	}
}

void MemeField::Tile::Reveal()
{
	assert(!IsRevealed());
	state = State::Revealed;
}

void MemeField::Tile::ToggleFlag()
{
	assert(!IsRevealed());
	if (IsFlagged())
	{
		state = State::Hidden;
	}
	else
	{
		state = State::Flagged;
	}
}

bool MemeField::Tile::IsRevealed() const
{
	return state == State::Revealed;
}

bool MemeField::Tile::IsFlagged() const
{
	return state == State::Flagged;
}

bool MemeField::Tile::HasNeighbourMemes() const
{
	return nNeighbourMemes > 0;
}
