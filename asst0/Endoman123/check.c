#include <stdlib.h>
#include <stdio.h>

typedef struct _Error { 
    int expressionNum;
    char* type;
    char* description;
    char* fragment;
    struct _Error* next;
} Error;

typedef enum _ExprType {
    UNKNOWN,
    ARITHMETIC,
    LOGIC
} ExpressionType;

Error* errorList = 0;

// Main pipeline functions
int tokenize(char*, char, char**);
ExpressionType evaluate(char*, int exprNum);

// Helper functions
int isDigit(char*);
int isBoolean(char*);
int isArithmeticOperator(char*);
int isLogicOperator(char*);
int matchesString(char*, char*);
char* copy(char*);
int throwError(int, char*, char*, char*);

int main(int argc, char** argv) {
    // Get expressions
    char** expressions = malloc(99 * sizeof(char*));
    int numExpressions = tokenize(argv[1], ';', expressions), 
        i = 0,
        numArith = 0,
        numLogic = 0; 
    
    // Evaluate each expression
    for (; i < numExpressions; i++) {
        ExpressionType curType = evaluate(expressions[i], i + 1);

        if (curType == ARITHMETIC)
           ++numArith;
        else if (curType == LOGIC)
           ++numLogic; 
    
        free(expressions[i]);
    } 
     
    printf("Found %d expressions: %d arithmetic, %d logical\n", numExpressions, numArith, numLogic); 
    
    if (errorList == 0)
        printf("OK\n");
    else {
        while (errorList != 0) { // Print and free errors
            Error* toRemove = errorList;
            printf(
                    "Error: %s in expression %d: %s:\n\t\"%s\"\n", 
                    errorList->type, 
                    errorList->expressionNum, 
                    errorList->description,
                    errorList->fragment
            );
            errorList = errorList->next;

            free(toRemove->description);
            free(toRemove->fragment);
            free(toRemove);
        }
    }

    return 0;
}

/**
 * Separates a string into tokens. Returns the number of tokens in the array
 *
 * NOTES:
 * - Function assumes that tokenArray is already initialized
 * - Be sure to free the array once finished using. 
 */
int tokenize(char* string, char delim, char** tokenArray) {
    // Begin with allocating all resources needed to hold tokens
    char* ptr = string; 
    int curToken = 0, curChar = 0;

    *tokenArray = malloc(10 * sizeof(char));
    // Loop through;
    // Use the temp pointer to point to each character in the string.
    for (; *ptr != '\0'; ptr++) {
        if (*ptr != delim) {
            tokenArray[curToken][curChar] = *ptr; 
            ++curChar;
        } else {
            tokenArray[curToken][curChar] = '\0';
            ++curToken;
            curChar = 0;
            tokenArray[curToken] = malloc(10 * sizeof(char));
        }
    }
    
    // Append null char to last token
    tokenArray[curToken][curChar] = '\0';

    return curToken + 1;
}

/**
 * Break expression into tokens and check for errors
 *
 * Format should go:
 * "<operand> <operator> <operand>" (Most expressions)
 * or
 * "NOT <logic operand>" (NOT operator)
 * 
 * Expressions should be separated by a ";" and a space
 */
ExpressionType evaluate(char* expression, int exprNum) {
    ExpressionType type = UNKNOWN, subtype = UNKNOWN; // Subtype only needed for an unexpected end

    // Get some errors out of the way
    if (exprNum > 1) {
        if (*expression != ' ')
            throwError(exprNum, "Scan error", "Expected space", expression);
        else
            ++expression;
    }

    // If the string is empty, just return.
    // Don't even both trying to do tokenization, evaluation, etc.
    if (*expression == '\0') {
        throwError(exprNum, "Scan error", "Missing expression", expression);
        return type;
    }

    // Start by breaking up expression into tokens
    char** tokens = malloc(99 * sizeof(char*));
    int numTokens = tokenize(expression, ' ', tokens), i = 0, maxTokens = 3, state = 0;
    
    // Using a state machine,
    // parse the tokens and check if all tokens are valid.
    // STATES:
    // 0) Look for a valid identifier
    // 1) Look for an operator
    // 2) Look for an operand
    // 3) Expression not ended, fall into next state
    // 4) Subexpression: valid identifier
    // 5) Subexpression: valid operator
    // 6) Subexpression: valid operand
    for (; i < numTokens; ++i) {
        char* curToken = tokens[i];

        switch (state) {
            case 0: // Find a valid identifier
                if (isDigit(curToken)) {
                    type = ARITHMETIC;
                    state = 1;
                } else if (isBoolean(curToken)) {
                    type = LOGIC;
                    state = 1;
                } else if (isLogicOperator(curToken)) {
                    type = LOGIC; 
                    state = 2;

                    if (!matchesString(curToken, "NOT"))
                        throwError(exprNum, "Parse error", "Unexpected operator", curToken);
                } else if (isArithmeticOperator(curToken)) {
                    type = ARITHMETIC;
                    throwError(exprNum, "Parse error", "Unexpected operator", curToken);
                    state = 2;
                } else 
                    throwError(exprNum, "Parse error", "Unknown identifier", curToken);
                break;
            case 1: // Find an operator for the expression type
                if (isLogicOperator(curToken)) {
                    if (type != LOGIC)
                        throwError(exprNum, "Operand mismatch", "Expected logic, found arithmetic", expression);

                    if (matchesString(curToken, "NOT"))
                        throwError(exprNum, "Parse error", "Unexpected operator", curToken);
                    
                    type = LOGIC;
                } else if (isArithmeticOperator(curToken)) {
                    if (type != ARITHMETIC)
                        throwError(exprNum, "Operand mismatch", "Expected arithmetic, found logic", expression);

                    type = ARITHMETIC;
                } else if (isDigit(curToken) || isBoolean(curToken))
                    throwError(exprNum, "Parse error", "Unexpected operand", curToken);
                else
                    throwError(exprNum, "Parse error", "Unknown operator", curToken);

                state = 2; // You will always move to state 2 after this
                break;
            case 2: // Find an operand for the expression type
                if (isBoolean(curToken) && type == ARITHMETIC)
                    throwError(exprNum, "Operand mismatch", "Expected arithmetic, found logic", expression);
                else if (isDigit(curToken) && type == LOGIC) 
                    throwError(exprNum, "Operand mismatch", "Expected logic, found arithmetic", expression);
                else if (isArithmeticOperator(curToken) || isLogicOperator(curToken)) {
                    throwError(exprNum, "Parse error", "Unexpected operator", curToken);

                    if (isArithmeticOperator(curToken))
                        subtype = ARITHMETIC;
                    else if (isLogicOperator(curToken))
                        subtype = LOGIC;
                } else if (!isBoolean(curToken) && !isDigit(curToken))
                    throwError(exprNum, "Parse error", "Unknown operand", curToken);

                 state = 3; // You will always move to state 3 after this
                 break;
            case 3: // If we are here, the expression has not ended for some reason.
                throwError(exprNum, "Parse error", "Expression not ended", expression);

                // Decide on where we are in the parsing process before continuing
                // We're either in state 4, figuring out the first identifier for the exprssion,
                // or in state 6, finding out if the operand even matches the expected subtype
                if (subtype == UNKNOWN)
                    state = 4;
                else
                    state = 6;
                
                // Repeat the state
                --i;
                continue;
                break;
            case 4: // Subexpression: Find a valid identifier
                if (isDigit(curToken)) {
                    throwError(exprNum, "Parse error", "Unexpected operand", curToken);
                    subtype = ARITHMETIC;
                    state = 5;
                } else if (isBoolean(curToken)) {
                    throwError(exprNum, "Parse error", "Unexpected operand", curToken);
                    subtype = LOGIC;
                    state = 5;
                } else if (isLogicOperator(curToken)) {
                    throwError(exprNum, "Parse error", "Unexpected operator", curToken);
                    subtype = LOGIC; 
                    state = 6;
                } else if (isArithmeticOperator(curToken)) {
                    throwError(exprNum, "Parse error", "Unexpected operator", curToken);
                    subtype = ARITHMETIC;
                    state = 6;
                } else 
                    throwError(exprNum, "Parse error", "Unexpected identifier", curToken);
                break;
            case 5: // Subexpression: Find an operator
                if (isLogicOperator(curToken)) {
                    if (subtype != LOGIC)
                        throwError(exprNum, "Operand mismatch", "Expected logic, found arithmetic", expression);

                    if (matchesString(curToken, "NOT"))
                        throwError(exprNum, "Parse error", "Unexpected operator", curToken);
                    
                    subtype = LOGIC;
                } else if (isArithmeticOperator(curToken)) {
                    if (subtype != ARITHMETIC)
                        throwError(exprNum, "Operand mismatch", "Expected arithmetic, found logic", expression);

                    subtype = ARITHMETIC;
                } else if (isDigit(curToken) || isBoolean(curToken))
                    throwError(exprNum, "Parse error", "Unexpected operand", curToken);
                else
                    throwError(exprNum, "Parse error", "Unknown operator", curToken);

                state = 6; // You will always move to state 6 after this
                break;
            case 6: // Subexpression: Find valid operand
                if (isArithmeticOperator(curToken) || isLogicOperator(curToken)) {
                    throwError(exprNum, "Parse error", "Unexpected operator", curToken);

                    if (isArithmeticOperator(curToken))
                        subtype = ARITHMETIC;
                    else if (isLogicOperator(curToken))
                        subtype = LOGIC;
                } else {
                    subtype = UNKNOWN;

                    if (isBoolean(curToken) && subtype == ARITHMETIC)
                        throwError(exprNum, "Operand mismatch", "Expected arithmetic, found logic", expression);
                    else if (isDigit(curToken) && subtype == LOGIC) 
                        throwError(exprNum, "Operand mismatch", "Expected logic, found arithmetic", expression);
                    else if (!isBoolean(curToken) && !isDigit(curToken))
                        throwError(exprNum, "Parse error", "Unknown operand", curToken);
                }
                
                state = 4; // Move back to state 4
                break;
        }

        free(curToken);
    }

    // If we have not gone through to state 3, the statement is incomplete.
    if (state < 3)
        throwError(exprNum, "Scan error", "Incomplete expression", expression);

    return type;
}

/**
 * Adds an error to the error list
 */
int throwError(int expressionNum, char* type, char* description, char* fragment) {
    Error* newError = malloc(sizeof(Error)); 
    Error* curError = errorList;

    newError->expressionNum = expressionNum;
    newError->type = copy(type);
    newError->description = copy(description);
    newError->fragment = copy(fragment);
    
    if (curError == 0)
        errorList = newError;
    else {
        while (curError->next != 0)
            curError = curError->next;

        curError->next = newError;
    }
    return 0;
}

/**
 * Checks if the passed character is a digit.
 *
 * This is done by getting the difference between the character code and 48,
 * the character code for "0."
 */
int isDigit(char* c) {
    char* curChar = c;
    
    if (*curChar == '\0') // Empty string
        return 0;

    for (; *curChar != '\0'; ++curChar) {
        int diff = *curChar - 48;

        if (diff < 0 || diff > 9)
            return 0;
    }
    
    return 1;
}

/**
 * Gets if the passed string is a boolean operand;
 * that is: true or false (case-sensitive)
 */
int isBoolean(char* c) {
    return matchesString(c, "true") || matchesString(c, "false");
}

/**
 * Checks if the passed string is an arithmetic operator;
 * that is: +, -, /, *.
 */
int isArithmeticOperator(char* c) {
    char* operators = "+-*/";
    int i = 0;

    if (c[1] == '\0') {
        for (; i < 4; i++) {
            if (c[0] == operators[i])
                return 1;
        }
    }

    return 0;
}

/**
 * Checks if the passed string is a logic operator;
 * that is: AND, OR, or NOT (case-sensitive)
 */
int isLogicOperator(char* c) {
    return matchesString(c, "AND") || matchesString(c, "OR") || matchesString(c, "NOT");
}

/**
 * Checks if the strings match.
 */
int matchesString(char* stringA, char* stringB) {
    int i = 0;

    for (; stringA[i] != '\0' || stringB[i] != '\0'; i++) {
        if (stringA[i] != stringB[i])
            return 0;
    }

    return 1;
}

/**
 * Copies a string to a new memory address 
 */
char* copy(char* string) {
    char* ret = malloc(99 * sizeof(char));
    
    int i = 0;
    do {
        ret[i] = string[i];
        ++i;
    } while (string[i] != '\0');

    return ret;
}
