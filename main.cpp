#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <term.h>

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
    // string player_names[2];
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
    }

    Player get_playing_player() {
        return playing_player;
    }

    string get_playing_player_name() {
        if (playing_player == Player::NONE) return string();
        else if (playing_player == Player::BOTTOM) return "White (Bottom)";
        else if (playing_player == Player::TOP) return "Black (Top)";
        return string();
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

    // void print_players()
    // {
    //     cout << "Player 1's name is " << player_names[0] << endl
    //          << "Player 2's name is " << player_names[1] << endl;
    // }
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

    bool check_bishop() {
        if (abs(pre_row - post_row) != abs(pre_col - post_col)) return false;

        if (pre_row > post_row) { // Upward movement
            if (pre_col > post_col) { // Leftward movement

                for (int i = pre_row - 1; i > post_row; i--) {
                    for (int j = pre_col - 1; j > post_col; j--) {
                        if (chess->board[i][j] != ' ') return false;
                    }
                }

            } else { // Rightward movement

                for (int i = pre_row - 1; i > post_row; i--) {
                    for (int j = pre_col + 1; j < post_col; j++) {
                        if (chess->board[i][j] != ' ') return false;
                    }
                }

            }
        } else { // Downward movement
            if (pre_col > post_col) { // Leftward movement

                for (int i = pre_row + 1; i < post_row; i++) {
                    for (int j = pre_col - 1; j > post_col; j--) {
                        if (chess->board[i][j] != ' ') return false;
                    }
                }

            } else { // Rightward movement

                for (int i = pre_row + 1; i < post_row; i++) {
                    for (int j = pre_col + 1; j < post_col; j++) {
                        if (chess->board[i][j] != ' ') return false;
                    }
                }

            }
        }

        return true;
    }

    bool check_king() {
        if (abs(pre_row - post_row) > 1 || abs(pre_col - post_col) > 1) return false;

        return true;
    }

public:
    static Player get_player_for_piece(const char &piece) {
        if (piece >= 65 && piece <= 90) return Player::BOTTOM;
        else if (piece >= 97 && piece <= 122) return Player::TOP;
        else return Player::NONE;
    }

    Player get_player_for_piece() {
        const char piece = chess->board[pre_row][pre_col];

        // if (piece >= 65 && piece <= 90) return Player::BOTTOM;
        // else if (piece >= 97 && piece <= 122) return Player::TOP;
        // else return Player::NONE;
        return Engine::get_player_for_piece(piece);
    }

    bool check_movement(const tuple<int, int, int, int> &input) {
        pre_row = get<0>(input);
        pre_col = get<1>(input);
        post_row = get<2>(input);
        post_col = get<3>(input);

        Piece piece = Piece::NOTHING;
        char pre_conversion = chess->board[pre_row][pre_col];
        if (pre_conversion == 'p' || pre_conversion == 'P') piece = Piece::PAWN;
        if (pre_conversion == 'r' || pre_conversion == 'R') piece = Piece::ROOK;
        if (pre_conversion == 'n' || pre_conversion == 'N') piece = Piece::NIGHT;
        if (pre_conversion == 'b' || pre_conversion == 'B') piece = Piece::BISHOP;
        if (pre_conversion == 'q' || pre_conversion == 'Q') piece = Piece::QUEEN;
        if (pre_conversion == 'k' || pre_conversion == 'K') piece = Piece::KING;

        if (piece == Piece::PAWN && !check_pawn()) return false;
        if (piece == Piece::ROOK && !check_rook()) return false;
        if (piece == Piece::NIGHT && !check_night()) return false;
        if (piece == Piece::BISHOP && !check_bishop()) return false;
        if (piece == Piece::QUEEN && !check_rook() && !check_bishop()) return false;
        if (piece == Piece::KING && !check_king()) return false;

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
    }

    void render_dummy_row(const char placement) {
        string char_to_be_placed_start = "┣";
        string char_to_be_placed_end = "┫";
        string char_to_be_placed_mid = "╋";
        if (placement == 1 || placement == 't') {
            char_to_be_placed_start = "┏";
            char_to_be_placed_end = "┓";
            char_to_be_placed_mid = "┳";
        } else if (placement == 2 || placement == 'b') {
            char_to_be_placed_start = "┗";
            char_to_be_placed_end = "┛";
            char_to_be_placed_mid = "┻";
        }
        

        cout << "  "
             << char_to_be_placed_start;
        for (int i = 0; i < 7; i++) {
        cout << "━━━" << char_to_be_placed_mid;
        }
        cout << "━━━"
             << char_to_be_placed_end
             << endl;
    }

    void render_data_row(const char (&row)[8], int &i)
    {
        cout << i + 1 << " ┃ ";

        for (int j = 0; j < 8; j++)
        {
            cout << row[j];
            cout << " ┃ ";
        }

        cout << i + 1 << endl;
    }

public:
    void render() {
        render_header_row();
        render_dummy_row('t');

        for (int i = 0; i < 8; i++) {
            render_data_row(chess->board[i], i);
            if (i == 7) render_dummy_row('b');
            else render_dummy_row(0);
        }

        render_header_row();
    }

    tuple<int, int, int, int, string> ask_for_movement() {
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

        if (chess->board[x][y] == ' ') return make_tuple(0,0,0,0, "No piece at position");

        cout << "Move to: ";
        cin >> move_to;
        if (move_to == "exit") exit(0);
        if (97 <= move_to[0] && move_to[0] <= 104) j = move_to[0] - 97;
        if (49 <= move_to[1] && move_to[1] <= 57) i = move_to[1] - 49;

        if (x == i && y == j) return make_tuple(0,0,0,0, "Cannot move piece to current position (itself)");

        if (Engine::get_player_for_piece(chess->board[x][y]) == Engine::get_player_for_piece(chess->board[i][j])) return make_tuple(0,0,0,0, "Cannot move piece to other pieces of same player (side)");
        if (chess->get_playing_player() != Player::NONE && Engine::get_player_for_piece(chess->board[x][y]) != chess->get_playing_player()) return make_tuple(0,0,0,0, "Cannot move other player's piece");;

        return make_tuple(x, y, i, j, string());
    }

    // static void clr_scr() {
    //     // if (!cur_term) {
    //     //     int result;
    //     //     setupterm(NULL, STDOUT_FILENO, &result );
    //     //     if (result <= 0) return;
    //     // }

    //     (void) putp( tigetstr( "clear" ) );
    // }

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

    string some_random_variable_that_i_needed_at_last_second = string();

    while (true) {
        cout << "\033[2J\033[1;1H";
        renderer.render();
        cout << some_random_variable_that_i_needed_at_last_second << endl;
        some_random_variable_that_i_needed_at_last_second = string();

        if (chess.get_playing_player() == Player::NONE) cout << "No player playing [DEBUG MODE ENABLED]" << endl;
        else cout << "Current player: " << chess.get_playing_player_name() << endl;

        tuple<int, int, int, int, string> result = renderer.ask_for_movement();

        if (!(get<0>(result) == 0 && get<1>(result) == 0 && get<2>(result) == 0 && get<3>(result) == 0)) {
            tuple<int, int, int, int> tangible_result = make_tuple(get<0>(result), get<1>(result), get<2>(result), get<3>(result));

            if (engine.check_movement(tangible_result)) {
                chess.move(tangible_result);
                chess.switch_player();
            } else {
                some_random_variable_that_i_needed_at_last_second = "Supplied movement is invalid";
            }
        } else {
            some_random_variable_that_i_needed_at_last_second = get<4>(result);
        }
    }

    return 0;
}