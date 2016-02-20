#ifndef MATHPARSER_POSTSCRIPT_H
#define MATHPARSER_POSTSCRIPT_H

#define PS_FORMAT_IDENTIFIER            "%%!PS-Adobe-2.0"       /* identifies format of file (recognized mainly by printers) */
#define PS_CREATOR_IDENTIFIER           "%%%%Creator: "         /* creator signature */
#define PS_TITLE_IDENTIFIER             "%%%%Title: "           /* document title */
#define PS_CREATION_DATE_IDENTIFIER     "%%%%CreationDate: "    /* date and time of creation */
#define PS_END_COMMENTS_IDENTIFIER      "%%%%EndComments"       /* ends comment section */

#define PS_NEW_PATH     "newpath"       /* creates new path (begins drawing there) */
#define PS_CLOSE_PATH   "closepath"     /* closes path (first point with ending) */
#define PS_STROKE       "stroke"        /* strokes path (finishes line drawing) */
#define PS_MOVE_TO      "moveto"        /* moves pen/internal position to specified coords */
#define PS_LINE_TO      "lineto"        /* draws line from current to new point */
#define PS_LINEWIDTH    "setlinewidth"  /* sets line thickness */
#define PS_SET_COLOR    "setrgbcolor"   /* sets color of output shapes */
#define PS_FIND_FONT    "findfont"      /* finds font in system */
#define PS_SCALE_FONT   "scalefont"     /* sets font size ("scale") */
#define PS_SET_FONT     "setfont"       /* finishes font options and sets font to use */
#define PS_TEXT_PRINT   "show"          /* command for printing anything preceding (on stack while parsing) (i.e. text) */

/* document structure, holding name, and file handle pointer */
struct _ps_document
{
    char* filename;
    FILE* file;
};
typedef struct _ps_document ps_document;

/* pen structure, holding current position and document we are drawing in */
struct _ps_pen
{
    ps_document* document;
    double pos_x, pos_y;
};
typedef struct _ps_pen ps_pen;

ps_document* ps_create_document(char* filename);
void ps_close_document(ps_document* doc);

ps_pen* ps_create_pen(ps_document* doc, double init_x, double init_y);
void ps_destroy_pen(ps_pen* pen);

void ps_write_header(ps_document* doc, const char* creator, const char* title, const char* datestring);

void ps_set_font(ps_document* doc, const char* font_family, int size);
void ps_print_text(ps_document* doc, char* text);
void ps_print_text_val(ps_document* doc, double val, const char* formatter);

void ps_set_position(ps_document* doc, double to_x, double to_y);
void ps_set_color(ps_document* doc, double r, double g, double b);

void ps_pen_move(ps_pen* pen, double to_x, double to_y);
void ps_pen_down(ps_pen* pen);
void ps_pen_draw(ps_pen* pen, double to_x, double to_y);
void ps_pen_up(ps_pen* pen);

void ps_set_line_width(ps_document* doc, double width);

#endif
