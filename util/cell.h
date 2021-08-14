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
    Shader *shader;
    Cell *cells; // splits into 4
};

void
initcell(Cell *cell, int isroot, int x1, int y1, int x2, int y2)
{
    cell->x1 = x1 + 1;
    cell->y1 = y1 + 1;
    cell->x2 = x2 - 1;
    cell->y2 = y2 - 1;
    cell->width  = cell->x2 - cell->x1;
    cell->height = cell->y2 - cell->y1;
    cell->isleaf = 1;
    cell->isroot = isroot;
}

void
cellupdate(Cell *cell)
{
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
            cellupdate(cell->cells++);

        cell->cells -= 4;
    }
}

void
cellsplit(Cell *cell)
{
    if (cell->isleaf == 0) return;

    Cell *cells;

    cells = memalloc(sizeof(Cell) * 4);

    // bottom left
    initcell(cells, 0, cell->x1, cell->y1, cell->x2 / 2, cell->y2 / 2);
    cells->shader = cell->shader;
    cells++;

    // top left
    initcell(cells, 0, cell->x1, cell->y2 / 2, cell->x2 / 2, cell->y2);
    cells->shader = cell->shader;
    cells++;

    // top right
    initcell(cells, 0, cell->x2 / 2, cell->y2 / 2, cell->x2, cell->y2);
    cells->shader = cell->shader;
    cells++;

    // bottom right
    initcell(cells, 0, cell->x2 / 2, cell->y1, cell->x2, cell->y2 / 2);
    cells->shader = cell->shader;
    cells++;

    cell->cells = (cells - 4);
    cell->isleaf = 0;
}

void
celldelete(Cell *cell)
{
    int i;

    if (cell->isleaf == 0) {
        for (i = 0; i < 4; i++)
            celldelete(cell->cells++);
        memfree(cell->cells - 4);
    }

    if (cell->isroot)
        memfree(cell);
}

#endif