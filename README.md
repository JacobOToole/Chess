# Chess

A rudimentary chess application with a rules-complete GUI and a planned UCI-compatible engine.

**Status:** GUI is fully playable with all chess rules implemented. The engine is currently scaffolded as a separate binary but cannot play moves.

## Features

**Rules (complete):**
- Piece movement for all 6 piece types
- Castling with attack and rights check
- En passant captures
- Pawn promotion with an interactive picker
- Check detection that uses a simulate-and-verify legality filter, which handles pinned pieces, discovered checks, and moves into check
- Complete draw detection, with zorbist hasing used to detect three-fold repetition

**Interface** 
- Click-to-move with legal move highlighting
- Last move square highlight
- Check highlight
- Coodrinate labels
- 'Game over' screen (Checkmate, stalemate, three types of draws)

**Planned**
- FEN import and export
- UCI engine integration
- **Custom engine** with minimax search and alpha-beta pruning
