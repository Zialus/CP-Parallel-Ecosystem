#pragma once

#include <cstdio>

typedef enum {
    RABBIT, FOX, ROCK, EMPTY
} ElementType;

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

    char getChar() {
        switch (this->element_type) {
            case RABBIT:
                return 'R';
            case FOX:
                return 'F';
            case ROCK:
                return '*';
            case EMPTY:
                return ' ';
        }
        return '?';
    }
};

void prepareTempForRabbit();
void prepareTempForFox();
void finalCopy();
void freeEverything();
