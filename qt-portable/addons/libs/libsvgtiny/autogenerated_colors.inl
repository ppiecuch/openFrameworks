/* ANSI-C code produced by gperf version 3.0.3 */
/* Command-line: /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/gperf --output-file=c.tmp  */
/* Computed positions: -k'1,3,6-8,12-13' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif


#include <string.h>
#include "svgtiny.h"
#include "svgtiny_internal.h"

struct svgtiny_named_color;

#define TOTAL_KEYWORDS 147
#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 20
#define MIN_HASH_VALUE 4
#define MAX_HASH_VALUE 565
/* maximum key range = 562, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
svgtiny_color_hash (register const char *str, register unsigned int len)
{
  static const unsigned short asso_values[] =
    {
      566, 566, 566, 566, 566, 566, 566, 566, 566, 566,
      566, 566, 566, 566, 566, 566, 566, 566, 566, 566,
      566, 566, 566, 566, 566, 566, 566, 566, 566, 566,
      566, 566, 566, 566, 566, 566, 566, 566, 566, 566,
      566, 566, 566, 566, 566, 566, 566, 566, 566, 566,
      566, 566, 566, 566, 566, 566, 566, 566, 566, 566,
      566, 566, 566, 566, 566, 566, 566, 566, 566, 566,
      566, 566, 566, 566, 566, 566, 566, 566, 566, 566,
      566, 566, 566, 566, 566, 566, 566, 566, 566, 566,
      566, 566, 566, 566, 566, 566, 566,   5,  55,   0,
       35,   0,  75,  10,   5,   0, 566, 250,  10,  40,
       85,  60,  70, 144,   0,  20,  45,  10,  30, 185,
       95, 195, 566,   0, 566, 566, 566, 566, 566, 566,
      566, 566, 566, 566, 566, 566, 566, 566, 566, 566,
      566, 566, 566, 566, 566, 566, 566, 566, 566, 566,
      566, 566, 566, 566, 566, 566, 566, 566, 566, 566,
      566, 566, 566, 566, 566, 566, 566, 566, 566, 566,
      566, 566, 566, 566, 566, 566, 566, 566, 566, 566,
      566, 566, 566, 566, 566, 566, 566, 566, 566, 566,
      566, 566, 566, 566, 566, 566, 566, 566, 566, 566,
      566, 566, 566, 566, 566, 566, 566, 566, 566, 566,
      566, 566, 566, 566, 566, 566, 566, 566, 566, 566,
      566, 566, 566, 566, 566, 566, 566, 566, 566, 566,
      566, 566, 566, 566, 566, 566, 566, 566, 566, 566,
      566, 566, 566, 566, 566, 566, 566, 566, 566, 566,
      566, 566, 566, 566, 566, 566, 566, 566
    };
  register unsigned int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[12]];
      /*FALLTHROUGH*/
      case 12:
        hval += asso_values[(unsigned char)str[11]];
      /*FALLTHROUGH*/
      case 11:
      case 10:
      case 9:
      case 8:
        hval += asso_values[(unsigned char)str[7]];
      /*FALLTHROUGH*/
      case 7:
        hval += asso_values[(unsigned char)str[6]];
      /*FALLTHROUGH*/
      case 6:
        hval += asso_values[(unsigned char)str[5]];
      /*FALLTHROUGH*/
      case 5:
      case 4:
      case 3:
        hval += asso_values[(unsigned char)str[2]+2];
      /*FALLTHROUGH*/
      case 2:
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval;
}

static const struct svgtiny_named_color *
svgtiny_color_lookup (register const char *str, register unsigned int len)
{
  static const struct svgtiny_named_color wordlist[] =
    {
      {"cyan",		svgtiny_RGB(  0, 255, 255)},
      {"gray",		svgtiny_RGB(128, 128, 128)},
      {"chartreuse",	svgtiny_RGB(127, 255,   0)},
      {"grey",		svgtiny_RGB(128, 128, 128)},
      {"green",		svgtiny_RGB(  0, 128,   0)},
      {"lightgrey",	svgtiny_RGB(211, 211, 211)},
      {"lightgreen",	svgtiny_RGB(144, 238, 144)},
      {"lightgray",	svgtiny_RGB(211, 211, 211)},
      {"skyblue",	svgtiny_RGB(135, 206, 235)},
      {"slategrey",	svgtiny_RGB(112, 128, 144)},
      {"sienna",		svgtiny_RGB(160,  82,  45)},
      {"slategray",	svgtiny_RGB(112, 128, 144)},
      {"seashell",	svgtiny_RGB(255, 245, 238)},
      {"teal",		svgtiny_RGB(  0, 128, 128)},
      {"coral",		svgtiny_RGB(255, 127,  80)},
      {"lightsalmon",	svgtiny_RGB(255, 160, 122)},
      {"lightslategrey",	svgtiny_RGB(119, 136, 153)},
      {"black",		svgtiny_RGB(  0,   0,   0)},
      {"lightslategray",	svgtiny_RGB(119, 136, 153)},
      {"orange",		svgtiny_RGB(255, 165,   0)},
      {"orangered",	svgtiny_RGB(255,  69,   0)},
      {"bisque",		svgtiny_RGB(255, 228, 196)},
      {"lime",		svgtiny_RGB(  0, 255,   0)},
      {"red",		svgtiny_RGB(255,   0,   0)},
      {"limegreen",	svgtiny_RGB( 50, 205,  50)},
      {"lightcoral",	svgtiny_RGB(240, 128, 128)},
      {"royalblue",	svgtiny_RGB( 65, 105, 225)},
      {"linen",		svgtiny_RGB(250, 240, 230)},
      {"fuchsia",	svgtiny_RGB(255,   0, 255)},
      {"darkgreen",	svgtiny_RGB(  0, 100,   0)},
      {"lightblue",	svgtiny_RGB(173, 216, 230)},
      {"darkorchid",	svgtiny_RGB(153,  50, 204)},
      {"springgreen",	svgtiny_RGB(  0, 255, 127)},
      {"magenta",	svgtiny_RGB(255,   0, 255)},
      {"gold",		svgtiny_RGB(255, 215,   0)},
      {"orchid",		svgtiny_RGB(218, 112, 214)},
      {"slateblue",	svgtiny_RGB(106,  90, 205)},
      {"darkmagenta",	svgtiny_RGB(139, 0,   139)},
      {"darkblue",	svgtiny_RGB(  0,   0, 139)},
      {"lightsteelblue",	svgtiny_RGB(176, 196, 222)},
      {"silver",		svgtiny_RGB(192, 192, 192)},
      {"seagreen",	svgtiny_RGB( 46, 139,  87)},
      {"steelblue",	svgtiny_RGB( 70, 130, 180)},
      {"tan",		svgtiny_RGB(210, 180, 140)},
      {"peru",		svgtiny_RGB(205, 133,  63)},
      {"purple",		svgtiny_RGB(128,   0, 128)},
      {"darkred",	svgtiny_RGB(139,   0,   0)},
      {"mintcream",	svgtiny_RGB(245, 255, 250)},
      {"firebrick",	svgtiny_RGB(178,  34,  34)},
      {"lightseagreen",	svgtiny_RGB( 32, 178, 170)},
      {"darkolivegreen",	svgtiny_RGB( 85, 107,  47)},
      {"mistyrose",	svgtiny_RGB(255, 228, 225)},
      {"indigo",		svgtiny_RGB( 75,   0, 130)},
      {"oldlace",	svgtiny_RGB(253, 245, 230)},
      {"pink",		svgtiny_RGB(255, 192, 203)},
      {"darksalmon",	svgtiny_RGB(233, 150, 122)},
      {"lavender",	svgtiny_RGB(230, 230, 250)},
      {"ivory",		svgtiny_RGB(255, 255, 240)},
      {"moccasin",	svgtiny_RGB(255, 228, 181)},
      {"cadetblue",	svgtiny_RGB( 95, 158, 160)},
      {"darkviolet",	svgtiny_RGB(148,   0, 211)},
      {"saddlebrown",	svgtiny_RGB(139,  69,  19)},
      {"darkslateblue",	svgtiny_RGB( 72,  61, 139)},
      {"palegreen",	svgtiny_RGB(152, 251, 152)},
      {"snow",		svgtiny_RGB(255, 250, 250)},
      {"indianred",	svgtiny_RGB(205,  92,  92)},
      {"lightgoldenrodyellow",	svgtiny_RGB(250, 250, 210)},
      {"tomato",		svgtiny_RGB(255,  99,  71)},
      {"lemonchiffon",	svgtiny_RGB(255, 250, 205)},
      {"lightpink",	svgtiny_RGB(255, 182, 193)},
      {"maroon",		svgtiny_RGB(128,   0,   0)},
      {"lavenderblush",	svgtiny_RGB(255, 240, 245)},
      {"turquoise",	svgtiny_RGB( 64, 224, 208)},
      {"darkorange",	svgtiny_RGB(255, 140,   0)},
      {"navy",		svgtiny_RGB(  0,   0, 128)},
      {"dodgerblue",	svgtiny_RGB( 30, 144, 255)},
      {"forestgreen",	svgtiny_RGB( 34, 139,  34)},
      {"midnightblue",	svgtiny_RGB( 25,  25, 112)},
      {"mediumseagreen",	svgtiny_RGB( 60, 179, 113)},
      {"darkseagreen",	svgtiny_RGB(143, 188, 143)},
      {"aqua",		svgtiny_RGB(  0, 255, 255)},
      {"azure",		svgtiny_RGB(240, 255, 255)},
      {"salmon",		svgtiny_RGB(250, 128, 114)},
      {"wheat",		svgtiny_RGB(245, 222, 179)},
      {"brown",		svgtiny_RGB(165,  42,  42)},
      {"aquamarine",	svgtiny_RGB(127, 255, 212)},
      {"chocolate",	svgtiny_RGB(210, 105,  30)},
      {"lawngreen",	svgtiny_RGB(124, 252,   0)},
      {"sandybrown",	svgtiny_RGB(244, 164,  96)},
      {"lightcyan",	svgtiny_RGB(224, 255, 255)},
      {"violet",		svgtiny_RGB(238, 130, 238)},
      {"lightyellow",	svgtiny_RGB(255, 255, 224)},
      {"mediumblue",	svgtiny_RGB(  0,   0, 205)},
      {"peachpuff",	svgtiny_RGB(255, 218, 185)},
      {"greenyellow",	svgtiny_RGB(173, 255,  47)},
      {"antiquewhite",	svgtiny_RGB(250, 235, 215)},
      {"blue",		svgtiny_RGB(  0,   0, 255)},
      {"mediumvioletred",	svgtiny_RGB(199,  21, 133)},
      {"mediumpurple",	svgtiny_RGB(147, 112, 219)},
      {"goldenrod",	svgtiny_RGB(218, 165,  32)},
      {"blanchedalmond",	svgtiny_RGB(255, 235, 205)},
      {"khaki",		svgtiny_RGB(240, 230, 140)},
      {"plum",		svgtiny_RGB(221, 160, 221)},
      {"mediumorchid",	svgtiny_RGB(186,  85, 211)},
      {"rosybrown",	svgtiny_RGB(188, 143, 143)},
      {"mediumslateblue",	svgtiny_RGB(123, 104, 238)},
      {"darkturquoise",	svgtiny_RGB(  0, 206, 209)},
      {"palevioletred",	svgtiny_RGB(219, 112, 147)},
      {"papayawhip",	svgtiny_RGB(255, 239, 213)},
      {"mediumspringgreen",	svgtiny_RGB(  0, 250, 154)},
      {"darkgrey",	svgtiny_RGB(169, 169, 169)},
      {"mediumturquoise",	svgtiny_RGB( 72, 209, 204)},
      {"darkgray",	svgtiny_RGB(169, 169, 169)},
      {"darkgoldenrod",	svgtiny_RGB(184, 134,  11)},
      {"dimgrey",	svgtiny_RGB(105, 105, 105)},
      {"dimgray",	svgtiny_RGB(105, 105, 105)},
      {"honeydew",	svgtiny_RGB(240, 255, 240)},
      {"beige",		svgtiny_RGB(245, 245, 220)},
      {"thistle",	svgtiny_RGB(216, 191, 216)},
      {"cornsilk",	svgtiny_RGB(255, 248, 220)},
      {"olive",		svgtiny_RGB(128, 128,   0)},
      {"blueviolet",	svgtiny_RGB(138,  43, 226)},
      {"mediumaquamarine",	svgtiny_RGB(102, 205, 170)},
      {"cornflowerblue",	svgtiny_RGB(100, 149, 237)},
      {"aliceblue",	svgtiny_RGB(240, 248, 255)},
      {"powderblue",	svgtiny_RGB(176, 224, 230)},
      {"paleturquoise",	svgtiny_RGB(175, 238, 238)},
      {"darkslategrey",	svgtiny_RGB( 47,  79,  79)},
      {"darkkhaki",	svgtiny_RGB(189, 183, 107)},
      {"darkslategray",	svgtiny_RGB( 47,  79,  79)},
      {"ghostwhite",	svgtiny_RGB(248, 248, 255)},
      {"olivedrab",	svgtiny_RGB(107, 142,  35)},
      {"palegoldenrod",	svgtiny_RGB(238, 232, 170)},
      {"darkcyan",	svgtiny_RGB(  0, 139, 139)},
      {"hotpink",	svgtiny_RGB(255, 105, 180)},
      {"gainsboro",	svgtiny_RGB(220, 220, 220)},
      {"deeppink",	svgtiny_RGB(255,  20, 147)},
      {"crimson",	svgtiny_RGB(220,  20,  60)},
      {"burlywood",	svgtiny_RGB(222, 184, 135)},
      {"floralwhite",	svgtiny_RGB(255, 250, 240)},
      {"white",		svgtiny_RGB(255, 255, 255)},
      {"navajowhite",	svgtiny_RGB(255, 222, 173)},
      {"yellow",		svgtiny_RGB(255, 255,   0)},
      {"yellowgreen",	svgtiny_RGB(154, 205,  50)},
      {"lightskyblue",	svgtiny_RGB(135, 206, 250)},
      {"deepskyblue",	svgtiny_RGB(  0, 191, 255)},
      {"whitesmoke",	svgtiny_RGB(245, 245, 245)}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      unsigned int key = svgtiny_color_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= MIN_HASH_VALUE)
        {
          register const struct svgtiny_named_color *resword;

          switch (key - 4)
            {
              case 0:
                resword = &wordlist[0];
                goto compare;
              case 10:
                resword = &wordlist[1];
                goto compare;
              case 16:
                resword = &wordlist[2];
                goto compare;
              case 20:
                resword = &wordlist[3];
                goto compare;
              case 21:
                resword = &wordlist[4];
                goto compare;
              case 25:
                resword = &wordlist[5];
                goto compare;
              case 26:
                resword = &wordlist[6];
                goto compare;
              case 30:
                resword = &wordlist[7];
                goto compare;
              case 33:
                resword = &wordlist[8];
                goto compare;
              case 35:
                resword = &wordlist[9];
                goto compare;
              case 37:
                resword = &wordlist[10];
                goto compare;
              case 40:
                resword = &wordlist[11];
                goto compare;
              case 44:
                resword = &wordlist[12];
                goto compare;
              case 45:
                resword = &wordlist[13];
                goto compare;
              case 46:
                resword = &wordlist[14];
                goto compare;
              case 52:
                resword = &wordlist[15];
                goto compare;
              case 55:
                resword = &wordlist[16];
                goto compare;
              case 56:
                resword = &wordlist[17];
                goto compare;
              case 60:
                resword = &wordlist[18];
                goto compare;
              case 62:
                resword = &wordlist[19];
                goto compare;
              case 65:
                resword = &wordlist[20];
                goto compare;
              case 67:
                resword = &wordlist[21];
                goto compare;
              case 70:
                resword = &wordlist[22];
                goto compare;
              case 74:
                resword = &wordlist[23];
                goto compare;
              case 75:
                resword = &wordlist[24];
                goto compare;
              case 76:
                resword = &wordlist[25];
                goto compare;
              case 80:
                resword = &wordlist[26];
                goto compare;
              case 81:
                resword = &wordlist[27];
                goto compare;
              case 83:
                resword = &wordlist[28];
                goto compare;
              case 85:
                resword = &wordlist[29];
                goto compare;
              case 90:
                resword = &wordlist[30];
                goto compare;
              case 91:
                resword = &wordlist[31];
                goto compare;
              case 92:
                resword = &wordlist[32];
                goto compare;
              case 93:
                resword = &wordlist[33];
                goto compare;
              case 95:
                resword = &wordlist[34];
                goto compare;
              case 97:
                resword = &wordlist[35];
                goto compare;
              case 100:
                resword = &wordlist[36];
                goto compare;
              case 102:
                resword = &wordlist[37];
                goto compare;
              case 104:
                resword = &wordlist[38];
                goto compare;
              case 105:
                resword = &wordlist[39];
                goto compare;
              case 107:
                resword = &wordlist[40];
                goto compare;
              case 109:
                resword = &wordlist[41];
                goto compare;
              case 110:
                resword = &wordlist[42];
                goto compare;
              case 114:
                resword = &wordlist[43];
                goto compare;
              case 115:
                resword = &wordlist[44];
                goto compare;
              case 117:
                resword = &wordlist[45];
                goto compare;
              case 118:
                resword = &wordlist[46];
                goto compare;
              case 120:
                resword = &wordlist[47];
                goto compare;
              case 125:
                resword = &wordlist[48];
                goto compare;
              case 129:
                resword = &wordlist[49];
                goto compare;
              case 130:
                resword = &wordlist[50];
                goto compare;
              case 135:
                resword = &wordlist[51];
                goto compare;
              case 137:
                resword = &wordlist[52];
                goto compare;
              case 138:
                resword = &wordlist[53];
                goto compare;
              case 140:
                resword = &wordlist[54];
                goto compare;
              case 141:
                resword = &wordlist[55];
                goto compare;
              case 144:
                resword = &wordlist[56];
                goto compare;
              case 145:
                resword = &wordlist[57];
                goto compare;
              case 149:
                resword = &wordlist[58];
                goto compare;
              case 155:
                resword = &wordlist[59];
                goto compare;
              case 156:
                resword = &wordlist[60];
                goto compare;
              case 157:
                resword = &wordlist[61];
                goto compare;
              case 159:
                resword = &wordlist[62];
                goto compare;
              case 160:
                resword = &wordlist[63];
                goto compare;
              case 164:
                resword = &wordlist[64];
                goto compare;
              case 165:
                resword = &wordlist[65];
                goto compare;
              case 166:
                resword = &wordlist[66];
                goto compare;
              case 167:
                resword = &wordlist[67];
                goto compare;
              case 168:
                resword = &wordlist[68];
                goto compare;
              case 170:
                resword = &wordlist[69];
                goto compare;
              case 172:
                resword = &wordlist[70];
                goto compare;
              case 174:
                resword = &wordlist[71];
                goto compare;
              case 175:
                resword = &wordlist[72];
                goto compare;
              case 176:
                resword = &wordlist[73];
                goto compare;
              case 180:
                resword = &wordlist[74];
                goto compare;
              case 181:
                resword = &wordlist[75];
                goto compare;
              case 182:
                resword = &wordlist[76];
                goto compare;
              case 183:
                resword = &wordlist[77];
                goto compare;
              case 185:
                resword = &wordlist[78];
                goto compare;
              case 188:
                resword = &wordlist[79];
                goto compare;
              case 190:
                resword = &wordlist[80];
                goto compare;
              case 191:
                resword = &wordlist[81];
                goto compare;
              case 192:
                resword = &wordlist[82];
                goto compare;
              case 196:
                resword = &wordlist[83];
                goto compare;
              case 200:
                resword = &wordlist[84];
                goto compare;
              case 201:
                resword = &wordlist[85];
                goto compare;
              case 209:
                resword = &wordlist[86];
                goto compare;
              case 210:
                resword = &wordlist[87];
                goto compare;
              case 211:
                resword = &wordlist[88];
                goto compare;
              case 215:
                resword = &wordlist[89];
                goto compare;
              case 221:
                resword = &wordlist[90];
                goto compare;
              case 222:
                resword = &wordlist[91];
                goto compare;
              case 226:
                resword = &wordlist[92];
                goto compare;
              case 230:
                resword = &wordlist[93];
                goto compare;
              case 232:
                resword = &wordlist[94];
                goto compare;
              case 238:
                resword = &wordlist[95];
                goto compare;
              case 240:
                resword = &wordlist[96];
                goto compare;
              case 241:
                resword = &wordlist[97];
                goto compare;
              case 243:
                resword = &wordlist[98];
                goto compare;
              case 245:
                resword = &wordlist[99];
                goto compare;
              case 250:
                resword = &wordlist[100];
                goto compare;
              case 251:
                resword = &wordlist[101];
                goto compare;
              case 255:
                resword = &wordlist[102];
                goto compare;
              case 258:
                resword = &wordlist[103];
                goto compare;
              case 260:
                resword = &wordlist[104];
                goto compare;
              case 261:
                resword = &wordlist[105];
                goto compare;
              case 263:
                resword = &wordlist[106];
                goto compare;
              case 269:
                resword = &wordlist[107];
                goto compare;
              case 271:
                resword = &wordlist[108];
                goto compare;
              case 278:
                resword = &wordlist[109];
                goto compare;
              case 279:
                resword = &wordlist[110];
                goto compare;
              case 281:
                resword = &wordlist[111];
                goto compare;
              case 284:
                resword = &wordlist[112];
                goto compare;
              case 289:
                resword = &wordlist[113];
                goto compare;
              case 293:
                resword = &wordlist[114];
                goto compare;
              case 298:
                resword = &wordlist[115];
                goto compare;
              case 299:
                resword = &wordlist[116];
                goto compare;
              case 306:
                resword = &wordlist[117];
                goto compare;
              case 308:
                resword = &wordlist[118];
                goto compare;
              case 309:
                resword = &wordlist[119];
                goto compare;
              case 311:
                resword = &wordlist[120];
                goto compare;
              case 316:
                resword = &wordlist[121];
                goto compare;
              case 321:
                resword = &wordlist[122];
                goto compare;
              case 330:
                resword = &wordlist[123];
                goto compare;
              case 335:
                resword = &wordlist[124];
                goto compare;
              case 336:
                resword = &wordlist[125];
                goto compare;
              case 338:
                resword = &wordlist[126];
                goto compare;
              case 344:
                resword = &wordlist[127];
                goto compare;
              case 345:
                resword = &wordlist[128];
                goto compare;
              case 349:
                resword = &wordlist[129];
                goto compare;
              case 350:
                resword = &wordlist[130];
                goto compare;
              case 355:
                resword = &wordlist[131];
                goto compare;
              case 364:
                resword = &wordlist[132];
                goto compare;
              case 369:
                resword = &wordlist[133];
                goto compare;
              case 373:
                resword = &wordlist[134];
                goto compare;
              case 380:
                resword = &wordlist[135];
                goto compare;
              case 384:
                resword = &wordlist[136];
                goto compare;
              case 398:
                resword = &wordlist[137];
                goto compare;
              case 410:
                resword = &wordlist[138];
                goto compare;
              case 426:
                resword = &wordlist[139];
                goto compare;
              case 436:
                resword = &wordlist[140];
                goto compare;
              case 437:
                resword = &wordlist[141];
                goto compare;
              case 467:
                resword = &wordlist[142];
                goto compare;
              case 482:
                resword = &wordlist[143];
                goto compare;
              case 483:
                resword = &wordlist[144];
                goto compare;
              case 552:
                resword = &wordlist[145];
                goto compare;
              case 561:
                resword = &wordlist[146];
                goto compare;
            }
          return 0;
        compare:
          {
            register const char *s = resword->name;

            if (*str == *s && !strcmp (str + 1, s + 1))
              return resword;
          }
        }
    }
  return 0;
}
