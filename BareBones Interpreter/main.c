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

char*** statementsToTokens(char** statements, int statementsSize, int** retSizes) {
    char*** tokenLists = malloc(statementsSize * sizeof(char**));
    for (int i = 0; i < statementsSize; i++) {

        char** tokenList = malloc(0);
        int numTokenLists = 0;
        int idx = 0;

        // Go to start of first token
        while (statements[i][idx] == ' ') {
            idx++;
        }

        while (statements[i][idx]) {

            numTokenLists++;
            tokenList = realloc(tokenList, numTokenLists * sizeof(char*));

            // Find length of token
            int start = idx;
            while (statements[i][idx] != ' ') {
                if (statements[i][idx] == 0) {
                    break;
                }
                idx++;
            }
            int length = idx - start;

            // Create token
            char* token = malloc((length + 1) * sizeof(char));
            for (int j = start; j < idx; j++) {
                token[j-start] = statements[i][j];
            }

            token[length] = 0;
            tokenList[numTokenLists-1] = token;
            idx++;

            // Go to start of next token
            while (statements[i][idx] == ' ') {
                idx++;
            }

        }

        (*retSizes)[i] = numTokenLists;
        tokenLists[i] = tokenList;
    }

    // Free up statements
    for (int i = 0; i < statementsSize; i++) {
        free(statements[i]);
    }

    free(statements);
    return tokenLists;
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

    int* numTokens = malloc(numStatements * sizeof(int));
    char*** tokens = statementsToTokens(statements, numStatements, &numTokens);

    printf("\n");

    for (int i = 0; i < numStatements; i++) {
        for (int j = 0; j < numTokens[i]; j++) {
            printf("%s\n", tokens[i][j]);
        }
        printf("\n");
    }
}
