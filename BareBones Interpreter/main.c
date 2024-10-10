#include <stdio.h>
#include <stdlib.h>



char** textToStatements(char* input, int* retSize) {
    // Returns a list of statements
    // NOTE: Returned list must be freed
    int length = 0;

    int numStatements = 0;
    while (input[length]) {
        if (input[length] == ';') numStatements++;
        length++;
    }

    char** statements = malloc(numStatements * sizeof(char*));
    int countStatement = 0;

    for (int i = 0; i < length; i++) {
        int j = i;
        int lenStatement = 0;
        while (input[j] != ';') {
            lenStatement++;
            j++;
        }

        char* statement = malloc((j - i + 1) * sizeof(char));
        statement[j-i] = 0;
        for (int idx = i; idx < j; idx++) {
            statement[idx-i] = input[idx];
        }

        statements[countStatement] = statement;
        countStatement++;
        i += j - i;
    }

    *retSize = numStatements;
    return statements;
}


int main(int argc, char* argv[]) {
    printf("Rex - BareBones Interpreter\n");
    char* inputFile;

    if (argc == 1) {
        inputFile = "input.txt";
    } else if (argc == 2) {
        inputFile = argv[1];
    } else {
        printf("ERROR: Expected at most 1 input, got %d inputs\n", argc-1);
        return 1;
    }

    // FILE* file = fopen(inputFile, "r");
    char* file = "Hello; this is a test; with delimiters;";

    int numStatements;
    char** statements = textToStatements(file, &numStatements);

    for (int i = 0; i < numStatements; i++) {
        printf("%s\n", statements[i]);
    }
}
