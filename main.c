#include "raylib.h"
#include "vector.h"
#include "main.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define FPS 60
#define FRAME_DISPLAY_DEBUG 

void renderGrid(int cols, int rows, int thiccness, int menuStartX) {
    float xSpacing = (float)menuStartX / (float)cols;

    for (int i = 0; i < cols; i++) {
        DrawLine(xSpacing * i, 0, xSpacing * i, GetScreenHeight(), LIGHTGRAY);

    }
    float ySpacing = (float)GetScreenHeight() / (float)rows;
    for (int j = 0; j < rows; j++) {
        DrawLine(0, j * ySpacing, menuStartX, j * ySpacing, LIGHTGRAY);
    }
}

void fillCell(int xCell, int yCell, int cols, int rows, int menuStartX, int aliveState, int doColor, int date, int framesSinceBeginning, int redC, int greenC, int blueC) {

    float xSpacing = (float)menuStartX / (float)cols;
    float ySpacing = (float)GetScreenHeight() / (float)rows;

    float startPosX = (float)xCell * xSpacing;
    float startPosY = (float)yCell * ySpacing;

    if (aliveState == 1) {
        DrawRectangle(startPosX, startPosY, xSpacing, ySpacing, LIGHTGRAY);
    } else {
        if (doColor == 1 && date != 0) {
            if (xCell == 0 && yCell == 0) {
            }
            double decay = framesSinceBeginning - date;
            
            DrawRectangle(startPosX, startPosY, xSpacing, ySpacing, CLITERAL(Color){ (255 / ((int)log2(decay) + 1)) * redC, (255 / ((int)log2(decay) + 1)) * greenC, (255 / ((int)log2(decay) + 1)) * blueC, 255});
            //DrawRectangle(startPosX, startPosY, xSpacing, ySpacing, CLITERAL(Color){ 255 - (35*decay )%256, 0, 255- (35*decay/255)%256 , 255});

        } else {
            DrawRectangle(startPosX, startPosY, xSpacing, ySpacing, BLACK);
        }
    }

}

int locToSlot(int posX, int posY, int rows, int cols) {
    return (posX % cols) + (posY * cols);
}

int isAlive(int posX, int posY, int rows, int cols, Cell listOfCells[])
    {
    if(posX < 0 || posX > cols || posY < 0 || posY > rows)
    {
        return 0;
    }
    return listOfCells[locToSlot(posX, posY, rows, cols)].alive;
    }


void getStateOfNeighbours(int *stateArray, int posX, int posY, Cell listOfCells[], int cols, int rows) {

    /*
     0 | 1 | 2
    ---|---|---
     7 | x | 3
    ---|---|---
     6 | 5 | 4
    */

    int cell0 = isAlive(posX - 1, posY - 1, rows, cols, listOfCells);
    int cell1 = isAlive(posX, posY - 1, rows, cols, listOfCells);
    int cell2 = isAlive(posX + 1, posY - 1, rows, cols, listOfCells);
    int cell3 = isAlive(posX + 1, posY, rows, cols, listOfCells);   
    int cell4 = isAlive(posX + 1, posY + 1, rows, cols, listOfCells);
    int cell5 = isAlive(posX, posY + 1, rows, cols, listOfCells);
    int cell6 = isAlive(posX - 1, posY + 1, rows, cols, listOfCells);
    int cell7 = isAlive(posX - 1, posY, rows, cols, listOfCells);
    int localArray[8] = {cell0, cell1, cell2, cell3, cell4, cell5, cell6, cell7};

    for (int i = 0; i < 8; i++) {
        stateArray[i] = localArray[i];
    }

}

int main() 
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Cellular Automata");


              // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    bool pause = false;
    bool pauseDebugMessage = false;
    bool pauseWhenClicked = true;
    bool reset = false;
    bool displayGrid = false;
    bool displayDebug = false;
    bool populateRandomly = false;
    bool populateRandomlyOnClick = false;
    
    bool doColor = false;
    bool redC = true;
    bool greenC = false;
    bool blueC = true;


    int sizeOfBrush = 3;
    int fps = 30;

    int menuStartX = GetScreenWidth() - 250;
    int menuStartY = 0;

    VECTOR_INIT(debugMessages);
    int debugMessageAlive = 0;

    long framesSinceBeginning = 1;

    int cols = 225;
    int rows = 175;
    
    Cell listOfCells[cols * rows];
    Cell listOfCellsToDraw[cols * rows];

    for (int i = 0; i < (cols * rows); i++) {
        Cell deadCell;
        deadCell.x = i % cols;
        deadCell.y = i / cols;
        deadCell.alive = 0;
        deadCell.dateCreated = 0;

        listOfCells[i] = deadCell;
        listOfCellsToDraw[i] = deadCell;
    }

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        SetTargetFPS(fps);     
        if (reset == 1) {
            // Clear all cells
            for (int i = 0; i < rows * cols; i++) {
                listOfCells[i].alive = 0;
                listOfCellsToDraw[i].alive = 0;
            }

            VECTOR_ADD(debugMessages, "The cells have been cleared !");

            reset = 0;
        }
        
        if (populateRandomly == 1) {
            // Create randomly some alive cells

            for (int i = 0; i < rows * cols; i++) {
                if (listOfCellsToDraw[i].alive == 0) {
                    int r = GetRandomValue(0, 1);
                    listOfCells[i].alive = r;
                    listOfCellsToDraw[i].alive = r;
                }
                
            }
            populateRandomly = 0;
        }


        if (pause == 1 || (((IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) && pauseWhenClicked == 1) && (GetMouseX() >= 0 && GetMouseX() <= menuStartX - 1 && GetMouseY() >= 0 && GetMouseY() <= GetScreenHeight() - 1))) {
            // Only allow to draw new cells
            if (pauseDebugMessage == false) {
                VECTOR_ADD(debugMessages, "Pause !");
                pauseDebugMessage = true;
            }
        } else {
            // Game of life !
            if (pauseDebugMessage == true) {
                VECTOR_ADD(debugMessages, "Resumed !");
                pauseDebugMessage = false;
            }

            // GAME LOGIC

            int stateArray[8] = {0};

            for (int i = 0; i < rows * cols; i++) {
                getStateOfNeighbours(stateArray, listOfCells[i].x, listOfCells[i].y, listOfCells, cols, rows);
                int sum = 0;
                for (int j = 0; j < 8; j++) {
                    if (stateArray[j] == 1) {
                        sum++;
                    }
                }
                if (sum == 3 || (listOfCells[i].alive == 1 && sum == 2)) {
                    listOfCellsToDraw[i].alive = 1;
                } else {
                    listOfCellsToDraw[i].alive = 0;
                }
                
            };

            framesSinceBeginning++;
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && (GetMouseX() >= 0 && GetMouseX() <= menuStartX && GetMouseY() >= 0 && GetMouseY() <= GetScreenHeight())) {
            int mousePosX = GetMouseX();
            int mousePosY = GetMouseY();

            int xCellClicked = (float)mousePosX / ((float)menuStartX / (float)cols);
            int yCellClicked = (float)mousePosY / ((float)GetScreenHeight() / (float)rows);

            for (int i = xCellClicked - sizeOfBrush; i < xCellClicked + sizeOfBrush + 1; i++) {
                for (int j = yCellClicked - sizeOfBrush; j < yCellClicked + sizeOfBrush + 1; j++) {
                    if (i >= 0 && i <= (cols - 1) && j >= 0 && j <= (rows - 1)) {
                        if (populateRandomlyOnClick) {
                            int r = GetRandomValue(0,1);
                            listOfCells[locToSlot(i, j, rows, cols)].alive = r;
                            listOfCellsToDraw[locToSlot(i, j, rows, cols)].alive = r;
                        }
                        else {
                            listOfCells[locToSlot(i, j, rows, cols)].alive = 1;
                            listOfCellsToDraw[locToSlot(i, j, rows, cols)].alive = 1;
                        }
                    }
                }
            }

        }

        else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && (GetMouseX() >= 0 && GetMouseX() <= menuStartX && GetMouseY() >= 0 && GetMouseY() <= GetScreenHeight())) {
            int mousePosX = GetMouseX();
            int mousePosY = GetMouseY();

            int xCellClicked = (float)mousePosX / ((float)menuStartX / (float)cols);
            int yCellClicked = (float)mousePosY / ((float)GetScreenHeight() / (float)rows);

            for (int i = xCellClicked - sizeOfBrush; i < xCellClicked + sizeOfBrush + 1; i++) {
                for (int j = yCellClicked - sizeOfBrush; j < yCellClicked + sizeOfBrush + 1; j++) {
                    if (i >= 0 && i <= (cols - 1) && j >= 0 && j <= (rows - 1)) {
                        listOfCells[locToSlot(i, j, rows, cols)].alive = 0;
                        listOfCellsToDraw[locToSlot(i, j, rows, cols)].alive = 0;
                    }
                }
            }

        }


        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(BLACK);

            // MENU
            DrawLine(menuStartX, menuStartY, menuStartX, GetScreenHeight(),  Fade(GRAY, 0.6f));
            DrawRectangle(menuStartX, menuStartY, GetScreenWidth() - menuStartX, GetScreenHeight(), LIGHTGRAY);
            DrawText("MENU", menuStartX + 100, menuStartY + 10, 20, YELLOW);

            pause = GuiCheckBox((Rectangle){ menuStartX + 10, menuStartY + 40, 25, 25 }, "Pause", pause);
            if (pause != 1) {
                pauseWhenClicked = GuiCheckBox((Rectangle){ menuStartX + 100, menuStartY + 40, 25, 25 }, "Pause when clicked", pauseWhenClicked);
            }
            reset = GuiCheckBox((Rectangle){ menuStartX + 10, GetScreenHeight() - 30, 25, 25 }, "Reset ?", reset);
            displayGrid = GuiCheckBox((Rectangle){ menuStartX + 10, menuStartY + 80, 25, 25 }, "Display Grid", displayGrid);
            displayDebug = GuiCheckBox((Rectangle){ menuStartX + 10, GetScreenHeight() - 70, 25, 25 }, "Display debug messages", displayDebug);
            populateRandomly = GuiCheckBox((Rectangle){ menuStartX + 10, GetScreenHeight() - 110, 25, 25 }, "Populate randomly", populateRandomly);
            populateRandomlyOnClick = GuiCheckBox((Rectangle){ menuStartX + 10, 230, 25, 25 }, "Populate randomly on click", populateRandomlyOnClick);
            
            doColor = GuiCheckBox((Rectangle){ menuStartX + 10, 270, 25, 25 }, "Colors", doColor);
            
            if (doColor) {
                redC = GuiCheckBox((Rectangle){ menuStartX + 35, 300, 15, 15 }, "Red", redC);
                greenC = GuiCheckBox((Rectangle){ menuStartX + 35, 320, 15, 15 }, "Green", greenC);
                blueC = GuiCheckBox((Rectangle){ menuStartX + 35, 340, 15, 15 }, "Blue", blueC);

            }

            sizeOfBrush = GuiSliderBar((Rectangle){ menuStartX + 100, menuStartY + 120, 120, 20 }, "Size of brush", NULL, sizeOfBrush, 0, 75);
            char *sizeText = (char*)malloc(16 * sizeof(char));
            sprintf(sizeText, "%d", sizeOfBrush);
            DrawText(sizeText, menuStartX + 100, menuStartY + 145, 15, BLACK);

            fps = GuiSliderBar((Rectangle){ menuStartX + 100, menuStartY + 180, 120, 20 }, "FPS", NULL, fps, 1, 120);
            char *fpsText = (char*)malloc(16 * sizeof(char));
            sprintf(fpsText, "%d", fps);
            DrawText(fpsText, menuStartX + 100, menuStartY + 205, 15, BLACK);

            // Jeu

            for (int i = 0; i < cols * rows; i++) {

                if (listOfCellsToDraw[i].alive == 1) {
                    listOfCellsToDraw[i].dateCreated = framesSinceBeginning;
                    fillCell(listOfCellsToDraw[i].x, listOfCellsToDraw[i].y, cols, rows, menuStartX, 1, doColor, listOfCellsToDraw[i].dateCreated, framesSinceBeginning, redC, greenC, blueC);

                } else {
                    fillCell(listOfCellsToDraw[i].x, listOfCellsToDraw[i].y, cols, rows, menuStartX, 0, doColor, listOfCellsToDraw[i].dateCreated, framesSinceBeginning, redC, greenC, blueC);

                }
            }

            for (int i = 0; i < cols * rows; i++) {
                listOfCells[i] = listOfCellsToDraw[i];
            }

            // Render

            if (displayGrid) {
                renderGrid(cols, rows, 3, menuStartX);
            }

            // Debug

            if (VECTOR_GET(debugMessages, char*, 0) != NULL) {
                if (displayDebug) {
                    DrawText(VECTOR_GET(debugMessages, char*, 0), 10, GetScreenHeight() - 50, 25, RED);
                }
                debugMessageAlive++;

                if (debugMessageAlive == 1) {

                    debugMessageAlive = 0;
                    VECTOR_DELETE(debugMessages, 0);

                }
            }
            
            DrawFPS(GetScreenWidth() - 100, GetScreenHeight() - 30);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    VECTOR_FREE(debugMessages);

    return 0;
}