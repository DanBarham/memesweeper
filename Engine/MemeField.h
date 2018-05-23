#pragma once

#include "Graphics.h"
#include "Vei2.h"
#include "RectI.h"

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
		void SetNeighborMemeCount( int memeCount );
	private:
		State state = State::HIDDEN;
		bool hasMeme = false;
		int neighborMemes = -1;
	};
public:
	MemeField( int nMemes );
	void Draw( Graphics& gfx ) const;
	RectI GetRect() const;
	void OnRevealClick( const Vei2 screenPos );
	void OnFlagClick( const Vei2 screenPos );
private:
	Tile& TileAt( const Vei2& gridPos );
	const Tile & TileAt( const Vei2& gridPos ) const;
	Vei2 ScreenToGrid( const Vei2& screenPos );
	int CountNeighborMemes( const Vei2& gridPos );
private:
	static constexpr int width = 20;
	static constexpr int height = 16;
	bool isFucked = false;
	Tile field[ width * height ];
};