#include <iostream>
#include <utility>
#include <vector>

using namespace std;

char** posMatrix;
char** posMatrixTemp;
//int** ageMatrix;
//int** hungryMatrix;

int GEN_PROC_RABBITS ; // number of generations until a rabbit can procreate
int GEN_PROC_FOXES ; // number of generations until a fox can procreate
int GEN_FOOD_FOXES ; // number of generations for a fox to die of starvation
int N_GEN ; // number of generations for the simulation
int R ; // number of rows of the matrix representing the ecosystem
int C ; // number of columns of the matrix representing the ecosystem
int N ; // number of objects in the initial ecosystem

typedef enum { RABBIT, FOX, ROCK} ElementType;

struct MatrixElement{

    ElementType element_type;

    union {
        struct Rabbit;
        struct Fox;
        struct Rock;
    } e;

    int ID;
};

struct Rock {

    Rock(int x,int y){
        pos_x = x;
        pos_y = y;
    }

    int pos_x;
    int pos_y;
};

struct Rabbit {

    Rabbit(int a, int x, int y) {
        procAge = a;
        pos_x = x;
        pos_y = y;
    }

    int procAge;
    int pos_x;
    int pos_y;
};

struct Fox {

    Fox(int a, int x, int y) {
        hungryAge = 0;
        procAge = a;
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
    if(X-1 > 0 && posMatrix[X-1][Y] == ' '){ //NORTH
        validPositions.push_back('N');
    }

    if(Y+1 < C && posMatrix[X][Y+1] == ' '){ //EAST
        validPositions.push_back('E');
    }

    if(X+1 < R && posMatrix[X+1][Y] == ' '){ // SOUTH
        validPositions.push_back('S');
    }

    if(Y-1 > 0 && posMatrix[X][Y-1] == ' ') { //WEST
        validPositions.push_back('W');
    }


    return validPositions;
}

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

    perror("Function was used improperly");
    exit(1);

};

void analyzeRabbits(vector<Rabbit> rabbitList, int currentGen){
    vector<Rabbit> rabbitsListTemp;
    while(!rabbitList.empty()){
        Rabbit r = rabbitList.front();
        rabbitList.erase(rabbitList.begin());
        int x = r.pos_x;
        int y = r.pos_y;
        vector<char> validPositions = checkAdjacencies(x,y);
        if(validPositions.size() > 0){
            pair<int,int> posToMove = chooseMovePosition(currentGen,x,y,validPositions);
            Rabbit rabbitTemp = Rabbit(r.procAge+1, get<0>(posToMove), get<1>(posToMove));
            rabbitsListTemp.push_back(rabbitTemp);

            //to do: add to posmatrixTemp
        }
        else{
            Rabbit rabbitTemp = Rabbit(r.procAge+1, x, y);
            rabbitsListTemp.push_back(rabbitTemp);
        }
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
    posMatrixTemp = new char*[R];

    for (int i = 0; i < R; ++i) {
        posMatrix[i] = new char[C];
        posMatrixTemp[i] = new char[C];
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
            Fox f = Fox(0,X,Y);
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

    for(int i=0; i<N_GEN; i++){
        simGen();
    }

    printMatrix(posMatrix, R, C);

    printFinalResults(posMatrix, R, C, GEN_PROC_RABBITS, GEN_PROC_FOXES, GEN_FOOD_FOXES);

    return 0;
}
