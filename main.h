#ifndef MATHPARSER_MAIN_H
#define MATHPARSER_MAIN_H

/*
 * MACROS
 */

#define RPN_STACK_SIZE 64           /* implicit stack size for RPN evaluation */
#define SY_OP_STACK_SIZE 32         /* implicit stack size for Shunting-Yard evaluation */

#ifndef M_PI /* i.e. MSVS case */
#define M_PI 3.14159265 /* math PI constant with sufficient precision */
#endif

#ifndef INFINITY

#ifndef _HUGE_ENUF
#define _HUGE_ENUF  1e+300 /* define huge enough number to overflow */
#endif /* _HUGE_ENUF */

#define INFINITY   ((double)(_HUGE_ENUF * _HUGE_ENUF))  /* causes warning C4756: overflow in constant arithmetic (by design) */

#endif

/*
 * ENUMS
 */

enum operator_type
{
    OP_NONE = -1,                   /* error flag */
    OP_ADD,                         /*   +   */
    OP_SUBTRACT,                    /*   -   */
    OP_MULTIPLY,                    /*   *   */
    OP_DIVIDE,                      /*   /   */
    OP_EXP_RAISE,                   /*   ^   */
    /* not really operators, but sy parser deals with them like that */
    PARENTHESIS_LEFT,               /*   (   */
    PARENTHESIS_RIGHT               /*   )   */
};

#define PARENTHESIS_NONE OP_NONE    /* error flag to allow generic handling with disambiguation for programmer */

enum supported_functions
{
    FUNC_UNSUPPORTED = -1,          /* error flag */

    FUNC_ABS,                       /* absolute value */
    FUNC_EXP,                       /* exponential, e^x */

    FUNC_SIN,                       /* sinus */
    FUNC_COS,                       /* cosinus */
    FUNC_TAN,                       /* tangens */
    FUNC_COTAN,                     /* cotangens */

    FUNC_ASIN,                      /* arcus sinus */
    FUNC_ACOS,                      /* arcus cosinus */
    FUNC_ATAN,                      /* arcus tangens */
    FUNC_ACOTAN,                    /* arcus cotangens */

    FUNC_LOG10,                     /* logarhitm base 10 */
    FUNC_LN,                        /* logarhitm base e */

    FUNC_SINH,                      /* hyperbolic sinus */
    FUNC_COSH,                      /* hyperbolic cosinus */
    FUNC_TANH,                      /* hyperbolic tangens */

    FUNC_TODEG,                     /* convert radians to degrees */
    FUNC_TORAD                      /* convert degrees to radians */
};

enum syntax_error_code
{
    SYNTAX_ERROR_NONE = 0,                  /* no error, everything OK */
    SYNTAX_ERROR_MISSING_PARENTHESIS,       /* missing closing parenthesis */
    SYNTAX_ERROR_REAL_NOTATION,             /* real number is written badly */
    SYNTAX_ERROR_OPERATOR_FREQUENCY,        /* unrecognized operator chain - i.e. ++ */
    SYNTAX_ERROR_FUNCTION_PARENTHESIS,      /* opening parenthesis after function call not found */
    SYNTAX_ERROR_BINARY_OPERATOR_OPERANDS,  /* invalid parameters in binary operator relation */
    SYNTAX_ERROR_INVALID_CHARACTER,         /* invalid character in supplied expression */
    SYNTAX_ERROR_NOTHING_TO_PARSE,          /* user did not specify valid string to parse */
    SYNTAX_ERROR_UNEXPECTED_SYMBOL,         /* unexpected symbol in input - expected something else */
    GENERAL_MEMORY_ERROR,                   /* when something went wrong during memory allocation */
};

#endif
