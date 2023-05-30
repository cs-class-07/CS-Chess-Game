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
    string player_names[2];
    Player playing_player = Player::BOTTOM; // 0 is bottom player, 1 is top player

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

        player_names[0] = "player blue";
        player_names[1] = "player red";
    }

    Player get_playing_player() {
        return playing_player;
    }

    string get_playing_player_name() {
        if (playing_player == Player::NONE) return string();
        return player_names[static_cast<int>(playing_player)];
    }

    void set_playing_player(Player player) { // For debugging purposes
        playing_player = player;
    }

    void switch_player() {
        if (playing_player == Player::BOTTOM) playing_player = Player::TOP;
        else if (playing_player == Player::TOP) playing_player = Player::BOTTOM;
    }

    void initialize()
    {
        // string player1;
        // string player2;
        
        // cout << "Player 1 name: ";
        // cin >> player1;

        // cout << "Player 2 name: ";
        // cin >> player2;
        
        // player_names[0] = player1;
        // player_names[1] = player2;

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
        cout << "Player 1's name is " << player_names[0] << endl
             << "Player 2's name is " << player_names[1] << endl;
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
            cout << "Cannot move piece to current position (itself)" << endl;
            return this->ask_for_movement(recursion + 1);
        }

        if (Engine::get_player_for_piece(chess->board[x][y]) == Engine::get_player_for_piece(chess->board[i][j])) {
            cout << "Cannot move piece to other pieces of same player (side)" << endl;
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

    bool check_pawn() {
        if (this->get_player_for_piece() == Player::BOTTOM) {
            if (postRow + 1 != preRow) return false;
        } else {
            if (postRow - 1 != preRow) return false;
        }

        if (preCol == postCol && chess->board[postRow][postCol] != ' ') return false;
        if (preCol != postCol && chess->board[postRow][postCol] == ' ') return false;
        return true;
    }

    bool check_rook() {
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

        if (this->get_player_for_piece() == this->get_player_for_piece(chess->board[postRow][postCol])) return false;

        return true;
    }

public:
    static Player get_player_for_piece(const char &piece) {
        if (piece >= 65 && piece <= 90) return Player::BOTTOM;
        else if (piece >= 97 && piece <= 122) return Player::TOP;
        else return Player::NONE;
    }

    Player get_player_for_piece() {
        const char piece = chess->board[preRow][preCol];

        if (piece >= 65 && piece <= 90) return Player::BOTTOM;
        else if (piece >= 97 && piece <= 122) return Player::TOP;
        else return Player::NONE;
    }

    bool check_movement(const tuple<int, int, int, int> &input) {
        preRow = get<0>(input);
        preCol = get<1>(input);
        postRow = get<2>(input);
        postCol = get<3>(input);

        if (preRow == 0 && preCol == 0 && postRow == 0 && postCol == 0) return true; // Returned by the Renderer::ask_for_movement function when invalid input has been passed

        Piece piece = Piece::NOTHING;
        char preConversion = (*chess).board[preRow][preCol];
        if (preConversion == 'p' || preConversion == 'P') piece = Piece::PAWN;
        if (preConversion == 'r' || preConversion == 'R') piece = Piece::ROOK;
        if (preConversion == 'n' || preConversion == 'N') piece = Piece::NIGHT;
        if (preConversion == 'b' || preConversion == 'B') piece = Piece::BISHOP;
        if (preConversion == 'q' || preConversion == 'Q') piece = Piece::QUEEN;
        if (preConversion == 'k' || preConversion == 'K') piece = Piece::KING;

        if (piece == Piece::PAWN && !check_pawn()) return false;
        if (piece == Piece::ROOK && !check_rook()) return false;
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
            if (strcmp(argv[i], "--debug") == 0) {
                chess.set_playing_player(Player::NONE);
            }
        }
    }

    chess.initialize();

    cout << endl;

    chess.print_players();

    renderer.render();

    while (true) {
        if (chess.get_playing_player() == Player::NONE) cout << "No player playing [DEBUG MODE ENABLED]" << endl;
        else cout << "Current player: " << chess.get_playing_player_name() << endl;

        tuple<int, int, int, int> result = renderer.ask_for_movement(0);
        if (engine.check_movement(result)) {
            chess.move(result);
            renderer.render();
            chess.switch_player();
        } else {
            cout << "Invalid movement!" << endl;
        }

        cout << endl << endl;
    }

    return 0;
}