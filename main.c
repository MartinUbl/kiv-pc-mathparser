#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "stack.h"
#include "rpn.h"
#include "main.h"
#include "shunting_yard.h"
#include "drawing.h"

#include "test.h"

/**
 * Function for parsing limits from input string
 * Returns NULL if not possible to parse
 */
static double* parse_limits(char* input)
{
    double* limits;

    limits = (double*)malloc(sizeof(double) * 4);
    if (sscanf(input, "%lf:%lf:%lf:%lf", &limits[0], &limits[1], &limits[2], &limits[3]) != 4)
    {
        printf("Invalid limits string supplied, falling back to default intervals\n");
        free(limits);
        return NULL;
    }

    return limits;
}

/**
 * Application entry point - main function
 */
int main(int argc, char **argv)
{
    char *input, *error_ptr;
    c_stack *parsed;
    int error;
    double* limits;

    /* "unit testing" */
    if (argc == 2 && strcmp(argv[1], "-test") == 0)
    {
        /* just return value from testing function */
        return test_evaluation();
    }

    /* verify argument count */
    if (argc < 3 || argc > 5)
    {
        printf("\nUsage: %s <func> <out-file> [<limits>]\n\n", argv[0]);
        printf("<func>      - expression representing math function\n");
        printf("<out-file>  - output PostScript file\n");
        printf("<limits>    - supplied limits in xmin:xmax:ymin:ymax format\n\n");
        printf("Or you can run test routine by typing: \n");
        printf("    %s -test\n\n", argv[0]);
        return 1;
    }

    /* function body is supplied as 1th parameter */
    input = argv[1];

    /* parse input to RPN form */
    parsed = sy_generate_rpn_stack(input, &error, &error_ptr);

    /* the parsing routine may return error */
    if (parsed == NULL || error != SYNTAX_ERROR_NONE)
    {
        printf("\n");

        switch (error)
        {
            case SYNTAX_ERROR_MISSING_PARENTHESIS:
                printf("Syntax error: missing parenthesis in supplied expression\n");
                break;
            case SYNTAX_ERROR_REAL_NOTATION:
                printf("Syntax error: wrong floating point value notation\n");
                break;
            case SYNTAX_ERROR_OPERATOR_FREQUENCY:
                printf("Syntax error: operator chain not recognized\n");
                break;
            case SYNTAX_ERROR_FUNCTION_PARENTHESIS:
                printf("Syntax error: function argument not enclosed in parenthesis\n");
                break;
            case SYNTAX_ERROR_BINARY_OPERATOR_OPERANDS:
                printf("Syntax error: missing binary operator operands\n");
                break;
            case SYNTAX_ERROR_INVALID_CHARACTER:
                printf("Syntax error: invalid character in supplied expression\n");
                break;
            case SYNTAX_ERROR_UNEXPECTED_SYMBOL:
                printf("Syntax error: unexpected symbol in supplied expression\n");
                break;
            case SYNTAX_ERROR_NOTHING_TO_PARSE:
                printf("Error: nothing to parse\n");
                break;
        }

        /* there we draw the "pointing" character ^ to error position, just like other parsers often have */
        if (error_ptr != NULL)
        {
            printf("\n  %s\n  ", input);
            /* print spaces until we hit the error position */
            while (input != error_ptr)
            {
                printf(" ");
                input++;
            }
            printf("^\n");
        }

        return 1;
    }

    limits = NULL;

    /* this means, the limits are supplied (or at least we assume that) */
    if (argc == 4)
        limits = parse_limits(argv[3]);

    /* limits weren't parsed successfully, or supplied at all */
    if (limits == NULL)
    {
        /* use implicit */
        limits = (double*)malloc(sizeof(double) * 4);
        limits[0] = -10.0;
        limits[1] = 10.0;
        limits[2] = -10.0;
        limits[3] = 10.0;
    }

    /* evaluate RPN stack and draw function to file*/
    drawing_process_output(input, argv[2], parsed, limits);

    /* cleanup */
    stck_destroy(parsed);
    free(limits);

    return 0;
}
