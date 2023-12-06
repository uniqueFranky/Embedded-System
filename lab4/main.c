#include <stdio.h>
#include "../common/common.h"

#define COLOR_BACKGROUND	FB_COLOR(0xff,0xff,0xff)
#define RED	FB_COLOR(255,0,0)
#define ORANGE	FB_COLOR(255,165,0)
#define YELLOW	FB_COLOR(255,255,0)
#define GREEN	FB_COLOR(0,255,0)
#define CYAN	FB_COLOR(0,127,255)
#define BLUE	FB_COLOR(0,0,255)
#define PURPLE	FB_COLOR(139,0,255)
#define WHITE   FB_COLOR(255,255,255)
#define BLACK   FB_COLOR(0,0,0)

#define RADIUS 60

struct TouchState {
	int x; // x == -1 means the touch is released
	int y;
} states[5];
const int colorForFinger[5] = {BLUE, YELLOW, GREEN, ORANGE, PURPLE};

static int touch_fd;
static void touch_event_cb(int fd)
{
	int type,x,y,finger;
	type = touch_read(fd, &x,&y,&finger);
	switch(type){
	case TOUCH_PRESS:
		fb_draw_circle(x, y, RADIUS, colorForFinger[finger]);
		states[finger].x = x;
		states[finger].y = y;
		break;
	case TOUCH_MOVE:
		fb_draw_circle(states[finger].x, states[finger].y, RADIUS, COLOR_BACKGROUND);
		fb_draw_circle(x, y, RADIUS, colorForFinger[finger]);
		states[finger].x = x;
		states[finger].y = y;
		break;
	case TOUCH_RELEASE:
		fb_draw_circle(states[finger].x, states[finger].y, RADIUS, COLOR_BACKGROUND);
		break;
	case TOUCH_ERROR:
		printf("close touch fd\n");
		close(fd);
		task_delete_file(fd);
		break;
	default:
		return;
	}
	fb_update();
	return;
}

int main(int argc, char *argv[])
{
	fb_init("/dev/fb0");
	fb_draw_rect(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,COLOR_BACKGROUND);
	fb_update();

	//打开多点触摸设备文件, 返回文件fd
	touch_fd = touch_init("/dev/input/event2");
	//添加任务, 当touch_fd文件可读时, 会自动调用touch_event_cb函数
	task_add_file(touch_fd, touch_event_cb);
	
	task_loop(); //进入任务循环
	return 0;
}
