#ifndef MATHPARSER_SY_H
#define MATHPARSER_SY_H

/* template for function matching record */
typedef struct _func_match_template
{
    int func_id;
    const char* func_name;
} func_match_template;

c_stack* sy_generate_rpn_stack(char *input, int *error, char** error_ptr);

#endif
