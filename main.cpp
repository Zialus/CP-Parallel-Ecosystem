#include <iostream>
#include <vector>
#include <string.h>
#include <sys/timeb.h>
#include <omp.h>
#include "main.hpp"

MatrixElement** posMatrix;
MatrixElement* posMatrixAux;

MatrixElement** posMatrixTemp;
MatrixElement* posMatrixTempAux;

omp_lock_t** lockMatrix;
omp_lock_t* lockMatrixAux;

int GEN_PROC_RABBITS ; // number of generations until a rabbit can procreate
int GEN_PROC_FOXES ; // number of generations until a fox can procreate
int GEN_FOOD_FOXES ; // number of generations for a fox to die of starvation
int N_GEN ; // number of generations for the simulation
int R ; // number of rows of the matrix representing the ecosystem
int C ; // number of columns of the matrix representing the ecosystem
int N ; // number of objects in the initial ecosystem

bool PRINT_FINAL_INFO;
bool PRINT_TIME;
bool PRINT_ALLGENS;
int NTHREADS = -1;

void printFinalResults(MatrixElement** matrix, int R, int C, int GEN_PROC_RABBITS, int GEN_PROC_FOXES, int GEN_FOOD_FOXES){

    int counter = 0;

    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++) {
            if (matrix[i][j].element_type == ElementType::RABBIT) {
                counter++;
            } else if (matrix[i][j].element_type == ElementType::FOX) {
                counter++;
            } else if (matrix[i][j].element_type == ElementType::ROCK) {
                counter++;
            }
        }
    }

    printf("%d %d %d %d %d %d %d\n", GEN_PROC_RABBITS, GEN_PROC_FOXES, GEN_FOOD_FOXES, 0, R, C, counter);

    if (PRINT_FINAL_INFO) {
        for (int i = 0; i < R; i++) {
            for (int j = 0; j < C; j++) {
                if (matrix[i][j].element_type == ElementType::RABBIT) {
                    printf("RABBIT %d %d\n", i, j);
                } else if (matrix[i][j].element_type == ElementType::FOX) {
                    printf("FOX %d %d\n", i, j);
                } else if (matrix[i][j].element_type == ElementType::ROCK) {
                    printf("ROCK %d %d\n", i, j);
                }
            }
        }
    }

}

void printMatrix(MatrixElement** matrix, int R, int C) {

    for (int i = 0; i < R+2; ++i) {
        printf("-");
    }
    printf("\n");

    for (int i = 0; i < R; ++i) {
        printf("|");
        for (int j = 0; j < C; ++j) {
            printf("%c", matrix[i][j].getChar());
        }
        printf("|\n");
    }

    for (int i = 0; i < R+2; ++i) {
        printf("-");
    }
    printf("\n");

}

std::vector<char> checkAdjacencies(MatrixElement** matrix ,int X, int Y,ElementType type){
    std::vector<char> validPositions;
    if(X-1 >= 0 && matrix[X-1][Y].element_type == type){ //NORTH
        validPositions.push_back('N');
    }

    if(Y+1 < C && matrix[X][Y+1].element_type == type){ //EAST
        validPositions.push_back('E');
    }

    if(X+1 < R && matrix[X+1][Y].element_type == type){ // SOUTH
        validPositions.push_back('S');
    }

    if(Y-1 >= 0 && matrix[X][Y-1].element_type == type) { //WEST
        validPositions.push_back('W');
    }

    return validPositions;
}

std::pair<int,int> chooseMovePosition(int currentGen, int xPos, int yPos, std::vector<char> posVal){
    int pos = (int) ((currentGen + xPos + yPos) % posVal.size());

    if(posVal[pos] == 'N'){
        std::pair<int, int> posPair = std::make_pair(xPos-1,yPos);
        return posPair;
    }

    else if(posVal[pos] == 'E'){
        std::pair<int, int> posPair = std::make_pair (xPos,yPos+1);
        return posPair;
    }

    else if(posVal[pos] == 'S'){
        std::pair<int, int> posPair = std::make_pair (xPos+1,yPos);
        return posPair;
    }

    else if(posVal[pos] == 'W'){
        std::pair<int, int> posPair = std::make_pair (xPos,yPos-1);
        return posPair;
    }

    perror("chooseMovePosition was used improperly");
    exit(1);

};

void analyzeRabbits(int currentGen) {
#pragma omp parallel for schedule(static)
    for (int x = 0; x < R; x++) {
        for (int y = 0; y < C; y++) {
            if (posMatrix[x][y].element_type == ElementType::RABBIT) {
                Rabbit rabbit = posMatrix[x][y].elem.rb;
                std::vector<char> validPositions = checkAdjacencies(posMatrix, x, y, ElementType::EMPTY);

                Rabbit rabbitTemp = Rabbit(rabbit.procAge+1,x,y);
                if (validPositions.size() > 0) {

                    std::pair<int, int> posToMove = chooseMovePosition(currentGen, x, y, validPositions);
                    int xToMove = posToMove.first;
                    int yToMove = posToMove.second;

                    bool canProc = rabbitTemp.procAge > GEN_PROC_RABBITS;
                    if (canProc)
                        rabbitTemp.procAge = 0;

                    omp_set_lock(&(lockMatrix[xToMove][yToMove]));
                    if (posMatrixTemp[xToMove][yToMove].element_type == ElementType::EMPTY) {

                        if (canProc) {
                            Rabbit fatherRabbit = Rabbit(0, xToMove, yToMove);
                            Rabbit babyRabbit = Rabbit(0, x, y);
                            MatrixElement elFather = MatrixElement(ElementType::RABBIT);
                            MatrixElement elBaby = MatrixElement(ElementType::RABBIT);
                            elFather.elem.rb = fatherRabbit;
                            elBaby.elem.rb = babyRabbit;

                            omp_set_lock(&(lockMatrix[x][y]));
                            posMatrixTemp[x][y] = elBaby;
                            omp_unset_lock(&(lockMatrix[x][y]));

                            posMatrixTemp[xToMove][yToMove] = elFather;


                        } else {
                            Rabbit newRabbit = Rabbit(rabbitTemp.procAge, xToMove, yToMove);
                            MatrixElement elNew = MatrixElement(ElementType::RABBIT);
                            elNew.elem.rb = newRabbit;

                            posMatrixTemp[xToMove][yToMove] = elNew;

                        }

                    } else if (posMatrixTemp[xToMove][yToMove].element_type == ElementType::RABBIT) {
                        if (rabbitTemp.procAge > posMatrixTemp[xToMove][yToMove].elem.rb.procAge) {


                            if (canProc) {
                                Rabbit fatherRabbit = Rabbit(0, xToMove, yToMove);
                                Rabbit babyRabbit = Rabbit(0, x, y);
                                MatrixElement elFather = MatrixElement(ElementType::RABBIT);
                                MatrixElement elBaby = MatrixElement(ElementType::RABBIT);
                                elFather.elem.rb = fatherRabbit;
                                elBaby.elem.rb = babyRabbit;

                                omp_set_lock(&(lockMatrix[x][y]));
                                posMatrixTemp[x][y] = elBaby;
                                omp_unset_lock(&(lockMatrix[x][y]));

                                posMatrixTemp[xToMove][yToMove] = elFather;

                            } else {

                                Rabbit newRabbit = Rabbit(rabbitTemp.procAge, xToMove, yToMove);
                                MatrixElement elNew = MatrixElement(ElementType::RABBIT);
                                elNew.elem.rb = newRabbit;

                                posMatrixTemp[xToMove][yToMove] = elNew;

                            }
                        } else if (canProc) {
                            Rabbit babyRabbit = Rabbit(0, x, y);
                            MatrixElement elBaby = MatrixElement(ElementType::RABBIT);
                            elBaby.elem.rb = babyRabbit;

                            omp_set_lock(&(lockMatrix[x][y]));
                            posMatrixTemp[x][y] = elBaby;
                            omp_unset_lock(&(lockMatrix[x][y]));
                        }

                        // else, he dies because he has a lower ProcAge

                    } else {
                        perror("analyzeRabbits was used improperly - you are trying to move into a rock or a fox");
                        exit(1);
                    }

                    omp_unset_lock(&(lockMatrix[xToMove][yToMove]));

                } else {
                    // There's no valid Position to move into. Rabbit raises the ProcAge and doesn't move.
                    Rabbit newRabbit = Rabbit(rabbitTemp.procAge, x, y);
                    MatrixElement elNew = MatrixElement(ElementType::RABBIT);
                    elNew.elem.rb = newRabbit;

                    omp_set_lock(&(lockMatrix[x][y]));
                    posMatrixTemp[x][y] = elNew;
                    omp_unset_lock(&(lockMatrix[x][y]));

                }

            }
        }

    }
}

void analyzeFoxes(int currentGen){
#pragma omp parallel for schedule(static)
    for (int x = 0; x < R; x++) {
        for (int y = 0; y < C; y++) {
            if (posMatrix[x][y].element_type == ElementType::FOX) {
                Fox fox = posMatrix[x][y].elem.fx;
                Fox foxTemp = Fox(fox.hungryAge, fox.procAge+1, x,y);

                std::vector<char> validPositionsWithRabbits = checkAdjacencies(posMatrix, x, y, ElementType::RABBIT);
                std::vector<char> validPositions = checkAdjacencies(posMatrix, x, y, ElementType::EMPTY);

                if (validPositionsWithRabbits.size() > 0) {
                    std::pair<int, int> posToMove = chooseMovePosition(currentGen, x, y, validPositionsWithRabbits);
                    int xToMove = posToMove.first;
                    int yToMove = posToMove.second;


                    bool canProc = foxTemp.procAge > GEN_PROC_FOXES;
                    if (canProc)
                        foxTemp.procAge = 0;

                    omp_set_lock(&(lockMatrix[xToMove][yToMove]));
                    if (posMatrixTemp[xToMove][yToMove].element_type == ElementType::RABBIT) {

                        if (canProc) {
                            Fox fatherFox = Fox(0, 0, xToMove, yToMove);
                            Fox babyFox = Fox(0, 0, x, y);
                            MatrixElement elFather = MatrixElement(ElementType::FOX);
                            MatrixElement elBaby = MatrixElement(ElementType::FOX);
                            elFather.elem.fx = fatherFox;
                            elBaby.elem.fx = babyFox;

                            omp_set_lock(&(lockMatrix[x][y]));
                            posMatrixTemp[x][y] = elBaby;
                            omp_unset_lock(&(lockMatrix[x][y]));

                            posMatrixTemp[xToMove][yToMove] = elFather;

                        } else {
                            Fox newFox = Fox(0, foxTemp.procAge, xToMove, yToMove);
                            MatrixElement elNew = MatrixElement(ElementType::FOX);
                            elNew.elem.fx = newFox;

                            posMatrixTemp[xToMove][yToMove] = elNew;

                        }

                    } else if (posMatrixTemp[xToMove][yToMove].element_type == ElementType::FOX) {

                        if (foxTemp.procAge  > posMatrixTemp[xToMove][yToMove].elem.fx.procAge ||
                            (foxTemp.procAge  == posMatrixTemp[xToMove][yToMove].elem.fx.procAge
                             && foxTemp.hungryAge+1  < posMatrixTemp[xToMove][yToMove].elem.fx.hungryAge)) {

                            // delete the fox that was there

                            if (canProc) {
                                Fox fatherFox = Fox(0, 0, xToMove, yToMove);
                                Fox babyFox = Fox(0, 0, x, y);
                                MatrixElement elFather = MatrixElement(ElementType::FOX);
                                MatrixElement elBaby = MatrixElement(ElementType::FOX);
                                elFather.elem.fx = fatherFox;
                                elBaby.elem.fx = babyFox;

                                omp_set_lock(&(lockMatrix[x][y]));
                                posMatrixTemp[x][y] = elBaby;
                                omp_unset_lock(&(lockMatrix[x][y]));

                                posMatrixTemp[xToMove][yToMove] = elFather;

                            } else {
                                Fox newFox = Fox(0, foxTemp.procAge , xToMove, yToMove);
                                MatrixElement elNew = MatrixElement(ElementType::FOX);
                                elNew.elem.fx = newFox;

                                posMatrixTemp[xToMove][yToMove] = elNew;

                            }

                        } else {

                            if (canProc) {
                                Fox babyFox = Fox(0, 0, x, y);
                                MatrixElement elBaby = MatrixElement(ElementType::FOX);
                                elBaby.elem.fx = babyFox;

                                omp_set_lock(&(lockMatrix[x][y]));
                                posMatrixTemp[x][y] = elBaby;
                                omp_unset_lock(&(lockMatrix[x][y]));

                            }
                        }

                    } else {
                        perror("analyzeFoxes was used improperly - you are trying to move into a rock");
                        exit(1);
                    }

                    omp_unset_lock(&(lockMatrix[xToMove][yToMove]));


                } else if (validPositions.size() > 0 && fox.hungryAge+1 < GEN_FOOD_FOXES) {

                    bool canProc = foxTemp.procAge > GEN_PROC_FOXES;
                    if (canProc)
                        foxTemp.procAge = 0;

                    std::pair<int, int> posToMove = chooseMovePosition(currentGen, x, y, validPositions);
                    int xToMove = posToMove.first;
                    int yToMove = posToMove.second;

                    omp_set_lock(&(lockMatrix[xToMove][yToMove]));
                    if (posMatrixTemp[xToMove][yToMove].element_type == ElementType::EMPTY) {

                        if (canProc){
                            Fox fatherFox = Fox(foxTemp.hungryAge+1, 0, xToMove, yToMove);
                            Fox babyFox = Fox(0, 0, x, y);
                            MatrixElement elFather = MatrixElement(ElementType::FOX);
                            MatrixElement elBaby = MatrixElement(ElementType::FOX);
                            elFather.elem.fx = fatherFox;
                            elBaby.elem.fx = babyFox;

                            omp_set_lock(&(lockMatrix[x][y]));
                            posMatrixTemp[x][y] = elBaby;
                            omp_unset_lock(&(lockMatrix[x][y]));

                            posMatrixTemp[xToMove][yToMove] = elFather;

                        } else {
                            Fox newFox = Fox(foxTemp.hungryAge+1, foxTemp.procAge, xToMove, yToMove);
                            MatrixElement elNew = MatrixElement(ElementType::FOX);
                            elNew.elem.fx = newFox;

                            posMatrixTemp[xToMove][yToMove] = elNew;

                        }

                    } else if (posMatrixTemp[xToMove][yToMove].element_type == ElementType::FOX) {

                        if (foxTemp.procAge > posMatrixTemp[xToMove][yToMove].elem.fx.procAge ||
                            (foxTemp.procAge == posMatrixTemp[xToMove][yToMove].elem.fx.procAge
                             && foxTemp.hungryAge +1 < posMatrixTemp[xToMove][yToMove].elem.fx.hungryAge)) {

                            if (canProc) {
                                Fox fatherFox = Fox(foxTemp.hungryAge +1 , 0, xToMove, yToMove);
                                Fox babyFox = Fox(0, 0, x, y);
                                MatrixElement elFather = MatrixElement(ElementType::FOX);
                                MatrixElement elBaby = MatrixElement(ElementType::FOX);
                                elFather.elem.fx = fatherFox;
                                elBaby.elem.fx = babyFox;

                                omp_set_lock(&(lockMatrix[x][y]));
                                posMatrixTemp[x][y] = elBaby;
                                omp_unset_lock(&(lockMatrix[x][y]));

                                posMatrixTemp[xToMove][yToMove] = elFather;

                            } else {
                                Fox newFox = Fox(foxTemp.hungryAge +1, foxTemp.procAge , xToMove, yToMove);
                                MatrixElement elNew = MatrixElement(ElementType::FOX);
                                elNew.elem.fx = newFox;

                                posMatrixTemp[xToMove][yToMove] = elNew;

                            }

                        } else {

                            if (canProc) {
                                Fox babyFox = Fox(0, 0, x, y);
                                MatrixElement elBaby = MatrixElement(ElementType::FOX);
                                elBaby.elem.fx = babyFox;

                                omp_set_lock(&(lockMatrix[x][y]));
                                posMatrixTemp[x][y] = elBaby;
                                omp_unset_lock(&(lockMatrix[x][y]));
                            }
                        }

                    } else {
                        perror("analyzeFoxes was used improperly - you are trying to move into a rock");
                        exit(1);
                    }

                    omp_unset_lock(&(lockMatrix[xToMove][yToMove]));

                } else if (foxTemp.hungryAge +1 < GEN_FOOD_FOXES) {
                    // There's no valid Position to move into. Fox raises its ProcAge and doesn't move.
                    Fox newFox = Fox(foxTemp.hungryAge +1 , foxTemp.procAge , x, y);

                    MatrixElement elNew = MatrixElement(ElementType::FOX);
                    elNew.elem.fx = newFox;

                    omp_set_lock(&(lockMatrix[x][y]));
                    posMatrixTemp[x][y] = elNew;
                    omp_unset_lock(&(lockMatrix[x][y]));

                } else if (foxTemp.hungryAge +1 >= GEN_FOOD_FOXES) {
                    omp_set_lock(&(lockMatrix[x][y]));
                    posMatrixTemp[x][y] = MatrixElement(ElementType::EMPTY);
                    omp_unset_lock(&(lockMatrix[x][y]));
                } else {
                    perror("analyzeFoxes was used improperly - the fox should have been dead already");
                    exit(1);
                }
            }
        }
    }

}


void simGen(int gen){

    memcpy(posMatrixTempAux, posMatrixAux, sizeof(MatrixElement)*R*C);

    prepareTempForRabbit();

    analyzeRabbits(gen);

    memcpy(posMatrixAux, posMatrixTempAux, R*C*sizeof(MatrixElement));

    prepareTempForFox();

    analyzeFoxes(gen);

    memcpy(posMatrixAux, posMatrixTempAux, R*C*sizeof(MatrixElement));
}

void prepareTempForRabbit() {

    for (int i = 0; i < R; ++i) {
        for (int j = 0; j < C; ++j) {
            if (posMatrixTemp[i][j].element_type == ElementType::RABBIT ) {
                posMatrixTemp[i][j] = MatrixElement(ElementType::EMPTY);
            }
        }
    }

}

void prepareTempForFox() {

    for (int i = 0; i < R; ++i) {
        for (int j = 0; j < C; ++j) {
            if (posMatrixTemp[i][j].element_type == ElementType::FOX) {
                posMatrixTemp[i][j] = MatrixElement(ElementType::EMPTY);
            }
        }
    }

}

void print_help() {
    printf("usage:\n\tecosystem [arguments]\n\n"
                   "Available arguments:\n"
                   "\t-h\t\tdisplay this help file\n"
                   "\t-np <np>\t\tuse <np> threads\n"
                   "\t-f <filename>\t\tuse <filename> as input\n"
                   "\t-pf\t\tprint final info\n"
                   "\t-pt\t\tprint computation time\n"
                   "\t-v\t\tverbose mode (print all generations)\n");
}

void read_input() {
    std::cin >> GEN_PROC_RABBITS >> GEN_PROC_FOXES >> GEN_FOOD_FOXES >> N_GEN >> R >> C >> N;

    posMatrix = new MatrixElement*[R];
    posMatrixAux = new MatrixElement[R*C];

    posMatrixTemp = new MatrixElement*[R];
    posMatrixTempAux = new MatrixElement[R*C];

    lockMatrix = new omp_lock_t*[R];
    lockMatrixAux = new omp_lock_t[R*C];

    for (int i = 0; i < R; ++i) {
        posMatrix[i] = &posMatrixAux[i*C];
        posMatrixTemp[i] = &posMatrixTempAux[i*C];
        lockMatrix[i] = &lockMatrixAux[i*C];
    }

    for (int i=0; i<R; i++) {
        for(int j=0; j<C; j++){
            omp_init_lock(&(lockMatrix[i][j]));
        }
    }

    for (int i = 0; i < R; ++i) {
        for (int j = 0; j < C; ++j) {
            MatrixElement el = MatrixElement(ElementType::EMPTY);
            posMatrix[i][j] = el;
            posMatrixTemp[i][j] = el;
        }
    }

    for (int i = 0; i < N; ++i) {
        std::string TYPE;
        int X;
        int Y;

        std::cin >> TYPE >> X >> Y;

        if (TYPE == "RABBIT"){

            Rabbit r = Rabbit(0, X, Y);
            MatrixElement el = MatrixElement(ElementType::RABBIT);
            el.elem.rb = r;
            posMatrix[X][Y] = el;

        } else if (TYPE == "FOX"){

            Fox f = Fox(0,0,X,Y);
            MatrixElement el = MatrixElement(ElementType::FOX);
            el.elem.fx = f;
            posMatrix[X][Y] = el;

        } else if (TYPE == "ROCK"){

            Rock rk = Rock(X,Y);
            MatrixElement el = MatrixElement(ElementType::ROCK);
            el.elem.rk = rk;
            posMatrix[X][Y] = el;

        } else {
            std::cout << "YOU FUCKED UP" << std::endl;
        }
    }
}

void parse_arguments(int argc, char* argv[]){

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0) {
            freopen(argv[i+1], "r", stdin);
            i++;
        } else if (strcmp(argv[i], "-v") == 0) {
            PRINT_ALLGENS = 1;
        } else if (strcmp(argv[i], "-pf") == 0) {
            PRINT_FINAL_INFO = 1;
        } else if (strcmp(argv[i], "-pt") == 0) {
            PRINT_TIME = 1;
        } else if (strcmp(argv[i], "-np") == 0) {
            NTHREADS = atoi(argv[i + 1]);
            if (NTHREADS < 0){
                exit(1);
            }
            i++;
        } else if (strcmp(argv[i], "-h") == 0) {
            print_help();
            exit(0);
        }
    }

}

int main(int argc, char* argv[]) {

    parse_arguments(argc, argv);

    read_input();

    struct timeb start, end;

    ftime(&start);

    if(NTHREADS != -1) {
        omp_set_num_threads(NTHREADS);
    }

    for (int gen = 0; gen < N_GEN; gen++) {
        if (PRINT_ALLGENS) {
            std::cout << "Generation " << gen << std::endl;
            printMatrix(posMatrix, R, C);
            std::cout << std::endl;
        }
        simGen(gen);
    }

    ftime(&end);

    if (PRINT_ALLGENS) {
        std::cout << "Generation " << N_GEN << std::endl;
        printMatrix(posMatrix, R, C);
        std::cout << std::endl;
    }

    int diff = (int) (1000.0 * (end.time - start.time) + (end.millitm - start.millitm));

    if (PRINT_TIME) {
        printf("Operation took %u milliseconds\n", diff);
    }

    printFinalResults(posMatrix, R, C, GEN_PROC_RABBITS, GEN_PROC_FOXES, GEN_FOOD_FOXES);

    return 0;
}
