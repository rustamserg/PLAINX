#include "plainx.h"
#include "gglob.h"
#include "objects.h"
#include "map.h"


CMap::CMap()
{
  m_Key[0] = 156;     m_Key[1] = 71;      m_Key[2] = 45;      m_Key[3] = 10;	  m_Key[4] = 30;
  m_Key[5] = 0;	      m_Key[6] = 175;	  m_Key[7] = 176;     m_Key[8] = 217;	  m_Key[9] = 254;
  m_Key[10] = 163;    m_Key[11] = 204;    m_Key[12] = 34;     m_Key[13] = 35;	  m_Key[14] = 66;
  m_Key[15] = 207;    m_Key[16] = 138;    m_Key[17] = 128;    m_Key[18] = 135;    m_Key[19] = 141;
  m_Key[20] = 85;     m_Key[21] = 178;    m_Key[22] = 234;    m_Key[23] = 113;    m_Key[24] = 26;
  m_Key[25] = 150;    m_Key[26] = 91;     m_Key[27] = 148;    m_Key[28] = 147;    m_Key[29] = 216;
  m_Key[30] = 116;    m_Key[31] = 46;     m_Key[32] = 50;     m_Key[33] = 174;    m_Key[34] = 232;
  m_Key[35] = 56;     m_Key[36] = 166;    m_Key[37] = 172;    m_Key[38] = 103;    m_Key[39] = 209;
  m_Key[40] = 98;     m_Key[41] = 70;     m_Key[42] = 189;    m_Key[43] = 75;	  m_Key[44] = 151;
  m_Key[45] = 2;      m_Key[46] = 182;    m_Key[47] = 79;     m_Key[48] = 11;     m_Key[49] = 100;
  m_Key[50] = 76;     m_Key[51] = 160;    m_Key[52] = 162;    m_Key[53] = 86;     m_Key[54] = 242;
  m_Key[55] = 6;      m_Key[56] = 183;    m_Key[57] = 63;     m_Key[58] = 64;     m_Key[59] = 154;
  m_Key[60] = 37;     m_Key[61] = 110;    m_Key[62] = 197;    m_Key[63] = 39;     m_Key[64] = 187;
  m_Key[65] = 58;     m_Key[66] = 184;    m_Key[67] = 180;    m_Key[68] = 191;    m_Key[69] = 249;
  m_Key[70] = 69;     m_Key[71] = 181;    m_Key[72] = 199;    m_Key[73] = 7;      m_Key[74] = 21;
  m_Key[75] = 164;    m_Key[76] = 250;    m_Key[77] = 59;     m_Key[78] = 194;    m_Key[79] = 96;
  m_Key[80] = 74;     m_Key[81] = 15;     m_Key[82] = 237;    m_Key[83] = 248;    m_Key[84] = 8;
  m_Key[85] = 142;    m_Key[86] = 203;    m_Key[87] = 149;    m_Key[88] = 146;    m_Key[89] = 47;
  m_Key[90] = 123;    m_Key[91] = 235;    m_Key[92] = 247;    m_Key[93] = 72;     m_Key[94] = 152;
  m_Key[95] = 213;    m_Key[96] = 90;     m_Key[97] = 24;     m_Key[98] = 67;     m_Key[99] = 57;
  m_Key[100] = 129;   m_Key[101] = 198;   m_Key[102] = 93;    m_Key[103] = 192;   m_Key[104] = 214;
  m_Key[105] = 205;   m_Key[106] = 80;    m_Key[107] = 73;    m_Key[108] = 233;   m_Key[109] = 29;
  m_Key[110] = 9;     m_Key[111] = 106;   m_Key[112] = 224;   m_Key[113] = 202;   m_Key[114] = 219;
  m_Key[115] = 107;   m_Key[116] = 28;    m_Key[117] = 218;   m_Key[118] = 212;   m_Key[119] = 140;
  m_Key[120] = 167;   m_Key[121] = 223;   m_Key[122] = 112;   m_Key[123] = 111;   m_Key[124] = 62;
  m_Key[125] = 33;    m_Key[126] = 168;   m_Key[127] = 77;    m_Key[128] = 158;   m_Key[129] = 16;
  m_Key[130] = 169;   m_Key[131] = 25;    m_Key[132] = 104;   m_Key[133] = 19;    m_Key[134] = 210;
  m_Key[135] = 12;    m_Key[136] = 155;   m_Key[137] = 252;   m_Key[138] = 60;    m_Key[139] = 13;
  m_Key[140] = 136;   m_Key[141] = 170;   m_Key[142] = 122;   m_Key[143] = 102;   m_Key[144] = 200;
  m_Key[145] = 44;    m_Key[146] = 126;   m_Key[147] = 188;   m_Key[148] = 201;   m_Key[149] = 225;
  m_Key[150] = 51;    m_Key[151] = 83;    m_Key[152] = 251;   m_Key[153] = 228;   m_Key[154] = 121;
  m_Key[155] = 119;   m_Key[156] = 132;   m_Key[157] = 55;    m_Key[158] = 52;    m_Key[159] = 244;
  m_Key[160] = 177;   m_Key[161] = 195;   m_Key[162] = 238;   m_Key[163] = 65;    m_Key[164] = 185;
  m_Key[165] = 120;   m_Key[166] = 226;   m_Key[167] = 27;    m_Key[168] = 17;    m_Key[169] = 53;
  m_Key[170] = 31;    m_Key[171] = 196;   m_Key[172] = 159;   m_Key[173] = 22;    m_Key[174] = 78;
  m_Key[175] = 68;    m_Key[176] = 1;     m_Key[177] = 115;   m_Key[178] = 145;   m_Key[179] = 84;
  m_Key[180] = 101;   m_Key[181] = 222;   m_Key[182] = 171;   m_Key[183] = 215;   m_Key[184] = 81;
  m_Key[185] = 134;   m_Key[186] = 206;   m_Key[187] = 92;    m_Key[188] = 161;   m_Key[189] = 41;
  m_Key[190] = 130;   m_Key[191] = 229;   m_Key[192] = 157;   m_Key[193] = 108;   m_Key[194] = 87;
  m_Key[195] = 49;    m_Key[196] = 42;    m_Key[197] = 5;     m_Key[198] = 40;    m_Key[199] = 137;
  m_Key[200] = 133;   m_Key[201] = 190;   m_Key[202] = 125;   m_Key[203] = 253;   m_Key[204] = 208;
  m_Key[205] = 127;   m_Key[206] = 221;   m_Key[207] = 139;   m_Key[208] = 246;   m_Key[209] = 124;
  m_Key[210] = 95;    m_Key[211] = 36;    m_Key[212] = 3;     m_Key[213] = 231;   m_Key[214] = 61;
  m_Key[215] = 165;   m_Key[216] = 239;   m_Key[217] = 18;    m_Key[218] = 89;    m_Key[219] = 109;
  m_Key[220] = 14;    m_Key[221] = 153;   m_Key[222] = 38;    m_Key[223] = 186;   m_Key[224] = 117;
  m_Key[225] = 54;    m_Key[226] = 240;   m_Key[227] = 23;    m_Key[228] = 236;   m_Key[229] = 245;
  m_Key[230] = 99;    m_Key[231] = 118;   m_Key[232] = 227;   m_Key[233] = 88;    m_Key[234] = 144;
  m_Key[235] = 241;   m_Key[236] = 82;    m_Key[237] = 4;     m_Key[238] = 193;   m_Key[239] = 255;
  m_Key[240] = 173;   m_Key[241] = 20;    m_Key[242] = 105;   m_Key[243] = 143;   m_Key[244] = 220;
  m_Key[245] = 32;    m_Key[246] = 243;   m_Key[247] = 179;   m_Key[248] = 94;    m_Key[249] = 131;
  m_Key[250] = 230;   m_Key[251] = 114;   m_Key[252] = 43;    m_Key[253] = 211;   m_Key[254] = 97;
  m_Key[255] = 48;

  ZeroMemory(&m_Map, sizeof(m_Map));
  m_pBorder = new CStone();
  m_pBorder->m_iX = 0;
  m_pBorder->m_iY = 0;
  ClearMap();
}


CMap::~CMap()
{
    ClearMap();
    delete m_pBorder;
}


MapID CMap::GetEncryptCode(BYTE db)
{
    for (int i = 0; i<256; i++) {
        if (m_Key[i] == db)
            return (MapID)i;
    }
    return ID_EMPTY;
}


void CMap::ClearMap()
{
    ZeroMemory(&m_Level, sizeof(Map));

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (m_Map[x][y] != NULL) {
                delete m_Map[x][y];
            }
            m_Map[x][y] = NULL;
        }
    }
}


BOOL CMap::LoadMap(const char* pcszName)
{
    FILE* fm = fopen(pcszName, "rb");
    if (fm == 0)
        return FALSE;

    fread(&m_Level, sizeof(Map), 1, fm);
    for (int i = 0; i < sizeof(m_Level.szName); i++) {
        m_Level.szName[i] = GetEncryptCode((BYTE)m_Level.szName[i]);
    }
    for (int i = 0; i < sizeof(m_Level.szSkin); i++) {
        m_Level.szSkin[i] = GetEncryptCode((BYTE)m_Level.szSkin[i]);
    }
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            CGObj* pObj = GetObj(GetEncryptCode(m_Level.pData[y*MAP_WIDTH + x]));
            if (pObj != NULL) {
                pObj->m_iX = x;
                pObj->m_iY = y;
            }
            m_Map[x][y] = pObj;
        }
    }
    fclose(fm);
    return TRUE;
}


CGObj* CMap::GetElement(int x, int y)
{
    if (x < 0 || x >= MAP_WIDTH)
        return m_pBorder;

    if (y < 0 || y >= MAP_HEIGHT)
        return m_pBorder;

    return m_Map[x][y];
}


void CMap::SetElement(int x, int y, CGObj* pObj)
{
    if (x < 0 || x >= MAP_WIDTH)
        return;

    if (y < 0 || y >= MAP_HEIGHT)
        return;

    m_Map[x][y] = pObj;
}