#include "parent.h"

#include <stdio.h>
#include <stdlib.h>

int main(void) {

    parent(getenv("PATH_TO_CHILD"), stdin);

    return 0;
}   