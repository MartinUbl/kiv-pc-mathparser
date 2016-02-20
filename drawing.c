#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>
#include "main.h"
#include "stack.h"
#include "rpn.h"
#include "postscript.h"
#include "drawing.h"

/**
 * Custom function to determine if the supplied value has NaN "value" (specific floating point
 * numeric value)
 */
static int isnan_d(double val)
{
    return (val != val) ? 1 : 0;
}

/**
 * Custom function to determine if supplied value is infinity (positive or negative)
 * The INFINITY macro is not defined by default on some platforms, the redefinition
 * may cause warning saying, that floating values multiplication flew over. That's
 * completelly fine, we want it to overflow to infinity
 */
static int isinf_d(double val)
{
    return (val == INFINITY || val == -INFINITY) ? 1 : 0;
}

/**
 * Decides helper line distance depending on input min-max distance
 */
static double decide_line_step(double inp)
{
    /*
     * purely guessed values to make it look fine
     * - this should guarantee at least 5 helper lines on each plot
     */

    if (inp < 1.0)
        return 0.1;
    else if (inp < 5.0)
        return 0.5;
    else if (inp < 10.0)
        return 1.0;
    else if (inp < 20.0)
        return 2.0;
    else if (inp < 50.0)
        return 5.0;
    else if (inp < 100.0)
        return 10.0;

    return 25.0;
}

/**
 * Prepares output document for writing; writes header also
 */
static ps_document* drawing_prepare_output(char* filename)
{
    ps_document* output;
    time_t now;
    struct tm *gmt;
    char* formatted_time;

    /* creates postscript document structure */
    output = ps_create_document(filename);
    if (output == NULL)
        return NULL;

    /* format time */
    now = time(NULL);
    gmt = gmtime(&now);
    formatted_time = asctime(gmt);

    /* and write header */
    ps_write_header(output, "Martin Ubl", "Matematicka funkce", formatted_time);

    return output;
}

/**
 * Function drawing helper lines - mainly the frame of plot, and also helper "gray" lines by uniform distances.
 * Also draws significant points coordinate lines and their values (minimum and maximum)
 */
static void drawing_draw_helper_lines(ps_document* output, ps_pen* pen, double* limits, double* eval_values, double step_coef, double val_coef, int fmin, int fmax, double val_step)
{
    double plot_step, plot_x;
    int valcount;

    /* the border is drawn with doubled thickness */
    ps_set_line_width(output, 2);

    /* draw frame */
    ps_pen_down(pen);
    ps_pen_draw(pen, DRAW_X_END, DRAW_Y_BEGIN);
    ps_pen_draw(pen, DRAW_X_END, DRAW_Y_BEGIN + (limits[3] - limits[2])*val_coef);
    ps_pen_draw(pen, DRAW_X_BEGIN, DRAW_Y_BEGIN + (limits[3] - limits[2])*val_coef);
    ps_pen_draw(pen, DRAW_X_BEGIN, DRAW_Y_BEGIN);
    ps_pen_up(pen);

    /* set implicit font for labels */
    ps_set_font(output, DRAW_LABEL_FONT_FAMILY, DRAW_LABEL_FONT_SIZE);

    /* switch thickness back to 1 to draw helper "gray" lines */
    ps_set_line_width(output, 1);

    /* decide stepping value */
    plot_step = decide_line_step(limits[1] - limits[0]);
    plot_x = limits[0] + plot_step;
    valcount = (int)(1.0 / PLOT_STEP_COEF) + 1;

    /* and draw vertical lines */
    ps_set_position(output, DRAW_X_BEGIN - 10, DRAW_Y_BEGIN - 20);
    ps_print_text_val(output, limits[0], PARAMETER_FORMATTER);
    while (plot_x < limits[1])
    {
        if (fabs(plot_x) < 0.05)
        {
            plot_x += plot_step;
            continue;
        }

        ps_set_color(output, 0.7, 0.7, 0.7);

        /* draw line */
        ps_pen_move(pen, DRAW_X_BEGIN + (plot_x - limits[0]) * step_coef, DRAW_Y_BEGIN);
        ps_pen_down(pen);
        ps_pen_draw(pen, DRAW_X_BEGIN + (plot_x - limits[0]) * step_coef, DRAW_Y_END);
        ps_pen_up(pen);

        ps_set_color(output, 0, 0, 0);

        /* draw label */
        ps_set_position(output, DRAW_X_BEGIN + (plot_x - limits[0]) * step_coef - 10, DRAW_Y_BEGIN - 20);
        ps_print_text_val(output, plot_x, PARAMETER_FORMATTER);

        plot_x += plot_step;
    }
    ps_set_position(output, DRAW_X_END - 10, DRAW_Y_BEGIN - 20);
    ps_print_text_val(output, limits[1], PARAMETER_FORMATTER);

    /* draw Y axis, if in viewport */
    if (limits[0] <= 0 && limits[1] >= 0)
    {
        ps_set_line_width(output, 2);
        ps_set_color(output, 1, 0.4, 0.4);

        /* draw line */
        ps_pen_move(pen, DRAW_X_BEGIN + (0 - limits[0]) * step_coef, DRAW_Y_BEGIN);
        ps_pen_down(pen);
        ps_pen_draw(pen, DRAW_X_BEGIN + (0 - limits[0]) * step_coef, DRAW_Y_END);
        ps_pen_up(pen);

        /* draw label */
        ps_set_position(output, DRAW_X_BEGIN + (0 - limits[0]) * step_coef - 10, DRAW_Y_BEGIN - 20);
        ps_print_text_val(output, 0, PARAMETER_FORMATTER);

        ps_set_line_width(output, 1);
    }

    /* and now draw vertical lines for minimum/maximum if needed and visible */

    /* the formulas used for conditions just verifies, if the line would not lay somewhere
       near the edge, and therefore be useless */

    ps_set_color(output, 0.7, 0.7, 0.7);

    /* minimum value line */
    if (fmin > 5 && fmin < valcount - 5)
    {
        ps_pen_move(pen, DRAW_X_BEGIN + fmin*val_step*step_coef, DRAW_Y_BEGIN);
        ps_pen_down(pen);
        ps_pen_draw(pen, DRAW_X_BEGIN + fmin*val_step*step_coef, DRAW_Y_END);
        ps_pen_up(pen);
    }

    /* maximum value line */
    if (fmax > 5 && fmin < valcount - 5)
    {
        ps_pen_move(pen, DRAW_X_BEGIN + fmax*val_step*step_coef, DRAW_Y_BEGIN);
        ps_pen_down(pen);
        ps_pen_draw(pen, DRAW_X_BEGIN + fmax*val_step*step_coef, DRAW_Y_END);
        ps_pen_up(pen);
    }

    ps_set_color(output, 0, 0, 0);

    /* minimum value label */
    if (fmin > 5 && fmin < valcount - 5)
    {
        ps_set_position(output, DRAW_X_BEGIN + fmin*val_step*step_coef - 10, DRAW_Y_BEGIN - 10);
        ps_print_text_val(output, limits[0] + fmin*val_step, PARAMETER_FORMATTER);
    }
    /* maximum value label */
    if (fmax > 5 && fmin < valcount - 5)
    {
        ps_set_position(output, DRAW_X_BEGIN + fmax*val_step*step_coef - 10, DRAW_Y_BEGIN - 10);
        ps_print_text_val(output, limits[0] + fmax*val_step, PARAMETER_FORMATTER);
    }


    /* repeat similar process to horizontal lines */


    /* decide stepping value */
    plot_step = decide_line_step(limits[3] - limits[2]);
    plot_x = limits[2] + plot_step;

    /* draw horizontal lines */
    ps_set_position(output, DRAW_X_BEGIN - 23, DRAW_Y_BEGIN - 6);
    ps_print_text_val(output, limits[2], PARAMETER_FORMATTER);
    while (plot_x < limits[3])
    {
        if (fabs(plot_x) < 0.05)
        {
            plot_x += plot_step;
            continue;
        }

        ps_set_color(output, 0.7, 0.7, 0.7);

        /* draw lines */
        ps_pen_move(pen, DRAW_X_BEGIN, DRAW_Y_BEGIN + (plot_x - limits[2])*val_coef);
        ps_pen_down(pen);
        ps_pen_draw(pen, DRAW_X_END, DRAW_Y_BEGIN + (plot_x - limits[2])*val_coef);
        ps_pen_up(pen);

        ps_set_color(output, 0, 0, 0);

        /* draw labels */
        ps_set_position(output, DRAW_X_BEGIN - 23, DRAW_Y_BEGIN + (plot_x - limits[2])*val_coef - 6);
        ps_print_text_val(output, plot_x, PARAMETER_FORMATTER);

        plot_x += plot_step;
    }
    ps_set_position(output, DRAW_X_BEGIN - 23, DRAW_Y_END - 6);
    ps_print_text_val(output, limits[3], PARAMETER_FORMATTER);

    /* draw X axis, if in viewport */
    if (limits[2] <= 0 && limits[3] >= 0)
    {
        ps_set_line_width(output, 2);
        ps_set_color(output, 1, 0.4, 0.4);

        /* draw lines */
        ps_pen_move(pen, DRAW_X_BEGIN, DRAW_Y_BEGIN + (0 - limits[2])*val_coef);
        ps_pen_down(pen);
        ps_pen_draw(pen, DRAW_X_END, DRAW_Y_BEGIN + (0 - limits[2])*val_coef);
        ps_pen_up(pen);

        /* draw labels */
        ps_set_position(output, DRAW_X_BEGIN - 23, DRAW_Y_BEGIN + (0 - limits[2])*val_coef - 6);
        ps_print_text_val(output, 0, PARAMETER_FORMATTER);

        ps_set_line_width(output, 1);
    }

    /* and now draw horizontal lines for minimum/maximum if needed and visible */

    /* the formulas used for conditions just verifies, if the line would not lay somewhere
       near the edge, and therefore be useless */

    ps_set_color(output, 0.7, 0.7, 0.7);

    /* minimal line */
    if (limits[2] - eval_values[fmin] < -0.05 && fabs(eval_values[fmin]) > 0.05)
    {
        ps_pen_move(pen, DRAW_X_BEGIN, DRAW_Y_BEGIN + (eval_values[fmin] - limits[2])*val_coef);
        ps_pen_down(pen);
        ps_pen_draw(pen, DRAW_X_END, DRAW_Y_BEGIN + (eval_values[fmin] - limits[2])*val_coef);
        ps_pen_up(pen);
    }

    /* maximal line */
    if (limits[3] - eval_values[fmax] > 0.05 && fabs(eval_values[fmax]) > 0.05)
    {
        ps_pen_move(pen, DRAW_X_BEGIN, DRAW_Y_BEGIN + (eval_values[fmax] - limits[2])*val_coef);
        ps_pen_down(pen);
        ps_pen_draw(pen, DRAW_X_END, DRAW_Y_BEGIN + (eval_values[fmax] - limits[2])*val_coef);
        ps_pen_up(pen);
    }

    ps_set_color(output, 0, 0, 0);

    /* minimal label */
    if (limits[2] - eval_values[fmin] < -0.05 && fabs(eval_values[fmin]) > 0.05)
    {
        ps_set_position(output, DRAW_X_BEGIN - 23, DRAW_Y_BEGIN + (eval_values[fmin] - limits[2])*val_coef - 6);
        ps_print_text_val(output, eval_values[fmin], PARAMETER_FORMATTER);
    }

    /* maximal label */
    if (limits[3] - eval_values[fmax] > 0.05 && fabs(eval_values[fmax]) > 0.05)
    {
        ps_set_position(output, DRAW_X_BEGIN - 23, DRAW_Y_BEGIN + (eval_values[fmax] - limits[2])*val_coef - 6);
        ps_print_text_val(output, eval_values[fmax], PARAMETER_FORMATTER);
    }
}

/**
 * Draws function created from parsed formula using supplied limits and output filename
 */
void drawing_process_output(char* expr, char* output_file, c_stack* rpn_stack, double* limits)
{
    ps_document* output;
    ps_pen* pen;
    double val, val_step, plot_x, step_coef, val_coef, stored_x, stored_y, stored_dydx;
    int penup, valcount, i, stored;
    double* eval_values;
    int fmax, fmin;
    char* outexpr;

    /* decide some essential values, just like number of values to be evaluated, step sizes and so */
    val_step = PLOT_STEP_COEF * (limits[1] - limits[0]);
    valcount = (int)(1.0/PLOT_STEP_COEF)+1;
    eval_values = (double*)malloc(valcount*sizeof(double));

    /* evaluate function values and store them to one big array, to reuse them later */
    i = 0;
    fmax = 0;
    fmin = 0;

    for (plot_x = limits[0]; i < valcount; plot_x += val_step)
    {
        eval_values[i] = rpn_evaluate_stack(rpn_stack, plot_x);

        /* store maximum/minimum as index in value array */
        if (eval_values[i] > eval_values[fmax] && eval_values[i] <= limits[3])
            fmax = i;
        else if (eval_values[i] < eval_values[fmin] && eval_values[i] >= limits[2])
            fmin = i;

        i++;
    }

    /* prepare drawing */
    output = drawing_prepare_output(output_file);
    if (output == NULL)
    {
        printf("Unable to create output file %s\n", output_file);
        return;
    }
    pen = ps_create_pen(output, DRAW_X_BEGIN, DRAW_Y_BEGIN);
    if (pen == NULL)
    {
        printf("Unable to allocate pen structure, drawing is not possible\n");
        return;
    }

    /* coefficients to make plot equal sized on both sides (square) */
    step_coef = (DRAW_X_END - DRAW_X_BEGIN) / (limits[1] - limits[0]);
    val_coef = (DRAW_Y_END - DRAW_Y_BEGIN) / (limits[3] - limits[2]);

    /* draw helper lines, border, etc. */
    drawing_draw_helper_lines(output, pen, limits, eval_values, step_coef, val_coef, fmin, fmax, val_step);

    /* print out f(x) = EXPR */
    outexpr = (char*)malloc(sizeof(char) * (strlen(expr) + 8));
    /**
     * WARNING: there is a line, which would Splint consider an error
     *          but it does not see, how much bytes i allocate, and that
     *          it will always fit in there
     **/
    sprintf(outexpr, "f(x) = %s", expr);
    ps_set_position(output, DRAW_X_BEGIN, DRAW_Y_END + 12);
    ps_print_text(output, outexpr);
    free(outexpr);

    /* the plot itself would be drawn using doubled thickness */
    ps_set_line_width(output, 2);

    /* move to first point to be drawn (has to be valid and in range) */
    i = 0;
    plot_x = eval_values[0];
    while (plot_x > limits[3] || plot_x < limits[2] || isnan_d(plot_x) || isinf_d(plot_x))
        plot_x = eval_values[++i];

    ps_pen_move(pen, DRAW_X_BEGIN + i*val_step*step_coef, DRAW_Y_BEGIN + (plot_x - limits[2])*val_coef);
    ps_pen_down(pen);
    penup = 0;

    /* blue! */
    ps_set_color(output, 0, 0, 1.0);

    if (i < 1)
        i++;
    else if (i > 1)
        i--;

    stored = 0;

    /* go through all points and draw lines */
    for (plot_x = limits[0] + val_step*i; i < valcount; plot_x += val_step, i++)
    {
        val = eval_values[i];

        if (stored == 0)
        {
            /* we will give it 2 chances to *not* change derivative value */
            stored = 2;
            stored_x = plot_x;
            stored_y = val;
            stored_dydx = fabs(eval_values[i] - eval_values[i-1]) / (val_step);
        }

        /* if the value "dropped out" of value range specified, just pick up the pen */
        if (val > limits[3] || val < limits[2] || isnan_d(val) || isinf_d(val))
        {
            if (penup != 1)
            {
                /* this code guarantees, that lines running too fast out of plot (lots of regular functions)
                   will not be cut somewhere far from the edge, but they will be drawn till the edge */
                if (!isnan_d(val) && !isinf_d(val) && !isnan_d(eval_values[i - 1]) && !isinf_d(eval_values[i - 1])
                    && eval_values[i - 1] < limits[3] && eval_values[i - 1] > limits[2])
                {
                    /* if previous value is larger, then we are going "down", so draw from the current to the edge */
                    if (eval_values[i - 1] > val)
                    {
                        ps_pen_draw(pen, DRAW_X_BEGIN + (plot_x - limits[0]) * step_coef, DRAW_Y_BEGIN);
                    }
                    else /* opposite case */
                    {
                        ps_pen_draw(pen, DRAW_X_BEGIN + (plot_x - limits[0]) * step_coef, DRAW_Y_END);
                    }
                }
                penup = 1;
                ps_pen_up(pen);
            }
        }
        else
        {
            /* if we just returned back to value range, do not draw, just move pen and put it down */
            if (penup == 1)
            {
                /* this code guarantees, that lines running too fast out of plot (lots of regular functions)
                   will not be cut somewhere far from the edge, but they will be drawn till the edge */
                if (!isnan_d(eval_values[i - 1]) && !isinf_d(eval_values[i - 1]))
                {
                    /* if previous value is larger, then we are going "down", so draw from the top to new value */
                    if (eval_values[i - 1] > val)
                    {
                        ps_pen_move(pen, DRAW_X_BEGIN + (plot_x - limits[0]) * step_coef, DRAW_Y_END);
                        ps_pen_down(pen);
                        ps_pen_draw(pen, DRAW_X_BEGIN + (plot_x - limits[0]) * step_coef, DRAW_Y_BEGIN + (val - limits[2])*val_coef);
                    }
                    else /* opposite case */
                    {
                        ps_pen_move(pen, DRAW_X_BEGIN + (plot_x - limits[0]) * step_coef, DRAW_Y_BEGIN);
                        ps_pen_down(pen);
                        ps_pen_draw(pen, DRAW_X_BEGIN + (plot_x - limits[0]) * step_coef, DRAW_Y_BEGIN + (val - limits[2])*val_coef);
                    }
                }
                else
                {
                    ps_pen_move(pen, DRAW_X_BEGIN + (plot_x - limits[0]) * step_coef, DRAW_Y_BEGIN + (val - limits[2])*val_coef);
                    ps_pen_down(pen);
                }
                penup = 0;
            }
            else /* otherwise draw normally */
            {
                /* if we have some stored value, and are not at the end */
                if (stored > 0 && i != valcount-1)
                {
                    /* verify, if the derivative increased a bit in this period */
                    if (fabs((fabs(stored_y - val) / (plot_x - stored_x)) - stored_dydx) < DRAW_MIN_DERIVATIVE)
                        continue;
                    /* if yes, decrease counter ("give it one more chance") and then continue to drawing */
                    stored--;
                }
                ps_pen_draw(pen, DRAW_X_BEGIN + (plot_x - limits[0]) * step_coef, DRAW_Y_BEGIN + (val - limits[2])*val_coef);
            }
        }
    }

    /* if neccessarry, finish path */
    if (penup == 0)
        ps_pen_up(pen);

    /* destroy pen structure */
    ps_destroy_pen(pen);

    /* finally, close document in order to save everything and so */
    ps_close_document(output);

    free(eval_values);
}
