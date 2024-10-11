#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define DEBUG false



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
        if (countStatement == numStatements) break;
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
        while (statements[i][idx] == ' ' || statements[i][idx] == '\n' || statements[i][idx] == '\r') {
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


unsigned int hash(char* name) {
    int value = 1;
    int i = 0;
    while (name[i]) {
        value += name[i] * (i+1);
        i++;
    }

    return value;
}

bool strEqual(char* str1, char* str2) {
    int i = 0;
    while (str1[i] || str2[i]) {
        if (str1[i] != str2[i]) return false;
        i++;
    }

    return true;
}

typedef struct {
    unsigned int value;
    char* name;
} TableValue;

typedef struct {
    int size;
    int maxLength;
    TableValue* values;
} HashTable;

HashTable initTable(void) {
    HashTable table;
    table.size = 0;
    table.maxLength = 1;
    table.values = malloc(sizeof(TableValue));
    table.values[0].name = "";

    return table;
}

void setVariable(HashTable* table, char* name, unsigned int value) {
    unsigned int hashValue = hash(name);
    int idx = hashValue;

    idx %= table->maxLength;

    // Go through table until we find a name match
    // If we find an empty slot, add variable
    bool nameFound = true;
    while (!strEqual(table->values[idx].name, name)) {
        if (table->values[idx].name[0] == 0) {
            table->values[idx].name = name;
            nameFound = false;
            break;
        }
        idx++;
        if (idx == table->maxLength) idx = 0;
    }

    // Set the value of the variable
    table->values[idx].value = value;

    // If the table is too small, double the maxLength then copy the table contents to a new hashTable value array
    if (!nameFound) {
        table->size++;
        if (2 * (table->size) > table->maxLength) {
            table->maxLength *= 2;
            TableValue* values = malloc(table->maxLength * sizeof(TableValue));
            for (int i = 0; i < table->maxLength; i++) values[i].name = "";
            for (int i = 0; i < table->maxLength/2; i++) {
                if (table->values[i].name[0] == 0) continue;
                values[hash(table->values[i].name) % table->maxLength] = table->values[i];
            }

            TableValue* oldValues = table->values;
            table->values = values;
            free(oldValues);
        }
    }
}

unsigned int getVariable(HashTable* table, char* name) {
    int idx = hash(name);
    idx %= table->maxLength;

    while (!strEqual(table->values[idx].name, name)) {
        idx++;
        if (idx == table->maxLength) idx = 0;
    }

    return table->values[idx].value;
}

bool declaredVariable(HashTable* table, char* name) {
    int idx = hash(name);
    idx %= table->maxLength;

    while (table->values[idx].name[0]) {
        if (strEqual(table->values[idx].name, name)) return true;
        idx++;
        if (idx == table->maxLength) idx = 0;
    }

    return false;
}

typedef struct {
    int line;
    char* name;
} WhileValue;

typedef struct {
    int length;
    WhileValue* values;
} Stack;

Stack initStack(void) {
    Stack stack;
    stack.length = 0;
    stack.values = malloc(0);

    return stack;
}

void stackPush(Stack* stack, int line, char* name) {
    stack->length++;
    stack->values = realloc(stack->values, stack->length * sizeof(WhileValue));
    stack->values[stack->length-1].line = line;
    stack->values[stack->length-1].name = name;
}

void stackPop(Stack* stack) {
    stack->length--;
    stack->values = realloc(stack->values, stack->length * sizeof(WhileValue));
}

WhileValue stackGet(Stack* stack) {
    return stack->values[stack->length-1];
}

void displayVariables(HashTable* varTable) {
    int i;
    for (i = 0; i < varTable->maxLength; i++) {
        if (varTable->values[i].name[0]) {
            printf("%s %u", varTable->values[i].name, varTable->values[i].value);
            break;
        }
    }

    for (int j = i+1; j < varTable->maxLength; j++) {
        if (varTable->values[j].name[0]) {
            printf(" | %s %u", varTable->values[j].name, varTable->values[j].value);
        }
    }

    printf("\n");
}

void beginError(void) {
    printf("\033[1;31mERROR: ");
}

void endError(void) {
    printf("\033[0m\n");
}


int main(int argc, char* argv[]) {
    printf("Rex - BareBones Interpreter\n");
    char* inputFile;

    if (argc == 1) {
        inputFile = "input.txt";
    } else if (argc == 2) {
        inputFile = argv[1];
    } else {
        beginError();
        printf("ERROR: Expected at most 1 input, got %d inputs", argc-1);
        endError();
        return 1;
    }

    // Get code from file
    FILE* file = fopen(inputFile, "rb");
    fseek(file, 0, SEEK_END);

    long size = ftell(file);
    rewind(file);

    char text[size+1];
    fread(text, size, 1, file);
    fclose(file);
    text[size] = 0;

    printf("----------\n");
    if (DEBUG) {
        printf("%s\n", text);
        printf("----------\n");
    }

    // Generate an array of statements
    int numStatements;
    char** statements = textToStatements(text, &numStatements);

    if (DEBUG) {
        for (int i = 0; i < numStatements; i++) {
            printf("%s\n", statements[i]);
        }
    }

    // Generate an array of token lists
    int* numTokens = malloc(numStatements * sizeof(int));
    char*** tokenLists = statementsToTokens(statements, numStatements, &numTokens);

    if (DEBUG) {
        printf("----------\n");

        for (int i = 0; i < numStatements; i++) {
            for (int j = 0; j < numTokens[i]; j++) {
                printf("%s\n", tokenLists[i][j]);
            }
            printf("\n");
        }

        printf("----------\n");
    }

    // Create the variables table
    HashTable varTable = initTable();

    // Create a stack for while loops variable
    Stack whileStack = initStack();

    // Go through tokens and execute
    for (int stIdx = 0; stIdx < numStatements; stIdx++) {

        char** tokenList = tokenLists[stIdx];

        if (strEqual(tokenList[0], "clear")) {
            setVariable(&varTable, tokenList[1], 0);
        }

        else if (strEqual(tokenList[0], "incr")) {
            if (!declaredVariable(&varTable, tokenList[1])) {
                // printf("\033[1;31mERROR: variable %s has not been declared\033[0m\n", tokenList[1]);
                beginError();
                printf("Variable %s has not been declared", tokenList[1]);
                endError();
                break;
            }
            setVariable(&varTable, tokenList[1], getVariable(&varTable, tokenList[1]) + 1);
        }

        else if (strEqual(tokenList[0], "decr")) {
            if (!declaredVariable(&varTable, tokenList[1])) {
                beginError();
                printf("Variable %s has not been declared", tokenList[1]);
                endError();
                break;
            }
            setVariable(&varTable, tokenList[1], getVariable(&varTable, tokenList[1]) - 1);
        }

        else if (strEqual(tokenList[0], "while")) {
            if (!declaredVariable(&varTable, tokenList[1])) {
                setVariable(&varTable, tokenList[1], 0);
            }
            stackPush(&whileStack, stIdx, tokenList[1]);
        }

        else if (strEqual(tokenList[0], "end")) {
            if (getVariable(&varTable, stackGet(&whileStack).name) == 0) {
                stackPop(&whileStack);
            } else {
                stIdx = stackGet(&whileStack).line;
            }
        }

        else {
            beginError();
            printf("Command %s is not valid", tokenList[0]);
            endError();
            break;
        }

        printf("Statement %d: ", stIdx);
        displayVariables(&varTable);
    }
    printf("----------\n");
    printf("Interpreter finished.\n");
}
