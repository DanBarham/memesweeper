#include "MemeField.h"
#include <assert.h>
#include <random>

void MemeField::Tile::SpawnMeme()
{
	assert( !hasMeme );
	hasMeme = true;
}

bool MemeField::Tile::HasMeme() const
{
	return hasMeme;
}

void MemeField::Tile::Draw( const Vei2& screenPos,bool fucked,Graphics& gfx ) const
{
	if( !fucked )
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
				SpriteCodex::DrawTileNumber( screenPos,neighborMemes,gfx );
			}
			else
			{
				SpriteCodex::DrawTileBomb( screenPos,gfx );
			}
			break;
		}
	}
	else
	{
		switch( state )
		{
		case State::HIDDEN:
			if( HasMeme() )
			{
				SpriteCodex::DrawTileBomb( screenPos,gfx );
			}
			SpriteCodex::DrawTileButton( screenPos,gfx );
			break;
		case State::FLAGGED:
			if( HasMeme() )
			{
				SpriteCodex::DrawTileBomb( screenPos,gfx );
			}
			else
			{
				SpriteCodex::DrawTileBomb( screenPos,gfx );
				SpriteCodex::DrawTileCross( screenPos,gfx );
			}			
			break;
		case State::REVEALED:
			if( !hasMeme )
			{
				SpriteCodex::DrawTileNumber( screenPos,neighborMemes,gfx );
			}
			else
			{
				SpriteCodex::DrawTileBombRed( screenPos,gfx );
			}
			break;
		}
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

void MemeField::Tile::ToggleFlag()
{
	assert( state != State::REVEALED );
	if( state == State::HIDDEN )
	{
		state = State::FLAGGED;
	}
	else
	{
		state = State::HIDDEN;
	}
}

bool MemeField::Tile::IsFlagged() const
{
	return state == State::FLAGGED;
}

bool MemeField::Tile::HasNoNeighborMemes() const
{
	return neighborMemes == 0;
}

void MemeField::Tile::SetNeighborMemeCount( int memeCount )
{
	assert( neighborMemes == -1 );
	neighborMemes = memeCount;
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

	for ( Vei2 gridPos = { 0,0 }; gridPos.y < height; ++gridPos.y )
	{
		for( gridPos.x = 0; gridPos.x < width; ++gridPos.x )
		{
			TileAt( gridPos ).SetNeighborMemeCount( CountNeighborMemes( gridPos ) );
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
			const Vei2 screenPosOffset = { (gridPos.x * SpriteCodex::tileSize) + xOffset,(gridPos.y * SpriteCodex::tileSize) + yOffset };
			TileAt( gridPos ).Draw( screenPosOffset,isFucked,gfx );
		}
	}
}

void MemeField::DrawBorder( Graphics& gfx ) const
{
	const int padding = 5;
	RectI borderRect = GetRect().GetExpanded( padding );

	for( int x = borderRect.left; x < borderRect.right; ++x )
	{
		for( int y = borderRect.top; y < borderRect.bottom; ++y )
		{
			if( !GetRect().Contains( { x,y } ) )
			{
				gfx.PutPixel( x,y,Colors::Blue );
			}
		}
	}
}

RectI MemeField::GetRect() const
{
	return RectI( xOffset,width * SpriteCodex::tileSize + xOffset,yOffset,height * SpriteCodex::tileSize + yOffset );
}

bool MemeField::OnRevealClick( const Vei2 screenPos )
{
	if( !isFucked )
	{
		const Vei2 gridPos = ScreenToGrid( screenPos );

		assert( gridPos.x >= 0 && gridPos.x < width && gridPos.y >= 0 && gridPos.y < height );
		Tile& tile = TileAt( gridPos );
		if( !tile.IsRevealed()  && !tile.IsFlagged() )
		{
			RevealTile( gridPos );
			if( tile.HasMeme() )
			{
				isFucked = true;
			}
		}
	}

	return isFucked;
}

void MemeField::OnFlagClick(const Vei2 screenPos)
{
	if( !isFucked )
	{
		const Vei2 gridPos = ScreenToGrid(screenPos);

		assert(gridPos.x >= 0 && gridPos.x < width && gridPos.y >= 0 && gridPos.y < height);
		Tile& tile = TileAt(gridPos);
		if (!tile.IsRevealed())
		{
			tile.ToggleFlag();
		}
	}
}

bool MemeField::CheckWinCondition()
{
	for( int x = 0; x < width; ++x )
	{
		for( int y = 0; y < height; ++y )
		{
			if ( !TileAt( { x,y } ).HasMeme() && !TileAt( { x,y } ).IsRevealed() )
			{
				return false;
			}
		}
	}

	return true;
}

void MemeField::RevealTile(const Vei2 & gridPos)
{
	Tile& tile = TileAt( gridPos );
	tile.Reveal();
	if( tile.HasNoNeighborMemes() )
	{
		const int xStart = std::max( 0,gridPos.x - 1 );
		const int yStart = std::max( 0,gridPos.y - 1 );
		const int xEnd = std::min( width - 1,gridPos.x + 1 );
		const int yEnd = std::min( height - 1,gridPos.y + 1 );

		for ( Vei2 gridPos = { xStart,yStart }; gridPos.y <= yEnd; ++gridPos.y )
		{
			for( gridPos.x = xStart; gridPos.x <= xEnd; ++gridPos.x )
			{
				if( !TileAt( gridPos ).IsRevealed() )
				{
					RevealTile( gridPos );
				}
			}
		}
	}
}

MemeField::Tile& MemeField::TileAt( const Vei2& gridPos )
{
	return field[ gridPos.y * width + gridPos.x ];
}

const MemeField::Tile& MemeField::TileAt( const Vei2& gridPos ) const
{
	return field[ gridPos.y * width + gridPos.x ];
}

Vei2 MemeField::ScreenToGrid( const Vei2& screenPos )
{
	const Vei2 screenPosOffset = { screenPos.x - xOffset,screenPos.y - yOffset };
	return (screenPosOffset / SpriteCodex::tileSize);
}

int MemeField::CountNeighborMemes( const Vei2& gridPos )
{
	const int xStart = std::max( 0,gridPos.x - 1 );
	const int yStart = std::max( 0,gridPos.y - 1 );
	const int xEnd = std::min( width - 1,gridPos.x + 1 );
	const int yEnd = std::min( height - 1,gridPos.y + 1 );

	int count = 0;
	for ( Vei2 gridPos = { xStart,yStart }; gridPos.y <= yEnd; ++gridPos.y )
	{
		for( gridPos.x = xStart; gridPos.x <= xEnd; ++gridPos.x )
		{
			if( TileAt( gridPos ).HasMeme() )
			{
				++count;
			}
		}
	}

	return count;
}
