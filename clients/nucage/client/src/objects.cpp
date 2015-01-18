#include "plainx.h"
#include "objects.h"


CGObj* GetObj(MapID eID)
{
    switch (eID) {
    case ID_STONE:
        return new CStone();
    case ID_PLATE:
        return new CPlate();
    case ID_KEY_RED:
        return new CKeyRed();
    case ID_KEY_GREEN:
        return new CKeyGreen();
    case ID_GATE_RED:
        return new CGateRed();
    case ID_GATE_GREEN:
        return new CGateGreen();
    case ID_STONE_OLD:
        return new CStoneOld();
    case ID_CLOCK:
        return new CClock();
    case ID_TELEPORT:
        return new CTeleport();
    case ID_STONE_BIG:
        return new CBigStone();
    case ID_STONE_BIG_REST:
        return new CBigStoneRest();
    case ID_ENTER:
        return new CEnter();
    case ID_EXIT:
        return new CExit();
    case ID_ARROW_LEFT:
        return new CArrowLeft();
    case ID_ARROW_RIGHT:
        return new CArrowRight();
    case ID_ARROW_CROSS:
        return new CArrowCross();
    case ID_ARROW_LEFTRIGHT:
        return new CArrowLeftRight();
    case ID_ARROW_UP:
        return new CArrowUp();
    case ID_ARROW_DOWN:
        return new CArrowDown();
    case ID_ARROW_UPDOWN:
        return new CArrowUpDown();
    }
    return NULL;
}
