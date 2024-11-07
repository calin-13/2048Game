#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream>

std::ofstream g("Text.txt");

// Dimensiunea matricei pentru jocul 2048
const int BOARD_SIZE = 4;

// Numărul maxim de generații
const int MAX_GENERATIONS = 100;

// Numărul de cromozomi (solutii) într-o populație
const int POPULATION_SIZE = 40;

// Șansa de mutație a unui cromozom
const double MUTATION_RATE = 0.4;

//// Procentul de supraviețuire
const int SURVIVAL_RATE_PERCENTAGE = 30; // Exemplu: 20% supraviețuire
const int SURVIVAL_RATE = (SURVIVAL_RATE_PERCENTAGE * POPULATION_SIZE) / 100;

// Reprezentarea unui cromozom
struct Chromosome {
    std::vector<int> moves;

    Chromosome(std::mt19937& rng) : moves(500, -1) {
        // Generare aleatoare de mutații pentru început
        std::uniform_int_distribution<int> dist(0, 3);
        for (int i = 0; i < 500; ++i) {
            moves[i] = dist(rng);
        }
    }
    // Evalueaza calitatea unui cromozom (functie de fitness)
    int scorFitness;
    int numMoves;
};

// Reprezentarea matricei de joc
struct Board {
    int data[BOARD_SIZE][BOARD_SIZE];

    Board() {
        // Initializare matrice cu 0
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                data[i][j] = 0;
            }
        }

        // Adaugare doua numere 2 sau 4 la inceput
        addRandomTile();
        addRandomTile();
    }

    // Adauga o valoare aleatoare la o celula goala
    void addRandomTile() {
        std::vector<std::pair<int, int>> emptyCells;

        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (data[i][j] == 0) {
                    emptyCells.push_back(std::make_pair(i, j));
                }
            }
        }

        if (emptyCells.empty()) {
            return; // Nu exista celule libere
        }

        // Alege o celula aleatoare
        std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int> dist(0, emptyCells.size() - 1);
        int index = dist(rng);
        int value = (dist(rng) % 2 + 1) * 2; // 2 sau 4

        data[emptyCells[index].first][emptyCells[index].second] = value;
    }

    void copyFrom(const Board& other) {
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                data[i][j] = other.data[i][j];
            }
        }
    }

    bool hasChanged(const Board& other) const {
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (data[i][j] != other.data[i][j]) {
                    return true;
                }
            }
        }
        return false;
    }

    void printBoard() const {
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                g << data[i][j] << "\t";
            }
            g << std::endl;
        }

    }

    // Efectueaza o mutatie pe baza unei miscari date de cromozom
    void applyMove(int move);

    // Verifica daca jocul este in stare de castig sau pierdere
    bool isGameOver();

};

// Functie pentru a muta elementele pe coloane in sus
void moveUp(Board& board) {
    // Parcurge fiecare coloană
    for (int j = 0; j < BOARD_SIZE; ++j) {
        // Variabile pentru a urmări indicele liniei (li) și coloanei (ri)
        int li = 0, ri = j;

        // Parcurge fiecare linie începând de la a doua (1-indexat)
        for (int i = 1; i < BOARD_SIZE; ++i) {
            // Verifică dacă celula curentă nu este goală
            if (board.data[i][j] != 0) {
                // Verifică dacă celula de deasupra este goală sau are aceeași valoare
                if (board.data[i - 1][j] == 0 || board.data[i - 1][j] == board.data[i][j]) {
                    // Verifică dacă celula de sus are aceeași valoare
                    if (board.data[li][ri] == board.data[i][j]) {
                        // Dublează valoarea și setează celula curentă la 0
                        board.data[li][ri] *= 2;
                        board.data[i][j] = 0;
                    }
                    else {
                        // Verifică dacă celula de deasupra este goală
                        if (board.data[li][ri] == 0) {
                            // Mută valoarea în celula goală de deasupra și setează celula curentă la 0
                            board.data[li][ri] = board.data[i][j];
                            board.data[i][j] = 0;
                        }
                        else {
                            // Mută valoarea în prima celulă goală de deasupra și setează celula curentă la 0
                            board.data[++li][ri] = board.data[i][j];
                            board.data[i][j] = 0;
                        }
                    }
                }
                else {
                    // Incrementarea indicelui liniei dacă nu se îndeplinesc condițiile de deplasare în sus
                    li++;
                }
            }
        }
    }
}


// Functie pentru a muta elementele pe coloane in jos
void moveDown(Board& board) {
    // Parcurge fiecare coloană
    for (int j = 0; j < BOARD_SIZE; ++j) {
        // Variabile pentru a urmări indicele liniei (li) și coloanei (ri)
        int li = BOARD_SIZE - 1, ri = j;

        // Parcurge fiecare linie începând de la penultima până la prima (de jos în sus)
        for (int i = BOARD_SIZE - 2; i >= 0; --i) {
            // Verifică dacă celula curentă nu este goală
            if (board.data[i][j] != 0) {
                // Verifică dacă celula de dedesubt este goală sau are aceeași valoare
                if (board.data[i + 1][j] == 0 || board.data[i + 1][j] == board.data[i][j]) {
                    // Verifică dacă celula de dedesubt are aceeași valoare
                    if (board.data[li][ri] == board.data[i][j]) {
                        // Dublează valoarea și setează celula curentă la 0
                        board.data[li][ri] *= 2;
                        board.data[i][j] = 0;
                    }
                    else {
                        // Verifică dacă celula de dedesubt este goală
                        if (board.data[li][ri] == 0) {
                            // Mută valoarea în celula goală de dedesubt și setează celula curentă la 0
                            board.data[li][ri] = board.data[i][j];
                            board.data[i][j] = 0;
                        }
                        else {
                            // Mută valoarea în prima celulă goală de dedesubt și setează celula curentă la 0
                            board.data[--li][ri] = board.data[i][j];
                            board.data[i][j] = 0;
                        }
                    }
                }
                else {
                    // Decrementarea indicelui liniei dacă nu se îndeplinesc condițiile de deplasare în jos
                    li--;
                }
            }
        }
    }
}


// Functie pentru a muta elementele pe linii la stanga
void moveLeft(Board& board) {
    // Parcurge fiecare linie
    for (int i = 0; i < BOARD_SIZE; ++i) {
        // Variabile pentru a urmări indicele liniei (li) și coloanei (ri)
        int li = i, ri = 0;

        // Parcurge fiecare coloană începând de la a doua până la ultima (de la stânga la dreapta)
        for (int j = 1; j < BOARD_SIZE; ++j) {
            // Verifică dacă celula curentă nu este goală
            if (board.data[i][j] != 0) {
                // Verifică dacă celula din stanga este goală sau are aceeași valoare
                if (board.data[i][j - 1] == 0 || board.data[i][j - 1] == board.data[i][j]) {
                    // Verifică dacă celula din stanga are aceeași valoare
                    if (board.data[li][ri] == board.data[i][j]) {
                        // Dublează valoarea și setează celula curentă la 0
                        board.data[li][ri] *= 2;
                        board.data[i][j] = 0;
                    }
                    else {
                        // Verifică dacă celula din stanga este goală
                        if (board.data[li][ri] == 0) {
                            // Mută valoarea în celula goală din stanga și setează celula curentă la 0
                            board.data[li][ri] = board.data[i][j];
                            board.data[i][j] = 0;
                        }
                        else {
                            // Mută valoarea în prima celulă goală din stanga și setează celula curentă la 0
                            board.data[li][++ri] = board.data[i][j];
                            board.data[i][j] = 0;
                        }
                    }
                }
                else {
                    // Incrementarea indicelui coloanei dacă nu se îndeplinesc condițiile de deplasare la stânga
                    ri++;
                }
            }
        }
    }
}


// Functie pentru a muta elementele pe linii la dreapta
void moveRight(Board& board) {
    // Parcurge fiecare linie
    for (int i = 0; i < BOARD_SIZE; ++i) {
        // Variabile pentru a urmări indicele liniei (li) și coloanei (ri)
        int li = i, ri = BOARD_SIZE - 1;

        // Parcurge fiecare coloană începând de la a doua până la prima (de dreapta la stânga)
        for (int j = BOARD_SIZE - 2; j >= 0; --j) {
            // Verifică dacă celula curentă nu este goală
            if (board.data[i][j] != 0) {
                // Verifică dacă celula din dreapta este goală sau are aceeași valoare
                if (board.data[i][j + 1] == 0 || board.data[i][j + 1] == board.data[i][j]) {
                    // Verifică dacă celula din dreapta are aceeași valoare
                    if (board.data[li][ri] == board.data[i][j]) {
                        // Dublează valoarea și setează celula curentă la 0
                        board.data[li][ri] *= 2;
                        board.data[i][j] = 0;
                    }
                    else {
                        // Verifică dacă celula din dreapta este goală
                        if (board.data[li][ri] == 0) {
                            // Mută valoarea în celula goală din dreapta și setează celula curentă la 0
                            board.data[li][ri] = board.data[i][j];
                            board.data[i][j] = 0;
                        }
                        else {
                            // Mută valoarea în prima celulă goală din dreapta și setează celula curentă la 0
                            board.data[li][--ri] = board.data[i][j];
                            board.data[i][j] = 0;
                        }
                    }
                }
                else {
                    // Decrementarea indicelui coloanei dacă nu se îndeplinesc condițiile de deplasare la dreapta
                    ri--;
                }
            }
        }
    }
}



// Verifica daca jocul este in stare de castig sau pierdere
bool Board::isGameOver() {
    // Verifica daca exista celule vecine cu aceeasi valoare
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            // Verifica celulele vecine de sus, jos, la stanga si la dreapta
            if ((i > 0 && data[i][j] == data[i - 1][j]) ||
                (i < BOARD_SIZE - 1 && data[i][j] == data[i + 1][j]) ||
                (j > 0 && data[i][j] == data[i][j - 1]) ||
                (j < BOARD_SIZE - 1 && data[i][j] == data[i][j + 1])) {
                return false; // Exista celule vecine cu aceeasi valoare
            }
        }
    }

    // Verifica daca exista celule goale
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (data[i][j] == 0) {
                return false; // Exista celule goale
            }
        }
    }

    // Daca nu exista nicio celula vecina cu aceeasi valoare si nu exista celule goale, jocul este incheiat
    return true;
}

// Efectueaza o mutatie pe baza unei miscari date de cromozom
void Board::applyMove(int move) {
    switch (move) {
    case 0: // Up
        moveUp(*this);
        break;
    case 1: // Right
        moveRight(*this);
        break;
    case 2: // Down
        moveDown(*this);
        break;
    case 3: // Left
        moveLeft(*this);
        break;
    default:
        // Handle invalid move
        break;
    }
}

int evaluateFitness(const Board& board) {
    int totalSum = 0;
    int emptyCells = 0;
    int maxTile = 0;
    int smoothness = 0;
    int monotonicity = 0;

    // Calculează suma valorilor din matrice și numărul de celule goale
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            totalSum += board.data[i][j];
            if (board.data[i][j] == 0) {
                ++emptyCells;
            }
            // Actualizează valoarea maximă de pe placă
            if (board.data[i][j] > maxTile) {
                maxTile = board.data[i][j];
            }

            // Calculează smoothness
            if (j < BOARD_SIZE - 1) {
                smoothness -= std::abs(board.data[i][j] - board.data[i][j + 1]);
            }
            if (i < BOARD_SIZE - 1) {
                smoothness -= std::abs(board.data[i][j] - board.data[i + 1][j]);
            }

            // Calculează monotonicity
            if (j < BOARD_SIZE - 1 && board.data[i][j] >= board.data[i][j + 1]) {
                monotonicity += board.data[i][j + 1] - board.data[i][j];
            }
            if (i < BOARD_SIZE - 1 && board.data[i][j] >= board.data[i + 1][j]) {
                monotonicity += board.data[i + 1][j] - board.data[i][j];
            }
        }
    }

    // Calculul scorului final
    int fitness = totalSum + emptyCells * 10 + maxTile * 100 + smoothness * 5 + monotonicity * 5;

    // Poți ajusta această logică în funcție de cerințele specifice ale jocului tău.

    return fitness;
}


// Functie pentru a realiza crossover intre doi parinti
void crossover(const Chromosome& parent1, const Chromosome& parent2, Chromosome& child) {
    // Se alege un punct de crossover aleator între 1 și size - 1

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(1, std::min(parent1.numMoves, parent2.numMoves));
    int crossoverPoint = dist(rng);

    // Copiază primele părți din parinte1
    std::copy(parent1.moves.begin(), parent1.moves.begin() + crossoverPoint, child.moves.begin());

    // Copiază ultimele părți din parinte2
    std::copy(parent2.moves.begin() + crossoverPoint, parent2.moves.end(), child.moves.begin() + crossoverPoint);

}

// Functie pentru a realiza mutatii asupra unui cromozom
void mutate(Chromosome& chromosome) {
    // Implementeaza logica de mutatie a cromozomului
    // (de exemplu, pentru fiecare gena, cu o probabilitate, schimba miscarea cu alta)
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    for (int i = 0; i < 500; ++i) {
        if (dist(rng) < MUTATION_RATE) {
            chromosome.moves[i] = dist(rng) * 4; // 4 este numarul de miscari posibile
        }
    }
}

int main() {
    // Initializare populație de cromozomi
    std::mt19937 rng(std::random_device{}());
    std::vector<Chromosome> population(POPULATION_SIZE, Chromosome(rng));

    int highestFitness = 0;

    for (int generation = 0; generation < MAX_GENERATIONS; ++generation) {
        for (int i = 0; i < POPULATION_SIZE; ++i) {
            // Creeaza o stare initiala a jocului
            Board board;
            board.printBoard();
            population[i].numMoves = 0;
            // Aplica miscarile specificate de cromozom
            for (int move : population[i].moves) {
                Board boardBeforeMove;
                boardBeforeMove.copyFrom(board);
                switch (move)
                {
                case 0: {g << "up\n"; break; }
                case 1: {g << "right\n"; break; }
                case 2: {g << "down\n"; break; }
                case 3: {g << "left\n"; break; }
                }
                board.applyMove(move);
                if (boardBeforeMove.hasChanged(board) == true)
                    board.addRandomTile();
                g << "\n\n";
                board.printBoard();
                population[i].numMoves++;
                if (board.isGameOver()) {
                    // Evalueaza calitatea solutiei
                    int fitness = evaluateFitness(board);
                    g << "Generatia: " << generation + 1 << " Cromozom: " << i + 1
                        << " Fitness: " << fitness << '\n';
                    population[i].scorFitness = fitness;
                    if (fitness > highestFitness)
                        highestFitness = fitness;
                    break;
                }
            }
        }
        g << "-------------------------\n";
        // Selecteaza cei mai buni cromozomi pentru a servi ca parinti
        std::sort(population.begin(), population.end(), [](const Chromosome& a, const Chromosome& b) {
            Board board;  // You need to create a Board object or use an existing one
            return a.scorFitness+a.numMoves*10 > b.scorFitness+b.numMoves*10;
            });

        // După sortarea populației
        std::vector<Chromosome> nextGeneration(POPULATION_SIZE, Chromosome(rng)); // Inițializare cu obiecte Chromosome

        // Copiază primii SURVIVAL_RATE cromozomi direct în noua populație
        std::copy(population.begin(), population.begin() + SURVIVAL_RATE, nextGeneration.begin());

        // Realizează crossover și mutații pentru a completa restul populației
        for (int i = SURVIVAL_RATE; i < POPULATION_SIZE; ++i) {

            crossover(population[i % SURVIVAL_RATE], population[(i + 1) % SURVIVAL_RATE], nextGeneration[i]);
            mutate(nextGeneration[i]);
        }

        // Actualizează populația curentă cu cea generată în această iterație
        population = nextGeneration;

    }
    std::cout << highestFitness;
    return 0;
}

