#include <iostream>
#include <string>
#include <vector>

using namespace std;

enum class Piece {
    PAWN,
    ROOK,
    NIGHT, // Intentional spelling mistake
    BISHOP,
    QUEEN,
    KING,
    NOTHING
};

enum class Player {
    TOP = 1,
    BOTTOM = 0,
    NONE = 2
};

class Chess
{
private:
    string playerNames[2];
    Player playingPlayer = Player::BOTTOM; // 0 is bottom player, 1 is top player

public:
    char board[8][8];

    Chess()
    {
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                board[i][j] = ' ';
            }
        }

        playerNames[0] = "player blue";
        playerNames[1] = "player red";
    }

    Player getPlayingPlayer() {
        return playingPlayer;
    }

    string getPlayingPlayerWithName() {
        if (playingPlayer == Player::NONE) return string();
        return playerNames[static_cast<int>(playingPlayer)];
    }

    void setPlayingPlayer(Player player) { // For debugging purposes
        playingPlayer = player;
    }

    void switchPlayer() {
        if (playingPlayer == Player::BOTTOM) playingPlayer = Player::TOP;
        else if (playingPlayer == Player::TOP) playingPlayer = Player::BOTTOM;
    }

    void initialize()
    {
        // string player1;
        // string player2;
        
        // cout << "Player 1 name: ";
        // cin >> player1;

        // cout << "Player 2 name: ";
        // cin >> player2;
        
        // playerNames[0] = player1;
        // playerNames[1] = player2;

        char layout1[] = "rnbqkbnr";
        char layout2[] = "pppppppp";

        char layout3[] = "PPPPPPPP";
        char layout4[] = "RNBQKBNR";

        for (int j = 0; j < 8; j++)
        {
            board[0][j] = layout1[j];
        }

        for (int j = 0; j < 8; j++)
        {
            board[1][j] = layout2[j];
        }

        for (int j = 0; j < 8; j++)
        {
            board[6][j] = layout3[j];
        }

        for (int j = 0; j < 8; j++)
        {
            board[7][j] = layout4[j];
        }
    }

    void move(const tuple<int, int, int, int> &input) {
        board[get<2>(input)][get<3>(input)] = board[get<0>(input)][get<1>(input)];
        board[get<0>(input)][get<1>(input)] = ' ';
    }

    void print_players()
    {
        cout << "Player 1's name is " << playerNames[0] << endl
             << "Player 2's name is " << playerNames[1] << endl;
    }
};

class Renderer
{
private:
    Chess * chess;

    void render_header_row()
    {
        cout << "    a   b   c   d   e   f   g   h" << endl;
        cout << "  ---------------------------------" << endl;
    }

    void render_data_row(const char (&row)[8], int &i)
    {
        cout << i + 1 << " | ";

        for (int j = 0; j < 8; j++)
        {
            cout << row[j];
            cout << " | ";
        }

        cout << i + 1 << endl;
    }

    void render_footer_row()
    {
        cout << "  ---------------------------------" << endl;
        cout << "    a   b   c   d   e   f   g   h" << endl;
    }

public:
    void render() {
        render_header_row();

        for (int i = 0; i < 8; i++) {
            render_data_row(chess->board[i], i);
        }

        render_footer_row();
    }

    tuple<int, int, int, int> ask_for_movement(const int recursion) {
        if (recursion > 9) {
            cout << "You have passed too many invalid movements, passing up turn" << endl;
            return make_tuple(0,0,0,0);
        }

        string piece;
        string moveTo;

        int x = 0;
        int y = 0;
        int i = 0;
        int j = 0;

        cout << "Piece to move: ";
        cin >> piece;
        if (piece == "exit") exit(0);
        if (97 <= piece[0] && piece[0] <= 104) y = piece[0] - 97;
        if (49 <= piece[1] && piece[1] <= 57) x = piece[1] - 49;

        if (chess->board[x][y] == ' ') {
            cout << "No piece at position" << endl;
            return this->ask_for_movement(recursion + 1);
        }

        cout << "Move to: ";
        cin >> moveTo;
        if (moveTo == "exit") exit(0);
        if (97 <= moveTo[0] && moveTo[0] <= 104) j = moveTo[0] - 97;
        if (49 <= moveTo[1] && moveTo[1] <= 57) i = moveTo[1] - 49;

        if (x == i && y == j) {
            cout << "Cannot move piece to current position" << endl;
            return this->ask_for_movement(recursion + 1);
        }

        return make_tuple(x, y, i, j);
    }

    Renderer(Chess &c) {
        chess = &c;
    }
};

class Engine {
private:
    Chess * chess;
    int preRow;
    int preCol;
    int postRow;
    int postCol;

    bool checkPawn() {
        if (chess->getPlayingPlayer() == Player::BOTTOM) {
            if (postRow + 1 != preRow) return false;
        } else {
            if (postRow - 1 != preRow) return false;
        }

        if (preCol == postCol && chess->board[postRow][postCol] != ' ') return false;
        if (preCol != postCol && chess->board[postRow][postCol] == ' ') return false;
        return true;
    }

    bool checkRook() {
        if (preRow != postRow && preCol != postCol) return false;
        if (preRow != postRow && preCol == postCol) { // Vertical (rows) movement
            if (preRow > postRow) {
                for (int i = preRow - 1; i > postRow; i--) { // Upward movement
                    if (chess->board[i][postCol] != ' ') return false;
                }
            } else {
                for (int i = preRow + 1; i < postRow; i++) { // Downward movement
                    if (chess->board[i][postCol] != ' ') return false;
                }
            }
        } else if (preRow == postRow && preCol != postCol) { // Horizontal (across/columns) movement
            if (preCol > postCol) {
                for (int i = preCol - 1; i > postCol; i--) { // Leftward movement
                    if (chess->board[postRow][i] != ' ') return false;
                }
            } else {
                for (int i = preCol + 1; i < postCol; i++) { // Rightward movement
                    if (chess->board[postRow][i] != ' ') return false;
                }
            }
        }

        if (chess->getPlayingPlayer() == this->getPlayerForPiece(chess->board[postRow][postCol])) return false;

        return true;
    }

public:
    Player getPlayerForPiece(const char &piece) {
        if (piece >= 65 && piece <= 90) return Player::BOTTOM;
        else if (piece >= 97 && piece <= 122) return Player::TOP;
        else return Player::NONE;
    }

    bool checkMovement(const tuple<int, int, int, int> &input) {
        preRow = get<0>(input);
        preCol = get<1>(input);
        postRow = get<2>(input);
        postCol = get<3>(input);

        if (preRow == 0 && preCol == 0 && postRow == 0 && postCol == 0) return true;

        Piece piece = Piece::NOTHING;
        char preConversion = (*chess).board[preRow][preCol];
        if (preConversion == 'p' || preConversion == 'P') piece = Piece::PAWN;
        if (preConversion == 'r' || preConversion == 'R') piece = Piece::ROOK;
        if (preConversion == 'n' || preConversion == 'N') piece = Piece::NIGHT;
        if (preConversion == 'b' || preConversion == 'B') piece = Piece::BISHOP;
        if (preConversion == 'q' || preConversion == 'Q') piece = Piece::QUEEN;
        if (preConversion == 'k' || preConversion == 'K') piece = Piece::KING;

        if (piece == Piece::PAWN && !checkPawn()) return false;
        if (piece == Piece::ROOK && !checkRook()) return false;
        return true;
    }

    Engine(Chess &c) {
        chess = &c;
    }
};

int main(int argc, char* argv[])
{
    Chess chess;
    Renderer renderer(chess);
    Engine engine(chess);

    if (argc >= 2) {
        for (int i = 1; i < argc; i++) {
            if (argv[i] == "--debug") {
                chess.setPlayingPlayer(Player::NONE);
            }
        }
    }

    chess.initialize();

    cout << endl;

    chess.print_players();

    renderer.render();

    while (true) {
        if (chess.getPlayingPlayer() == Player::NONE) cout << "No player playing [DEBUG MODE ENABLED]" << endl;
        else cout << "Current player: " << chess.getPlayingPlayerWithName() << endl;

        tuple<int, int, int, int> result = renderer.ask_for_movement(0);
        if (engine.checkMovement(result)) {
            chess.move(result);
            renderer.render();
            chess.switchPlayer();
        } else {
            cout << "Invalid movement!" << endl;
        }

        cout << endl << endl;
    }

    return 0;
}