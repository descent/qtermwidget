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

Linux程式設計入門 - termios/keymap/terminal

發布於2005-05-28 被讀173次 【字體：大 中 小】

Linux程式設計入門 - termios/keymap/terminal programming
termios
int tcgetattr (int fd, struct termios *termios_p);
int tcsetattr (int fd, int optional_actions,const struct termios
*termios_p);
keymap
我寫了一個小程式來專門處理Linux上的keymap，keymap.h及keymap.c。
在Linux Terminal上，如果您想要設定某些按鍵返回特定值，您會需要用到以下這些技巧。
設定keymap
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
STATE為狀態鍵組合
/usr/include/linux/keyboard.h中
#define KG_SHIFT 0
#define KG_CTRL 2
#define KG_ALT 3
#define KG_ALTGR 1
#define KG_SHIFTL 4
#define KG_SHIFTR 5
#define KG_CTRLL 6
#define KG_CTRLR 7
#define KG_CAPSSHIFT 8
使用方式如:
#define KST_CTRL (1< #define KST_ALT (1< #define KST_SHIFT (1< #define KST_CTRL_ALT (KST_CTRL|KST_ALT)
#define KST_ALT_SHIFT (KST_ALT|KST_SHIFT)
SCANCODE為鍵盤掃描碼
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
/usr/include/linux/kd.h中
struct kbentry {
unsigned char kb_table;
unsigned char kb_index;
unsigned short kb_value;
};
#define KDGKBENT 0x4B46 /* gets one entry in translation
table */
#define KDSKBENT 0x4B47 /* sets one entry in translation
table */
而console為
console = open("/dev/console",O_RDWR);
讀取按鍵
read(console,&c,sizeof(char));

terminal programming
term.h/term.c是我寫來專門處理一些小型的互動界面程式。
Terminal指令集
設定顏色 : 33[colorm
其中color可以是以下的值
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
30 : 前景，黑色
31 : 前景，紅色
32 : 前景，綠色
33 : 前景，黃色
34 : 前景，籃色
35 : 前景，紫色
36 : 前景，青色
37 : 前景，白色
40 : 背景，黑色
41 : 背景，紅色
42 : 背景，綠色
43 : 背景，黃色
44 : 背景，籃色
45 : 背景，紫色
46 : 背景，青色
47 : 背景，白色
清除螢幕 : 33c
設定水平游標位置 : 33[XG
X為水平游標位置。
設定垂直游標位置 : 33[Xd
Y為垂直游標位置。
33[YA
Current_Cursor_Y -= Y
33[YB或33[Ye
Current_Cursor_Y += Y
33[XC或33[Xa
Current_Cursor_X += X
33[XD
Current_Cursor_X -= X
33[YE
gotoxy(0,Current_Cursor_Y+Y)
33[YF
gotoxy(0,Current_Cursor_Y-Y)
33[Y;XH
gotoxy(X,Y);
33[0K : 刪除從游標到該行結尾
33[1K : 刪除從該行開始到游標處
33[2K : 刪除整行
33[0J : 刪除游標到螢幕結尾
33[1J : 刪除從螢幕開始到游標處
33[2J : 刪除整個螢幕
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

#include <QtGui>


using namespace std;
// 蕞蕞 text attribute, 
// initial value : 蕞蕞, 蕞蕞�


QString cn;
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
  1               Bold on (我的感覺好像是高亮度)
  4               Underscore on
  5               Blink on
  7               Reverse video on
  Any other parameter values are ignored.

  ref: http://www.cyut.edu.tw/~ckhung/b/mi/vt100_reference_card.txt


      前景顏色  高    亮    背景顏色
      ──────────────── 
      30  黑色  1,30  灰色  40  黑色
      31  紅色  1,31  鮮紅  41  紅色
      32  綠色  1,32  鮮綠  42  綠色
      33  棕色  1,33  黃色  43  棕色
      34  藍色  1,34  鮮藍  44  藍色
      35  紫色  1,35  鮮紫  45  紫色
      36  青色  1,36  鮮青  46  青色
      37  白色  1,37  白色  47  白色

      SET TERM SCREEN-BACKGROUND REVERSE
      把螢幕顏色顛倒。

      this mybe be mode 7


 */

/*\[([0-9]*(;|[ ])*)*[a-zA-Z] {*/
}


([\xa1-\xfe]|[\x8e-\xa0]|[\x81-\x8d])([\x40-\x7e]|[\xa1-\xfe]) {
// BIG5 char
#if 1  
  //print_attr();
  //printf("big5 char\n");
  //extern char big5_char[];

  //printf("%c", yytext[0]);
  //printf("%c\n", yytext[1]);



#ifdef DEBUG_INPUT
  char h[2];
  get_hex(yytext[0],h);
  printf("hex 0x%c%c\n",h[0],h[1]);

  get_hex(yytext[1],h);
  printf("hex 0x%c%c\n",h[0],h[1]);
#endif

  char big5_char[]={0,0,0};
  big5_char[0]=yytext[0];
  big5_char[1]=yytext[1];
  extern QTextEdit *text_edit;
  extern QTextCodec *big5_codec;

  //QString cn;
  cn += big5_codec->toUnicode(big5_char);
  //text_edit->document()->setPlainText(cn);
  //text_edit->append(cn);
  //text_edit->insertPlainText(cn);
  //text_edit->moveCursor(QTextCursor::Up);
#endif
}

[\x20-\x7f] { /* 32 ~ 127 seeable ASCII*/
  char big5_char[]={0,0,0};
  big5_char[0]=yytext[0];
  big5_char[1]=yytext[1];
  extern QTextEdit *text_edit;
  extern QTextCodec *big5_codec;

  //QString cn = big5_codec->toUnicode(big5_char);
  QString cn = "a";
  //text_edit->append(cn);
  //text_edit->insertPlainText(cn);
  //printf("%c", yytext[0]);
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
    //printf("\n");
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
