#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct Chess
{
    char board[8][8];
    string player_names[2];

    Chess()
    {
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                board[i][j] = ' ';
            }
        }

        player_names[0] = "no name";
        player_names[1] = "no name";
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

    void move(const int x, const int y, const int i, const int j) {
        board[i][j] = board[x][y];
        board[x][y] = ' ';
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

    void ask_for_movement() {
        string piece;
        string moveTo;
        int x;
        int y;
        int i;
        int j;

        cout << "Piece to move: ";
        cin >> piece;
        cout << "Move to: ";
        cin >> moveTo;

        if (97 <= piece[0] <= 104) x = piece[0] - 97;
        if (49 <= piece[1] <= 57) y = piece[1] - 49;

        if (97 <= moveTo[0] <= 104) i = piece[0] - 97;
        if (49 <= moveTo[1] <= 57) j = piece[1] - 49;

        chess->move(x,y,i,j);
    }

    
    Renderer(Chess c) {
        chess = &c;
    }
};

int main()
{
    Chess C;
    Renderer renderer(C);

    C.initialize();

    cout << endl;

    C.print_players();

    renderer.render();

    while (true) {
        renderer.ask_for_movement();
        renderer.render();
    }

    return 0;
}