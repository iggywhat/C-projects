#include <iostream>

using namespace std;

// Definicja klas (puste pole, pionki, damki)
enum class Piece { Empty, WhitePawn, BlackPawn, WhiteKing, BlackKing };
// Definicja planszy jako tablicy 8x8
Piece board[8][8];

// Zaczyna bialy, ustawiamy currentPlayer na Player::White
enum class Player { White, Black };
Player currentPlayer = Player::White;

void initializeBoard() {
    for (int row = 0; row < 8; ++row) { // kolejne wiersze
        for (int col = 0; col < 8; ++col) { // kolejne kolumny
            if (row % 2 != col % 2) {
                if (row < 3) board[row][col] = Piece::BlackPawn; // ustawienie czarnych na dwoch gornych rzedach
                else if (row > 4) board[row][col] = Piece::WhitePawn; // ustawienie bialych na dwoch dolnych rzedach
                else board[row][col] = Piece::Empty; // jesli nie ma pionka to puste pole
            } else {
                board[row][col] = Piece::Empty; // jesli nie ma pionka to puste pole
            }
        }
    }
}

void printBoard() {
    for (int row = 0; row < 8; ++row) {
        cout << 0 + row << " "; // // numeracja koordynatow dla lepszej widocznosci dla uzytkownika
        for (int col = 0; col < 8; ++col) {
            switch (board[row][col]) { // print planszy w zaleznosci od stanu danego pola
                case Piece::Empty: cout << "."; break;
                case Piece::WhitePawn: cout << "w"; break;
                case Piece::BlackPawn: cout << "b"; break;
                case Piece::WhiteKing: cout << "W"; break;
                case Piece::BlackKing: cout << "B"; break;
            }
            cout << " ";
        }
        cout << endl;
    }
    cout << "  0 1 2 3 4 5 6 7 " << endl;  // numeracja koordynatow dla lepszej widocznosci dla uzytkownika

    // Wyswietl informacje o tym czyj jest ruch na koniec printu
    if (currentPlayer == Player::Black) {
        cout << "RUCH CZARNYCH\n";
    } else {
        cout << "RUCH BIALYCH\n";
    }
}

bool isValidMove(int startX, int startY, int endX, int endY) {
    if (endX < 0 || endX >= 8 || endY < 0 || endY >= 8) return false; // czy koordynaty w zakresie planszy?
    if (board[endX][endY] != Piece::Empty) return false; // czy pole jest puste?
    int dx = endX - startX; // roznica x
    int dy = endY - startY; // roznica y
    if (abs(dx) != abs(dy)) return false;  // czy ruch jest po skosie?

    Piece piece = board[startX][startY];
    if (piece == Piece::Empty) return false;  // czy ruch jest na puste pole?

    // Czy pionek nalezy do aktywnego gracza?
    if ((piece == Piece::WhitePawn || piece == Piece::WhiteKing) && currentPlayer != Player::White) return false;
    if ((piece == Piece::BlackPawn || piece == Piece::BlackKing) && currentPlayer != Player::Black) return false;

    // RUCH
    if (piece == Piece::WhitePawn) {
        if (dx == -1 && abs(dy) == 1) return true;  // RUCH BIALYCH - przesuniecie
        if (dx == -2 && abs(dy) == 2 && board[startX - 1][startY + dy / 2] != Piece::Empty &&
            (board[startX - 1][startY + dy / 2] == Piece::BlackPawn || board[startX - 1][startY + dy / 2] == Piece::BlackKing)) return true;  // RUCH BIALYCH - zbicie czarnego (hehe)
    } else if (piece == Piece::BlackPawn) {
        if (dx == 1 && abs(dy) == 1) return true;  // RUCH CZARNYCH - przesuniecie
        if (dx == 2 && abs(dy) == 2 && board[startX + 1][startY + dy / 2] != Piece::Empty &&
            (board[startX + 1][startY + dy / 2] == Piece::WhitePawn || board[startX + 1][startY + dy / 2] == Piece::WhiteKing)) return true;  // RUCH CZARNYCH - zbicie bialego
    }

        // RUCH DAMEK (przesuniecie lub zbicie)
    else if (piece == Piece::WhiteKing || piece == Piece::BlackKing) {
        int xDir = dx > 0 ? 1 : -1;
        int yDir = dy > 0 ? 1 : -1;
        bool capturePossible = false;

        for (int i = 1; i < abs(dx); ++i) {
            if (board[startX + i * xDir][startY + i * yDir] != Piece::Empty) {

                if (capturePossible || board[startX + i * xDir][startY + i * yDir] == piece ||
                    ((piece == Piece::WhiteKing && board[startX + i * xDir][startY + i * yDir] != Piece::BlackPawn &&
                      board[startX + i * xDir][startY + i * yDir] != Piece::BlackKing) ||
                     (piece == Piece::BlackKing && board[startX + i * xDir][startY + i * yDir] != Piece::WhitePawn &&
                      board[startX + i * xDir][startY + i * yDir] != Piece::WhiteKing))) {

                    return false;
                } else {
                    capturePossible = true;
                }
            }
        }
        return true;
    }

    return false;
}

void makeMove(int startX, int startY, int endX, int endY) {
    if (isValidMove(startX, startY, endX, endY)) {
        int dx = endX - startX;
        int dy = endY - startY;
        int xDir = dx > 0 ? 1 : -1;
        int yDir = dy > 0 ? 1 : -1;

        if (abs(dx) > 1) {  // Capture move
            for (int i = 1; i < abs(dx); ++i) {
                if (board[startX + i * xDir][startY + i * yDir] != Piece::Empty) {
                    board[startX + i * xDir][startY + i * yDir] = Piece::Empty;  // Usun zbity pionek
                    break;
                }
            }
        }

        board[endX][endY] = board[startX][startY];
        board[startX][startY] = Piece::Empty;

        // zamiana na damke, jesli pionek na koncu planszy
        if (board[endX][endY] == Piece::WhitePawn && endX == 0) {
            board[endX][endY] = Piece::WhiteKing;
        } else if (board[endX][endY] == Piece::BlackPawn && endX == 7) {
            board[endX][endY] = Piece::BlackKing;
        }

        // ZBICIE WIELOKROTNE
        if (abs(dx) > 1) {
            if (isValidMove(endX, endY, endX - 2, endY - 2) || isValidMove(endX, endY, endX - 2, endY + 2) ||
                isValidMove(endX, endY, endX + 2, endY - 2) || isValidMove(endX, endY, endX + 2, endY + 2)) {
                printBoard();  // Printuj tablice przed ponownym zapytaniem
                cout << "Istnieje możliwość wielokrotnego zbicia! Wprowadz koordynaty kolejnego ruchu: ";
                cin >> startX >> startY >> endX >> endY;
                makeMove(startX, startY, endX, endY);  // kontynuuj zbijanie
                return;  // wroc, zeby nie zamienic graczy niepotrzebnie
            }
        }

        currentPlayer = (currentPlayer == Player::White) ? Player::Black : Player::White;

        // ZAKONCZENIE GRY:
        // sprawdz czy zostaly pionki biale lub czarne
        bool whitePiecesRemaining = false;
        bool blackPiecesRemaining = false;

        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                if (board[row][col] == Piece::WhitePawn || board[row][col] == Piece::WhiteKing) {
                    whitePiecesRemaining = true;
                } else if (board[row][col] == Piece::BlackPawn || board[row][col] == Piece::BlackKing) {
                    blackPiecesRemaining = true;
                }
            }
        }

        // Jesli nie ma pionkow jednego gracza to drugi wygrywa - KONIEC
        if (!whitePiecesRemaining) {
            cout << "GRACZ CZARNY WYGRYWA!" << endl;
            exit(0);
        } else if (!blackPiecesRemaining) {
            cout << "GRACZ BIALY WYGRWYWA!" << endl;
            exit(0);
        }

    } else {
        cout << "Nieprawidlowy ruch. Sproboj ponownie." << endl; // Nieprawidlowy ruch, tura tego samego gracza od poczatku
    }
}


void gameLoop() {
    while (true) {
        printBoard();
        int startX, startY, endX, endY;
        cout << "Wprowadz koordynaty twojego ruchu, oddzielone spacja. (format: startY startX endY endX): \n";
        cin >> startX >> startY >> endX >> endY;
        cout << "---------------------------------------------------------------------------------------------------------------\n";
        makeMove(startX, startY, endX, endY);
    }
}

void howToPlay() {
    cout << "---------------------------------------------------------------------------------------------------------------\n";
    cout << "Witaj w grze w warcaby!\n";
    cout << endl;
    cout << "Instrukcja:\n";
    cout << "Aby wykonac ruch, wprowadz koordynaty Y i X (od 0 do 7) pionka, ktorym chcesz wykonac ruch.\n";
    cout << "Nastepnie, wprowadz koordynaty Y i X (od 0 do 7) pola na planszy, na ktore chcesz przesunac swoj pionek.\n";
    cout << endl;
    cout << "Po wykonaniu ruchu, zamien sie z drugim graczem, aby ten mogl wykonac swoj ruch w ten sam sposob.\n";
    cout << "Powodzenia!\n";
    cout << "---------------------------------------------------------------------------------------------------------------\n";
    cout << endl;
    cout << "Wcisnij dowolny klawisz aby rozpoczac gre: \n";
    cout << endl;
}

int main() {
    howToPlay();
    char start;
    cin >> start;
    while (start) {
        initializeBoard();
        gameLoop();
    }
    return 0;
}