#ifndef MATHPARSER_DRAWING_H
#define MATHPARSER_DRAWING_H

#define PLOT_STEP_COEF 0.0001                   /* we will step by this percentage value when drawing */
#define DRAW_X_BEGIN 25.0                       /* absolute X position of drawing start */
#define DRAW_X_END   570.0                      /* absolute X position of drawing end */
#define DRAW_Y_BEGIN 25.0                       /* absolute Y position of drawing start */
#define DRAW_Y_END   570.0                      /* absolute Y position of drawing end*/

#define PARAMETER_FORMATTER "%.4g"              /* formatter to use when printing number labels */
#define DRAW_LABEL_FONT_FAMILY "Times-Roman"    /* implicit font family to use for labels */
#define DRAW_LABEL_FONT_SIZE 10                 /* implicit label font size */

#define DRAW_MIN_DERIVATIVE 0.01                /* minimum derivative difference to draw next line segment */

void drawing_process_output(char* expr, char* output_file, c_stack* rpn_stack, double* limits);

#endif
