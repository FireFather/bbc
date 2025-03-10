# bbc-nnue
**new:**
- updated to C++ (std::C++17)
- separate source and header files
- embedded nnue eval (incbin)
- optimized code base via Clang/Resharper C++ guidelines
- Visual Studio 2022 project files and
- avx2, bmi2, popcnt binaries included

# BBC (Bit Board Chess) + Stockfish NNUE!
 - UCI chess engine by Code Monkey King<br>
 - written for didactic purposes<br>
 - covered in 95 YouTube video series

# Subscribers' projects based on BBC
KhepriChess by Kurt1288: https://github.com/kurt1288/KhepriChess (TypeScript)<br>
KittyEngine by UnboxTheCat: https://github.com/evanhyd/KittyEngineV4 (C++)

# Project sponsors
 - Eduardo Cáceres

# Play BBC online
[![IMAGE ALT TEXT HERE](https://github.com/maksimKorzh/uci-gui/blob/main/gui.png)](https://maksimkorzh.pythonanywhere.com)

# Final release
[![IMAGE ALT TEXT HERE](https://github.com/maksimKorzh/bbc/blob/master/LOGO.png)](https://github.com/maksimKorzh/bbc/releases/tag/1.4)

# YouTube tutorials
[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/QUNP-UjujBM/0.jpg)](https://www.youtube.com/watch?v=QUNP-UjujBM&list=PLmN0neTso3Jxh8ZIylk74JpwfiWNI76Cs)

# Final version CCRL rating is <a href="https://www.computerchess.org.uk/ccrl/404/cgi/engine_details.cgi?match_length=30&each_game=1&print=Details&each_game=1&eng=BBC%201.4%20Stockfish%20NNUE%2064-bit#BBC_1_4_Stockfish_NNUE_64-bit">2937 ELO</a>
 - BBC 1.4 + SF NNUE FINAL VERSION <a href="https://www.computerchess.org.uk/ccrl/404/cgi/engine_details.cgi?match_length=30&each_game=1&print=Details&each_game=1&eng=BBC%201.4%20Stockfish%20NNUE%2064-bit#BBC_1_4_Stockfish_NNUE_64-bit">2937 Elo</a>
 - BBC 1.3 + SF NNUE (<a href="http://talkchess.com/forum3/viewtopic.php?f=6&t=75536">2700+ Elo</a>)
 - BBC 1.2 <a href="https://www.computerchess.org.uk/ccrl/404/cgi/engine_details.cgi?match_length=30&each_game=1&print=Details&each_game=1&eng=BBC%201.2%2064-bit#BBC_1_2_64-bit">1993 ELO</a> 
 - BBC 1.1 <a href="https://www.computerchess.org.uk/ccrl/404/cgi/engine_details.cgi?print=Details&each_game=1&eng=BBC%201.1%2064-bit#BBC_1_1_64-bit">2096 ELO</a>
 - BBC 1.0 <a href="https://www.computerchess.org.uk/ccrl/404/cgi/engine_details.cgi?print=Details&each_game=1&eng=BBC%201.0%2064-bit#BBC_1_0_64-bit">1957 ELO </a>

# Features
 - own online <a href="https://github.com/maksimKorzh/uci-gui">GUI</a>
 - bitboard board representation
 - pre-calculated attack tables
 - magic bitboards for sliding pieces
 - encoding moves as integers
 - copy/make approach for making moves
 - negamax search with alpha beta pruning
 - PV/killer/history move ordering
 - iterative deepening
 - PVS (Principle Variation Search)
 - LMR (Late Move Reduction)
 - NMP (Null Move Pruning)
 - razoring
 - evaluation pruning / static null move pruning
 - Transposition table (up to 128MB)
 - PURE Stockfish NNUE evaluation + 50 move rule penalty
 - UCI protocol

# Credits
  
  This project would never be completed<br>
  without the help of Talk Chess forum<br>
  community members who has been helping me<br>
  during the development process significantly<br>
  I would like to thank to:<br>
  <br>
   - HaraldLuessen & Pedro Castro Elgarresta for neverending hints and technical support
   - Sven Schüle for explaining how mating scores works in TT (in great details!)
   - Jay Warendorff for testing development versions of BBC<br>
   - Marcel Vanthoor for teaching me linear algebra and general inspirations
   - Ronald Friederich for providing PeSTO evaluation tables and sharing his ideas
   - Gabor Szots from CCRL for testing & rating BBC
   - Silvian R. for running private tests
   - Chris Rea for testing and fixing bugs
   - Brendan J. Norman for creating logo for BBC
   - Daniel Shawul for providing NNUE probe library
  <br>
  And all the subscribers watching my videos and<br>
  motivating me to keep the development!<br>

# Test matches
<a href="https://github.com/maksimKorzh/bbc/tree/master/pgn">Get PGN games</a>

# Support the project
<a href="https://www.patreon.com/code_monkey_king">Become a Patron!</a>
  
  
  
  
