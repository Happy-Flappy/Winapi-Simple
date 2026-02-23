#include <iostream>
#include "winsimple.h"


class Game
{
	public:
	
	ws::Vec2i MPosition = {0,0};
	bool mouseDown = false;
	ws::Texture boardTex;
	ws::Texture menTex;
	ws::Sprite board;
	ws::Sprite menSpr;
	bool playerVSbot = false;
	bool botVSbot = false;
	ws::Timer delay;
	ws::Timer delay2;
	
	
	Game()
	{
		boardTex.loadFromFile("board.png");
		board.setTexture(boardTex);
		menTex.loadFromFile("men.png");
		menSpr.setTexture(menTex);
		init();
	}
	
	
	//It's easier to spell men than pieces when coding.
	struct Men
	{
		int ID;
		int x,y;
		bool firstTime = true;
		int player;
		
		
		Men()
		{}
		
		Men(int ID,int x,int y,int player) : ID(ID),x(x),y(y),player(player)
		{}
		
		bool operator==(const Men& other) const 
		{
			return ID == other.ID &&
				   x == other.x &&
				   y == other.y &&
				   firstTime == other.firstTime &&
				   player == other.player;
		}		
	};
	
	std::vector<Men> state;
	int selected = -1;
	
	int WHITE_KING = 1,WHITE_QUEEN = 2,WHITE_BISHOP = 3,WHITE_KNIGHT = 4,WHITE_ROOK = 5,WHITE_PAWN = 6;
	int BLACK_KING = 8,BLACK_QUEEN = 9,BLACK_BISHOP = 10,BLACK_KNIGHT = 11,BLACK_ROOK = 12,BLACK_PAWN = 13;
	int PLAYER_WHITE = 15,PLAYER_BLACK = 16;
	int PLAYER_CURRENT = PLAYER_WHITE;
	int MAX = PLAYER_BLACK,MIN = PLAYER_WHITE;
	Men *choosePromote = nullptr;
	
	std::vector<ws::Vec2i> getSpots(std::vector<Men> &s,Men &m)
	{
		std::vector<ws::Vec2i> spots;
		
		if(m.ID == WHITE_PAWN || m.ID == BLACK_PAWN)
		{
			int direction = (m.player == PLAYER_WHITE) ? -1 : 1;
			
			
			if(findManAt(s,m.x,m.y + direction) == -1)
			{
				spots.push_back({m.x,m.y + direction});
				if(m.firstTime)
				{
					if(findManAt(s,m.x,m.y + direction + direction) == -1)
						spots.push_back({m.x,m.y + direction + direction});
				}
			}
			
			int man = findManAt(s,m.x - 1,m.y + direction);
			if(man != -1 && s[man].player != m.player)
				spots.push_back({m.x-1,m.y + direction});
			
			man = findManAt(s,m.x + 1,m.y + direction);
			if(man != -1 && s[man].player != m.player)
				spots.push_back({m.x+1,m.y + direction});
			
			
			//remove out of bounds spots.
			for(int a=0;a<spots.size();a++)
			{
				if(spots[a].x < 0 || spots[a].x > 7 || spots[a].y > 7 || spots[a].y < 0)
				{
					spots.erase(spots.begin() + a);
					a--;
				}
			}				
		}
		
		if(m.ID == WHITE_KING || m.ID == BLACK_KING)
		{
			std::vector<ws::Vec2i> effects = {{-1,-1},{0,-1},{1,-1},{-1,0},{1,0},{-1,1},{0,1},{1,1}};
			
			for(auto &e : effects)
			{
				int man = findManAt(s,m.x + e.x,m.y + e.y);
				if(man != -1 && s[man].player != m.player)
					spots.push_back({m.x + e.x,m.y + e.y});
				if(man == -1)
					spots.push_back({m.x + e.x,m.y + e.y});
			}
			
			//remove out of bounds spots.
			for(int a=0;a<spots.size();a++)
			{
				if(spots[a].x < 0 || spots[a].x > 7 || spots[a].y > 7 || spots[a].y < 0)
				{
					spots.erase(spots.begin() + a);
					a--;
				}
			}
		}
		
		if(m.ID == WHITE_QUEEN || m.ID == BLACK_QUEEN)
		{
			std::vector<ws::Vec2i> dirs = {{0,-1},{0,1},{-1,0},{1,0},{-1,-1},{-1,1},{1,-1},{1,1}};
			for (auto& d : dirs) {
				int nx = m.x + d.x;
				int ny = m.y + d.y;
				while (nx >= 0 && nx < 8 && ny >= 0 && ny < 8) 
				{
					int manIdx = findManAt(s, nx, ny);
					if (manIdx == -1) {
						spots.push_back({nx, ny});
					} 
					else 
					{
						if (s[manIdx].player != m.player)
							spots.push_back({nx, ny});
						break;
					}
					nx += d.x;
					ny += d.y;
				}
			}	
			
			//remove spots from start position
			for(int a=0;a<spots.size();a++)
			{
				if(spots[a].x == m.x && spots[a].y == m.y)
				{
					spots.erase(spots.begin() + a);
					a--;
				}
			}
			
			//remove out of bounds spots.
			for(int a=0;a<spots.size();a++)
			{
				if(spots[a].x < 0 || spots[a].x > 7 || spots[a].y > 7 || spots[a].y < 0)
				{
					spots.erase(spots.begin() + a);
					a--;
				}
			}			
		}

		if(m.ID == WHITE_KNIGHT || m.ID == BLACK_KNIGHT)
		{
			std::vector<ws::Vec2i> points = {{-1,-2},{1,-2},{-2,-1},{-2,1},{-1,2},{1,2},{2,1},{2,-1}};
			
			for(auto& p:points)
			{
				if(m.x + p.x >= 0 && m.x + p.x < 8 && m.y + p.y >= 0 && m.y + p.y < 8)
				{
					int man = findManAt(s,m.x + p.x,m.y + p.y);
					if(man != -1 && s[man].player != m.player)
					{
						spots.push_back({m.x + p.x,m.y + p.y});
					}
					if(man == -1)
						spots.push_back({m.x + p.x,m.y + p.y});
				}
			}
			
		}
		
		
		if(m.ID == WHITE_BISHOP || m.ID == BLACK_BISHOP)
		{
			std::vector<ws::Vec2i> dir = {{-1,-1},{1,-1},{-1,1},{1,1}};
			
			for(auto &d:dir)
			{
				int nx = m.x + d.x;
				int ny = m.y + d.y;
				
				while(nx >= 0 && nx < 8 && ny >= 0 && ny < 8)
				{
					int man = findManAt(s, nx, ny);
					if (man == -1) {
						spots.push_back({nx, ny});
					} 
					else 
					{
						if (s[man].player != m.player)
							spots.push_back({nx, ny});
						break;
					}
					nx += d.x;
					ny += d.y;
				}
			}
		}
		
		if(m.ID == WHITE_ROOK || m.ID == BLACK_ROOK)
		{
			std::vector<ws::Vec2i> dir = {{-1,0},{0,-1},{1,0},{0,1}};
			
			for(auto &d:dir)
			{
				int nx = m.x + d.x;
				int ny = m.y + d.y;
				
				while(nx >= 0 && nx < 8 && ny >= 0 && ny < 8)
				{
					int man = findManAt(s, nx, ny);
					if (man == -1) {
						spots.push_back({nx, ny});
					} 
					else 
					{
						if (s[man].player != m.player)
							spots.push_back({nx, ny});
						break;
					}
					nx += d.x;
					ny += d.y;
				}
			}
		}
		

		
		return spots;
	}
	
	
	
	bool move(std::vector<Men> &s,Men &m,ws::Vec2i r,int PLAYER)
	{
		choosePromote = nullptr;
		std::vector<ws::Vec2i> spots = getSpots(s,m);
		
		for(int a=0;a<spots.size();a++)
		{
			if(spots[a].x == r.x && spots[a].y == r.y)
			{
				int man = findManAt(s,r.x,r.y);




				if(PLAYER == PLAYER_BLACK)
				{
					if(s == state)
						PLAYER_CURRENT = PLAYER_WHITE;
					else
						PLAYER = PLAYER_WHITE;
				}
				else
				{
					if(s == state)
						PLAYER_CURRENT = PLAYER_BLACK;
					else
						PLAYER = PLAYER_BLACK;
				}


				m.x = r.x;
				m.y = r.y;
				m.firstTime = false;

				if (man != -1)
				{
					if(s[man].player != m.player)
					{
						s.erase(s.begin() + man);
					}
				}
				
				if((m.ID == WHITE_PAWN && m.y == 0) || (m.ID == BLACK_PAWN && m.y == 7))
					choosePromote = &m;
				

				return true;
			}
		}
		
		return false;
	}

	
	int findManAt(std::vector<Men> &s,int x,int y)
	{
		for(int a=0;a<s.size();a++)
		{
			if(s[a].x == x && s[a].y == y)
				return a;
		}
		return -1;
	}

	void initTestBoard()
	{
		//state.push_back(Men(BLACK_ROOK,0,0,PLAYER_BLACK));
		state.push_back(Men(BLACK_KNIGHT,1,0,PLAYER_BLACK));
		state.push_back(Men(BLACK_BISHOP,2,0,PLAYER_BLACK));
		state.push_back(Men(BLACK_QUEEN,3,0,PLAYER_BLACK));
		state.push_back(Men(BLACK_KING,4,0,PLAYER_BLACK));
		state.push_back(Men(BLACK_BISHOP,5,0,PLAYER_BLACK));
		state.push_back(Men(BLACK_KNIGHT,6,0,PLAYER_BLACK));
		state.push_back(Men(BLACK_ROOK,7,0,PLAYER_BLACK));
		
		//for(int x=0;x<8;x++)
		//	state.push_back(Men(BLACK_PAWN,x,1,PLAYER_BLACK));
		state.push_back(Men(WHITE_PAWN,0,1,PLAYER_WHITE));
		
		state.push_back(Men(WHITE_ROOK,0,7,PLAYER_WHITE));
		state.push_back(Men(WHITE_KNIGHT,1,7,PLAYER_WHITE));
		state.push_back(Men(WHITE_BISHOP,2,7,PLAYER_WHITE));
		state.push_back(Men(WHITE_QUEEN,3,7,PLAYER_WHITE));
		state.push_back(Men(WHITE_KING,4,7,PLAYER_WHITE));
		state.push_back(Men(WHITE_BISHOP,5,7,PLAYER_WHITE));
		state.push_back(Men(WHITE_KNIGHT,6,7,PLAYER_WHITE));
		state.push_back(Men(WHITE_ROOK,7,7,PLAYER_WHITE));
		
		for(int x=0;x<8;x++)
			state.push_back(Men(WHITE_PAWN,x,6,PLAYER_WHITE));
						
	}


	void initNormalBoard()
	{
		state.push_back(Men(BLACK_ROOK,0,0,PLAYER_BLACK));
		state.push_back(Men(BLACK_KNIGHT,1,0,PLAYER_BLACK));
		state.push_back(Men(BLACK_BISHOP,2,0,PLAYER_BLACK));
		state.push_back(Men(BLACK_QUEEN,3,0,PLAYER_BLACK));
		state.push_back(Men(BLACK_KING,4,0,PLAYER_BLACK));
		state.push_back(Men(BLACK_BISHOP,5,0,PLAYER_BLACK));
		state.push_back(Men(BLACK_KNIGHT,6,0,PLAYER_BLACK));
		state.push_back(Men(BLACK_ROOK,7,0,PLAYER_BLACK));
		
		for(int x=0;x<8;x++)
			state.push_back(Men(BLACK_PAWN,x,1,PLAYER_BLACK));
		
		state.push_back(Men(WHITE_ROOK,0,7,PLAYER_WHITE));
		state.push_back(Men(WHITE_KNIGHT,1,7,PLAYER_WHITE));
		state.push_back(Men(WHITE_BISHOP,2,7,PLAYER_WHITE));
		state.push_back(Men(WHITE_QUEEN,3,7,PLAYER_WHITE));
		state.push_back(Men(WHITE_KING,4,7,PLAYER_WHITE));
		state.push_back(Men(WHITE_BISHOP,5,7,PLAYER_WHITE));
		state.push_back(Men(WHITE_KNIGHT,6,7,PLAYER_WHITE));
		state.push_back(Men(WHITE_ROOK,7,7,PLAYER_WHITE));
		
		for(int x=0;x<8;x++)
			state.push_back(Men(WHITE_PAWN,x,6,PLAYER_WHITE));
				
	}


	void initAllKings()
	{
			
/* 		std::vector<std::vector<int>> pattern = 
		{
			{0,0,0,0,0,0,0,0},
			{0,0,0,1,1,0,0,0},
			{0,0,1,1,1,1,0,0},
			{0,1,1,1,1,1,1,0},
			{0,1,1,1,1,1,1,0},
			{0,0,1,1,1,1,0,0},
			{0,0,0,1,1,0,0,0},
			{0,0,0,0,0,0,0,0}
		}; */
		
		std::vector<std::vector<int>> pattern = 
		{
			{0,0,0,1,1,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,2,2,0,0,0}
		};		
		
		
		for(int a = 0;a < pattern.size();a++)
		{
			for(int b=0;b<pattern[a].size();b++)
			{
				if(pattern[a][b] == 2)
					state.push_back(Men(WHITE_QUEEN,a,b,PLAYER_WHITE));
				if(pattern[a][b] == 1)
					state.push_back(Men(BLACK_QUEEN,a,b,PLAYER_BLACK));
			}
		}
	}

	
	void init()
	{
		//initTestBoard();
		initNormalBoard();		
		//initAllKings();
	}
	
	
	int score(std::vector<Men> &s)
	{
		int score = 0;
		
		int PAWN_VALUE = 100;
		int KNIGHT_VALUE = 320;
		int BISHOP_VALUE = 330;
		int ROOK_VALUE = 500;
		int QUEEN_VALUE = 900;
		int KING_VALUE = 20000; 
		
		for(auto& piece : s)
		{
			int pieceValue = 0;
			
			if(piece.ID == WHITE_PAWN || piece.ID == BLACK_PAWN)
				pieceValue = PAWN_VALUE;
			if(piece.ID == WHITE_KNIGHT || piece.ID == BLACK_KNIGHT)
				pieceValue = KNIGHT_VALUE;
			if(piece.ID == WHITE_BISHOP || piece.ID == BLACK_BISHOP)
				pieceValue = BISHOP_VALUE;
			if(piece.ID == WHITE_ROOK || piece.ID == BLACK_ROOK)
				pieceValue = ROOK_VALUE;
			if(piece.ID == WHITE_QUEEN || piece.ID == BLACK_QUEEN)
				pieceValue = QUEEN_VALUE;
			if(piece.ID == WHITE_KING || piece.ID == BLACK_KING)
				pieceValue = KING_VALUE;
			
			
			if(piece.player == MAX)
				score += pieceValue;
			else
				score -= pieceValue;
		}
		
		return score;
	}
	
	
	
	bool terminal(std::vector<Men> &s)
	{
		return false;
	}
	
	int minimax(std::vector<Men> s,int depth,int PLAYER,bool isMaximizing)
	{
		if(terminal(s) || depth == 0)
			return score(s);
		
		if(isMaximizing)
		{
			int maxEval = -99999;
			bool hasValidMove = false;
			
			for(int i=0;i<s.size();i++)
			{
				if(s[i].player != PLAYER)//skip opponent pieces
					continue;
					
				std::vector<ws::Vec2i> moves = getSpots(s, s[i]);	
				
				for(auto& movePos : moves)
				{
					hasValidMove = true;
					std::vector<Men> ns = s;
					Men &piece = ns[i];
					
					move(ns,piece,movePos,PLAYER);
					
					int eval = minimax(ns,depth - 1,(PLAYER == MAX) ? MIN : MAX,false);
					
					maxEval = (maxEval < eval) ? eval : maxEval;
				}
				
			}
			
			if(!hasValidMove)
				return score(s);
			
			return maxEval;
		}
		else
		{
			int minEval = 99999;
			bool hasValidMove = false;
			
			for(int i=0;i<s.size();i++)
			{
				if(s[i].player != PLAYER)//skip opponent pieces
					continue;
					
				std::vector<ws::Vec2i> moves = getSpots(s, s[i]);	
				
				for(auto& movePos : moves)
				{
					hasValidMove = true;
					std::vector<Men> ns = s;
					Men &piece = ns[i];
					
					move(ns,piece,movePos,PLAYER);
					
					int eval = minimax(ns,depth - 1,(PLAYER == MIN) ? MAX : MIN,true);
					
					minEval = (minEval > eval) ? eval : minEval;
				}
				
			}
			
			
			if(!hasValidMove)
				return score(s);
			
			return minEval;		
		}
	}
	
	
	ws::Vec2i findBestMove(std::vector<Men>& s, int PLAYER, int depth)
	{
		int bestValue = (PLAYER == MAX) ? -999999 : 999999;
		ws::Vec2i bestMove = {-1, -1};
		int bestPieceIndex = -1;
		
		for(int i = 0; i < s.size(); i++)
		{
			if(s[i].player != PLAYER) continue;
			
			std::vector<ws::Vec2i> moves = getSpots(s, s[i]);
			
			for(auto& movePos : moves)
			{
				std::vector<Men> newState = s;
				Men& piece = newState[i];
				move(newState, piece, movePos, PLAYER);
				
				int moveValue = minimax(newState, depth - 1, (PLAYER == MAX ? MIN : MAX), PLAYER == MIN);
				
				if(PLAYER == MAX && moveValue > bestValue)
				{
					bestValue = moveValue;
					bestMove = movePos;
					bestPieceIndex = i;
				}
				else if(PLAYER == MIN && moveValue < bestValue)
				{
					bestValue = moveValue;
					bestMove = movePos;
					bestPieceIndex = i;
				}
			}
		}
		
		// apply the best move to the actual state
		if(bestPieceIndex != -1)
			move(s, s[bestPieceIndex], bestMove, PLAYER);
		
		return bestMove;
	}
	
	
	
	
	void update(ws::Window &window)
	{
		
		
		if(playerVSbot)
		{
			if(PLAYER_CURRENT == PLAYER_WHITE)
			{
				//user Controlled Game
				
				//get offset from the corner of the actual board grid.
				int offX = (MPosition.x - 108);
				int offY = (MPosition.y - 108);
				
				if(offX == 0)
					offX = 1;
				if(offY == 0)
					offY = 1;
				
				int mgridx = offX / (580/8);
				int mgridy = offY / (580/8);
				
				//Get selection
				
				if(mgridx >= 0 && mgridx < 8 && mgridy >= 0 && mgridy < 8)
				{
					if(mouseDown)
					{
						
						int man = findManAt(state,mgridx,mgridy);
						if(man != -1 && state[man].player == PLAYER_CURRENT)
							selected = man;
						else if(selected != -1)
						{
							if(move(state,state[selected],ws::Vec2i(mgridx,mgridy),PLAYER_CURRENT))
								selected = -1; 
						}
					}
				}
			}
			else
			{
				//now it's the bots turn
				findBestMove(state,PLAYER_CURRENT,3);
			}
			
			
			
			if(choosePromote != nullptr)
			{
				
				ws::Window request(300,150,"Choose a promotion!");
				request.removeStyle(WS_THICKFRAME | WS_MAXIMIZEBOX);
				
				ws::Button queenbtn,bishopbtn,knightbtn,rookbtn;
				queenbtn.setSize(100,50);
				queenbtn.setText("Queen");
				queenbtn.setPosition(request.getSize().x * 0.25  - queenbtn.getSize().x/2,request.getSize().y * 0.25 - queenbtn.getSize().y/2);
				request.addChild(queenbtn);
				
				bishopbtn.setSize(100,50);
				bishopbtn.setText("Bishop");
				bishopbtn.setPosition(request.getSize().x * 0.75  - queenbtn.getSize().x/2,request.getSize().y * 0.25  - queenbtn.getSize().y/2);
				request.addChild(bishopbtn);
				
				knightbtn.setSize(100,50);
				knightbtn.setText("Knight");
				knightbtn.setPosition(request.getSize().x * 0.25  - queenbtn.getSize().x/2,request.getSize().y * 0.75  - queenbtn.getSize().y/2);
				request.addChild(knightbtn);
				
				rookbtn.setSize(100,50);
				rookbtn.setText("Rook");
				rookbtn.setPosition(request.getSize().x * 0.75  - queenbtn.getSize().x/2,request.getSize().y * 0.75  - queenbtn.getSize().y/2);
				request.addChild(rookbtn);
				
				
				
				while(request.isOpen())
				{
					MSG m;
					while(request.pollEvent(m))
					{
						if(queenbtn.isPressed(m))
						{
							if(choosePromote->player == PLAYER_WHITE)
								choosePromote->ID = WHITE_QUEEN;
							else
								choosePromote->ID = BLACK_QUEEN;
							choosePromote = nullptr;
							request.close();
							continue;
						}
						if(bishopbtn.isPressed(m))
						{
							if(choosePromote->player == PLAYER_WHITE)
								choosePromote->ID = WHITE_BISHOP;
							else
								choosePromote->ID = BLACK_BISHOP;
							choosePromote = nullptr;
							request.close();
							continue;
						}							
						if(knightbtn.isPressed(m))
						{
							if(choosePromote->player == PLAYER_WHITE)
								choosePromote->ID = WHITE_KNIGHT;
							else
								choosePromote->ID = BLACK_KNIGHT;
							choosePromote = nullptr;
							request.close();
							continue;
						}							
						if(rookbtn.isPressed(m))
						{
							if(choosePromote->player == PLAYER_WHITE)
								choosePromote->ID = WHITE_ROOK;
							else
								choosePromote->ID = BLACK_ROOK;
							choosePromote = nullptr;
							request.close();
							continue;
						}							
					}
					
					request.clear(ws::Hue(37,37,37));
					request.display();
				}
				//if it reaches this point and choosePromote is not nullptr, then assume queen promotion.
				if(choosePromote != nullptr)
				{
					if(choosePromote->player == PLAYER_WHITE)
						choosePromote->ID = WHITE_QUEEN;
					else
						choosePromote->ID = BLACK_QUEEN;
					choosePromote = nullptr;
				}
					
			}
			
		}
		
		if(botVSbot)
		{
			

				findBestMove(state,PLAYER_CURRENT,3);
				int r = std::rand() % 13;
				r++;
				int x,y;
				x = rand()%8;
				y = rand()%8;
				
				for(int a=0;a<state.size();a++)
				{
					if(x == state[a].x && y == state[a].y)
					{
						x = rand()%8;
						y = rand()%8;
						
					}
				}
				
				for(int a=0;a<state.size();a++)
				{
					if(x == state[a].x && y == state[a].y)
						return;
				}
				
				if(r != 7)
					state.push_back(Men(r,x,y,(r <= 6) ? PLAYER_WHITE : PLAYER_BLACK));
			
			
			
			

		}
		
		
		
		
	}
	
	
	void draw(ws::Window &window)
	{
		window.draw(board);
		
		//108,108
		for(int a=0;a<state.size();a++)
		{
			int x = 108 + state[a].x * (580/8);
			int y = 108 + state[a].y * (580/8);
			
			
			menSpr.setPosition(x,y);
			if(state[a].ID == WHITE_PAWN)
				menSpr.setTextureRect(ws::IntRect(50 * 8,50 * 1,50,50));
			if(state[a].ID == BLACK_PAWN)
				menSpr.setTextureRect(ws::IntRect(50 * 8,50 * 0,50,50));
			
			if(state[a].ID == WHITE_ROOK)
				menSpr.setTextureRect(ws::IntRect(50 * 0,50 * 1,50,50));
			if(state[a].ID == WHITE_KNIGHT)
				menSpr.setTextureRect(ws::IntRect(50 * 1,50 * 1,50,50));
			if(state[a].ID == WHITE_BISHOP)
				menSpr.setTextureRect(ws::IntRect(50 * 2,50 * 1,50,50));
			if(state[a].ID == WHITE_QUEEN)
				menSpr.setTextureRect(ws::IntRect(50 * 3,50 * 1,50,50));
			if(state[a].ID == WHITE_KING)
				menSpr.setTextureRect(ws::IntRect(50 * 4,50 * 1,50,50));

			if(state[a].ID == BLACK_ROOK)
				menSpr.setTextureRect(ws::IntRect(50 * 0,50 * 0,50,50));
			if(state[a].ID == BLACK_KNIGHT)
				menSpr.setTextureRect(ws::IntRect(50 * 1,50 * 0,50,50));
			if(state[a].ID == BLACK_BISHOP)
				menSpr.setTextureRect(ws::IntRect(50 * 2,50 * 0,50,50));
			if(state[a].ID == BLACK_QUEEN)
				menSpr.setTextureRect(ws::IntRect(50 * 3,50 * 0,50,50));
			if(state[a].ID == BLACK_KING)
				menSpr.setTextureRect(ws::IntRect(50 * 4,50 * 0,50,50));
			
			if(selected != -1)
			{
				if(selected == a)
				{
					ws::IntRect r = menSpr.getTextureRect();
					r.top += 100;
					menSpr.setTextureRect(r);
				}
			}
			menSpr.setScale(1.5,1.5);
			window.draw(menSpr);
		}
		
	
		
		//draw an identifier where a move is valid.
		if(selected != -1)
		{
			std::vector<ws::Vec2i> spots;
			spots = getSpots(state,state[selected]);
			
			
			ws::Round round;
			round.setSize(580/8 - 3,580/8 - 3);
			round.setFillColor(ws::Hue(100,200,100,150));
			round.setBorderWidth(0);
			round.setBorderColor(ws::Hue(0,0,0,0));
			for(int a=0;a<spots.size();a++)
			{
				int x = 108 + (spots[a].x ) * (580/8);
				int y = 108 + (spots[a].y) * (580/8);
				round.setPosition(x + 5,y - 2);
				window.draw(round);
			}
		}
		
	
	}
	
}game;


int main()
{
	ws::Window window(800,800,"Chess Game");
	game.botVSbot = true;
	
	while(window.isOpen())
	{
		game.mouseDown = false;
		MSG m;
		while(window.pollEvent(m))
		{
			
			if(m.message == WM_LBUTTONDOWN)
				game.mouseDown = true;
			if(m.message == WM_MOUSEMOVE)
			{
				int x = GET_X_LPARAM(m.lParam);
				int y = GET_Y_LPARAM(m.lParam);
				game.MPosition = window.toWorld({x,y});
			}
			
		}
		
		window.clear();
		game.update(window);
		game.draw(window);
		window.display();
	}
}