#ifndef GGLOB_H
#define GGLOB_H

#define MAP_WIDTH 20
#define MAP_HEIGHT 11
#define MAP_CELL_SIZE 32
#define TIME_BONUS_SEC 60

#define PLAYER_MOVE_DIF 4
#define PLAYER_MOVE_DELTA 30
#define CELL_MOVE_DIF 4
#define CELL_MOVE_DELTA 4 


enum MapID {
    ID_EMPTY = 0,
    ID_STONE = 1,
    ID_PLATE = 2,
    ID_KEY_RED = 3,
    ID_KEY_GREEN = 4,
    ID_GATE_RED = 5,
    ID_GATE_GREEN = 6,
    ID_STONE_OLD = 7,
    ID_ENTER = 8,
    ID_EXIT = 9,
    ID_CLOCK = 10,
    ID_TELEPORT = 11,
    ID_STONE_BIG = 12,
    ID_STONE_BIG_REST = 13,
    ID_ARROW_UP = 14,
    ID_ARROW_DOWN = 15,
    ID_ARROW_LEFT = 16,
    ID_ARROW_RIGHT = 17,
    ID_ARROW_UPDOWN = 18,
    ID_ARROW_LEFTRIGHT = 19,
    ID_ARROW_CROSS = 20
};

enum MoveID {
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT
};

enum PlStateID {
    PL_STATE_IDLE,
    PL_STATE_MOVE,
};

enum KeyID {
    KEY_RED,
    KEY_GREEN,
};

// player's rows definition
#define PL_ROW_LEFT 0
#define PL_ROW_RIGHT 1
#define PL_ROW_UP 2
#define PL_ROW_DOWN 3
#define PL_ROW_IDLE 4
#define PL_ROW_ENTER 5
#define PL_ROW_EXIT 6

//defined states id ///////////////////////////////////////////////////////////

// level listener states
#define ES_LEVEL_LOAD ES_USERSTATE + 1
#define ES_LEVEL_SUCCEEDED ES_USERSTATE + 2
#define ES_LEVEL_FAILED ES_USERSTATE + 3
#define ES_LEVEL_SELECT ES_USERSTATE + 4
#define ES_LEVEL_START ES_USERSTATE + 5
#define ES_LEVEL_RESTART ES_USERSTATE + 6

// level with player states
#define ES_LEVEL_PLAYER_LEVING_CELL ES_USERSTATE + 11
#define ES_LEVEL_PLAYER_MOVED_BY_CELL ES_USERSTATE + 12
#define ES_LEVEL_PLAYER_ENTER_CELL ES_USERSTATE + 13

// player states
#define ES_PLAYER_START_MOVE ES_USERSTATE + 20
#define ES_PLAYER_SELF_MOVING ES_USERSTATE + 21
#define ES_PLAYER_STOP_MOVE ES_USERSTATE + 22
#define ES_PLAYER_ENTER_LEVEL ES_USERSTATE + 23
#define ES_PLAYER_EXIT_LEVEL ES_USERSTATE + 24

// time states
#define ES_TIME_LOOP ES_USERSTATE + 40

// engine states
#define ES_ENGINE_CHECKMUSIC ES_USERSTATE + 50
#define ES_ENGINE_INITLANG ES_USERSTATE + 51

// rt vars
#define NA_RT_CURR_LEVEL_NUMBER "CurrLevelNumber"
#define NA_RT_WORLD_SIZE "WorldSize"
#define NA_RT_MINI_LEVEL_NUMBER "MiniLevelNumber"
#define NA_RT_MAX_LEVEL_NUMBER "MaxLevelNumber"
#define NA_RT_GAME_STATE "GameState"
#define NA_RT_BACK_MUSIC "BackMusic"

#define NA_MAX_LEVELS 60
#define NA_NUM_WORLDS 4
#define NA_WORLD_DESC_NUM_LINES 5

#define GS_MENU 0
#define GS_GAME 1
#define GS_INFRA 2
#define GS_WINSCR 3

#endif // GGLOB_H