#include <iostream>
#include <vector>

using namespace std;

struct Rabbit {

    Rabbit(int a, int x, int y) {
        age = a;
        pos_x = x;
        pos_y = y;
    }

    int age;
    int pos_x;
    int pos_y;
};

struct Fox {

    Fox(int a, int x, int y) {
        age = a;
        pos_x = x;
        pos_y = y;
    }

    int age;
    int pos_x;
    int pos_y;
};


void printFinalResults(char** matrix, int R, int C, int GEN_PROC_RABBITS, int GEN_PROC_FOXES, int GEN_FOOD_FOXES){

    int counter = 0;

    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++) {
            if (matrix[i][j] == 'R') {
                counter++;
            } else if (matrix[i][j] == 'F') {
                counter++;
            } else if (matrix[i][j] == '*') {
                counter++;
            }
        }
    }

    printf("%d %d %d %d %d %d %d\n", GEN_PROC_RABBITS, GEN_PROC_FOXES, GEN_FOOD_FOXES, 0, R, C, counter);

    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++){
            if (matrix[i][j] == 'R') {
                printf("RABBIT %d %d\n", i, j);
            } else if (matrix[i][j] == 'F') {
                printf("FOX %d %d\n", i, j);
            } else if (matrix[i][j] == '*') {
                printf("ROCK %d %d\n", i, j);
            }
        }
    }

}

void printMatrix(char** matrix, int R, int C) {

    for (int i = 0; i < R+2; ++i) {
        printf("-");
    }
    printf("\n");

    for (int i = 0; i < R; ++i) {
        printf("|");
        for (int j = 0; j < C; ++j) {
            printf("%c", matrix[i][j]);
        }
        printf("|\n");
    }

    for (int i = 0; i < R+2; ++i) {
        printf("-");
    }
    printf("\n");

}

int main(int argc, char* argv[]) {

    vector<Rabbit> RabbitsList;
    vector<Fox> FoxesList;

    int GEN_PROC_RABBITS ; // number of generations until a rabbit can procreate
    int GEN_PROC_FOXES ; // number of generations until a fox can procreate
    int GEN_FOOD_FOXES ; // number of generations for a fox to die of starvation
    int N_GEN ; // number of generations for the simulation
    int R ; // number of rows of the matrix representing the ecosystem
    int C ; // number of columns of the matrix representing the ecosystem
    int N ; // number of objects in the initial ecosystem

    if (argc == 2){
        freopen(argv[1], "r", stdin);
    }

    cin >> GEN_PROC_RABBITS >> GEN_PROC_FOXES >> GEN_FOOD_FOXES >> N_GEN >> R >> C >> N;

    char** posMatrix = new char*[R];

    for (int i = 0; i < R; ++i) {
        posMatrix[i] = new char[C];
    }


    for (int i = 0; i < R; ++i) {
        for (int j = 0; j < C; ++j) {
            posMatrix[i][j] = ' ';
        }
    }

    for (int i = 0; i < N; ++i) {
        string TYPE;
        int X;
        int Y;

        cin >> TYPE >> X >> Y;

        if (TYPE == "RABBIT"){
            Rabbit r = Rabbit(0, X, Y);
            RabbitsList.push_back(r);
            posMatrix[X][Y] = 'R';
        }
        else if (TYPE == "FOX"){
            Fox f = Fox(0, X, Y);
            FoxesList.push_back(f);
            posMatrix[X][Y] = 'F';
        }
        else if (TYPE == "ROCK"){
            posMatrix[X][Y] = '*';
        }
        else {
            cout << "YOU FUCKED UP" << endl;
        }
    }

    printMatrix(posMatrix, R, C);

    printFinalResults(posMatrix, R, C, GEN_PROC_RABBITS, GEN_PROC_FOXES, GEN_FOOD_FOXES);

    return 0;
}
