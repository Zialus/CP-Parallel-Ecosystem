#pragma once

#include <cstdio>

enum class ElementType {
    RABBIT, FOX, ROCK, EMPTY
};

struct Rock {

    Rock() = default;

    Rock(int x, int y) {
        pos_x = x;
        pos_y = y;
    }

    int pos_x;
    int pos_y;

};

struct Rabbit {

    Rabbit() = default;

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

    Fox() = default;

    Fox(int h, int a, int x, int y) {
        hungryAge = h;
        procAge = a;
        pos_x = x;
        pos_y = y;
    }

    int hungryAge;
    int procAge;
    int pos_x;
    int pos_y;

};

struct MatrixElement {

    MatrixElement() = default;

    explicit MatrixElement(ElementType type) {
        element_type = type;
    }

    ElementType element_type;

    union {
        struct Rabbit rb;
        struct Fox fx;
        struct Rock rk;
    } elem;

    char getChar() const {
        switch (this->element_type) {
            case ElementType::RABBIT:
                return 'R';
            case ElementType::FOX:
                return 'F';
            case ElementType::ROCK:
                return '*';
            case ElementType::EMPTY:
                return ' ';
        }
        return '?';
    }
};

void printFinalResults(MatrixElement** matrix, FILE* file);

void printMatrix(MatrixElement** matrix);

std::vector<char> checkAdjacencies(MatrixElement** matrix, int X, int Y, ElementType type);

std::pair<int, int> chooseMovePosition(int currentGen, int xPos, int yPos, std::vector<char> posVal);

void analyzeRabbits(int currentGen);

void analyzeFoxes(int currentGen);

void simGen(int gen);

void finalCopy();

void prepareTempForRabbit();

void prepareTempForFox();

void print_help();

void read_input();

void parse_arguments(int argc, char* argv[]);

template<typename InputIterator1, typename InputIterator2>
bool range_equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2);

bool compare_files(const std::string& filename1, const std::string& filename2);

void dealWithOutput();

void freeEverything();
