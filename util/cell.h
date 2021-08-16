#ifndef CELL_H_
#define CELL_H_

#include "shader.h"

typedef struct Cell Cell;
struct Cell
{
    int x1, y1;
    int x2, y2;
    int width, height;
    int isleaf;
    int isroot;
    int isactive;
    Shader *shader;
    Cell *cells; // splits into 4

    #define botleft  cells
    #define topleft  cells + 1
    #define topright cells + 2
    #define botright cells + 3
};

void
initcell(Cell *cell, int isroot, int x1, int y1, int x2, int y2)
{
    cell->x1 = x1 + 2;
    cell->y1 = y1 + 2;
    cell->x2 = x2 - 2;
    cell->y2 = y2 - 2;
    cell->width  = cell->x2 - cell->x1;
    cell->height = cell->y2 - cell->y1;
    cell->isleaf = 1;
    cell->isroot = isroot;
    cell->isactive = 1;
}

void
cellupdate(Cell *cell)
{
    if (cell->isactive) {
        if (cell->isleaf) {
            glUseProgram(cell->shader->progid);
            glUniform1i(cell->shader->uwidth, cell->width);
            glUniform1i(cell->shader->uheight, cell->height);
            glUniform1i(cell->shader->ux1, cell->x1);
            glUniform1i(cell->shader->uy1, cell->y1);
            glUniform1i(cell->shader->ux2, cell->x2);
            glUniform1i(cell->shader->uy2, cell->y2);

            glDispatchCompute(cell->width, cell->height, 1);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        } else {
            int i;

            for (i = 0; i < 4; i++) 
                cellupdate((cell->cells) + i);
        }
    }
}

void
cellsplit(Cell *cell)
{
    if (cell->isleaf == 0) return;

    Cell *cells;
    // printf("splt %d, %d, %d, %d\n", cell->x1, cell->y1, cell->x2, cell->y2);
    cells = memalloc(sizeof(Cell) * 4);

    // bottom left
    initcell(cells, 0, cell->x1, cell->y1, (cell->x1 + cell->x2) / 2, (cell->y1 + cell->y2) / 2);
    cells->shader = cell->shader;
    // printf("botl %d, %d, %d, %d, %d, %d\n", cells->x1, cells->y1, cells->x2, cells->y2, cells->width, cells->height);
    cells++;

    // top left
    initcell(cells, 0, cell->x1, (cell->y1 + cell->y2) / 2, (cell->x1 + cell->x2) / 2, cell->y2);
    cells->shader = cell->shader;
    // printf("topl %d, %d, %d, %d, %d, %d\n", cells->x1, cells->y1, cells->x2, cells->y2, cells->width, cells->height);
    cells++;

    // top right
    initcell(cells, 0, (cell->x1 + cell->x2) / 2, (cell->y1 + cell->y2) / 2, cell->x2, cell->y2);
    cells->shader = cell->shader;
    // printf("topr %d, %d, %d, %d, %d, %d\n", cells->x1, cells->y1, cells->x2, cells->y2, cells->width, cells->height);
    cells++;

    // bottom right
    initcell(cells, 0, (cell->x1 + cell->x2) / 2, cell->y1, cell->x2, (cell->y1 + cell->y2) / 2);
    cells->shader = cell->shader;
    // printf("botr %d, %d, %d, %d, %d, %d\n", cells->x1, cells->y1, cells->x2, cells->y2, cells->width, cells->height);
    cells++;

    cell->cells = (cells - 4);
    cell->isleaf = 0;
    // printf("\n");
}

void
celldelete(Cell *cell)
{
    int i;

    if (cell->isleaf == 0) {
        for (i = 0; i < 4; i++)
            celldelete((cell->cells) + i);
        memfree(cell->cells);
    }

    if (cell->isroot)
        memfree(cell);
}

#endif