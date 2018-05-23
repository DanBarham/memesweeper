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
		void Draw( const Vei2& screenPos,Graphics& gfx ) const;
		void Reveal();
		bool IsRevealed() const;
		void ToggleFlag();
		bool IsFlagged() const;
	private:
		State state = State::HIDDEN;
		bool hasMeme = false;
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
private:
	static constexpr int width = 20;
	static constexpr int height = 16;
	Tile field[ width * height ];
};