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

class Engine {
private:
    Chess * chess;
    int pre_row;
    int pre_col;
    int post_row;
    int post_col;

    bool check_pawn() {
        if (get_player_for_piece() == Player::BOTTOM) {
            if (post_row + 1 != pre_row) return false;
        } else {
            if (post_row - 1 != pre_row) return false;
        }

        if (pre_col == post_col && chess->board[post_row][post_col] != ' ') return false;
        if (pre_col != post_col && chess->board[post_row][post_col] == ' ') return false;
        return true;
    }

    bool check_rook() {
        if (pre_row != post_row && pre_col != post_col) return false;
        if (pre_row != post_row && pre_col == post_col) { // Vertical (rows) movement
            if (pre_row > post_row) {
                for (int i = pre_row - 1; i > post_row; i--) { // Upward movement
                    if (chess->board[i][post_col] != ' ') return false;
                }
            } else {
                for (int i = pre_row + 1; i < post_row; i++) { // Downward movement
                    if (chess->board[i][post_col] != ' ') return false;
                }
            }
        } else if (pre_row == post_row && pre_col != post_col) { // Horizontal (across/columns) movement
            if (pre_col > post_col) {
                for (int i = pre_col - 1; i > post_col; i--) { // Leftward movement
                    if (chess->board[post_row][i] != ' ') return false;
                }
            } else {
                for (int i = pre_col + 1; i < post_col; i++) { // Rightward movement
                    if (chess->board[post_row][i] != ' ') return false;
                }
            }
        }

        return true;
    }

    bool check_night() {
        if (abs(pre_row - post_row) == 2) { // Vertical movements of 2 and horizontal movements of 1
            if (abs(pre_col - post_col) == 1) return true;
        } else if (abs(pre_col - post_col) == 2) { // Horizontal movements of 2 and vertical movements of 1
            if (abs(pre_row - post_row) == 1) return true;
        }

        return false;
    }

public:
    static Player get_player_for_piece(const char &piece) {
        if (piece >= 65 && piece <= 90) return Player::BOTTOM;
        else if (piece >= 97 && piece <= 122) return Player::TOP;
        else return Player::NONE;
    }

    Player get_player_for_piece() {
        const char piece = chess->board[pre_row][pre_col];

        if (piece >= 65 && piece <= 90) return Player::BOTTOM;
        else if (piece >= 97 && piece <= 122) return Player::TOP;
        else return Player::NONE;
    }

    bool check_movement(const tuple<int, int, int, int> &input) {
        pre_row = get<0>(input);
        pre_col = get<1>(input);
        post_row = get<2>(input);
        post_col = get<3>(input);

        if (pre_row == 0 && pre_col == 0 && post_row == 0 && post_col == 0) return true; // Returned by the Renderer::ask_for_movement function when invalid input has been passed

        Piece piece = Piece::NOTHING;
        char pre_conversion = (*chess).board[pre_row][pre_col];
        if (pre_conversion == 'p' || pre_conversion == 'P') piece = Piece::PAWN;
        if (pre_conversion == 'r' || pre_conversion == 'R') piece = Piece::ROOK;
        if (pre_conversion == 'n' || pre_conversion == 'N') piece = Piece::NIGHT;
        if (pre_conversion == 'b' || pre_conversion == 'B') piece = Piece::BISHOP;
        if (pre_conversion == 'q' || pre_conversion == 'Q') piece = Piece::QUEEN;
        if (pre_conversion == 'k' || pre_conversion == 'K') piece = Piece::KING;

        if (piece == Piece::PAWN && !check_pawn()) return false;
        if (piece == Piece::ROOK && !check_rook()) return false;
        if (piece == Piece::NIGHT && !check_night()) return false;
        return true;
    }

    Engine(Chess &c) {
        chess = &c;
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
        string move_to;

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
        cin >> move_to;
        if (move_to == "exit") exit(0);
        if (97 <= move_to[0] && move_to[0] <= 104) j = move_to[0] - 97;
        if (49 <= move_to[1] && move_to[1] <= 57) i = move_to[1] - 49;

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