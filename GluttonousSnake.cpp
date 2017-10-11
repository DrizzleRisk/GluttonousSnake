////////////////////////////////////////////
// 程序名称：贪吃蛇
// 编译环境：Visual C++ 6.0 / 2010，EasyX_2011惊蛰版
// 程序编写：轻雨漫步 <512***756@qq.com>
// 最后更新：2011-3-11
//

////////////////////////////////////////////
//开始编写游戏需要的类
////////////////////////////////////////////

#include <graphics.h> //EasyX_2011惊蛰版（绘图库）
#include <stdio.h>
#include <conio.h>
#include <time.h>

#define UP		1
#define DOWN	2
#define LEFT	3
#define RIGHT	4
////////////////////////////////////////////
//格子类，定义一个格子的属性及行为
////////////////////////////////////////////
class Cell
{
public:

	friend class GameArea;  //设置友元函数

	void setxy(short x, short y)   //设置格子左上角坐标
	{
		m_x = x, m_y = y;
	}
	void setfull(short full)  //设置格子属性，0为空，1为障碍，2为食物
	{
		m_full = full;
	}
	void display(COLORREF color)  //设置格子颜色并显示
	{
		m_color = color;
		setfillstyle(m_color);
		bar(m_x, m_y, m_x+7, m_y+7);
	}
	void LaserDisplay()  //显示镭射状态的格子
	{
		IMAGE image(10,10);
		SetWorkingImage(&image);
		DWORD* pMem = GetImageBuffer(&image);
		for(int i = 0; i < 10 * 10; i++)
		pMem[i] = BGR(RGB(0, 0, i * 256 / (10*10) ));
		SetWorkingImage(NULL);
		putimage(m_x,m_y,&image);
	}
	short ReturnFull()  //返回格子状态
	{
		return m_full;
	}
	Cell()  //构造函数
	{
		m_x = 0,m_y = 0,m_full = 0;
		m_color = BLACK;
	}

private:

	short m_x;			//格子的左上角X坐标
	short m_y;			//格子的左上角Y坐标
	short m_full;		//0为空，1为阻挡，2为食物
	COLORREF m_color;	//格子颜色
};
////////////////////////////////////////////
//游戏区类，编写有关游戏区的一些操作
////////////////////////////////////////////
class GameArea  //游戏区域
{
public:

	Cell m_game[60][60];			//定义游戏区域（由360个格子组成）

	friend void MessageDispose();	//设置友元函数

	bool CreatFood()				//产生随机食物
	{
		srand(time(NULL));			//初始化随机数种子
		m_random1 = rand()%58+1;		//随机生成一个0 - 58的整数
		m_random2 = rand()%58+1;		//随机一个0 - 58的整数
		if(m_game[m_random2][m_random2].m_full == 0)	//检查生成的食物坐标是否在障碍上
		{
			m_game[m_random1][m_random2].display(GREEN);
			m_game[m_random1][m_random2].m_full = 2;
			return true;
		}
		//如果随机的食物正好出现在蛇身上，则进入下面的循环寻找可以生成食物的地方
		for(m_random1 = 1;m_random1 < 59;m_random1++)
		{
			for(m_random2 = 1;m_random2 < 59;m_random2++)
			{
				if(m_game[m_random2][m_random2].m_full == 0)
				{
					m_game[m_random1][m_random2].display(GREEN);
					m_game[m_random1][m_random2].m_full = 2;
					return true;
				}
			}
		}
		//如果没有找到可以生成食物的地方，则通关
		return false;  //返回false即表示通关
	}
	void DelFood()	//删除食物
	{
		m_game[m_random1][m_random2].m_full = 0;  //设置为0即代表格子属性为空
	}
    void ChangeColor(int flag1,int flag2,COLORREF color)  //设置指定格子的颜色
	{
		m_game[flag1][flag2].display(color);
	}
	void Init()					//初始化GAME区域
	{
		int flag1,flag2;		//标识变量
		BeginBatchDraw();		//开始批量绘图
		setfillstyle(BLACK);	//设置当前颜色
		bar(0,0,600,600);		//画无边框填充矩形（在这里的用途是用黑色清空游戏区域）
		for(flag1 = 0;flag1 < 60;flag1++)
		{
			for(flag2 = 0;flag2 < 60;flag2++)
			{
				if(flag1 == 0 || flag1 == 59 || flag2 == 0 || flag2 == 59)	//创建边界
				{                       
					m_game[flag1][flag2].setfull(1);
					m_game[flag1][flag2].setxy(flag1*10,flag2*10);
					m_game[flag1][flag2].display(RGB(237,28,36));
				}
				else														//创建游戏区域
				{
					m_game[flag1][flag2].setfull(0);
					m_game[flag1][flag2].setxy(flag1*10,flag2*10);
					m_game[flag1][flag2].display(BLACK);
				}
			}
		}
		CreatFood();
		EndBatchDraw();
	}

private:

	int m_random1, m_random2;
};
////////////////////////////////////////////
//蛇类，定义蛇的数据结构以及蛇的行为
////////////////////////////////////////////
class Snake
{
public:

	int toward;		//蛇头朝向	
	friend void MessageDispose();
	friend int HitDetect();
	friend void ReInit(); 
	Snake()
	{
		head = NULL;
		last = NULL;
		now  = NULL;
	}
	void Init()			//初始化蛇的结构
	{
		if(head!=NULL)  //重玩游戏时，释放原先的链表所有结点
		{
			for(now = head->next;now->next != NULL;now = now->next)
			{
				free(now->prior);
			}
		}
		head = (struct node*)malloc(sizeof(struct node));  //为蛇头分配内存
		head->prior = NULL;
		head->m_x = 300;
		head->m_y = 300;
		now = (struct node*)malloc(sizeof(struct node));
		head->next = now;
		now->prior = head;
		now->next = NULL;
		now->m_x = 300;
		now->m_y = 290;
		last = now;
		toward = DOWN;
	}
	void SnakeAdd()
	{
		now = head;			//当前指向蛇头
		now->prior = (struct node*)malloc(sizeof(struct node));  //为新增的结点分配内存
		now = now->prior;	//让当前指向新分配的结点
		now->prior = NULL;  //置空当前结点的前趋
		now->next = head;	//让当前结点的后继指向蛇头
	
		switch(toward)		//根据当前蛇头方向确定新增部分的坐标
		{
		case UP:
			now->m_x = head->m_x;
			now->m_y = head->m_y -10;
			break;
			case DOWN:
			now->m_x = head->m_x;
			now->m_y = head->m_y + 10;
			break;
		case LEFT:
			now->m_x = head->m_x - 10;
			now->m_y = head->m_y;
			break;
		case RIGHT:
			now->m_x = head->m_x + 10;
			now->m_y = head->m_y;
			break;
		}
		head = now; //设置当前结点为蛇头

	}
	void SnakeDel()		//释放蛇尾结点（删除蛇尾）
	{
		last = last->prior;
		free(last->next);
		last->next = NULL;
	}
	void SnakeMove()	//蛇身移动一格
	{
		SnakeAdd();		//增加蛇头
		SnakeDel();		//删除蛇尾
	}

private:

	struct node				 //蛇身链表
	{
		int m_x;
		int m_y;
		struct node *next;  //下个结点
		struct node *prior; //上个结点
	};
	struct node *head;
	struct node *last;
	struct node *now;
};
////////////////////////////////////////////
//游戏类，用来初始化游戏的参数，及实现其他游戏操作
////////////////////////////////////////////
class Game
{
public:

	void Init()
	{
		closegraph();
		initgraph(800,600);
		setbkmode(TRANSPARENT);					//设置输出文字背景为透明
		LOGFONT f;
		getfont(&f);							// 获取当前字体设置
		f.lfHeight = 50;						// 设置字体高度为 48（包含行距）
		_tcscpy(f.lfFaceName, _T("黑体"));		// 设置字体为“黑体”
		f.lfQuality = ANTIALIASED_QUALITY;		// 设置输出效果为抗锯齿  
		setfont(&f);							// 设置字体样式
	}
	void FailGame()  //游戏失败显示的画面
	{
		setcolor(RED);
		setfont(70, 0, _T("微软雅黑"));
		outtextxy(150, 265, _T("YOU LOSE!"));
	}
	void WinGame() //游戏胜利时显示的画面
	{
		setcolor(RED);
		setfont(70, 0, _T("微软雅黑"));
		outtextxy(150, 265, _T("YOU WIN!"));
		FlushBatchDraw();
		Sleep(10000);
		exit(0);
	}
};
////////////////////////////////////////////
//管理区类，定义管理区域的相关操作
////////////////////////////////////////////
class ManageArea  
{
public:

	friend void MessageDispose();

	void TimeAdd(double add)	//增加时间（可在类外直接调用实现动态刷新时间）
	{
		m_time += add;
		DisplayTime();
	}
	void ScoreAdd(short add)	//增加分数（可在类外直接调用实现动态刷新分数）
	{
		m_score += add;
		DisplayScore();
	}
	void DisplayPause()			//显示暂停或者开始
	{
		BeginBatchDraw();
		_stprintf(m_str_score, _T("%d"), m_score);
		setfont(25, 0, _T("微软雅黑"));
		setfillstyle(m_bgcolor);
		bar(625,490,800,515);
		setcolor(BLUE);
		if(m_pause)
		outtextxy(625, 490, _T("开始（P键）"));
		else
		outtextxy(625, 490, _T("暂停（P键）"));
		EndBatchDraw();
	}
	void Init()  //初始化管理界面
	{
		m_time = 0.0;
		m_score = 0;
		m_leave = 1;
		m_pause = false;
		_stprintf(m_str_score, _T("%d"), m_score);  //格式化转换类型
		_stprintf(m_str_time, _T("%.1lf"), m_time);
		m_bgcolor = BLACK;
		BeginBatchDraw();
		setfillstyle(BLACK);
		bar(601,0,800,600);
		setfont(60, 0, _T("微软雅黑"));
		setcolor(GREEN);
		outtextxy(625, 30, _T("贪吃蛇"));
		setfont(30, 0, _T("微软雅黑"));
		setcolor(RGB(128,0,255));
		outtextxy(625, 140, _T("制作：轻雨漫步"));
		setfont(25, 0, _T("微软雅黑"));
		setcolor(BLUE);
		outtextxy(625, 430, _T("时间："));
		outtextxy(625, 460, _T("分数："));
		outtextxy(625, 490, _T("暂停（P键）"));
		outtextxy(625, 520, _T("重新游戏（R键）"));
		setcolor(RED);
		outtextxy(720, 400, m_str_leave);
		outtextxy(680, 460, m_str_score);
		outtextxy(680, 430, m_str_time);
		EndBatchDraw();
	} 

private:

	double	m_time;
	short	m_score;
	short	m_leave;
	bool	m_pause;
	TCHAR	m_str_leave[2], m_str_time[33], m_str_score[5];	
	COLORREF m_bgcolor;

	void DisplayTime()  //显示当前耗时
	{
		BeginBatchDraw();
		_stprintf(m_str_time, _T("%.1lf"), m_time);
		setfont(25, 0, _T("微软雅黑"));
		setfillstyle(m_bgcolor);
		bar(680,430,800,455);
		setcolor(RED);
		outtextxy(680,430,m_str_time);
		EndBatchDraw();
	}
	void DisplayScore()  //显示当前分数
	{
		BeginBatchDraw();
		_stprintf(m_str_score, _T("%d"), m_score);
		setfont(25, 0, _T("微软雅黑"));
		setfillstyle(m_bgcolor);
		bar(680,460,800,485);
		setcolor(RED);
		outtextxy(680,460,m_str_score);
		EndBatchDraw();
	}
};


////////////////////////////////////////////
//声明游戏需要的类
////////////////////////////////////////////

Game		game;		//用于初始化游戏、设置游戏相关参数
GameArea	a;			//用于初始化游戏区域，设置，改变游戏区域相关参数
ManageArea	manager;	//用于初始化管理区域，设置管理区相关参数
Snake		s;			//用于初始化蛇，数据化蛇，操作蛇

////////////////////////////////////////////
//游戏的消息控制，流程控制函数
////////////////////////////////////////////
void MessageDispose()  //消息处理函数
{
	char c;
	s.now = NULL;
	bool Keepdown = false;

	while(true)
	{
		Keepdown = false;  //是否持续按W A S D 中的一个按键
		if(kbhit())
		{
			switch(c = getch())  //处理按键消息（W A S D）
			{
			case 'w':
			case 'W':			
				if(s.toward == UP)Keepdown = true;
				else if(s.toward != DOWN&&s.toward != UP)s.toward = UP;
				break;
			case 's':
			case 'S':
				if(s.toward == DOWN)Keepdown = true;
				else if(s.toward != UP&&s.toward != DOWN)s.toward = DOWN;
				break;
			case 'a':
			case 'A':
				if(s.toward == LEFT)Keepdown = true;
				else if(s.toward != RIGHT&&s.toward != LEFT)s.toward = LEFT;
				break;
			case 'd':
			case 'D':
				if(s.toward == RIGHT)Keepdown = true;
				else if(s.toward != LEFT&&s.toward != RIGHT)s.toward = RIGHT;
				break;
			case 'p':
			case 'P':
				manager.m_pause = !manager.m_pause;	//设置暂停或开始
				manager.DisplayPause();	//显示暂停或开始
				break;
			case 'r':
			case 'R':
				ReInit();
				break;
			}
		}
		if(true == manager.m_pause)	//如果暂停，直接进行下次循环
		{
			continue;
		}
		BeginBatchDraw();
		switch(a.m_game[s.head->m_x/10][s.head->m_y/10].ReturnFull())	//检测蛇头遇到的情况
		{
		case 2:				//遇到食物，蛇身加长
			a.ChangeColor(s.head->m_x/10,s.head->m_y/10,BLUE);
			s.SnakeAdd();	//蛇增长
			a.ChangeColor(s.head->m_x/10,s.head->m_y/10,BLUE);
			a.DelFood();	//删除食物
			if(a.CreatFood() == false)	//创建新食物,并检查是否通关
			{
				game.WinGame();			//游戏通关（当地图没地方创建食物时）
			}
			manager.ScoreAdd(1);		//加一分
			break;
		case 0:
			a.m_game[s.last->m_x/10][s.last->m_y/10].setfull(0);				//设置蛇尾经过处无障碍
			a.ChangeColor(s.last->m_x/10,s.last->m_y/10,BLACK);
			s.SnakeMove();  //蛇移动一次
			a.ChangeColor(s.head->m_x/10,s.head->m_y/10,BLUE);
			a.m_game[s.head->next->m_x/10][s.head->next->m_y/10].setfull(1);	//设置蛇头经过处有障碍
			break;
		case 1:				//遇到障碍物
			game.FailGame();
			a.ChangeColor(s.head->m_x/10,s.head->m_y/10,RGB(255,127,39));
			FlushBatchDraw();
			{
				char c = ' '; 
				while(c != 'r' && c != 'R')  //当游戏失败时，按R键可重新进行游戏
				{
					if(kbhit())
					{
						c = getch();
					}
					Sleep(10);
				}
			}
			ReInit(); //重新开始游戏
			break;
		}
		EndBatchDraw();
		if(Keepdown==false)
		{
			Sleep(100);
			manager.TimeAdd(0.1);	//增加时间
		}
		else						//当持续按下按方向键时
		{
			Sleep(40);				//适当休眠可以增加游戏流畅性
			manager.TimeAdd(0.04);  //增加时间
		}
	}
}
////////////////////////////////////////////
//游戏初始化函数
////////////////////////////////////////////
void ReInit()
{
	a.Init(); //初始化游戏区
	s.Init();  //初始化蛇
	manager.Init();  //初始化管理区
	//绘制蛇的开始状态
	s.now = s.head;
	a.ChangeColor(s.now->m_x/10,s.now->m_y/10,BLUE);
	s.now = s.last;
	a.ChangeColor(s.now->m_x/10,s.now->m_y/10,BLUE);
}
////////////////////////////////////////////
//main函数，程序入口
////////////////////////////////////////////
int main(void)
{
	game.Init();		//初始化游戏参数、设置
	ReInit();			//初始化其他
	MessageDispose();	//消息处理函数

	return 0;
}
