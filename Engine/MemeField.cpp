#include "MemeField.h"
#include <assert.h>
#include <random>
#include "SpriteCodex.h"

void MemeField::Tile::SpawnMeme()
{
	assert( !hasMeme );
	hasMeme = true;
}

bool MemeField::Tile::HasMeme() const
{
	return hasMeme;
}

void MemeField::Tile::Draw( const Vei2& screenPos,Graphics& gfx ) const
{
	switch( state )
	{
	case State::HIDDEN:
		SpriteCodex::DrawTileButton( screenPos,gfx );
		break;
	case State::FLAGGED:
		SpriteCodex::DrawTileButton( screenPos,gfx );
		SpriteCodex::DrawTileFlag( screenPos,gfx );
		break;
	case State::REVEALED:
		if( !hasMeme )
		{
			SpriteCodex::DrawTile0( screenPos,gfx );
		}
		else
		{
			SpriteCodex::DrawTileBomb( screenPos,gfx );
		}
		break;
	}
}

void MemeField::Tile::Reveal()
{
	assert( state == State::HIDDEN );
	state = State::REVEALED;
}

bool MemeField::Tile::IsRevealed() const
{
	return state == State::REVEALED;
}

MemeField::MemeField( int nMemes )
{
	assert( nMemes > 0 && nMemes < width * height );

	std::random_device rd;
	std::mt19937 rng( rd() );
	std::uniform_int_distribution<int> xDist( 0,width - 1 );
	std::uniform_int_distribution<int> yDist( 0,height -1 );

	
	for( int nSpawned = 0; nSpawned < nMemes; ++nSpawned )
	{
		Vei2 spawnPos;
		do
		{
			spawnPos = { xDist( rng ),yDist( rng ) };
		} while( TileAt( spawnPos ).HasMeme() );

		TileAt( spawnPos ).SpawnMeme();
	}

	//reveal test

	for( int i = 0;i < 120; ++i )
	{
		const Vei2 gridPos = { xDist( rng ),yDist( rng ) };
		if( !TileAt( gridPos ).IsRevealed() )
		{
			TileAt(gridPos).Reveal();
		}
	}
}

void MemeField::Draw( Graphics& gfx ) const
{
	gfx.DrawRect( GetRect(),SpriteCodex::baseColor );
	for (Vei2 gridPos = { 0,0 }; gridPos.y < height; ++gridPos.y)
	{
		for( gridPos.x = 0; gridPos.x < width; ++gridPos.x )
		{
			TileAt( gridPos ).Draw( gridPos * SpriteCodex::tileSize,gfx );
		}
	}
}

RectI MemeField::GetRect() const
{
	return RectI( 0,width * SpriteCodex::tileSize,0,height * SpriteCodex::tileSize );
}

MemeField::Tile& MemeField::TileAt( const Vei2& gridPos )
{
	return field[ gridPos.y * width + gridPos.x ];
}

const MemeField::Tile& MemeField::TileAt( const Vei2& gridPos ) const
{
	return field[ gridPos.y * width + gridPos.x ];
}
