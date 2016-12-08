#include <iostream>
#include <vector>
#include <unordered_set>

#include <string.h>
#include <sys/timeb.h>

#include "main.hpp"

MatrixElement** posMatrix;
MatrixElement** posMatrixTemp;

MatrixElement* auxMatrix;
MatrixElement* auxMatrixTemp;

std::unordered_set<Rabbit> RabbitSet;
std::unordered_set<Fox>    FoxSet;
std::unordered_set<Rock>   RockSet;

int GEN_PROC_RABBITS ; // number of generations until a rabbit can procreate
int GEN_PROC_FOXES ; // number of generations until a fox can procreate
int GEN_FOOD_FOXES ; // number of generations for a fox to die of starvation
int N_GEN ; // number of generations for the simulation
int R ; // number of rows of the matrix representing the ecosystem
int C ; // number of columns of the matrix representing the ecosystem
int N ; // number of objects in the initial ecosystem

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

    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++){
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

std::unordered_set<Rabbit> analyzeRabbits(std::unordered_set<Rabbit> RabbitSet, int currentGen){

    std::unordered_set<Rabbit> RabbitSetTemp;

    for (auto rabbit: RabbitSet) {

        int x = rabbit.pos_x;
        int y = rabbit.pos_y;
        std::vector<char> validPositions = checkAdjacencies(posMatrix, x, y, ElementType::EMPTY);

        if (validPositions.size() > 0) {

            std::pair<int, int> posToMove = chooseMovePosition(currentGen, x, y, validPositions);
            int xToMove = posToMove.first;
            int yToMove = posToMove.second;

            if (posMatrixTemp[xToMove][yToMove].element_type == ElementType::EMPTY) {
                if (rabbit.procAge >= GEN_PROC_RABBITS) {
                    Rabbit fatherRabbit = Rabbit(0, xToMove, yToMove);
                    Rabbit babyRabbit = Rabbit(0, x, y);
                    MatrixElement elFather = MatrixElement(ElementType::RABBIT);
                    MatrixElement elBaby = MatrixElement(ElementType::RABBIT);
                    elFather.elem.rb = fatherRabbit;
                    elBaby.elem.rb = babyRabbit;
                    posMatrixTemp[x][y] = elBaby;
                    posMatrixTemp[xToMove][yToMove] = elFather;
                    RabbitSetTemp.insert(fatherRabbit);
                    RabbitSetTemp.insert(babyRabbit);
                } else {
                    Rabbit newRabbit = Rabbit(rabbit.procAge + 1, xToMove, yToMove);
                    MatrixElement elNew = MatrixElement(ElementType::RABBIT);
                    elNew.elem.rb = newRabbit;
                    posMatrixTemp[xToMove][yToMove] = elNew;
                    RabbitSetTemp.insert(newRabbit);
                }

            } else if (posMatrixTemp[xToMove][yToMove].element_type == ElementType::RABBIT) {
                if (rabbit.procAge > posMatrixTemp[xToMove][yToMove].elem.rb.procAge) {

                    // Delete the rabbit with worse ProcAge
                    RabbitSetTemp.erase(posMatrixTemp[xToMove][yToMove].elem.rb);

                    if (rabbit.procAge >= GEN_PROC_RABBITS) {
                        Rabbit fatherRabbit = Rabbit(0, xToMove, yToMove);
                        Rabbit babyRabbit = Rabbit(0, x, y);
                        MatrixElement elFather = MatrixElement(ElementType::RABBIT);
                        MatrixElement elBaby = MatrixElement(ElementType::RABBIT);
                        elFather.elem.rb = fatherRabbit;
                        elBaby.elem.rb = babyRabbit;
                        posMatrixTemp[x][y] = elBaby;
                        posMatrixTemp[xToMove][yToMove] = elFather;
                        RabbitSetTemp.insert(fatherRabbit);
                        RabbitSetTemp.insert(babyRabbit);
                    } else {

                        Rabbit newRabbit = Rabbit(rabbit.procAge + 1, xToMove, yToMove);
                        MatrixElement elNew = MatrixElement(ElementType::RABBIT);
                        elNew.elem.rb = newRabbit;
                        posMatrixTemp[xToMove][yToMove] = elNew;
                        RabbitSetTemp.insert(newRabbit);
                    }
                }

                // else, he dies cuz he has a lower ProcAge

            } else {
                perror("analyzeRabbits was used improperly - you are trying to move into a rock or a fox");
                exit(1);
            }


        } else {
            // There's no valid Position to move into. Rabbit raises the ProcAge and doesn't move.
            Rabbit newRabbit = Rabbit(rabbit.procAge + 1, x, y);

            RabbitSetTemp.insert(newRabbit);
            MatrixElement elNew = MatrixElement(ElementType::RABBIT);
            elNew.elem.rb = newRabbit;
            posMatrixTemp[x][y] = elNew;
        }

    }

    return RabbitSetTemp;
}

std::unordered_set<Fox> analyzeFoxes(std::unordered_set<Fox> FoxSet, int currentGen){

    std::unordered_set<Fox> FoxSetTemp;

    for (auto fox: FoxSet) {

        int x = fox.pos_x;
        int y = fox.pos_y;
        int h = fox.hungryAge;

        int p = fox.procAge;
        std::vector<char> validPositionsWithRabbits = checkAdjacencies(posMatrix, x, y, ElementType::RABBIT);
        std::vector<char> validPositions = checkAdjacencies(posMatrix, x, y, ElementType::EMPTY);

        if(validPositionsWithRabbits.size() > 0){
            std::pair<int,int> posToMove = chooseMovePosition(currentGen,x,y,validPositionsWithRabbits);
            int xToMove = posToMove.first;
            int yToMove = posToMove.second;

            if(posMatrixTemp[xToMove][yToMove].element_type == ElementType::RABBIT){

                // delete the rabbit that was there
                RabbitSet.erase(posMatrixTemp[xToMove][yToMove].elem.rb);


                if(fox.procAge >= GEN_PROC_FOXES){
                    Fox fatherFox = Fox(0 ,0, xToMove, yToMove);
                    Fox babyFox = Fox(0, 0, x, y);
                    MatrixElement elFather = MatrixElement(ElementType::FOX);
                    MatrixElement elBaby = MatrixElement(ElementType::FOX);
                    elFather.elem.fx = fatherFox;
                    elBaby.elem.fx = babyFox;
                    posMatrixTemp[x][y] = elBaby;
                    posMatrixTemp[xToMove][yToMove] = elFather;
                    FoxSetTemp.insert(fatherFox);
                    FoxSetTemp.insert(babyFox);
                }
                else{
                    Fox newFox = Fox(0, p+1, xToMove, yToMove);
                    MatrixElement elNew = MatrixElement(ElementType::FOX);
                    elNew.elem.fx = newFox;
                    posMatrixTemp[xToMove][yToMove] = elNew;
                    FoxSetTemp.insert(newFox);
                }

            }
            else if(posMatrixTemp[xToMove][yToMove].element_type == ElementType::FOX){

                // delete the fox that was there
//                FoxSet.erase(posMatrixTemp[xToMove][yToMove].elem.fx);


                if(fox.procAge > posMatrixTemp[xToMove][yToMove].elem.fx.procAge ||
                   ( fox.procAge == posMatrixTemp[xToMove][yToMove].elem.fx.procAge
                     && fox.hungryAge < posMatrixTemp[xToMove][yToMove].elem.fx.hungryAge) ){

                    FoxSetTemp.erase(posMatrixTemp[xToMove][yToMove].elem.fx);

                    if(fox.procAge >= GEN_PROC_FOXES){
                        Fox fatherFox = Fox(h+1 ,0, xToMove, yToMove);
                        Fox babyFox = Fox(0, 0, x, y);
                        MatrixElement elFather = MatrixElement(ElementType::FOX);
                        MatrixElement elBaby = MatrixElement(ElementType::FOX);
                        elFather.elem.fx = fatherFox;
                        elBaby.elem.fx = babyFox;
                        posMatrixTemp[x][y] = elBaby;
                        posMatrixTemp[xToMove][yToMove] = elFather;
                        FoxSetTemp.insert(fatherFox);
                        FoxSetTemp.insert(babyFox);
                    }
                    else{
                        Fox newFox = Fox(h+1, p+1, xToMove, yToMove);
                        MatrixElement elNew = MatrixElement(ElementType::FOX);
                        elNew.elem.fx = newFox;
                        posMatrixTemp[xToMove][yToMove] = elNew;
                        FoxSetTemp.insert(newFox);
                    }

                }

            }
            else{
                perror("analyzeFoxes was used improperly");
                exit(1);
            }


        }
        else if(validPositions.size() > 0 && fox.hungryAge+1 < GEN_FOOD_FOXES){

            std::pair<int,int> posToMove = chooseMovePosition(currentGen,x,y,validPositions);
            int xToMove = posToMove.first;
            int yToMove = posToMove.second;

            if(posMatrixTemp[xToMove][yToMove].element_type == ElementType::EMPTY){
                if(fox.procAge >= GEN_PROC_FOXES){
                    Fox fatherFox = Fox(h+1 ,0, xToMove, yToMove);
                    Fox babyFox = Fox(0, 0, x, y);
                    MatrixElement elFather = MatrixElement(ElementType::FOX);
                    MatrixElement elBaby = MatrixElement(ElementType::FOX);
                    elFather.elem.fx = fatherFox;
                    elBaby.elem.fx = babyFox;
                    posMatrixTemp[x][y] = elBaby;
                    posMatrixTemp[xToMove][yToMove] = elFather;
                    FoxSetTemp.insert(fatherFox);
                    FoxSetTemp.insert(babyFox);
                }
                else{
                    Fox newFox = Fox(h+1, p+1, xToMove, yToMove);
                    MatrixElement elNew = MatrixElement(ElementType::FOX);
                    elNew.elem.fx = newFox;
                    posMatrixTemp[xToMove][yToMove] = elNew;
                    FoxSetTemp.insert(newFox);
                }

            }

            else if(posMatrixTemp[xToMove][yToMove].element_type == ElementType::FOX){

                if(fox.procAge > posMatrixTemp[xToMove][yToMove].elem.fx.procAge ||
                   ( fox.procAge == posMatrixTemp[xToMove][yToMove].elem.fx.procAge
                     && fox.hungryAge < posMatrixTemp[xToMove][yToMove].elem.fx.hungryAge) ){

                    FoxSetTemp.erase(posMatrixTemp[xToMove][yToMove].elem.fx);


                    if(fox.procAge >= GEN_PROC_FOXES){
                        Fox fatherFox = Fox(h+1 ,0, xToMove, yToMove);
                        Fox babyFox = Fox(0, 0, x, y);
                        MatrixElement elFather = MatrixElement(ElementType::FOX);
                        MatrixElement elBaby = MatrixElement(ElementType::FOX);
                        elFather.elem.fx = fatherFox;
                        elBaby.elem.fx = babyFox;
                        posMatrixTemp[x][y] = elBaby;
                        posMatrixTemp[xToMove][yToMove] = elFather;
                        FoxSetTemp.insert(fatherFox);
                        FoxSetTemp.insert(babyFox);
                    }
                    else{
                        Fox newFox = Fox(h+1, p+1, xToMove, yToMove);
                        MatrixElement elNew = MatrixElement(ElementType::FOX);
                        elNew.elem.fx = newFox;
                        posMatrixTemp[xToMove][yToMove] = elNew;
                        FoxSetTemp.insert(newFox);
                    }
                }

            }
            else{
                perror("analyzeFoxes was used improperly");
                exit(1);
            }

        }

        else if(fox.hungryAge+1 < GEN_FOOD_FOXES){
            Fox newFox = Fox(h+1, p+1, x, y);
            FoxSetTemp.insert(newFox);
            MatrixElement elNew = MatrixElement(ElementType::FOX);
            elNew.elem.fx = newFox;
            posMatrixTemp[x][y] = elNew;
        }
        else if(fox.hungryAge+1 == GEN_FOOD_FOXES) {
            posMatrixTemp[x][y] = MatrixElement(ElementType::EMPTY);
        }

        else{
            std::cout << "Foxes fucked up.." << std::endl;
        }
    }

    return FoxSetTemp;
}



void simGen(int gen){

    prepareTempForRabbit();

    RabbitSet = analyzeRabbits(RabbitSet, gen);

    memcpy(auxMatrix, auxMatrixTemp, R*C*sizeof(MatrixElement));

    prepareTempForFox();

    FoxSet = analyzeFoxes(FoxSet, gen);

    memcpy(auxMatrix, auxMatrixTemp, R*C*sizeof(MatrixElement));
}

void prepareTempForRabbit() {
    memcpy(auxMatrixTemp, auxMatrix, sizeof(MatrixElement)*R*C);

    for (int i = 0; i < R; ++i) {
        for (int j = 0; j < C; ++j) {
            if (posMatrixTemp[i][j].element_type == ElementType::RABBIT ){
                posMatrixTemp[i][j] = MatrixElement(ElementType::EMPTY);
            }
        }
    }
}

void prepareTempForFox() {

    for (int i = 0; i < R; ++i) {
        for (int j = 0; j < C; ++j) {
            if (posMatrixTemp[i][j].element_type == ElementType::FOX){
                posMatrixTemp[i][j] = MatrixElement(ElementType::EMPTY);
            }
        }
    }
}



int main(int argc, char* argv[]) {

    if (argc == 2){
        freopen(argv[1], "r", stdin);
    }

    std::cin >> GEN_PROC_RABBITS >> GEN_PROC_FOXES >> GEN_FOOD_FOXES >> N_GEN >> R >> C >> N;

    posMatrix = new MatrixElement*[R];
    auxMatrix = new MatrixElement[R*C];
    posMatrixTemp = new MatrixElement*[R];
    auxMatrixTemp = new MatrixElement[R*C];

    for (int i = 0; i < R; ++i) {
        posMatrix[i] = &auxMatrix[i*C];
        posMatrixTemp[i] = &auxMatrixTemp[i*C];
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
            RabbitSet.insert(r);
            MatrixElement el = MatrixElement(ElementType::RABBIT);
            el.elem.rb = r;
            posMatrix[X][Y] = el;
        }
        else if (TYPE == "FOX"){
            Fox f = Fox(0,0,X,Y);
            FoxSet.insert(f);
            MatrixElement el = MatrixElement(ElementType::FOX);
            el.elem.fx = f;
            posMatrix[X][Y] = el;
        }
        else if (TYPE == "ROCK"){
            Rock rk = Rock(X,Y);
            RockSet.insert(rk);
            MatrixElement el = MatrixElement(ElementType::ROCK);
            el.elem.rk = rk;
            posMatrix[X][Y] = el;
        }
        else {
            std::cout << "YOU FUCKED UP" << std::endl;
        }
    }

    struct timeb start, end;
    int diff;

    ftime(&start);
    for(int gen=0; gen<N_GEN; gen++){
        std::cout << "Generation " << gen << std::endl;
        printMatrix(posMatrix, R,C);
        std::cout << std::endl;
        simGen(gen);
    }
    ftime(&end);

    std::cout << "Generation " << N_GEN << std::endl;
    printMatrix(posMatrix, R, C);
    std::cout << std::endl;

    diff = (int) (1000.0 * (end.time - start.time) + (end.millitm - start.millitm));
    printf("Operation took %u milliseconds\n", diff);

    printFinalResults(posMatrix, R, C, GEN_PROC_RABBITS, GEN_PROC_FOXES, GEN_FOOD_FOXES);

    return 0;
}
