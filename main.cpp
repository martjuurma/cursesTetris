#include <iostream>
#include <array>
#include <string>
#include <vector>
#include <curses.h>

std::string tetromino[7];

const int fieldWidth = 12;
const int fieldHeight = 18;

unsigned char* screen = nullptr;
unsigned char* playField = nullptr;


const int screenWidth = 80;
const int screenHeight = 30;
const int startY = 0;
const int startX = 0;

// function prototypes
WINDOW* createNewWindow(int screeHeight, int screenWidth, int startY, int startX);
void destroyWindow(WINDOW* localWindow);
bool DoesPieceFit(int nTetromino, int rotation, int posY, int posX);
int Rotate(int px, int py, int r);

int main()
{
    //WINDOW* tetrisWin;

    initscr();                  //start curses
    cbreak();                   //disable terminal buffering
    keypad(stdscr, TRUE);       //enable keypad functions
    resize_term(screenHeight, screenWidth);
    timeout(50);                // timeout for blocking functions (like getch())


    //create tetromino assets
    tetromino[0].append("..X.");
    tetromino[0].append("..X.");
    tetromino[0].append("..X.");
    tetromino[0].append("..X.");

    tetromino[1].append("..X.");
    tetromino[1].append(".XX.");
    tetromino[1].append(".X..");
    tetromino[1].append("....");

    tetromino[2].append(".X..");
    tetromino[2].append(".XX.");
    tetromino[2].append("..X.");
    tetromino[2].append("....");

    tetromino[3].append("....");
    tetromino[3].append(".XX.");
    tetromino[3].append(".XX.");
    tetromino[3].append("....");

    tetromino[4].append("..X.");
    tetromino[4].append(".XX.");
    tetromino[4].append("..X.");
    tetromino[4].append("....");

    tetromino[5].append("....");
    tetromino[5].append(".XX.");
    tetromino[5].append(".X..");
    tetromino[5].append(".X..");

    tetromino[6].append("..X.");
    tetromino[6].append("..X.");
    tetromino[6].append(".XX.");
    tetromino[6].append("....");

    screen = new unsigned char[screenWidth * screenHeight];
    for (int i = 0; i < screenWidth * screenHeight; i++) { screen[i] = ' '; }

    playField = new unsigned char[fieldHeight * fieldWidth];

    for (int x = 0; x < fieldWidth; x++)
        for (int y = 0; y < fieldHeight; y++)
            playField[y * fieldWidth + x] = (x == 0 || x == fieldWidth - 1 || y == fieldHeight - 1) ? 9 : 0;

    //GAME LOGIC
    bool gameOver = false;
    bool rotateHold = true;
    bool forceDown = false;
    int currentPiece = 1;
    int currentRotation = 0;
    int currentX = fieldWidth / 2;
    int currentY = 0;
    int c;
    int speed = 20;
    int speedCounter = 0;
    std::vector<int> lines;
    int pieceCounter = 0;
    int score = 0;

     // draw field
    for (int x = 0; x < fieldWidth; x++)
        for (int y = 0; y < fieldHeight; y++)
            screen[(y + 2) * screenWidth + (x + 2)] = " ABCDEFG=#"[playField[y * fieldWidth + x]];

    // draw current piece
    for (int px = 0; px < 4; px++)
        for (int py = 0; py < 4; py++)
            if (tetromino[currentPiece][Rotate(px, py, currentRotation)] == 'X')
                screen[(currentY + py + 2) * screenWidth + (currentX + px + 2)] = currentPiece + 65;

    // write the screen to buffer
    addstr(reinterpret_cast<const char*>(screen));

    // display the screen buffer
    refresh();

    while (!gameOver)
    {
        //GAME TIMING ****************************************************
        //napms(50); // getch() ja timeout() hoolitsevad praegu ajastuse eest

        //USER INPUT *****************************************************
        c = getch();
        speedCounter++;
        forceDown = (speedCounter == speed);

        if (c == KEY_LEFT)
        {
            if (DoesPieceFit(currentPiece, currentRotation, currentY, currentX - 1))
            {
                currentX = currentX - 1;
            }
        }
        if (c == KEY_RIGHT)
        {
            if (DoesPieceFit(currentPiece, currentRotation, currentY, currentX + 1))
            {
                currentX = currentX + 1;
            }
        }
        if (c == KEY_DOWN)
        {
            if (DoesPieceFit(currentPiece, currentRotation, currentY + 1, currentX))
            {
                currentY = currentY + 1;
            }
        }
        if (c == KEY_UP)
        {
/*        	if (rotateHold && DoesPieceFit(currentPiece, currentRotation + 1, currentY, currentX))
            {
                rotateHold = false;
            }
            else
            {
                while(getch() != ERR) rotateHold = true;
                currentRotation = currentRotation + 1;
            } */
            currentRotation += (rotateHold && DoesPieceFit(currentPiece, currentRotation + 1, currentY, currentX)) ? 1 : 0;
			rotateHold = false;
        }
        else
        {
            rotateHold = true;
        }
        if (c == KEY_BACKSPACE) gameOver = true;


        //GAME LOGIC *****************************************************
        if(forceDown)
        {
            speedCounter = 0;

            // test if piece can be pushed down
            if(DoesPieceFit(currentPiece, currentRotation, currentY + 1, currentX))
                    currentY++;
            else
            {
				// It can't! Lock the piece in place
				for (int px = 0; px < 4; px++)
					for (int py = 0; py < 4; py++)
						if (tetromino[currentPiece][Rotate(px, py, currentRotation)] == 'X')
							playField[(currentY + py) * fieldWidth + (currentX + px)] = currentPiece + 1;

                pieceCounter++;
                    if(pieceCounter % 10 == 0)
                        if(speed >= 10) speed--;

                // check for horizontal lines
                for(int py = 0; py < 4; py++)
                    if(currentY + py < fieldHeight - 1)
                    {
                        bool line = true;
                        for(int px = 1; px < fieldWidth - 1 ; px++ )
                            line &= (playField[(currentY + py) * fieldWidth + px]) != 0;
                        
                        if(line)
                        {
                            // Remove line, set line to =
                            for(int px = 1; px < fieldWidth - 1; px++)
                                playField[(currentY + py) * fieldWidth + px] = 8;
                                lines.push_back(currentY + py);
                        }
                    }
                score += 25;
                if(!lines.empty()) score += (1 << lines.size()) * 100;

                // choose next piece
                currentPiece = rand() % 7;
                currentRotation = 0;
                currentX = fieldWidth / 2;
                currentY = 0;

                // if piece don' fit, end game
                gameOver = !DoesPieceFit(currentPiece, currentRotation, currentY, currentX);
            } //end else
        } //end if(forceDown)

        //RENDER OUTPUT **************************************************
       
        // Clear the terminal screen
        clear();

        // Draw playing field
        for (int x = 0; x < fieldWidth; x++)
            for (int y = 0; y < fieldHeight; y++)
                screen[(y + 2) * screenWidth + (x + 2)] = " ABCDEFG=#"[playField[y * fieldWidth + x]];

        // Draw current piece
        for (int px = 0; px < 4; px++)
            for (int py = 0; py < 4; py++)
                if (tetromino[currentPiece][Rotate(px, py, currentRotation)] != '.')
                    screen[(currentY + py + 2) * screenWidth + (currentX + px + 2)] = currentPiece + 65;
        
        // Line completion animation
        if(!lines.empty())
        {
            //write the screen to buffer
            addstr(reinterpret_cast<const char*>(screen));
            //display
            refresh();
            napms(400);

            for(auto &v : lines)
            {
                for(int px = 1; px < fieldWidth - 1; px++)
                {
                    for(int py = v; py > 0; py--)
                        playField[py * fieldWidth + px] = playField[(py - 1) * fieldWidth + px];
                        playField[px] = 0;
                }

            }
            lines.clear();
        }

        // write the whole screen to buffer
        mvaddstr(0,0,reinterpret_cast<const char*>(screen));
        // display score
         mvprintw(0,16,"SCORE: %4d", score);
        
        // display frame
        refresh();
    }

    endwin(); //uninitialize ncurses
    return 0;
}

int Rotate(int px, int py, int r)
{
    switch (r % 4)
    {
    case 0: return py * 4 + px;         // rotate 0
    case 1: return 12 + py - (px * 4);  // rotate 90
    case 2: return 15 - (py * 4) - px;  // rotate 180
    case 3: return 3 - py + (px * 4);   // rotate 270
    }
    return 0;
}

bool DoesPieceFit(int nTetromino, int rotation, int posY, int posX)
{
        for (int px = 0; px < 4; px++)
        for (int py = 0; py < 4; py++)
        {
            //Get index into piece
            int pi = Rotate(px, py, rotation);
            //Get index into field
            int fi = (posY + py) * fieldWidth + (posX + px);

            if (posX + px >= 0 && posX + px < fieldWidth)
            {
                if (posY + py >= 0 && posY + py < fieldHeight)
                {
                    if ((tetromino[nTetromino][pi] == 'X') && (playField[fi] != 0))
                        return false; // fail on first hit
                }
            }

       }
    return true;
}

WINDOW* createNewWindow(int screenHeight, int screenWidth, int startY, int startX)
{
    WINDOW* localWindow;
    localWindow = newwin(screenHeight, screenWidth, startY, startX);

    wrefresh(localWindow);
    return localWindow;
}

void destroyWindow(WINDOW* localWindow)
{
    delwin(localWindow);
}
