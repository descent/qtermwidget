%{
/*
 * for terminal controll sequence
 *
 *
 */

#define PRINT_YYTEXT(state) \
  printf("state : %s\n", state); \
  for (int i=0; i < yyleng; ++i) \
  { \
    if (yytext[i]==0x1b) \
      printf("^["); \
    else \
      printf("%c", yytext[i]); \
  }; \
  printf("\tyyleng: %d\n", yyleng);

int get_hex(char c,char hex[])
{
  char ascii[]="0123456789abcdef";
  int low=(c & 0x0f);
  int high= ((c >> 4) & 0x0f);
  hex[0]=ascii[high];
  hex[1]=ascii[low];
}
#if 0
http://www.chinalinuxpub.com/read.php?wid=99

Linuxµ{¦¡³]­p¤Jªù - termios/keymap/terminal

µo¥¬©ó2005-05-28 ³QÅª173¦¸ ¡i¦rÅé¡G¤j ¤¤ ¤p¡j

Linuxµ{¦¡³]­p¤Jªù - termios/keymap/terminal programming
termios
int tcgetattr (int fd, struct termios *termios_p);
int tcsetattr (int fd, int optional_actions,const struct termios
*termios_p);
keymap
§Ú¼g¤F¤@­Ó¤pµ{¦¡¨Ó±Mªù³B²zLinux¤Wªºkeymap¡Akeymap.h¤Îkeymap.c¡C
¦bLinux Terminal¤W¡A¦pªG±z·Q­n³]©w¬Y¨Ç«öÁäªð¦^¯S©w­È¡A±z·|»Ý­n¥Î¨ì¥H¤U³o¨Ç§Þ¥©¡C
³]©wkeymap
#include
#include
void setkeymap(void)
{
struct kbentry KEYMAP;
KEYMAP.kb_table=STATE;
KEYMAP.kb_index=SCANCODE;
KEYMAP.kb_value=VALUE;
ioctl(console,KDSKBENT,&KEYMAP);
}
STATE¬°ª¬ºAÁä²Õ¦X
/usr/include/linux/keyboard.h¤¤
#define KG_SHIFT 0
#define KG_CTRL 2
#define KG_ALT 3
#define KG_ALTGR 1
#define KG_SHIFTL 4
#define KG_SHIFTR 5
#define KG_CTRLL 6
#define KG_CTRLR 7
#define KG_CAPSSHIFT 8
¨Ï¥Î¤è¦¡¦p:
#define KST_CTRL (1< #define KST_ALT (1< #define KST_SHIFT (1< #define KST_CTRL_ALT (KST_CTRL|KST_ALT)
#define KST_ALT_SHIFT (KST_ALT|KST_SHIFT)
SCANCODE¬°Áä½L±½´y½X
#define SCAN_ESC 0x01
#define SCAN_1 0x02
#define SCAN_2 0x03
#define SCAN_3 0x04
#define SCAN_4 0x05
#define SCAN_5 0x06
#define SCAN_6 0x07
#define SCAN_7 0x08
#define SCAN_8 0x09
#define SCAN_9 0x0A
#define SCAN_0 0x0B
#define SCAN_MINUS 0x0C
#define SCAN_PLUS 0x0D
#define SCAN_BACK 0x0E
#define SCAN_TAB 0x0F
#define SCAN_Q 0x10
#define SCAN_W 0x11
#define SCAN_E 0x12
#define SCAN_R 0x13
#define SCAN_T 0x14
#define SCAN_Y 0x15
#define SCAN_U 0x16
#define SCAN_I 0x17
#define SCAN_O 0x18
#define SCAN_P 0x19
#define SCAN_LTQUOTE 0x1A
#define SCAN_RTQUOTE 0x1B
#define SCAN_ENTER 0x1C
#define SCAN_CTRL 0x1D
#define SCAN_A 0x1E
#define SCAN_S 0x1F
#define SCAN_D 0x20
#define SCAN_F 0x21
#define SCAN_G 0x22
#define SCAN_H 0x23
#define SCAN_J 0x24
#define SCAN_K 0x25
#define SCAN_L 0x26
#define SCAN_SPLIT 0x27
#define SCAN_QUOTE 0x28
#define SCAN_MARK 0x29
#define SCAN_LSHIFT 0x2A
#define SCAN_STAND 0x2B
#define SCAN_Z 0x2C
#define SCAN_X 0x2D
#define SCAN_C 0x2E
#define SCAN_V 0x2F
#define SCAN_B 0x30
#define SCAN_N 0x31
#define SCAN_M 0x32
#define SCAN_LSQUOTE 0x33
#define SCAN_RSQUOTE 0x34
#define SCAN_QUESTION 0x35
#define SCAN_RSHIFT 0x36
#define SCAN_PRTSCR 0x37
#define SCAN_ALT 0x38
#define SCAN_SPACE 0x39
#define SCAN_CAPSLOCK 0x3A
#define SCAN_F1 0x3B
#define SCAN_F2 0x3C
#define SCAN_F3 0x3D
#define SCAN_F4 0x3E
#define SCAN_F5 0x3F
#define SCAN_F6 0x40
#define SCAN_F7 0x41
#define SCAN_F8 0x42
#define SCAN_F9 0x43
#define SCAN_F10 0x44
#define SCAN_NUMLOCK 0x45
#define SCAN_HOME 0x47
#define SCAN_UP 0x48
#define SCAN_PGUP 0x49
#define SCAN_LEFT 0x4B
#define SCAN_RIGHT 0x4D
#define SCAN_END 0x4F
#define SCAN_DOWN 0x50
#define SCAN_PGDN 0x51
#define SCAN_INSERT 0x52
#define SCAN_DELETE 0x53
#define SCAN_F11 0x85
#define SCAN_F12 0x86
/usr/include/linux/kd.h¤¤
struct kbentry {
unsigned char kb_table;
unsigned char kb_index;
unsigned short kb_value;
};
#define KDGKBENT 0x4B46 /* gets one entry in translation
table */
#define KDSKBENT 0x4B47 /* sets one entry in translation
table */
¦Óconsole¬°
console = open("/dev/console",O_RDWR);
Åª¨ú«öÁä
read(console,&c,sizeof(char));

terminal programming
term.h/term.c¬O§Ú¼g¨Ó±Mªù³B²z¤@¨Ç¤p«¬ªº¤¬°Ê¬É­±µ{¦¡¡C
Terminal«ü¥O¶°
³]©wÃC¦â : 33[colorm
¨ä¤¤color¥i¥H¬O¥H¤Uªº­È
0 : Reset Color Attributes
1 : bold on
2 : bold off
4 : underline on
5 : blink on
7 : reverse on
21/22 : bold normal
24 : underline off
25 : blink off
27 : reverse off
30 : «e´º¡A¶Â¦â
31 : «e´º¡A¬õ¦â
32 : «e´º¡Aºñ¦â
33 : «e´º¡A¶À¦â
34 : «e´º¡AÄx¦â
35 : «e´º¡Aµµ¦â
36 : «e´º¡A«C¦â
37 : «e´º¡A¥Õ¦â
40 : ­I´º¡A¶Â¦â
41 : ­I´º¡A¬õ¦â
42 : ­I´º¡Aºñ¦â
43 : ­I´º¡A¶À¦â
44 : ­I´º¡AÄx¦â
45 : ­I´º¡Aµµ¦â
46 : ­I´º¡A«C¦â
47 : ­I´º¡A¥Õ¦â
²M°£¿Ã¹õ : 33c
³]©w¤ô¥­´å¼Ð¦ì¸m : 33[XG
X¬°¤ô¥­´å¼Ð¦ì¸m¡C
³]©w««ª½´å¼Ð¦ì¸m : 33[Xd
Y¬°««ª½´å¼Ð¦ì¸m¡C
33[YA
Current_Cursor_Y -= Y
33[YB©Î33[Ye
Current_Cursor_Y += Y
33[XC©Î33[Xa
Current_Cursor_X += X
33[XD
Current_Cursor_X -= X
33[YE
gotoxy(0,Current_Cursor_Y+Y)
33[YF
gotoxy(0,Current_Cursor_Y-Y)
33[Y;XH
gotoxy(X,Y);
33[0K : §R°£±q´å¼Ð¨ì¸Ó¦æµ²§À
33[1K : §R°£±q¸Ó¦æ¶}©l¨ì´å¼Ð³B
33[2K : §R°£¾ã¦æ
33[0J : §R°£´å¼Ð¨ì¿Ã¹õµ²§À
33[1J : §R°£±q¿Ã¹õ¶}©l¨ì´å¼Ð³B
33[2J : §R°£¾ã­Ó¿Ã¹õ
33[N@ : insert N char
33[P : delete char
33[M : delete line
33[L : insert line
33[s : save cursor position
33[u : restore cursor position
33E : carry ; linefeed
33M : Current_Cursor_Y-1
33D : linefeed

OK STATION, Webmaster, Brian Lin 

#endif

int level=0;

#include <cctype>
#include <vector>

using namespace std;

// ¥Î¨Ó¬ö¿ý text attribute, 
// initial value ¶Â©³¥Õ¦rµL¥ô¦óÄÝ©Ê

int fg=37, bg=40, attr=0; 

inline void init_attr()
{
  fg=37; bg=40; attr=0; 
}
inline void print_attr()
{
  printf("fg : %d\n", fg);
  printf("bg : %d\n", bg);
  printf("attr : %d\n", attr);
}

%}


%%

\[([0-9])*;([0-9])*H {
#if 0

/*

   Direct cursor addressing        ESC [ Pl; Pc H  or ESC [ Pl; Pc f
   Pl = line number (default 0); Pc = column number (default 0)

 */
  ECHO;
  printf("\nmove cursor\n");
#endif
}

\[K { // ???
}
\[([0-9])*J { // ???
}
\[([0-9])*H { // ???
}

\[m { // end text attribute status
#if 0
  PRINT_YYTEXT("END CHAR ATTR");
#endif
}

\[([0-9]*(;|[ ])*)*m { // begin text attribute status
#if 0

  PRINT_YYTEXT("BEGIN CHAR ATTR");

  //std::vector<stdvector<int> > numbers;

  char num[3];
  int c=0;


for (int i=0; i < yyleng; ++i)
{
  if (isdigit(yytext[i]))
  {
    //printf ("%c\n",yytext[i]);
    num[c++]=yytext[i];
  }
  else
  {
    if (c==0)
    {
    }
    else // num has data
    {
      //numbers.push_back(num);
      num[c++]=0;
      // char 00 -> number 0
      int n=atoi(num);
      printf("num: %d\n",atoi(num));
      c=0;
      if ( 40 <= n && n <= 47 )
        bg=n;
      if ( 30 <= n && n <= 37 )
        fg=n;
      if ( 1 <= n && n <= 8 )
        attr=n;
      if ( n==0)
        init_attr();
    }
  }
}
#endif

/*
  Character Attributes
  ESC [ Ps;Ps;Ps;...,Ps m
  Ps =    0 or None       All Attributes Off
  1               Bold on (§Úªº·PÄ±¦n¹³¬O°ª«G«×)
  4               Underscore on
  5               Blink on
  7               Reverse video on
  Any other parameter values are ignored.

  ref: http://www.cyut.edu.tw/~ckhung/b/mi/vt100_reference_card.txt


      «e´ºÃC¦â  °ª    «G    ­I´ºÃC¦â
      ¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w 
      30  ¶Â¦â  1,30  ¦Ç¦â  40  ¶Â¦â
      31  ¬õ¦â  1,31  ÂA¬õ  41  ¬õ¦â
      32  ºñ¦â  1,32  ÂAºñ  42  ºñ¦â
      33  ´Ä¦â  1,33  ¶À¦â  43  ´Ä¦â
      34  ÂÅ¦â  1,34  ÂAÂÅ  44  ÂÅ¦â
      35  µµ¦â  1,35  ÂAµµ  45  µµ¦â
      36  «C¦â  1,36  ÂA«C  46  «C¦â
      37  ¥Õ¦â  1,37  ¥Õ¦â  47  ¥Õ¦â

      SET TERM SCREEN-BACKGROUND REVERSE
      §â¿Ã¹õÃC¦âÄA­Ë¡C

      this mybe be mode 7


 */

/*\[([0-9]*(;|[ ])*)*[a-zA-Z] {*/
}


([\xa1-\xfe]|[\x8e-\xa0]|[\x81-\x8d])([\x40-\x7e]|[\xa1-\xfe]) {
// BIG5 char
#if 1  
  //print_attr();
  //printf("big5 char\n");
  printf("%c", yytext[0]);
  printf("%c", yytext[1]);
#endif
}

[\x20-\x7f] { /* 32 ~ 127 */
  printf("%c", yytext[0]);
#if 0
//printf("SEEABLE ASCII\n"); BEGIN 0;
  print_attr();
  printf("seeable %c\n", yytext[0]);
  char h[2];
  get_hex(yytext[0],h);
  printf("seeable hex 0x%c%c\n",h[0],h[1]);
#endif
}

[\x0d\x0a] { // \n
    printf("\n");
}

[\x00-\x1f] { /* 00 ~ 31 */
#if 0
  if (yytext[0]==0x0a)
    printf("\n");
  if (yytext[0]==0x0d)
    printf("\n");
// NONSEEABLE ASCII I don't draw it
  PRINT_YYTEXT("NONSEEABLE");
  char h[2];
  get_hex(yytext[0],h);
  printf("nonseeable 0x%c%c\n",h[0],h[1]);
#endif
}
%%

#if 0
int main()
{
  yylex();
  return 0;
}
#endif
