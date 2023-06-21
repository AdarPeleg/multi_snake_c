/*
           /^\/^\
         _|__|  O|
\/     /~     \_/ \
 \____|__________/  \
        \_______      \
                `\     \                 \
                  |     |                  \
                 /      /                    \
                /     /                       \\
              /      /                         \ \
             /     /                            \  \
           /     /             _----_            \   \
          /     /           _-~      ~-_         |   |
         (      (        _-~    _--_    ~-_     _/   |
          \      ~-____-~    _-~    ~-_    ~-_-~    /
            ~-_           _-~          ~-_       _-~
               ~--______-~                ~-___-~ 

    Project by Adar Peleg.
    Technion 2021.
    course: 23411440.
*/

/*-------------------------------------------------------------------------
  Include files:
--------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

/*=========================================================================
  Constants , definitions and structures
==========================================================================*/


/* #defines here*/
#define N 25
#define BOARD_ELEMENT '#'
#define PERCENT 0
#define FIRST_PLAYER PERCENT
#define PERCENT_HEAD '%'
#define SHTRODEL 1
#define SHTRODEL_HEAD '@'
#define ILLEGAL_MOVE 0
#define STRARVED 1
#define SHTRODEL_BODY '+'
#define PERCENT_BODY '*'


#define MIN_BOARD_SIZE 4
#define MIN_MAX_STEPS 2
#define DOWN 2 
#define LEFT 4
#define UP 8
#define RIGHT 6
#define AXIS 2
#define BODY_START_SIZE 3

#define LINE 0
#define COLUMN 1
#define STARTING_FOOD 0

#define EMPTY_SPACE ' '
#define FOOD 'F'

/*structures*/
typedef struct environment_objects
{
    char board[N][N];
    int size;
    int Food_location[AXIS];
    int max_steps;
}settings;

typedef struct players {
    // PERSENT PLAYER 0
    int id;
    int steps_left; //starts at max_steps and goes up again when eats
    int body[N][AXIS];
    int Food;
}snake;

/*-------------------------------------------------------------------------
*   Function declartion
*--------------------------------------------------------------------------*/

void print_introduction_msg();                        //print welcome message , and introduction message to the game
void print_board_size_msg();                          //print message to enter board size
void print_steps_without_food();                      //print message to enter maximum number of steps can snake do without food
void print_food_location_msg();                       //print message to insert place for next food
void help_print_board(int size);                      //printing static part of the board
void print_board(char board[N][N], int size);          //print the current board for the user
void print_player_message(int player);                //print message when we switch between users
void print_insert_direction();			              //print message asking the user to insert his move
void print_finsih_msg(int player,int finish_reason ); //get the player who lose and the reason of losing and print message state
void print_error_input();                             //print message - there is error in the input
void print_invalid_input();			                  //print message - re-insert valid input
void print_full_board();		                      //print message - full board - no winner

void get_game_settings(settings* penv);
void board_reset(settings* penv);
int get_board_size();
void is_error1_input(int* pinput);
void get_food_location(settings* penv);
void place_food(settings* penv);
int invalid_free_space(settings* penv,
    int row, int  column);
void is_error2_input(int* prow, int* pcolumn);
int get_max_steps();
void start_game(settings* penv);
void game_loop(settings* penv, snake* shtrodel, snake* percent);
int get_valid_direction();
int valid_direction(int input);
int  move_snake(settings* penv, snake* player, int direction);
int move_not_needed(settings* penv, snake* player,
    int  direction);
void body_move(snake* player, int  direction);
void new_head_pos(int body[][AXIS], int direction, int headpos[AXIS]);
int reach_food(int body[][AXIS], int direction, int Food_location[AXIS]);
int illegal_move(settings* penv, snake* player, int direction);
int non_full_board(settings* penv);
void game_tied(settings* penv, snake* player, int direction);
void place_snake(settings* penv, snake* player);
char body_sign(int id);
char head_sign(int id);
void up_date_board(settings* penv, snake* player1, snake* player2);




int main()
{
  //add here you code
    settings env, *penv;
    penv = &env;
    get_game_settings(penv);
    start_game(penv);
  return 0;
}

//add here your own function implementation.


/*======================================================================
                        GAME SETTINGS
======================================================================*/

//game start settings.
void get_game_settings(settings*penv)
{
    print_introduction_msg();
    print_board_size_msg();
    penv->size = get_board_size();
    board_reset(penv);
    print_steps_without_food();
    penv->max_steps = get_max_steps();
}

//scans valid input / error
int get_board_size()
{
    int input;
    is_error1_input(&input);
    while (input > N || input < MIN_BOARD_SIZE) {
        print_invalid_input();
        is_error1_input(&input);
    }
    return input;
}

// checks if non error input. 1 int.
void is_error1_input(int* pinput) {
    if (scanf("%d", pinput) < 1) {
        print_error_input();
        exit(1);
    }
}

// fills board with spaces.
void board_reset(settings* penv) {
    for (int i = 0; i <= penv->size; i++) {
        for (int j = 0; j <= penv->size; j++) {
            penv->board[i][j] = EMPTY_SPACE;
        }
    }
}

// scans valid input / error max steps
int get_max_steps()
{
    int input;
    is_error1_input(&input);
    while (input < MIN_MAX_STEPS) {
        print_invalid_input();
        is_error1_input(&input);
    }
    return input;
}

// scans valid input / error and changes board value.
void get_food_location(settings* penv) {
    print_food_location_msg();
    int row, column;
    is_error2_input(&row, &column);
    while (!invalid_free_space(penv, row, column)) {
        print_invalid_input();
        is_error2_input(&row, &column);
    }
    penv->Food_location[LINE] = row; // - 1 if 1 to size-1
    penv->Food_location[COLUMN] = column; // - 1 if 1to size-1
    // place_food(penv); XX
}

// changes food positin on penv->board to the right element. 
void place_food(settings* penv) {
    penv->board[penv->Food_location[LINE]][penv->Food_location[COLUMN]] =
        FOOD;
}




// is place on board and free.
int invalid_free_space(settings* penv, int row, int  column) {
    // normalize input from player
    if ((penv->size > row && penv->size > column) 
        && (column >= 0 && row >= 0) && (penv->board[row][column] == 
            EMPTY_SPACE)) {
        // - 1 if 1 to size-1 and != 0
        return 1;
    }
    else return 0;
}

// checks if non error input. (for 2 int.)
void is_error2_input(int *prow, int *pcolumn) {
    if (scanf("%d%d", prow, pcolumn) < 2) {
        print_error_input();
        exit(1);
    }
}

/*======================================================================
                               GAME LOOP
========================================================================*/

// init players and calls GAME LOOP
void start_game(settings* penv)
{
    // seting start values
    snake shtrodel = {.id = SHTRODEL, .Food = STARTING_FOOD, 
        .steps_left = penv->max_steps, .body[0][LINE] = penv->size - 1,
        .body[1][LINE] = penv->size - 1, .body[2][LINE] = penv->size - 1,
        .body[0][COLUMN] = 0, .body[1][COLUMN] = 1, .body[2][COLUMN] = 2};
    snake percent = { .id = PERCENT, .Food = STARTING_FOOD,
        .steps_left = penv->max_steps, .body[0][LINE] = 0, 
        .body[1][LINE] = 0, .body[2][LINE] = 0, .body[0][COLUMN] = 0,
        .body[1][COLUMN] = 1, .body[2][COLUMN] = 2 };

    game_loop(penv, &shtrodel, &percent);
}

// main GAME LOOP
void game_loop(settings* penv, snake* shtrodel, snake* percent) {
    int direction, game_over = 0, round = FIRST_PLAYER;
    up_date_board(penv, shtrodel, percent);
    get_food_location(penv);
    place_food(penv);
    while (game_over == 0) {
        print_board(penv->board, penv->size);
        print_player_message(round % 2);
        print_insert_direction();
        direction = get_valid_direction();
        if (round % 2 == SHTRODEL) {
            game_over = move_snake(penv, shtrodel, direction);
        }
        else {
            game_over = move_snake(penv, percent, direction);
        }
        round++;
        board_reset(penv);
        up_date_board(penv, shtrodel, percent);
        place_food(penv);
    } 
}


// gets a valid direction to return.
int get_valid_direction() {
    int input;
    is_error1_input(&input);
    while (!valid_direction(input)) {
        print_invalid_input();
        is_error1_input(&input);
    }
    return input;
}

int valid_direction(int input) {
    switch (input)
    {
    case DOWN:
    case LEFT:
    case RIGHT:
    case UP: return 1;
    default:
        return 0;
    }
}

//need to be short
int move_snake(settings* penv, snake* player, int direction) {
    if (move_not_needed(penv, player, direction)) {
        return 1;
    }else {
        if (reach_food(player->body, direction, penv->Food_location)) {
            player->Food++;
            //board isn't filled
            if (non_full_board(penv)) {
                get_food_location(penv);
            }
            //full filled
            else {
                game_tied(penv, player, direction);
                return 3;
            }
        }
        body_move(player, direction);
        player->steps_left--;
        return 0;
    }
}

// move_not_needed if the move is an illegal move or player haven't
// reached food and this is last step to do so.
int move_not_needed(settings* penv, snake* player, int direction) {
    // illegal move
    if (illegal_move(penv, player, direction)) {
        print_finsih_msg(player->id, ILLEGAL_MOVE); // illegal move
        return 1;
    }
    // haven't reached food and this is last step to do so.
    else if (!reach_food(player->body, direction, penv->Food_location)
        && player->steps_left == 1) {
        print_finsih_msg(player->id, STRARVED);
        return 2;
         }
    return 0;
}

//return if the move is illegal
// ilegal move if: out of board or new pos on player.
int illegal_move(settings* penv, snake* player, int direction) {
    int headpos[AXIS];
    new_head_pos(player->body, direction, headpos);
    //out of board.
    if (headpos[LINE] >= penv->size || headpos[COLUMN] >= penv->size) {
        return 1;
    }
    // new pos on player.
    else if (penv->board[headpos[LINE]][headpos[COLUMN]] != EMPTY_SPACE &&
        penv->board[headpos[LINE]][headpos[COLUMN]] != FOOD) return 1;
    else return 0;
}

// returns 1 if new given location of the head is FOOD pos.
int reach_food(int body[][AXIS], int direction, int Food_location[AXIS]) {
    int headpos[AXIS];
    new_head_pos(body, direction ,headpos);
    if (headpos[LINE]  == Food_location[LINE] &&
        headpos[COLUMN]  == Food_location[COLUMN]) return 1;
    else return 0;
}

// return 1 if there is empty space on board;
int non_full_board(settings* penv) {
    for (int i = 0; i < penv->size; i++) {
        for (int j = 0; j < penv->size; j++) {
            if (penv->board[i][j] == EMPTY_SPACE) {
                return 1;
            }
        }
    }
    return 0;
}

//update game status and print
void game_tied(settings* penv, snake* player, int direction) {
    body_move(player, direction);
    place_snake(penv, player);
    print_board(penv->board, penv->size);
    print_full_board();
}

// changes headpos[AXIS] to body[0] new position. 
//(for move evaluations amd movement)
void new_head_pos(int body[][AXIS], int direction, int headpos[AXIS]) {
    switch (direction)
    {
    case UP:
        headpos[LINE] = body[0][LINE] - 1;
        headpos[COLUMN] = body[0][COLUMN];
        break;
    case DOWN:
        headpos[LINE] = body[0][LINE] + 1;
        headpos[COLUMN] = body[0][COLUMN];
        break;
    case RIGHT:
        headpos[LINE] = body[0][LINE];
        headpos[COLUMN] = body[0][COLUMN] + 1;
        break;
    case LEFT:
        headpos[LINE] = body[0][LINE];
        headpos[COLUMN] = body[0][COLUMN] - 1;
        break;
    }
}

// moves body part to the next body part & changes head pos to new one.
void body_move(snake* player, int direction) {
    int new_pos[AXIS];
    new_head_pos(player->body, direction, new_pos);
    for (int i = BODY_START_SIZE + player->Food; i > 0; i--) {
        for (int j = 0; j < AXIS; j++) {
            player->body[i][j] = player->body[i - 1][j];
        }
    }
    player->body[0][LINE] = new_pos[LINE];
    player->body[0][COLUMN] = new_pos[COLUMN];
}

// changes all needed body and head positions on board
void place_snake(settings* penv, snake* player) {
    //update head
    penv->board[player->body[0][LINE]][player->body[0][COLUMN]] =
        head_sign(player->id);
    //update body
    for (int i = 1; i < BODY_START_SIZE + player->Food; i++) {
        penv->board[player->body[i][LINE]][player->body[i][COLUMN]] =
            body_sign(player->id);
    }
}

// returns given player's body sign.
char body_sign(int id) {
    if (id == SHTRODEL) {
        return SHTRODEL_BODY;
    }
    else {
        return PERCENT_BODY;
    }
}

// returns given player's head sign.
char head_sign(int id) {
    if (id == SHTRODEL) {
        return SHTRODEL_HEAD;
    }
    else {
        return PERCENT_HEAD;
    }
}

// updates the penv->board contant.
void up_date_board(settings* penv, snake* player1,
    snake* player2) {
    place_snake(penv, player1);
    place_snake(penv, player2);
}





void print_introduction_msg()
{
    printf("Welcome to multi-snake  game, The game have 2 player Percent and Shtrodel.\n"
    "The game starts with PERCENT player.\n");
    printf("You have to choose 4 moves :Up,Down,Right and Left "
    "The Percent player is the snake that starts in (0,0)\n\n");
}

void print_board_size_msg(){
    printf("Insert board size between 4 and 25:\n");
}

void print_steps_without_food(){
    printf("Insert the maximum number of steps the snake can do without food:\n");
}

void print_food_location_msg(){
    printf("in order to continue the game please Insert a row then column numbers to place the next food: \n");
}

void help_print_board(int size)
{
    int i=0;
    printf("%c",BOARD_ELEMENT);
    for(i=0;i<size;i++)
    {
        printf("%c",BOARD_ELEMENT);
    }
    printf("%c\n",BOARD_ELEMENT);
}

void print_board(char board[N][N], int size)
{
    int i=0,j=0;
    help_print_board(size);
    for(i=0;i<size;i++)
    {
        printf("%c",BOARD_ELEMENT);
        for(j=0;j<size;j++)
        {
            printf("%c",board[i][j]);
        }
        printf("%c\n",BOARD_ELEMENT);
    }
    help_print_board(size);
    printf("\n\n");
}

void print_player_message(int player )
{
    printf("******* Turn of %c ***********\n",player==PERCENT?PERCENT_HEAD:SHTRODEL_HEAD);
}

void print_insert_direction(){
    printf("Enter the direction to move your snake: (2-down,4-left,6-right,8-up):\n");
}

void print_finsih_msg(int player ,int finish_reason)
{
    printf("\nThe game finish and the winner is:\n");
    printf("*** %s player ****.\n",player==PERCENT?"SHTRODEL":"PERCENT");
    printf("The reason is %s of the ",finish_reason==ILLEGAL_MOVE? "Illegal move" : "Snake die");
    printf("%s player.\n\n",player==PERCENT?"PERCENT":"SHTRODEL");
}

void print_invalid_input(){
    printf("Please re-Inset valid input:\n");
}

void print_full_board(){
    printf("Full Board. The game over with no Winner.\n");
}

void print_error_input(){
    printf("Error with the input.\n");
}
