#pragma once

#include "Graphics.h"
#include "Vei2.h"
#include "RectI.h"
#include "SpriteCodex.h"

class MemeField
{
private:
	class Tile 
	{
	public:
		enum class State
		{
			HIDDEN,
			FLAGGED,
			REVEALED
		};
	public:
		void SpawnMeme();
		bool HasMeme() const;
		void Draw( const Vei2& screenPos,bool fucked,Graphics& gfx ) const;
		void Reveal();
		bool IsRevealed() const;
		void ToggleFlag();
		bool IsFlagged() const;
		bool HasNoNeighborMemes() const;
		void SetNeighborMemeCount( int memeCount );
	private:
		State state = State::HIDDEN;
		bool hasMeme = false;
		int neighborMemes = -1;
	};
public:
	MemeField( int nMemes );
	void Draw( Graphics& gfx ) const;
	void DrawBorder( Graphics& gfx ) const;
	RectI GetRect() const;
	bool OnRevealClick( const Vei2 screenPos );
	void OnFlagClick( const Vei2 screenPos );
	bool CheckWinCondition();
private:
	void RevealTile( const Vei2& gridPos );
	Tile& TileAt( const Vei2& gridPos );
	const Tile & TileAt( const Vei2& gridPos ) const;
	Vei2 ScreenToGrid( const Vei2& screenPos );
	int CountNeighborMemes( const Vei2& gridPos );
private:
	static constexpr int width = 20;
	static constexpr int height = 15;
	static constexpr int xOffset = Graphics::ScreenWidth / 2 - (width * SpriteCodex::tileSize) / 2;
	static constexpr int yOffset = Graphics::ScreenHeight / 2 - (height * SpriteCodex::tileSize) / 2;
	bool isFucked = false;
	Tile field[ width * height ];
};