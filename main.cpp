#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct chess {
    char board[8][8];
    string player_names[2];

    chess() {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                board[i][j] = '0';
            }
        }

        player_names[0] = "no name";
        player_names[1] = "no name";
    }

    void initialize() {
        char layout1[] = "rnbqkbnr";
        char layout2[] = "pppppppp";
        
        char layout3[] = "PPPPPPPP";
        char layout4[] = "RNBQKBNR";

        for (int j = 0; j < 8; j++) {
            board[0][j] = layout1[j];
        }
        
        for (int j = 0; j < 8; j++) {
            board[1][j] = layout2[j];
        }
        
        for (int j = 0; j < 8; j++) {
            board[6][j] = layout3[j];
        }
        
        for (int j = 0; j < 8; j++) {
            board[7][j] = layout4[j];
        }
    }

    void ls() {
        cout << "    a   b   c   d   e   f   g   h";
        cout << "  --------------------------------";
        cout << "8 |   |   |   |   |   |   |   |  | 8";
        
    }
    
    void print_players() {
        cout << "Player 1's name is " << player_names[0] << endl << "Player 2's name is " << player_names[1] << endl;
    }
};

int main() {
    chess C;
    
    C.initialize();
    
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            cout << C.board[i][j];
        }
        cout << endl;
    }
    
    cout << endl;
    
    C.print_players();
    
    return 0;
}