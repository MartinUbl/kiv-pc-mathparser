#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "postscript.h"

/**
 * Creates document structure, opens file for writing
 */
ps_document* ps_create_document(char* filename)
{
    ps_document* doc = (ps_document*)malloc(sizeof(ps_document));
    if (!doc)
        return NULL;
    doc->filename = filename;
    doc->file = fopen(filename, "w");

    /* if the file could not be opened, return NULL and let the caller handle it */
    if (doc->file == NULL)
    {
        free(doc);
        return NULL;
    }

    return doc;
}

/**
 * Closes document file handle and frees the structure
 */
void ps_close_document(ps_document* doc)
{
    fclose(doc->file);
    free(doc);
}

/**
* Creates pen structure and points it to initial location
*/
ps_pen* ps_create_pen(ps_document* doc, double init_x, double init_y)
{
    ps_pen* pen = (ps_pen*)malloc(sizeof(ps_pen));
    if (pen == NULL)
        return NULL;
    pen->document = doc;
    pen->pos_x = init_x;
    pen->pos_y = init_y;
    return pen;
}

/**
 * Destroys pen structure
 */
void ps_destroy_pen(ps_pen* pen)
{
    free(pen);
}

/**
 * Writes PostScript header to file with supplied values
 */
void ps_write_header(ps_document* doc, const char* creator, const char* title, const char* datestring)
{
    fprintf(doc->file, PS_FORMAT_IDENTIFIER "\n");
    fprintf(doc->file, PS_CREATOR_IDENTIFIER "%s\n", creator);
    fprintf(doc->file, PS_TITLE_IDENTIFIER "%s\n", title);
    fprintf(doc->file, PS_CREATION_DATE_IDENTIFIER "%s", datestring);
    fprintf(doc->file, PS_END_COMMENTS_IDENTIFIER "\n");
}

/**
 * Sets font to be used, needs to have supplied font family and size multiplier
 */
void ps_set_font(ps_document* doc, const char* font_family, int size)
{
    fprintf(doc->file, "/%s " PS_FIND_FONT " %i " PS_SCALE_FONT " " PS_SET_FONT "\n", font_family, size);
}

/**
 * Prints plain text on current location
 */
void ps_print_text(ps_document* doc, char* text)
{
    fprintf(doc->file, "(%s) " PS_TEXT_PRINT "\n", text);
}

/**
 * Prints numeric value on current location, using custom number formatter
 */
void ps_print_text_val(ps_document* doc, double val, const char* formatter)
{
    char outbuff[64];
    /* due to absence of snprintf in MSVC, and also in C89, the Splint tool
     * will complain at following line */
    sprintf(outbuff, "(%s) " PS_TEXT_PRINT "\n", formatter);
    fprintf(doc->file, outbuff, val);
}

/**
 * Sets output color, this applies to all shapes, lines and text
 */
void ps_set_color(ps_document* doc, double r, double g, double b)
{
    fprintf(doc->file, "%f %f %f " PS_SET_COLOR "\n", r, g, b);
}

/**
 * Sets current position in document
 */
void ps_set_position(ps_document* doc, double to_x, double to_y)
{
    fprintf(doc->file, "%f %f " PS_MOVE_TO "\n", to_x, to_y);
}

/**
 * Moves pen to specified location
 */
void ps_pen_move(ps_pen* pen, double to_x, double to_y)
{
    pen->pos_x = to_x;
    pen->pos_y = to_y;
}

/**
 * Puts pen down, starting new path
 */
void ps_pen_down(ps_pen* pen)
{
    fprintf(pen->document->file, PS_NEW_PATH "\n");
    fprintf(pen->document->file, "%f %f " PS_MOVE_TO "\n", pen->pos_x, pen->pos_y);
}

/**
 * Draws line from current position to supplied position, and updates pen position
 */
void ps_pen_draw(ps_pen* pen, double to_x, double to_y)
{
    pen->pos_x = to_x;
    pen->pos_y = to_y;

    fprintf(pen->document->file, "%f %f " PS_LINE_TO "\n", pen->pos_x, pen->pos_y);
}

/**
 * Closes path (making it circular) drawn with pen
 */
void ps_pen_close_path(ps_pen* pen)
{
    fprintf(pen->document->file, PS_CLOSE_PATH "\n");
}

/**
 * Puts pen up, stroking the path we've drawn
 */
void ps_pen_up(ps_pen* pen)
{
    fprintf(pen->document->file, PS_STROKE "\n");
}

/**
 * Sets line thickness
 */
void ps_set_line_width(ps_document* doc, double width)
{
    fprintf(doc->file, "%f " PS_LINEWIDTH "\n", width);
}
