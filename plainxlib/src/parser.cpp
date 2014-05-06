#include "plainx.h"
#include "parser.h"


CParser::CParser()
{
  m_pFile = NULL;
  m_bOpened = FALSE;
  m_bIsCrypted = FALSE;
  m_vars.clear();
  
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
}


CParser::~CParser()
{
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CParser::Open(const char* pszFilename)
{
  // check for crypt
  m_bIsCrypted = p_CheckForCrypt(pszFilename);
  
  // encrypt file to temp file
  if (!p_Encrypt(pszFilename))
    return FALSE;

  // open temp file
  m_pFile = fopen(m_tmpname, "r");
  if (!m_pFile)
    return FALSE;

  // read temp file and fill vars struct
  char pszGroupLabel[MAX_BUFFER + 1];
  char pszGroupName[MAX_BUFFER + 1];
  char szFullGroupName[MAX_BUFFER];
  GROUP_VAR* pGroupVar;

  // go to first section
  if (p_GoToSection(pszGroupLabel, pszGroupName, TRUE)) {

	// create var list for first group
    pGroupVar = new GROUP_VAR;//(GROUP_VAR*)malloc(sizeof(GROUP_VAR));
    //memset(pGroupVar, 0, sizeof(GROUP_VAR));
	pGroupVar->vList.clear();

    strcpy(pGroupVar->szGroupName, pszGroupName);
    strcpy(pGroupVar->szGroupLabel, pszGroupLabel);

    // make full group name
    wsprintf(szFullGroupName, "%s_%s", pszGroupLabel, pszGroupName);

    // add group var to list
    m_vars[szFullGroupName] = pGroupVar;
    p_ReadSection(szFullGroupName);

    while (p_GoToSection(pszGroupLabel, pszGroupName)) //read other sections
    {
      pGroupVar = new GROUP_VAR;//(GROUP_VAR*)malloc(sizeof(GROUP_VAR));
      //memset(pGroupVar, 0, sizeof(GROUP_VAR));
	  pGroupVar->vList.clear();

      strcpy(pGroupVar->szGroupName, pszGroupName);
      strcpy(pGroupVar->szGroupLabel, pszGroupLabel);
      wsprintf(szFullGroupName, "%s_%s", pszGroupLabel, pszGroupName);
      m_vars[szFullGroupName] = pGroupVar;
      p_ReadSection(szFullGroupName);
    }
  }
  fclose(m_pFile); //close temp file
  DeleteFile(m_tmpname); //delete temp file

  m_bOpened = TRUE;

  return TRUE;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CParser::Close()
{
  if (!m_bOpened)
    return;

  // flush vars to temp file
  p_Flush();

  // crypt temp file and copy to origin
  p_Crypt();

  // free vars struct
  for (GROUP_VAR_LIST_ITERATOR i = m_vars.begin(); i != m_vars.end(); i++) {
    GROUP_VAR* pgv = (GROUP_VAR*)(*i).second;
    if (pgv) {
      for (VAR_LIST_ITERATOR j = pgv->vList.begin(); j != pgv->vList.end(); j++) {
        VAR* pv = (VAR*)(*j).second;
		if (pv) free(pv);
      }
      pgv->vList.clear();
      //free(pgv);
	  delete pgv;
    }
  }
  m_vars.clear();

  m_bOpened = FALSE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CParser::GetVar(const char* pszGroupLabel, 
                     const char* pszGroupName, 
                     const char* pszVarName, 
                     char* pszVarValue)
{
  char szFullGroupName[MAX_BUFFER];

  wsprintf(szFullGroupName, "%s_%s", pszGroupLabel, pszGroupName);
  GROUP_VAR* pgv = m_vars[szFullGroupName];
  if (pgv) {
    VAR* pv = pgv->vList[pszVarName];
    if (pv) {
      strcpy(pszVarValue, pv->szVarValue);
      return TRUE;
    }
  }
  return FALSE;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
int CParser::GetVarCount(const char* pszGroupLabel,
                         const char* pszGroupName,
                         const char* pszVarName)
{
  int nVarCount = 0;
  char szFullGroupName[MAX_BUFFER];

  wsprintf(szFullGroupName, "%s_%s", pszGroupLabel, pszGroupName);
  GROUP_VAR* pgv = m_vars[szFullGroupName];
  if (pgv) {
    VAR* pv = pgv->vList[pszVarName];
    if (pv) {
      nVarCount = 1;
      char* pTemp = pv->szVarValue;
      pTemp = strstr(pTemp, P_COMMA);
      while (pTemp) {
        nVarCount++;
	pTemp = strstr(pTemp, P_COMMA);
      }
    }
  }
  return nVarCount;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CParser::SetVar(const char* pszGroupLabel,
                     const char* pszGroupName,
                     const char* pszVarName,
                     char* pszVarValue)
{
  char szFullGroupName[MAX_BUFFER];

  wsprintf(szFullGroupName, "%s_%s", pszGroupLabel, pszGroupName);
  GROUP_VAR* pgv = m_vars[szFullGroupName];
  if (pgv) {
    VAR* pv = pgv->vList[pszVarName];
    if (pv) {
      strcpy(pv->szVarValue, pszVarValue);
      return TRUE;
    }
  }
  return FALSE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CParser::GetNextGroup(const char* pszGroupLabel,
                           char* pszGroupName,
                           BOOL bIsFirst)
{
  if (bIsFirst)
    m_ivars = m_vars.begin();

  for (GROUP_VAR_LIST_ITERATOR i = m_ivars; i != m_vars.end(); i++) {
    GROUP_VAR* pgv = (GROUP_VAR*)(*i).second;
    if (pgv) {
      if (strcmp(pgv->szGroupLabel, pszGroupLabel) == 0) {
        strcpy(pszGroupName, pgv->szGroupName);
		m_ivars = ++i;
		return TRUE;
      }
    }
  }
  return FALSE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CParser::p_GoToSection(char* pszGroupLabel,
                            char* pszGroupName,
                            BOOL bFirst)
{
  if (bFirst)
    rewind(m_pFile);

  char buffer[MAX_BUFFER];

  while (!feof(m_pFile)) {
    if (fgets(buffer, sizeof(buffer), m_pFile) == NULL)
      return FALSE;

    p_TrimBuffer(buffer);

    if (strstr(buffer, P_COMMENT) == buffer)
      continue;

    if (strstr(buffer, P_BEGIN) == buffer) {
      char* pLabel = strstr(buffer, P_DELIMETER);
      if (pLabel == NULL)
        return FALSE;

      pLabel++;
      char* pName = strstr(pLabel, P_COMMA);
      if (pName == NULL)
        return FALSE;

      pName[0] = 0;
      pName++;

      p_TrimBuffer(pLabel);
      p_TrimBuffer(pName);
      strcpy(pszGroupLabel, pLabel);
      strcpy(pszGroupName, pName);
      return TRUE;
    }
  }
  return FALSE;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CParser::p_ReadSection(const char* pszFullGroupName)
{
  char buffer[MAX_BUFFER];

  while (!feof(m_pFile)) {
    if (fgets(buffer, sizeof(buffer), m_pFile) == NULL)
      return;

    p_TrimBuffer(buffer);

    if (strstr(buffer, P_COMMENT) == buffer)
      continue;

    if (strstr(buffer, P_END) == buffer)
      break;

    if (strstr(buffer, P_DELIMETER) != NULL) {
      char* pValue = strstr(buffer, P_DELIMETER);
      if (pValue == NULL)
        return;

      pValue[0] = 0;
      pValue++;

      p_TrimBuffer(pValue);
      p_TrimBuffer(buffer);

      VAR* pv = (VAR*)malloc(sizeof(VAR));
      strcpy(pv->szVarName, buffer);
      strcpy(pv->szVarValue, pValue);
      GROUP_VAR* pgv = m_vars[pszFullGroupName];
      if (pgv) pgv->vList[pv->szVarName] = pv;
    }
  }
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CParser::p_TrimBuffer(char* buffer)
{
  int l = strlen(buffer);
  int i = 0;

  while ((buffer[i] >= 0) && (buffer[i] < 33) && i < l)
    i++;

  for (int j = 0; j < l; j++) {
    if ((i + j) < l)
      buffer[j] = buffer[i + j];
    else
      buffer[j] = 0;
  }

  for (i = 0; i < l; i++) {
    if ((buffer[l - i] < 33) && (buffer[l - i] >= 0))
      buffer[l - i] = 0;
    else
      break;
  }
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BYTE CParser::p_GetCCode(BYTE nCode)
{
  return m_Key[nCode];
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CParser::p_Flush()
{
  // free vars struct
  FILE* ftemp = fopen(m_tmpname, "w");
  for (GROUP_VAR_LIST_ITERATOR i = m_vars.begin(); i != m_vars.end(); i++) {
    GROUP_VAR* pgv = (GROUP_VAR*)(*i).second;
    if (pgv) {
      fprintf(ftemp, "%s=%s,%s\n", P_BEGIN, pgv->szGroupLabel, pgv->szGroupName);
      for (VAR_LIST_ITERATOR j = pgv->vList.begin(); j != pgv->vList.end(); j++) {
        VAR* pv = (VAR*)(*j).second;
        if (pv) fprintf(ftemp, " %s=%s\n", pv->szVarName, pv->szVarValue);
      }
      fprintf(ftemp, "%s\n\n", P_END);
    }
  }
  fclose(ftemp);
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CParser::p_Crypt()
{
  HANDLE hCrypt = CreateFile(m_tmpname,
                             GENERIC_READ,
                             0,
                             NULL,
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL,
                             0);
  if (hCrypt == INVALID_HANDLE_VALUE)
    return;

  DWORD dwSize = GetFileSize(hCrypt, NULL);
  DWORD dwReaded;

  BYTE* buf = (BYTE*)malloc(dwSize);
  if (buf == NULL) {
    CloseHandle(hCrypt);
    return;
  }

  if (!ReadFile(hCrypt, buf, dwSize, &dwReaded, 0) || dwReaded != dwSize) {
    CloseHandle(hCrypt);
    free(buf);
    return;
  }

  if (m_bIsCrypted) {
    for (DWORD i = 0; i < dwSize; i++)
      buf[i] = p_GetCCode(buf[i]);
  }

  HANDLE hOut = CreateFile(m_origname,
                           GENERIC_WRITE,
                           0,
                           NULL,
                           CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL,
                           0);
  if (hOut == INVALID_HANDLE_VALUE) {
    free(buf);
    CloseHandle(hCrypt);
    return;
  }

  if (m_bIsCrypted) {
    WriteFile(hOut, "PXC", 3, &dwReaded, 0);
  }
  WriteFile(hOut, buf, dwSize, &dwReaded, 0);

  CloseHandle(hOut);
  CloseHandle(hCrypt);
  DeleteFile(m_tmpname);
  free(buf);
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BYTE CParser::p_GetECode(BYTE nCode)
{
  for (BYTE i = 0; i < 256; i++) {
    if (m_Key[i] == nCode)
      return i;
  }
  return 0;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CParser::p_Encrypt(const char* pszFilename)
{
  HANDLE hCrypt = CreateFile(pszFilename,
                             GENERIC_READ,
                             0,
                             NULL,
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL,
                             0);
  if (hCrypt == INVALID_HANDLE_VALUE)
    return FALSE;

  DWORD dwSize = GetFileSize(hCrypt, NULL);
  DWORD dwReaded;

  if (m_bIsCrypted) {
    dwSize -= 3;
  }

  BYTE* buf = (BYTE*)malloc(dwSize);
  if (buf == NULL) {
    CloseHandle(hCrypt);
    return FALSE;
  }

  if (m_bIsCrypted) {
    ReadFile(hCrypt, buf, 3, &dwReaded, 0);
  }

  if (!ReadFile(hCrypt, buf, dwSize, &dwReaded, 0) || dwReaded != dwSize) {
    CloseHandle(hCrypt);
    free(buf);
    return FALSE;
  }

  if (m_bIsCrypted) {
    for (DWORD i = 0; i < dwSize; i++)
      buf[i] = p_GetECode(buf[i]);
  }

  char szTmpName[_MAX_PATH];
  char szTmpPath[_MAX_PATH];
  GetTempPath(_MAX_PATH, szTmpPath);
  GetTempFileName(szTmpPath, "pxc", 0, szTmpName);
  HANDLE hOut = CreateFile(szTmpName, 
                           GENERIC_WRITE, 
                           0, 
                           NULL,
                           CREATE_ALWAYS, 
                           FILE_ATTRIBUTE_NORMAL, 
                           0);
  if (hOut == INVALID_HANDLE_VALUE) {
    free(buf);
    CloseHandle(hCrypt);
    return FALSE;
  }

  if (!WriteFile(hOut, buf, dwSize, &dwReaded, 0) || dwReaded != dwSize) {
    CloseHandle(hOut);
    CloseHandle(hCrypt);
    DeleteFile(szTmpName);
    free(buf);
    return FALSE;
  }

  CloseHandle(hOut);
  CloseHandle(hCrypt);
  free(buf);

  strcpy(m_tmpname, szTmpName);
  strcpy(m_origname, pszFilename);

  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CParser::p_CheckForCrypt(const char* pszFilename)
{
  FILE* ftmp;
  char szCryptLabel[4];

  if (pszFilename == 0)
    return FALSE;

  ftmp = fopen(pszFilename, "rb");
  if (ftmp == 0)
    return FALSE;

  memset(szCryptLabel, 0, 4);
  fread(szCryptLabel, 1, 3, ftmp);
  fclose(ftmp);

  if (strcmp(szCryptLabel, "PXC") == 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}
