#include "stdafx.h"
#include <omp.h>
#include "Resource.h"
#include <mmsystem.h>
#include <ddraw.h>
#include "audio.h"
#include "gamelib.h"
#include "Blast.h"

namespace game_framework
{
	static int	redBmp[] = {
	IDB_RED_EXPL1, IDB_RED_EXPL2, IDB_RED_EXPL3, IDB_RED_EXPL4, IDB_RED_EXPL5,
	IDB_RED_EXPL6, IDB_RED_EXPL7, IDB_RED_EXPL8, IDB_RED_EXPL9, IDB_RED_EXPL10,
	IDB_RED_SHAT1, IDB_RED_SHAT2, IDB_RED_SHAT3, IDB_RED_SHAT4, IDB_RED_SHAT5,
	IDB_RED_SHAT6, IDB_RED_SHAT7, IDB_RED_SHAT8, IDB_RED_SHAT9, IDB_RED_SHAT10,
	IDB_RED_SHAT11, IDB_RED_SHAT12, IDB_RED_SHAT13, IDB_RED_SHAT14, IDB_RED_SHAT15,
	IDB_RED_VEXPL1, IDB_RED_VEXPL2, IDB_RED_VEXPL3, IDB_RED_VEXPL4, IDB_RED_VEXPL5,
	IDB_RED_VEXPL6, IDB_RED_VEXPL7, IDB_RED_VEXPL8, IDB_RED_VEXPL9, IDB_RED_VEXPL10,
	IDB_RED_VEXPL11, IDB_RED_VEXPL12, IDB_RED_VEXPL13, IDB_RED_VEXPL14, IDB_RED_VEXPL15,
	IDB_RED_VEXPL16, IDB_RED_VEXPL17, IDB_RED_VEXPL18, IDB_RED_VEXPL19, IDB_RED_VEXPL20,
	IDB_RED_VEXPL21, IDB_RED_VEXPL22, IDB_RED_VEXPL23, IDB_RED_VEXPL24, IDB_RED_VEXPL25,
	IDB_RED_VEXPL26, IDB_RED_VEXPL27, IDB_RED_VEXPL28, IDB_RED_VEXPL29, IDB_RED_VEXPL30,
	IDB_RED_HEXPL1, IDB_RED_HEXPL2, IDB_RED_HEXPL3, IDB_RED_HEXPL4, IDB_RED_HEXPL5,
	IDB_RED_HEXPL6, IDB_RED_HEXPL7, IDB_RED_HEXPL8, IDB_RED_HEXPL9, IDB_RED_HEXPL10,
	IDB_RED_HEXPL11, IDB_RED_HEXPL12, IDB_RED_HEXPL13, IDB_RED_HEXPL14, IDB_RED_HEXPL15,
	IDB_RED_HEXPL16, IDB_RED_HEXPL17, IDB_RED_HEXPL18, IDB_RED_HEXPL19, IDB_RED_HEXPL20,
	IDB_RED_HEXPL21, IDB_RED_HEXPL22, IDB_RED_HEXPL23, IDB_RED_HEXPL24, IDB_RED_HEXPL25,
	IDB_RED_HEXPL26, IDB_RED_HEXPL27, IDB_RED_HEXPL28, IDB_RED_HEXPL29, IDB_RED_HEXPL30 },


			orangeBmp[] = {
	IDB_ORG_EXPL1, IDB_ORG_EXPL2, IDB_ORG_EXPL3, IDB_ORG_EXPL4, IDB_ORG_EXPL5,
	IDB_ORG_EXPL6, IDB_ORG_EXPL7, IDB_ORG_EXPL8, IDB_ORG_EXPL9, IDB_ORG_EXPL10,
	IDB_ORG_SHAT1, IDB_ORG_SHAT2, IDB_ORG_SHAT3, IDB_ORG_SHAT4, IDB_ORG_SHAT5,
	IDB_ORG_SHAT6, IDB_ORG_SHAT7, IDB_ORG_SHAT8, IDB_ORG_SHAT9, IDB_ORG_SHAT10,
	IDB_ORG_SHAT11, IDB_ORG_SHAT12, IDB_ORG_SHAT13, IDB_ORG_SHAT14, IDB_ORG_SHAT15,
	IDB_ORG_VEXPL1, IDB_ORG_VEXPL2, IDB_ORG_VEXPL3, IDB_ORG_VEXPL4, IDB_ORG_VEXPL5,
	IDB_ORG_VEXPL6, IDB_ORG_VEXPL7, IDB_ORG_VEXPL8, IDB_ORG_VEXPL9, IDB_ORG_VEXPL10,
	IDB_ORG_VEXPL11, IDB_ORG_VEXPL12, IDB_ORG_VEXPL13, IDB_ORG_VEXPL14, IDB_ORG_VEXPL15,
	IDB_ORG_VEXPL16, IDB_ORG_VEXPL17, IDB_ORG_VEXPL18, IDB_ORG_VEXPL19, IDB_ORG_VEXPL20,
	IDB_ORG_VEXPL21, IDB_ORG_VEXPL22, IDB_ORG_VEXPL23, IDB_ORG_VEXPL24, IDB_ORG_VEXPL25,
	IDB_ORG_VEXPL26, IDB_ORG_VEXPL27, IDB_ORG_VEXPL28, IDB_ORG_VEXPL29, IDB_ORG_VEXPL30,
	IDB_ORG_HEXPL1, IDB_ORG_HEXPL2, IDB_ORG_HEXPL3, IDB_ORG_HEXPL4, IDB_ORG_HEXPL5,
	IDB_ORG_HEXPL6, IDB_ORG_HEXPL7, IDB_ORG_HEXPL8, IDB_ORG_HEXPL9, IDB_ORG_HEXPL10,
	IDB_ORG_HEXPL11, IDB_ORG_HEXPL12, IDB_ORG_HEXPL13, IDB_ORG_HEXPL14, IDB_ORG_HEXPL15,
	IDB_ORG_HEXPL16, IDB_ORG_HEXPL17, IDB_ORG_HEXPL18, IDB_ORG_HEXPL19, IDB_ORG_HEXPL20,
	IDB_ORG_HEXPL21, IDB_ORG_HEXPL22, IDB_ORG_HEXPL23, IDB_ORG_HEXPL24, IDB_ORG_HEXPL25,
	IDB_ORG_HEXPL26, IDB_ORG_HEXPL27, IDB_ORG_HEXPL28, IDB_ORG_HEXPL29, IDB_ORG_HEXPL30 },


			yellowBmp[] = {
	IDB_YEL_EXPL1, IDB_YEL_EXPL2, IDB_YEL_EXPL3, IDB_YEL_EXPL4, IDB_YEL_EXPL5,
	IDB_YEL_EXPL6, IDB_YEL_EXPL7, IDB_YEL_EXPL8, IDB_YEL_EXPL9, IDB_YEL_EXPL10,
	IDB_YEL_SHAT1, IDB_YEL_SHAT2, IDB_YEL_SHAT3, IDB_YEL_SHAT4, IDB_YEL_SHAT5,
	IDB_YEL_SHAT6, IDB_YEL_SHAT7, IDB_YEL_SHAT8, IDB_YEL_SHAT9, IDB_YEL_SHAT10,
	IDB_YEL_SHAT11, IDB_YEL_SHAT12, IDB_YEL_SHAT13, IDB_YEL_SHAT14, IDB_YEL_SHAT15,
	IDB_YEL_VEXPL1, IDB_YEL_VEXPL2, IDB_YEL_VEXPL3, IDB_YEL_VEXPL4, IDB_YEL_VEXPL5,
	IDB_YEL_VEXPL6, IDB_YEL_VEXPL7, IDB_YEL_VEXPL8, IDB_YEL_VEXPL9, IDB_YEL_VEXPL10,
	IDB_YEL_VEXPL11, IDB_YEL_VEXPL12, IDB_YEL_VEXPL13, IDB_YEL_VEXPL14, IDB_YEL_VEXPL15,
	IDB_YEL_VEXPL16, IDB_YEL_VEXPL17, IDB_YEL_VEXPL18, IDB_YEL_VEXPL19, IDB_YEL_VEXPL20,
	IDB_YEL_VEXPL21, IDB_YEL_VEXPL22, IDB_YEL_VEXPL23, IDB_YEL_VEXPL24, IDB_YEL_VEXPL25,
	IDB_YEL_VEXPL26, IDB_YEL_VEXPL27, IDB_YEL_VEXPL28, IDB_YEL_VEXPL29, IDB_YEL_VEXPL30,
	IDB_YEL_HEXPL1, IDB_YEL_HEXPL2, IDB_YEL_HEXPL3, IDB_YEL_HEXPL4, IDB_YEL_HEXPL5,
	IDB_YEL_HEXPL6, IDB_YEL_HEXPL7, IDB_YEL_HEXPL8, IDB_YEL_HEXPL9, IDB_YEL_HEXPL10,
	IDB_YEL_HEXPL11, IDB_YEL_HEXPL12, IDB_YEL_HEXPL13, IDB_YEL_HEXPL14, IDB_YEL_HEXPL15,
	IDB_YEL_HEXPL16, IDB_YEL_HEXPL17, IDB_YEL_HEXPL18, IDB_YEL_HEXPL19, IDB_YEL_HEXPL20,
	IDB_YEL_HEXPL21, IDB_YEL_HEXPL22, IDB_YEL_HEXPL23, IDB_YEL_HEXPL24, IDB_YEL_HEXPL25,
	IDB_YEL_HEXPL26, IDB_YEL_HEXPL27, IDB_YEL_HEXPL28, IDB_YEL_HEXPL29, IDB_YEL_HEXPL30 },


			greenBmp[] = {
	IDB_GRE_EXPL1, IDB_GRE_EXPL2, IDB_GRE_EXPL3, IDB_GRE_EXPL4, IDB_GRE_EXPL5,
	IDB_GRE_EXPL6, IDB_GRE_EXPL7, IDB_GRE_EXPL8, IDB_GRE_EXPL9, IDB_GRE_EXPL10,
	IDB_GRE_SHAT1, IDB_GRE_SHAT2, IDB_GRE_SHAT3, IDB_GRE_SHAT4, IDB_GRE_SHAT5,
	IDB_GRE_SHAT6, IDB_GRE_SHAT7, IDB_GRE_SHAT8, IDB_GRE_SHAT9, IDB_GRE_SHAT10,
	IDB_GRE_SHAT11, IDB_GRE_SHAT12, IDB_GRE_SHAT13, IDB_GRE_SHAT14, IDB_GRE_SHAT15,
	IDB_GRE_VEXPL1, IDB_GRE_VEXPL2, IDB_GRE_VEXPL3, IDB_GRE_VEXPL4, IDB_GRE_VEXPL5,
	IDB_GRE_VEXPL6, IDB_GRE_VEXPL7, IDB_GRE_VEXPL8, IDB_GRE_VEXPL9, IDB_GRE_VEXPL10,
	IDB_GRE_VEXPL11, IDB_GRE_VEXPL12, IDB_GRE_VEXPL13, IDB_GRE_VEXPL14, IDB_GRE_VEXPL15,
	IDB_GRE_VEXPL16, IDB_GRE_VEXPL17, IDB_GRE_VEXPL18, IDB_GRE_VEXPL19, IDB_GRE_VEXPL20,
	IDB_GRE_VEXPL21, IDB_GRE_VEXPL22, IDB_GRE_VEXPL23, IDB_GRE_VEXPL24, IDB_GRE_VEXPL25,
	IDB_GRE_VEXPL26, IDB_GRE_VEXPL27, IDB_GRE_VEXPL28, IDB_GRE_VEXPL29, IDB_GRE_VEXPL30,
	IDB_GRE_HEXPL1, IDB_GRE_HEXPL2, IDB_GRE_HEXPL3, IDB_GRE_HEXPL4, IDB_GRE_HEXPL5,
	IDB_GRE_HEXPL6, IDB_GRE_HEXPL7, IDB_GRE_HEXPL8, IDB_GRE_HEXPL9, IDB_GRE_HEXPL10,
	IDB_GRE_HEXPL11, IDB_GRE_HEXPL12, IDB_GRE_HEXPL13, IDB_GRE_HEXPL14, IDB_GRE_HEXPL15,
	IDB_GRE_HEXPL16, IDB_GRE_HEXPL17, IDB_GRE_HEXPL18, IDB_GRE_HEXPL19, IDB_GRE_HEXPL20,
	IDB_GRE_HEXPL21, IDB_GRE_HEXPL22, IDB_GRE_HEXPL23, IDB_GRE_HEXPL24, IDB_GRE_HEXPL25,
	IDB_GRE_HEXPL26, IDB_GRE_HEXPL27, IDB_GRE_HEXPL28, IDB_GRE_HEXPL29, IDB_GRE_HEXPL30 },


			blueBmp[] = {
	IDB_BLU_EXPL1, IDB_BLU_EXPL2, IDB_BLU_EXPL3, IDB_BLU_EXPL4, IDB_BLU_EXPL5,
	IDB_BLU_EXPL6, IDB_BLU_EXPL7, IDB_BLU_EXPL8, IDB_BLU_EXPL9, IDB_BLU_EXPL10,
	IDB_BLU_SHAT1, IDB_BLU_SHAT2, IDB_BLU_SHAT3, IDB_BLU_SHAT4, IDB_BLU_SHAT5,
	IDB_BLU_SHAT6, IDB_BLU_SHAT7, IDB_BLU_SHAT8, IDB_BLU_SHAT9, IDB_BLU_SHAT10,
	IDB_BLU_SHAT11, IDB_BLU_SHAT12, IDB_BLU_SHAT13, IDB_BLU_SHAT14, IDB_BLU_SHAT15,
	IDB_BLU_VEXPL1, IDB_BLU_VEXPL2, IDB_BLU_VEXPL3, IDB_BLU_VEXPL4, IDB_BLU_VEXPL5,
	IDB_BLU_VEXPL6, IDB_BLU_VEXPL7, IDB_BLU_VEXPL8, IDB_BLU_VEXPL9, IDB_BLU_VEXPL10,
	IDB_BLU_VEXPL11, IDB_BLU_VEXPL12, IDB_BLU_VEXPL13, IDB_BLU_VEXPL14, IDB_BLU_VEXPL15,
	IDB_BLU_VEXPL16, IDB_BLU_VEXPL17, IDB_BLU_VEXPL18, IDB_BLU_VEXPL19, IDB_BLU_VEXPL20,
	IDB_BLU_VEXPL21, IDB_BLU_VEXPL22, IDB_BLU_VEXPL23, IDB_BLU_VEXPL24, IDB_BLU_VEXPL25,
	IDB_BLU_VEXPL26, IDB_BLU_VEXPL27, IDB_BLU_VEXPL28, IDB_BLU_VEXPL29, IDB_BLU_VEXPL30,
	IDB_BLU_HEXPL1, IDB_BLU_HEXPL2, IDB_BLU_HEXPL3, IDB_BLU_HEXPL4, IDB_BLU_HEXPL5,
	IDB_BLU_HEXPL6, IDB_BLU_HEXPL7, IDB_BLU_HEXPL8, IDB_BLU_HEXPL9, IDB_BLU_HEXPL10,
	IDB_BLU_HEXPL11, IDB_BLU_HEXPL12, IDB_BLU_HEXPL13, IDB_BLU_HEXPL14, IDB_BLU_HEXPL15,
	IDB_BLU_HEXPL16, IDB_BLU_HEXPL17, IDB_BLU_HEXPL18, IDB_BLU_HEXPL19, IDB_BLU_HEXPL20,
	IDB_BLU_HEXPL21, IDB_BLU_HEXPL22, IDB_BLU_HEXPL23, IDB_BLU_HEXPL24, IDB_BLU_HEXPL25,
	IDB_BLU_HEXPL26, IDB_BLU_HEXPL27, IDB_BLU_HEXPL28, IDB_BLU_HEXPL29, IDB_BLU_HEXPL30 },


			purpleBmp[] = {
	IDB_PUR_EXPL1, IDB_PUR_EXPL2, IDB_PUR_EXPL3, IDB_PUR_EXPL4, IDB_PUR_EXPL5,
	IDB_PUR_EXPL6, IDB_PUR_EXPL7, IDB_PUR_EXPL8, IDB_PUR_EXPL9, IDB_PUR_EXPL10,
	IDB_PUR_SHAT1, IDB_PUR_SHAT2, IDB_PUR_SHAT3, IDB_PUR_SHAT4, IDB_PUR_SHAT5,
	IDB_PUR_SHAT6, IDB_PUR_SHAT7, IDB_PUR_SHAT8, IDB_PUR_SHAT9, IDB_PUR_SHAT10,
	IDB_PUR_SHAT11, IDB_PUR_SHAT12, IDB_PUR_SHAT13, IDB_PUR_SHAT14, IDB_PUR_SHAT15,
	IDB_PUR_VEXPL1, IDB_PUR_VEXPL2, IDB_PUR_VEXPL3, IDB_PUR_VEXPL4, IDB_PUR_VEXPL5,
	IDB_PUR_VEXPL6, IDB_PUR_VEXPL7, IDB_PUR_VEXPL8, IDB_PUR_VEXPL9, IDB_PUR_VEXPL10,
	IDB_PUR_VEXPL11, IDB_PUR_VEXPL12, IDB_PUR_VEXPL13, IDB_PUR_VEXPL14, IDB_PUR_VEXPL15,
	IDB_PUR_VEXPL16, IDB_PUR_VEXPL17, IDB_PUR_VEXPL18, IDB_PUR_VEXPL19, IDB_PUR_VEXPL20,
	IDB_PUR_VEXPL21, IDB_PUR_VEXPL22, IDB_PUR_VEXPL23, IDB_PUR_VEXPL24, IDB_PUR_VEXPL25,
	IDB_PUR_VEXPL26, IDB_PUR_VEXPL27, IDB_PUR_VEXPL28, IDB_PUR_VEXPL29, IDB_PUR_VEXPL30,
	IDB_PUR_HEXPL1, IDB_PUR_HEXPL2, IDB_PUR_HEXPL3, IDB_PUR_HEXPL4, IDB_PUR_HEXPL5,
	IDB_PUR_HEXPL6, IDB_PUR_HEXPL7, IDB_PUR_HEXPL8, IDB_PUR_HEXPL9, IDB_PUR_HEXPL10,
	IDB_PUR_HEXPL11, IDB_PUR_HEXPL12, IDB_PUR_HEXPL13, IDB_PUR_HEXPL14, IDB_PUR_HEXPL15,
	IDB_PUR_HEXPL16, IDB_PUR_HEXPL17, IDB_PUR_HEXPL18, IDB_PUR_HEXPL19, IDB_PUR_HEXPL20,
	IDB_PUR_HEXPL21, IDB_PUR_HEXPL22, IDB_PUR_HEXPL23, IDB_PUR_HEXPL24, IDB_PUR_HEXPL25,
	IDB_PUR_HEXPL26, IDB_PUR_HEXPL27, IDB_PUR_HEXPL28, IDB_PUR_HEXPL29, IDB_PUR_HEXPL30 };

	void Blast::SetTopLeft(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

	CMovingBitmap NormalBlast::normalBlast[6][10], NormalBlast::shatter[6][15];

	NormalBlast::NormalBlast() :curShow(0), totalShow(0)
	{
		sound = &CGameState::sound;
	}

	NormalBlast::NormalBlast(int style, int x, int y) :curShow(0), size(1.8), totalShow(rand() % 2 + 2)
	{
		this->style = style;
		SetTopLeft(x, y);

		int direction[] = { -2, -1, 0, 2, 1 };	//direction & speed for shatter
		#pragma omp parallel for
		for (int i = 0; i < totalShow; i++)
		{
			//init shatters' position at center
			shatPosition[i][0] = x;
			shatPosition[i][1] = y;

			shatShow[i] = rand() % 15;				//set shatter start frame
			shift[i][0] = direction[rand() % 5];	//random direction x
			shift[i][1] = direction[rand() % 2 + 3];//random dirention y
			shift[i][2] = rand() % 2;				//set shatter rotation { 1 = clockwise, 0 = counter-clockwise}
		}
	}

	void NormalBlast::LoadBitmap()
	{
		int *bmpID[] = { blueBmp, purpleBmp, orangeBmp, greenBmp , redBmp, yellowBmp};

		for (int i = 0; i < 6; i++)
		{
			for (int j = 0; j < 10; j++)
				normalBlast[i][j].LoadBitmap(bmpID[i][j], RGB(254, 191, 200));

			for (int j = 0; j < 15; j++)
				shatter[i][j].LoadBitmap(bmpID[i][j + 10], RGB(254, 191, 200));
		}
	}

	void NormalBlast::OnMove()
	{
		curShow++;
		if(curShow % 2) size -= 0.1;	//zoom-out shatter

		#pragma omp parallel for
		for (int i = 0; i < totalShow; i++)
		{	//move shatters
			shatPosition[i][0] += shift[i][0];
			shatPosition[i][1] += shift[i][1];

			//rotate shatters
			if (!(curShow % 3))
			{
				if(shift[i][2]) shatShow[i] = (shatShow[i] - 1) < 0 ? (shatShow[i] - 1) + 15 : (shatShow[i] - 1);	//counter-clockwise
				else shatShow[i] = (shatShow[i] + 1) % 15;	//clockwise
			}
		}
	}

	void NormalBlast::OnShow()
	{
		if (curShow < 10)
		{	//show blast circle
			normalBlast[style - 1][curShow].SetTopLeft(x - (normalBlast[style - 1][curShow].Width() / 2) + 25, y - (normalBlast[style - 1][curShow].Height() / 2) + 25);
			normalBlast[style - 1][curShow].ShowBitmap();
		}

		if (curShow >= 4)
		{	//show shatters
			#pragma omp parallel for
			for (int i = 0; i < totalShow; i++)
			{
				shatter[style - 1][shatShow[i]].SetTopLeft(shatPosition[i][0], shatPosition[i][1]);
				shatter[style - 1][shatShow[i]].ShowBitmap(size);
			}
		}
	}

	bool NormalBlast::IsLast()
	{
		return (curShow == 25);
	}

	CMovingBitmap LineBlast::horizontal[6][30], LineBlast::vertical[6][30];

	LineBlast::LineBlast(int style, int x, int y, int power) :powStyle(power), curShow(0)
	{
		this->style = style;
		SetTopLeft(x, y);
		sound = &CGameState::sound;
	}

	void LineBlast::LoadBitmap()
	{
		int *bmpID[] = { blueBmp, purpleBmp, orangeBmp, greenBmp , redBmp, yellowBmp };

		for (int i = 0; i < 6; i++)
		{
			for (int j = 0; j < 30; j++)
				horizontal[i][j].LoadBitmap(bmpID[i][j + 55], RGB(254, 191, 200));

			for (int j = 0; j < 30; j++)
				vertical[i][j].LoadBitmap(bmpID[i][j + 25], RGB(254, 191, 200));
		}
	}

	void LineBlast::OnMove()
	{
		curShow++;
	}

	void LineBlast::OnShow()
	{
		if (*sound && curShow == 1) CAudio::Instance()->Play(AUDIO_LINE_BLAST, false);
		switch (powStyle)
		{
		case 1:
			horizontal[style - 1][curShow].SetTopLeft(x - (horizontal[style - 1][curShow].Width() / 2 - 25), y);
			horizontal[style - 1][curShow].ShowBitmap();
			break;
		case 2:
			vertical[style - 1][curShow].SetTopLeft(x, y - (vertical[style - 1][curShow].Height() / 2 - 25));
			vertical[style - 1][curShow].ShowBitmap();
			break;
		default:
			GAME_ASSERT(0, "Invalid power style");
		}
	}

	bool LineBlast::IsLast()
	{
		return curShow >= 29;
	}

	CAnimation SuperBlast::chocalate;

	SuperBlast::SuperBlast(int x, int y, int delay, bool showAll) :curShow(0), lightningDelay(delay), showAll(showAll)
	{
		this->x = x;
		this->y = y;
		chocalate.SetDelayCount(3);
		sound = &CGameState::sound;
	}

	SuperBlast::~SuperBlast()
	{
		CAudio::Instance()->Stop(AUDIO_SUPER_REMOVE);
	}

	void SuperBlast::OnMove()
	{
		curShow++;
		chocalate.OnMove();

		for (auto i = magicBlasts.begin(); i != magicBlasts.end();)
		{
			if ((*i).IsLast()) i = magicBlasts.erase(i);
			else
			{
				(*i).OnMove();
				i++;
			}
		}
	}

	void SuperBlast::OnShow()
	{
		if (!target.size()) return;

		showAll ? ShowLightning(true) : ShowLightning();

		for (auto i = magicBlasts.begin(); i != magicBlasts.end(); i++)
		{	//Show magic blast effect on every target
			(*i).OnShow();
		}
	}

	void SuperBlast::ShowLightning(bool showAll)
	{
		chocalate.SetTopLeft(x, y);
		chocalate.OnShow();	//Show chocalate
		CDC *pDC = CDDraw::GetBackCDC();

		CPen penLighting;
		CPen *pPen;

		penLighting.CreatePen(PS_SOLID | PS_COSMETIC, showAll ? 5 : rand() % 10 + 1, RGB(207, 249, 245));

		pPen = pDC->SelectObject(&penLighting);

		if (showAll)
		{	// Show all lightning effect at one time
			for (auto i = target.begin(); i != target.end(); i++)
			{
				DrawLine(pDC, CPoint(x + 25, y + 25), *i); //Draw a lightning from current position to target
			}
		}
		else
		{
			for (int i = lightningDelay; i >= 0; i--)
			{
				if (curShow - i >= 0 && curShow - i < target.size())
				{
					DrawLine(pDC, CPoint(x + 25, y + 25), target[curShow - i]);//Draw a lightning from current position to target
				}
			}
		}
		// Restore the previous pen.
		pDC->SelectObject(pPen);
		CDDraw::ReleaseBackCDC();
		Sleep(5);
	}

	void SuperBlast::DrawLine(CDC* pDC, const CPoint& start, const CPoint& end)
	{
		list<CPoint>* route = GetRoutePoints(start, end); //Get points on lighting route

		//DrawLine
		pDC->MoveTo(*route->begin());
		for (auto j = route->begin()++; j != route->end(); j++)
		{
			pDC->LineTo(*j);
		}
		delete route;

		MagicBlast blast(end);
		bool blastExists = false;
		for (auto i = magicBlasts.begin(); i != magicBlasts.end(); i++)
		{	//to avoid show magic blast repeatly
			if ((*i) == blast)
			{
				blastExists = true;
				break;
			}
		}
		if (!blastExists) magicBlasts.push_back(blast);//push a new magic blast if it doesn't exists
	}

	list<CPoint>* SuperBlast::GetRoutePoints(CPoint start, CPoint end)
	{
		// Get points on line from start to end by straight line equation
		list<CPoint>* route = new list<CPoint>();
		int interval = abs(start.x - end.x) / (abs(start.x - end.x) > 10 ? abs(start.x - end.x) / 10 : abs(start.x - end.x) > 0 ? abs(start.x - end.x) : 1);
		int totalPoint = abs(start.x - end.x) > 10 ? abs(start.x - end.x) / 10 : abs(start.x - end.x);
		int reverse = start.x > end.x ? -1 : 1;
		for (int i = 0; i < totalPoint; i++)
		{
			CPoint point;
			point.x = start.x + interval * i * reverse;
			point.y = (point.x * end.y - point.x * start.y - start.x * end.y + end.x * start.y) / (end.x - start.x);
			route->push_back(point);
		}
		route->push_back(end);

		// move position of points in a stable range randomly
		int j = 0;
		for (auto i = route->begin(); i != route->end(); i++, j++)
		{
			if (j % 2) i->x += rand() % 20 - 10;
			else i->y += rand() % 20 - 10;
		}

		return route;
	}

	void SuperBlast::AddPoint(int x, int y)
	{
		target.push_back(CPoint(x, y));
	}

	bool SuperBlast::IsLast()
	{
		return ((curShow >= target.size() + lightningDelay) || (showAll && curShow >= 10));
	}

	void SuperBlast::LoadBitmap()
	{
		chocalate.AddBitmap("Bitmaps\\BlastSuperCandy1.bmp", RGB(254, 191, 200));
		chocalate.AddBitmap("Bitmaps\\BlastSuperCandy2.bmp", RGB(254, 191, 200));
		chocalate.AddBitmap("Bitmaps\\BlastSuperCandy3.bmp", RGB(254, 191, 200));
		chocalate.AddBitmap("Bitmaps\\BlastSuperCandy4.bmp", RGB(254, 191, 200));
		chocalate.AddBitmap("Bitmaps\\BlastSuperCandy5.bmp", RGB(254, 191, 200));
	}

	CMovingBitmap MagicBlast::bmp[4];

	MagicBlast::MagicBlast(int x, int y) :x(x), y(y), curShow(0)
	{
		sound = &CGameState::sound;
	}

	MagicBlast::MagicBlast(CPoint p) :x(p.x), y(p.y), curShow(0)
	{ }

	bool MagicBlast::IsLast()
	{
		return curShow == 3;
	}

	void MagicBlast::LoadBitmap()
	{
		bmp[0].LoadBitmap("Bitmaps\\MagicBlast1.bmp", RGB(255, 255, 255));
		bmp[1].LoadBitmap("Bitmaps\\MagicBlast2.bmp", RGB(255, 255, 255));
		bmp[2].LoadBitmap("Bitmaps\\MagicBlast3.bmp", RGB(255, 255, 255));
		bmp[3].LoadBitmap("Bitmaps\\MagicBlast4.bmp", RGB(255, 255, 255));
	}

	void MagicBlast::OnMove()
	{
		delay = delay == 1 ? 0 : 1;
		if (delay) curShow++;
	}

	void MagicBlast::OnShow()
	{
		if (delay)
		{
			bmp[curShow].SetTopLeft(x - bmp[curShow].Width() / 2, y - bmp[curShow].Height() / 2);
			bmp[curShow].ShowBitmap();
		}
	}

	bool MagicBlast::operator==(const MagicBlast & rhs)
	{
		return rhs.x == x && rhs.y == y;
	}

}