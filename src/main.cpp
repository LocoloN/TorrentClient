#include <filesystem>
#include <iostream>

#include "parser/bencodeparse.hpp"

using namespace std;

int main() {
    int *p = (int *)malloc(sizeof(int));
    free(p);
    return 0;
}
