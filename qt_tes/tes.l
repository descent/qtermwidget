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

Linux�{���]�p�J�� - termios/keymap/terminal

�o����2005-05-28 �QŪ173�� �i�r��G�j �� �p�j

Linux�{���]�p�J�� - termios/keymap/terminal programming
termios
int tcgetattr (int fd, struct termios *termios_p);
int tcsetattr (int fd, int optional_actions,const struct termios
*termios_p);
keymap
�ڼg�F�@�Ӥp�{���ӱM���B�zLinux�W��keymap�Akeymap.h��keymap.c�C
�bLinux Terminal�W�A�p�G�z�Q�n�]�w�Y�ǫ����^�S�w�ȡA�z�|�ݭn�Ψ�H�U�o�ǧޥ��C
�]�wkeymap
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
STATE�����A��զX
/usr/include/linux/keyboard.h��
#define KG_SHIFT 0
#define KG_CTRL 2
#define KG_ALT 3
#define KG_ALTGR 1
#define KG_SHIFTL 4
#define KG_SHIFTR 5
#define KG_CTRLL 6
#define KG_CTRLR 7
#define KG_CAPSSHIFT 8
�ϥΤ覡�p:
#define KST_CTRL (1< #define KST_ALT (1< #define KST_SHIFT (1< #define KST_CTRL_ALT (KST_CTRL|KST_ALT)
#define KST_ALT_SHIFT (KST_ALT|KST_SHIFT)
SCANCODE����L���y�X
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
/usr/include/linux/kd.h��
struct kbentry {
unsigned char kb_table;
unsigned char kb_index;
unsigned short kb_value;
};
#define KDGKBENT 0x4B46 /* gets one entry in translation
table */
#define KDSKBENT 0x4B47 /* sets one entry in translation
table */
��console��
console = open("/dev/console",O_RDWR);
Ū������
read(console,&c,sizeof(char));

terminal programming
term.h/term.c�O�ڼg�ӱM���B�z�@�Ǥp�������ʬɭ��{���C
Terminal���O��
�]�w�C�� : 33[colorm
�䤤color�i�H�O�H�U����
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
30 : �e���A�¦�
31 : �e���A����
32 : �e���A���
33 : �e���A����
34 : �e���A�x��
35 : �e���A����
36 : �e���A�C��
37 : �e���A�զ�
40 : �I���A�¦�
41 : �I���A����
42 : �I���A���
43 : �I���A����
44 : �I���A�x��
45 : �I���A����
46 : �I���A�C��
47 : �I���A�զ�
�M���ù� : 33c
�]�w������Ц�m : 33[XG
X��������Ц�m�C
�]�w������Ц�m : 33[Xd
Y��������Ц�m�C
33[YA
Current_Cursor_Y -= Y
33[YB��33[Ye
Current_Cursor_Y += Y
33[XC��33[Xa
Current_Cursor_X += X
33[XD
Current_Cursor_X -= X
33[YE
gotoxy(0,Current_Cursor_Y+Y)
33[YF
gotoxy(0,Current_Cursor_Y-Y)
33[Y;XH
gotoxy(X,Y);
33[0K : �R���q��Ш�Ӧ浲��
33[1K : �R���q�Ӧ�}�l���гB
33[2K : �R�����
33[0J : �R����Ш�ù�����
33[1J : �R���q�ù��}�l���гB
33[2J : �R����ӿù�
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

// �ΨӬ��� text attribute, 
// initial value �©��զr�L�����ݩ�

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
  1               Bold on (�ڪ��Pı�n���O���G��)
  4               Underscore on
  5               Blink on
  7               Reverse video on
  Any other parameter values are ignored.

  ref: http://www.cyut.edu.tw/~ckhung/b/mi/vt100_reference_card.txt


      �e���C��  ��    �G    �I���C��
      �w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w 
      30  �¦�  1,30  �Ǧ�  40  �¦�
      31  ����  1,31  �A��  41  ����
      32  ���  1,32  �A��  42  ���
      33  �Ħ�  1,33  ����  43  �Ħ�
      34  �Ŧ�  1,34  �A��  44  �Ŧ�
      35  ����  1,35  �A��  45  ����
      36  �C��  1,36  �A�C  46  �C��
      37  �զ�  1,37  �զ�  47  �զ�

      SET TERM SCREEN-BACKGROUND REVERSE
      ��ù��C���A�ˡC

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