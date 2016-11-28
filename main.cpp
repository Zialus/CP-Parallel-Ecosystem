#include <iostream>
#include <vector>

using namespace std;

char** posMatrix;
int** ageMatrix;
int** hungryMatrix;

int GEN_PROC_RABBITS ; // number of generations until a rabbit can procreate
int GEN_PROC_FOXES ; // number of generations until a fox can procreate
int GEN_FOOD_FOXES ; // number of generations for a fox to die of starvation
int N_GEN ; // number of generations for the simulation
int R ; // number of rows of the matrix representing the ecosystem
int C ; // number of columns of the matrix representing the ecosystem
int N ; // number of objects in the initial ecosystem

struct Rabbit {

    Rabbit(int x, int y) {
        procAge = 0;
        pos_x = x;
        pos_y = y;
    }

    int procAge;
    int pos_x;
    int pos_y;
};

struct Fox {

    Fox(int x, int y) {
        hungryAge = 0;
        procAge = 0;
        pos_x = x;
        pos_y = y;
    }

    int hungryAge;
    int procAge;
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

void simGen(){


}


vector<char> checkAdjacencies(int X, int Y){
    vector<char> validPositions;
    char n = posMatrix[X-1][Y]; // NORTH
    if (n == ' ') {
        validPositions.push_back(n);
    };
    char w = posMatrix[X][Y-1]; // WEST
    if (w == ' ') {
        validPositions.push_back(n);
    };
    char s = posMatrix[X+1][Y]; // SOUTH
    if (s == ' ') {
        validPositions.push_back(n);
    };
    char e = posMatrix[X][Y+1]; // EAST
    if (e == ' ') {
        validPositions.push_back(n);
    };
    return validPositions;
};

pair<int,int> chooseMovePosition(int currentGen, int xPos, int yPos, vector<char> posVal){
    int pos = (int) ((currentGen + xPos + yPos) % posVal.size());

    if(posVal[pos] == 'N'){
        pair<int, int> posPair = make_pair(xPos-1,yPos);
        return posPair;
    }

    else if(posVal[pos] == 'E'){
        pair<int, int> posPair = make_pair (xPos,yPos+1);
        return posPair;
    }

    else if(posVal[pos] == 'S'){
        pair<int, int> posPair = make_pair (xPos+1,yPos);
        return posPair;
    }

    else if(posVal[pos] == 'W'){
        pair<int, int> posPair = make_pair (xPos,yPos-1);
        return posPair;
    }

    return NULL;
};

void analyzeRabbits(vector<Rabbit> rabbitList, int currentGen){
    while(!rabbitList.empty()){
        Rabbit r = rabbitList.front();
        rabbitList.erase(rabbitList.begin());
        int x = r.pos_x;
        int y = r.pos_y;
        vector<char> validPositions = checkAdjacencies(x,y);
        chooseMovePosition(currentGen,x,y,validPositions);
    }
}

int main(int argc, char* argv[]) {

    vector<Rabbit> RabbitsList;
    vector<Fox> FoxesList;

    if (argc == 2){
        freopen(argv[1], "r", stdin);
    }

    cin >> GEN_PROC_RABBITS >> GEN_PROC_FOXES >> GEN_FOOD_FOXES >> N_GEN >> R >> C >> N;

    posMatrix = new char*[R];

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
            Rabbit r = Rabbit(X, Y);
            RabbitsList.push_back(r);
            posMatrix[X][Y] = 'R';
            ageMatrix[X][Y] = r.procAge;
        }
        else if (TYPE == "FOX"){
            Fox f = Fox(X, Y);
            FoxesList.push_back(f);
            posMatrix[X][Y] = 'F';
            ageMatrix[X][Y] = f.procAge;
            hungryMatrix[X][Y] = f.hungryAge;
        }
        else if (TYPE == "ROCK"){
            posMatrix[X][Y] = '*';
        }
        else {
            cout << "YOU FUCKED UP" << endl;
        }
    }

    for(int i=0; i<N_GEN; i++){
        simGen();
    }

    printMatrix(posMatrix, R, C);

    printFinalResults(posMatrix, R, C, GEN_PROC_RABBITS, GEN_PROC_FOXES, GEN_FOOD_FOXES);

    return 0;
}
