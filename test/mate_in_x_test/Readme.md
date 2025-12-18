# Mate-in-x test

The test analyses positions which have a forced mate at a given search depth.
The position, mate depth, and best move are read from an EPD file that can
contain many such test cases. The test shall make sure that the search algorithm
for moves in positions is working correctly.

Each test record in the input EPD file has to specify the position (as by EPD
format specification), the `bm` best move (can also give multiple moves) and the
`pv` with a move list (starting with one of the best moves). The latter move
list is currently used to determine the expected mate depth.

## lichess puzzle database

As a source of mate-in-x puzzles, the [lichess puzzles](https://database.lichess.org/#puzzles)
can be downloaded from the website. These contain a set of mate-in-x puzzles.
To extract only the mate-in-x puzzles, the **lichess_converter** is provided. It
reads the lichess database CSV file, prepares the puzzles and writes them as an
EPD file.

### Conversion process

The lichess database is a CSV file that specifies a puzzle id, a position, a
list of moves, and a puzzle theme (amongst other information).

> The position is _not_ the starting position for the puzzle! Instead, the first
move of the "solution" has to be applied. Only the moves starting from the
second move in the move list describe the solution.

The lichess_converter handles this by applying the first move to the position
and outputting the new position as the puzzle start. The second move in the move
list is given as the `bm` (best move), while the move list without the first
move is stored as the `pv` for the puzzle.

### Ambiguous best moves

For mate in 1 puzzles, only a single best move is given in the CSV file,
although multiple moves might be possible. The lichess_converter can therefore
been provided with the path to stockfish. This is then used to find multiple
possible solutons for mate in 1 puzzles.

Here is an example of using stockfish:

```sh
> ./stockfish

uci
isready
setoption name MultiPV value 10

position fen 6k1/p1p3pp/4N3/1p6/2q1r1n1/2B5/PP4PP/3R1R1K w - - 0 1
go depth 1
```

Stockfish output:

```sh
info depth 1 seldepth 3 multipv 1 score mate 1 nodes 521 nps 173666 hashfull 0 tbhits 0 time 3 pv d1d8
info depth 1 seldepth 2 multipv 2 score mate 1 nodes 521 nps 173666 hashfull 0 tbhits 0 time 3 pv f1f8
info depth 1 seldepth 2 multipv 3 score cp 301 nodes 521 nps 173666 hashfull 0 tbhits 0 time 3 pv e6g7
info depth 1 seldepth 3 multipv 4 score cp 159 nodes 521 nps 130250 hashfull 0 tbhits 0 time 4 pv b2b4
info depth 1 seldepth 3 multipv 5 score cp 142 nodes 521 nps 130250 hashfull 0 tbhits 0 time 4 pv h2h4
info depth 1 seldepth 3 multipv 6 score cp -41 nodes 521 nps 130250 hashfull 0 tbhits 0 time 4 pv c3b4
info depth 1 seldepth 3 multipv 7 score cp -55 nodes 521 nps 130250 hashfull 0 tbhits 0 time 4 pv b2b3 c4c3
info depth 1 seldepth 3 multipv 8 score cp -76 nodes 521 nps 130250 hashfull 0 tbhits 0 time 4 pv h2h3 e4e6
info depth 1 seldepth 3 multipv 9 score cp -133 nodes 521 nps 104200 hashfull 0 tbhits 0 time 5 pv a2a3 c4e6
info depth 1 seldepth 3 multipv 10 score cp -138 nodes 521 nps 104200 hashfull 0 tbhits 0 time 5 pv c3e5 g4e5
bestmove d1d8
```

Lines with `score mate x` describe a solution with a mate-in-x. Here, we see two
possible solutions `d1d8` and `f1f8`.

This is automatically done for mate-in-1 puzzles, when the conveter is started
with the `--stockfish <path>` argument.

### Puzzle output

If a file name is specified, all puzzles are written into that file. If instead
a directory is given, the converter writes different files with names
`mate_in_x.epd`, where x is replaced by the mate depth.
