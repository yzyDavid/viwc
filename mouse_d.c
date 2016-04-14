/****************************************************/
/* Mouse Event Handler demo in Watcom C             */
/* copyright (C) Black White.  Mar 9, 2014          */
/* email: iceman@zju.edu.cn                         */
/* web: http://10.71.45.100/bhh                     */
/****************************************************/

#include <graphics.h>
#define MOUSE_SIZE               16   // 16*16��
#define MOUSE_MOVE               0x01
#define MOUSE_LEFT_BUTTON_DOWN   0x02
#define MOUSE_LEFT_BUTTON_UP     0x04
#define MOUSE_RIGHT_BUTTON_DOWN  0x08
#define MOUSE_RIGHT_BUTTON_UP    0x10
#define MOUSE_MIDDLE_BUTTON_DOWN 0x20
#define MOUSE_MIDDLE_BUTTON_UP   0x40

void (far *old_mouse_event_handler)(int event, int x, int y);
volatile int stop = 0;

word screen_mask[] =
{
   /* 16 words of screen mask */
   0x3FFF, /*0011111111111111*/
   0x1FFF, /*0001111111111111*/
   0x0FFF, /*0000111111111111*/
   0x07FF, /*0000011111111111*/
   0x03FF, /*0000001111111111*/
   0x01FF, /*0000000111111111*/
   0x00FF, /*0000000011111111*/
   0x007F, /*0000000001111111*/
   0x01FF, /*0000000111111111*/
   0x10FF, /*0001000011111111*/
   0xB0FF, /*1011000011111111*/
   0xF87F, /*1111100001111111*/
   0xF87F, /*1111100001111111*/
   0xFC3F, /*1111110000111111*/
   0xFC3F, /*1111110000111111*/
   0xFE1F, /*1111111000011111*/
};

word cursor_mask[] =
{
   /* 16 words of cursor mask */
   0x0000, /*0000000000000000*/
   0x4000, /*0100000000000000*/
   0x6000, /*0110000000000000*/
   0x7000, /*0111000000000000*/
   0x7800, /*0111100000000000*/
   0x7C00, /*0111110000000000*/
   0x7E00, /*0111111000000000*/
   0x7F00, /*0111111100000000*/
   0x7C00, /*0111110000000000*/
   0x4600, /*0100011000000000*/
   0x0600, /*0000011000000000*/
   0x0300, /*0000001100000000*/
   0x0300, /*0000001100000000*/
   0x0180, /*0000000110000000*/
   0x0180, /*0000000110000000*/
   0x00C0, /*0000000011000000*/
};

struct 
{
   int x, y;
   struct picture *old_img;
   struct picture *mask;
   struct picture *img;
} mouse;

int  init_mouse(void);
void draw_mouse(int x, int y);
void recover_mouse_background(void);
void save_mouse_background(int x, int y);
void move_mouse(int x, int y);
void __loadds far mouse_event_handler(int event, int x, int y);
void expand_mask_to_image(word m[16], byte *p, byte color, int width, int height);
void build_mouse_image(void);

void __loadds far mouse_event_handler(int event, int x, int y)
{  // ��������������������¼��Ļص��������������ɲ���ϵͳ�����ã���������main()��
   // ���á������״̬�����仯ʱ��������ƶ������Ҽ������»��ͷţ��򲻹ܵ�ǰCPU����
   // ʲô���飬����ϵͳ����Ҫ��CPU��ͣ�������Ĺ�������תȥ���ñ���������˱�������
   // �������жϴ�������ע����괦�����������__loadds��far���Σ�����__loadds��
   // ��ǿ�Ʊ������ڱ������Ŀ�ͷ���ɶ����ݶμĴ���DS���и�ֵ�Ĵ��룬��far��ǿ�Ʊ���
   // ���ڳ���ĩβ����retfԶ����ָ���Դ���retn������ָ�
   #pragma aux mouse_event_handler parm [EAX] [ECX] [EDX];
   //#pragma aux�Ǳ���ָʾ���,���涨����event�ɼĴ���EAX����, x��ECX����, y��EDX����
   static left_button_hold_down = 0;
   if(event & MOUSE_MOVE)
   {
      if(left_button_hold_down)
      {
         recover_mouse_background();
         setfillstyle(XHATCH_FILL, RED);
         fillellipse(x, y, 10, 10);
         save_mouse_background(x, y);
      }
      move_mouse(x, y);
   }
   else if(event & MOUSE_RIGHT_BUTTON_UP)
   {
      stop = 1;
   }
   else if(event & MOUSE_LEFT_BUTTON_DOWN)
   {
      left_button_hold_down = 1;
   }
   else if(event & MOUSE_LEFT_BUTTON_UP)
   {
      left_button_hold_down = 0;
   }
}

void expand_mask_to_image(word m[16], byte *p, byte color, int width, int height)
{
   int i, j;
   for(i=0; i<height; i++)
   {
      for(j=0; j<width; j++)
      {
         if(m[i] & 1<<(width-1-j))
            p[i*width+j] = color;
         else
            p[i*width+j] = 0x00;
      }
   }
}

void build_mouse_image(void)
{
   mouse.old_img = malloc(imagesize(0, 0, MOUSE_SIZE-1, MOUSE_SIZE-1));
   mouse.mask = malloc(imagesize(0, 0, MOUSE_SIZE-1, MOUSE_SIZE-1)); 
   mouse.img = malloc(imagesize(0, 0, MOUSE_SIZE-1, MOUSE_SIZE-1));
   mouse.mask->picwidth = mouse.mask->picheight = MOUSE_SIZE;
   mouse.img->picwidth = mouse.img->picheight = MOUSE_SIZE;
   expand_mask_to_image(screen_mask, &mouse.mask->buffer, 0xFF, MOUSE_SIZE, MOUSE_SIZE);
   expand_mask_to_image(cursor_mask, &mouse.img->buffer, WHITE, MOUSE_SIZE, MOUSE_SIZE);
}

void save_mouse_background(int x, int y)
{
   getimage(x, y, x+MOUSE_SIZE-1, y+MOUSE_SIZE-1, mouse.old_img);
   mouse.x = x;
   mouse.y = y;
}

void draw_mouse(int x, int y)
{
   putimage(x, y, mouse.mask, AND_PUT);
   putimage(x, y, mouse.img, OR_PUT);
}

void recover_mouse_background(void)
{
   putimage(mouse.x, mouse.y, mouse.old_img, COPY_PUT);
}

void move_mouse(int x, int y)
{
   recover_mouse_background();
   save_mouse_background(x, y);
   draw_mouse(x, y);
}

int init_mouse(void)
{
   int x=_width/2, y=_height/2;
   union REGS r;
   struct SREGS sr;

   r.w.ax = 0;      /* check mouse driver */
   int86(0x33, &r, &r);
   if(r.w.ax != 0xFFFF)
      return 0;

   r.w.ax = 0x0007; /* set horizontal range */
   r.w.cx = 0;
   r.w.dx = _width-1;
   int86(0x33, &r, &r);
   r.w.ax = 0x0008; /* set vertical range */
   r.w.cx = 0;
   r.w.dx = _height-1;
   int86(0x33, &r, &r);

   r.w.ax = 0x0004; /* position cursor */
   r.w.cx = x;
   r.w.dx = y;
   int86(0x33, &r, &r);

   r.w.ax = 2;      /* hide cursor */
   int86(0x33, &r, &r);

   r.w.ax = 0x0014; /* install mouse event handler */
   r.w.cx = MOUSE_MOVE | MOUSE_LEFT_BUTTON_DOWN | MOUSE_LEFT_BUTTON_UP |
            MOUSE_RIGHT_BUTTON_DOWN | MOUSE_RIGHT_BUTTON_UP; /* monitor these events */
   r.x.edx = FP_OFF(mouse_event_handler);
   memset(&sr, 0, sizeof(sr));
   sr.es   = FP_SEG(mouse_event_handler);
   int86x(0x33, &r, &r, &sr);
   old_mouse_event_handler = MK_FP(sr.es, r.x.edx);

   build_mouse_image();
   save_mouse_background(x, y);
   move_mouse(x, y);

   return 1;
}

void close_mouse(void)
{
   union REGS r;
   struct SREGS sr;
   r.w.ax = 0x0014; /* install mouse event handler */
   r.w.cx = 0;      // DO NOT monitor any events!!!
   r.x.edx = FP_OFF(old_mouse_event_handler);
   memset(&sr, 0, sizeof(sr));
   sr.es   = FP_SEG(old_mouse_event_handler);
   int86x(0x33, &r, &r, &sr);
}

main()
{
   int driver=0, mode=VESA_1024x768x8bit;
   initgraph(&driver, &mode, "");
   load_8bit_bmp(0, 0, "pic256.bmp");
   init_mouse();
   while(!stop && !bioskey(1))
   {
   }
   close_mouse();
   free(mouse.old_img);
   free(mouse.mask);
   free(mouse.img);
   closegraph();
   return 1;
}
