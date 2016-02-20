#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "stack.h"
#include "rpn.h"
#include "main.h"
#include "shunting_yard.h"
#include "test.h"

/* structure for storing test case */
typedef struct
{
    const char* expression;
    double expected_result;
    int error;
} test_case;

/* static array of test cases */
static test_case cases[] = {

    /* success tests */
    /* all results from more complex expressions confirmed using WolframAlpha (http://www.wolframalpha.com) */

    /* expression,            expected result,  error */
    { "2+2",                        4.0,        0 },
    { "5-2",                        3.0,        0 },
    { "125-85",                     40.0,       0 },
    { "12*8",                       96.0,       0 },
    { "6*2/4",                      3.0,        0 },
    { "6/2*4",                      12.0,       0 },
    { "2^3",                        8.0,        0 },
    { "2^3^2",                      512.0,      0 },
    { "(2^3)^2",                    64.0,       0 },
    { "2^(3^2)",                    512.0,      0 },
    { "-5+2",                       -3.0,       0 },
    { "-5-2",                       -7.0,       0 },
    { "2*sin(1.92)",                1.879291,   0 },
    { "-2*sin(1.92)",               -1.879291,  0 },
    { "(x*sin(x)+x)",               2.996242,   0 },
    { "2^-3",                       0.125,      0 },
    { "25*(225-52",                 0.0,        1 },
    { "3*5-2*(8-3)+7-6/2",          9.0,        0 },
    { "(3*5-2*(8-3)+7-6/2)/3",      3.0,        0 },
    { "2*abs(x-3)",                 3.0,        0 },
    { "5+-4",                       1.0,        0 },
    { "2*2*2*2*2*2",                64.0,       0 },
    { "2+2*6+4",                    18.0,       0 },
    { "sin(cos(tan(abs(-5))))",     -0.825786,  0 },
    { "sin(2-cos(tan(2+5)-3))",     0.574759,   0 },
    { "-cos(-sin(2*2))",            -0.727035,  0 },
    { "asin(0.5)",                  0.523599,   0 },
    { "todeg(asin(0.5))",           30.0,       0 },
    { "torad(180.0)",               3.1415927,  0 },
    { "x-x",                        0.0,        0 },
    { "2^0",                        1.0,        0 },
    { "2/0",                        INFINITY,   0 }, /* for needs of drawing plot the "infinity" value is sufficient indicator */
    { "1/x",                        0.666667,   0 },
    { "0^0",                        1.0,        0 },
    { "5*(3*(2*(1*(-5))))",         -150.0,     0 },
    { "-(2+5)",                     -7.0,       0 },

    /* error tests */
    { "-",          0.0, 5 },
    { "*",          0.0, 5 },
    { "2*",         0.0, 5 },
    { "*2",         0.0, 5 },
    { "^10",        0.0, 5 },
    { "!?",         0.0, 6 },
    { "5++4",       0.0, 3 },
    { "5--4",       0.0, 3 },
    { "--5--4",     0.0, 3 },
    { "sin5",       0.0, 4 },
    { "sin(5",      0.0, 1 },
    { "",           0.0, 7 },
    { "(sin)",      0.0, 4 },
    { "()",         0.0, 0 }, /* may be considered error, but empty value is also value, assuming 0 */
};

/* evaluation test function - goes through all test cases and verifies their output / error */
int test_evaluation(void)
{
    int i;
    int size;
    c_stack *tmp;
    int error;
    char *error_ptr, *expr_cpy;
    int fail, success, failed;
    double res;

    /* counters */
    success = 0;
    failed = 0;
    error = 0;
    error_ptr = NULL;
    tmp = NULL;

    /* go through every case */
    size = (int) (sizeof(cases) / sizeof(test_case));
    for (i = 0; i < size; i++)
    {
        fail = 0;

        /* for not passing const parameter to function retaining non-const, copy it */
        expr_cpy = (char*)malloc(sizeof(char)*strlen(cases[i].expression)+1);
        strcpy(expr_cpy, cases[i].expression);

        printf("Expression: %s\n", cases[i].expression);
        /* generate stack */
        tmp = sy_generate_rpn_stack(expr_cpy, &error, &error_ptr);

        /* verify expected error if set */
        if (cases[i].error > 0 || (cases[i].error == 0 && error != 0))
        {
            printf("Error:      %i (expected %i)\n", error, cases[i].error);
            if (error != cases[i].error)
                fail = 1;
        }
        /* if no error, evaluate stack */
        if (error == 0)
        {
            res = rpn_evaluate_stack(tmp, TEST_CASE_VARIABLE_VAL);
            printf("Result:     %f (expected %f)\n", res, cases[i].expected_result);
            /* verify result */
            if (fabs(res - cases[i].expected_result) > COMPARISON_EPSILON)
                fail = 1;
        }

        /* increase appropriate counter */
        if (fail == 0)
        {
            printf("OK\n\n");
            success++;
        }
        else
        {
            printf("FAILED\n\n");
            failed++;
        }

        if (tmp)
            stck_destroy(tmp);

        free(expr_cpy);
    }

    printf("Done.\nSuccess: %i\nFailed: %i\n\n", success, failed);

    return (failed == 0) ? 0 : 1;
}
