#include "stdafx.h"
#include <omp.h>
#include "Resource.h"
#include <mmsystem.h>
#include <fstream>
#include <string>
#include <ddraw.h>
#include <algorithm>
#include <set>
#include "audio.h"
#include "gamelib.h"
#include "Blast.h"
#include "Candy.h"
#include "Stage.h"
#include "ScoreBoard.h"
#include "GameArea.h"

namespace game_framework
{
	static int audioID[12] = { AUDIO_COMBO1, AUDIO_COMBO2, AUDIO_COMBO3, AUDIO_COMBO4, AUDIO_COMBO5, AUDIO_COMBO6,
						AUDIO_COMBO7, AUDIO_COMBO8, AUDIO_COMBO9, AUDIO_COMBO10, AUDIO_COMBO11, AUDIO_COMBO12 };

	GameArea::GameArea()
		:x(280), y(35), MAX_RAND_NUM(4), initiating(1), ending(0), running(1), gameOver(0), delay(0),
		delayRemoveStyle(0), delayRemove(0), currentComboSound(0), goldFinger(0), releaseSwap(0), totalCandyCleared(0),
		playingVoice(0)
	{
		scoreBoard.score = 0;
		for (int i = 0; i < MaxHeight; i++)
			for (int j = 0; j < MaxWidth; j++)
				curPosition[i][j] = NULL;

		sound = &CGameState::sound;
	}

	GameArea::~GameArea()
	{
		for (auto i = blasts.begin(); i != blasts.end(); i++)
			delete *i;
		for (auto i = removeList.begin(); i != removeList.end(); i++)
			delete *i;
	}

	void GameArea::LoadBitmap()
	{
		area.LoadBitmap(IDB_CONTAINER);
		singleJelly.LoadBitmap("Bitmaps\\Jelly1.bmp");
		doubleJelly.LoadBitmap("Bitmaps\\Jelly2.bmp");
		scoreBoard.LoadBitmap();
		sweet.LoadBitmap("Bitmaps\\sweet.bmp", RGB(0, 0, 0));
		tasty.LoadBitmap("Bitmaps\\tasty.bmp", RGB(0, 0, 0));
		delicious.LoadBitmap("Bitmaps\\delicious.bmp", RGB(0, 0, 0));
		divine.LoadBitmap("Bitmaps\\divine.bmp", RGB(0, 0, 0));
		sugarCrush.LoadBitmap("Bitmaps\\SugarCrush.bmp", RGB(0, 0, 0));
		portalStart1.LoadBitmap("Bitmaps\\portalStart1.bmp", RGB(0, 0, 0));
		portalStart2.LoadBitmap("Bitmaps\\portalStart2.bmp", RGB(0, 0, 0));
		portalEnd1.LoadBitmap("Bitmaps\\portalEnd1.bmp", RGB(0, 0, 0));
		portalEnd2.LoadBitmap("Bitmaps\\portalEnd2.bmp", RGB(0, 0, 0));
		Candy::LoadBitmap();
		NormalBlast::LoadBitmap();
		LineBlast::LoadBitmap();
		SuperBlast::LoadBitmap();
		MagicBlast::LoadBitmap();
	}

	void GameArea::LoadStage(vector<Stage*>& stages, int index)
	{
		int totalJelly = 0;
		//Update new spawn area
		spawnArea.clear();
		for (int i = 0; i < MaxHeight; i++)
		{
			for (int j = 0; j < MaxWidth; j++)
			{
				map[i][j] = stages[index]->map[i][j];
				if (map[i][j] == 1)
					spawnArea.push_back(pair<int, int>(i, j));
				else if (map[i][j] == 3)
					totalJelly += 1;
				else if (map[i][j] == 4)
					totalJelly += 2;
			}
		}

		//Update new gamedata
		MAX_RAND_NUM = stages[index]->candyType;
		scoreBoard.oneStar = stages[index]->scoreOne;
		scoreBoard.twoStar = stages[index]->scoreTwo;
		scoreBoard.threeStar = stages[index]->scoreThree;
		scoreBoard.lastHighScore = stages[index]->lastHighScore;
		scoreBoard.moves = stages[index]->maxStep;
		scoreBoard.mode = stages[index]->mode;
		this->stage = find(stages.begin(), stages.end(), stages[index]);

		//Init GameArea
		InitGameArea(totalJelly);
		InitCandy(stages[index]->initcandy);
	}

	//get candy position
	void GameArea::Find(Candy *candy, unsigned &row, unsigned &column)
	{
		for (unsigned i = 0; i < MaxHeight; i++)
			for (unsigned j = 0; j < MaxWidth; j++)
				if (&candies[i][j] == candy)
				{
					row = i;
					column = j;
					return;
				}
	}

	void GameArea::ReleasePower(Candy *candy, unsigned row, unsigned column)
	{
		if(candy != NULL) Find(candy, row, column); //if candy != NULL, get its' position in array
		else candy = &candies[row][column];			//else, row & column is candy's position

		if (!map[row][column]) return;	//return if candy is out of range in map

		if (!initiating && (map[row][column] == 3 || map[row][column] == 4))
		{	//break if there is a jelly at this position
			map[row][column]--;
			scoreBoard.target--;	//deduct total amount of jelly
		}

		if (!initiating)scoreBoard.score += 60;	//increase total score

		if (!initiating && candy->GetStyle())
		{	//play animation of a candy being destroyed
			blasts.push_back(new NormalBlast(candy->GetStyle(), candy->GetTopLeftX(), candy->GetTopLeftY()));
			totalCandyCleared++;
		}

		int power = candy->GetPower(), style = candy->GetStyle();
		candy->Kill();

		switch (power)
		{
		case 0:
			break;
		case 1://trigger horizontal Line blast
			if (!initiating && style)
			{	
				blasts.push_back(new LineBlast(style, candy->GetTopLeftX(), candy->GetTopLeftY(), 1));
			}
			RemoveRow(row);
			break;
		case 2://trigger vertical Line Blast
			if (!initiating && style)
			{	
				blasts.push_back(new LineBlast(style, candy->GetTopLeftX(), candy->GetTopLeftY(), 2));
			}
			RemoveColumn(column);
			break;
		case 3://trigger pack blast
			RemoveSquare(row, column, 1);
			if (!initiating && *sound)CAudio::Instance()->Play(AUDIO_SQUARE_REMOVE1, false);
			break;
		case 4:	//trigger super Blast
			RemoveStyle(candy->GetTopLeftX(), candy->GetTopLeftY());
			break;
		}
	}

	void GameArea::ReleaseSwap()
	{
		int firstPow = clickedCandies[0]->GetPower(), secondPow = clickedCandies[1]->GetPower();
		if (firstPow == 4 && secondPow == 4)
		{	//Swap 2 super candy
			unsigned row, column;
			Find(clickedCandies[0], row, column);
			clickedCandies[0]->Kill();
			clickedCandies[1]->Kill();
			RemoveAll(row, column);
		}
		else if (firstPow == 4 && secondPow)
		{	//Swap 1 super candy with power 1~3
			PowerAll(clickedCandies[1]->GetStyle(), secondPow, clickedCandies[0]->GetTopLeftX(), clickedCandies[0]->GetTopLeftY());
			clickedCandies[0]->Kill();
		}
		else if (secondPow == 4 && firstPow)
		{	//Swap 1 super candy with power 1~3
			PowerAll(clickedCandies[0]->GetStyle(), firstPow, clickedCandies[1]->GetTopLeftX(), clickedCandies[1]->GetTopLeftY());
			clickedCandies[1]->Kill();
		}
		else if (firstPow == 4 && !secondPow)
		{	//Swap 1 super candy with normal candy
			RemoveStyle(clickedCandies[0]->GetTopLeftX(), clickedCandies[0]->GetTopLeftY(), clickedCandies[1]->GetStyle());
			clickedCandies[0]->Kill();
		}
		else if (secondPow == 4 && !firstPow)
		{	//Swap 1 super candy with normal candy
			RemoveStyle(clickedCandies[1]->GetTopLeftX(), clickedCandies[1]->GetTopLeftY(), clickedCandies[0]->GetStyle());
			clickedCandies[1]->Kill();
		}
		else if (firstPow > 0 && firstPow < 3 && secondPow > 0 && secondPow < 3)
		{	//Swap 2 striped candy
			unsigned row, column;
			Find(clickedCandies[0], row, column);
			blasts.push_back(new LineBlast(clickedCandies[0]->GetStyle(), clickedCandies[0]->GetTopLeftX(), clickedCandies[0]->GetTopLeftY(), 1));
			blasts.push_back(new LineBlast(clickedCandies[0]->GetStyle(), clickedCandies[1]->GetTopLeftX(), clickedCandies[1]->GetTopLeftY(), 2));
			clickedCandies[0]->Kill();
			clickedCandies[1]->Kill();
			RemoveRow(row);
			RemoveColumn(column);
		}
		else if (firstPow == 3 && secondPow == 3)
		{	//Swap 2 wrapped candy
			unsigned row, column;
			Find(clickedCandies[0], row, column);
			RemoveSquare(row, column, 2);
			if (*sound) CAudio::Instance()->Play(AUDIO_SQUARE_REMOVE2, false);
		}
		else if (firstPow == 3 && secondPow > 0 && secondPow < 3)
		{	//Swap 1 wrapped candy with striped candy
			unsigned row, column;
			Find(clickedCandies[1], row, column);
			for (unsigned i = row - 1; i < row + 2; i++)
				RemoveRow(i);
			for (unsigned i = column - 1; i < column + 2; i++)
				RemoveColumn(i);
		}
		else if (secondPow == 3 && firstPow > 0 && firstPow < 3)
		{	//Swap 1 wrapped candy with striped candy
			unsigned row, column;
			Find(clickedCandies[0], row, column);
			for (unsigned i = row - 1; i < row + 2; i++)
				RemoveRow(i);
			for (unsigned i = column - 1; i < column + 2; i++)
				RemoveColumn(i);
		}
		InitClickedCandy();
		clickedCandies.clear();
		scoreBoard.moves--;
	}

	void GameArea::RemoveRow(unsigned row)
	{
		for (unsigned i = 0; i < MaxWidth; i++)
			if (map[row][i])
				ReleasePower(&candies[row][i]);
	}

	void GameArea::RemoveColumn(unsigned column)
	{
		for (unsigned i = 0; i < MaxWidth; i++)
			if (map[i][column])
				ReleasePower(&candies[i][column]);
	}

	void GameArea::RemoveSquare(int row, int column, int level)
	{	//Level 1: wrapped candy be activated by normal way
		if (level == 1)
		{
			for (int i = row - 2; i < row + 3; i++)
				for (int j = column - 2; j < column + 3; j++)
				{
					if (i == row && j == column) continue;
					if (i >= 0 && i < MaxHeight && j >= 0 && j < MaxWidth)
						if (i >= row - 1 && i < row + 2 && j >= column - 1 && j < column + 2)
							ReleasePower(NULL, i, j);
						else
						{
							int pushX = Compare(candies[i][j].GetTopLeftX(), candies[row][column].GetTopLeftX());
							int pushY = Compare(candies[i][j].GetTopLeftY(), candies[row][column].GetTopLeftY());
							candies[i][j].Push(pushX, pushY);
						}
				}
		}
		//Level 2: two wrapped candies being swapped with each other
		else if (level == 2)
		{
			for (int i = row - 2; i < row + 3; i++)
				for (int j = column - 2; j < column + 3; j++)
				{
					if (i == row && j == column) continue;
					if (i >= 0 && i < MaxHeight && j >= 0 && j < MaxWidth)
						ReleasePower(NULL, i, j);
				}
		}
	}

	void GameArea::RemoveAll(int row, int column)
	{	//two superCandy being swapped with each other
		for (int i = 0; i < MaxHeight; i++)
		{
			SuperBlast* superBlast = new SuperBlast(column * 50 + 280, row * 50 + 35);
			removeList.push_back(new list<Candy*>);
			for (int j = 0; j < MaxWidth; j++)
			{
				if (map[i][j])
				{
					(*removeList.back()).push_back(&candies[i][j]);
					superBlast->AddPoint(candies[i][j].GetTopLeftX() + 25, candies[i][j].GetTopLeftY() + 25);
				}
			}
			blasts.push_back(superBlast);

			if (!(*removeList.back()).size())
			{	//to avoid game crash, delete list if there's no candy to be removed in this row
				delete removeList.back();
				removeList.pop_back();
				delete blasts.back();
				blasts.pop_back();
			}
		}
	}

	void GameArea::RemoveStyle(int x, int y, int style)
	{
		if (!style) style = rand() % MAX_RAND_NUM + 1;

		removeList.push_back(new list<Candy*>);
		SuperBlast* superBlast = (x || y) ? new SuperBlast(x, y, 4) : NULL;

		for (int i = 0; i < MaxHeight; i++)
			for (int j = 0; j < MaxWidth; j++)
				if (candies[i][j].GetStyle() == style && candies[i][j].GetPower() != 4)
				{
					(*removeList.rbegin())->push_back(&candies[i][j]);
					if (superBlast != NULL) superBlast->AddPoint(candies[i][j].GetTopLeftX() + 25, candies[i][j].GetTopLeftY() + 25);
				}

		if (superBlast != NULL) blasts.push_back(superBlast);
					
		if (!(*removeList.back()).size())
		{	//to avoid game crash, delete list if there's no candy to be removed in this row
			delete removeList.back();
			removeList.pop_back();
		}
		else if (superBlast != NULL && *sound) CAudio::Instance()->Play(AUDIO_SUPER_REMOVE, false);
	}

	void GameArea::PowerAll(int style, int power, int x, int y)
	{
		SuperBlast* superBlast = new SuperBlast(x, y, 0, true);
		for (int i = 0; i < MaxHeight; i++)
			for (int j = 0; j < MaxWidth; j++)
				if (candies[i][j].GetStyle() == style && candies[i][j].GetPower() != 4)
				{
					if (power == 1 || power == 2) power = rand() % 2 + 1;
					candies[i][j].SetPower(power);
					superBlast->AddPoint(candies[i][j].GetTopLeftX() + 25, candies[i][j].GetTopLeftY() + 25);
				}
		blasts.push_back(superBlast);
		if (*sound) CAudio::Instance()->Play(AUDIO_POWER_ALL, false);
		delay = (int)(1000.0 / GAME_CYCLE_TIME);
		delayRemoveStyle = style;
		delayRemove = true;
	}

	int GameArea::GetScore()
	{
		return scoreBoard.score.GetInteger();
	}

	int GameArea::Compare(int first, int second)
	{
		return first < second ? - 1 : first > second ? 1 : 0;
	}

	void GameArea::Delay()
	{
		if (delayRemove)
		{
			if (delay > 0) delay--;
			else
			{
				RemoveStyle(0, 0, delayRemoveStyle);
				delayRemove = false;
			}
		}
	}

	void GameArea::UpdateCurPosition()
	{
		for (int i = 0; i < MaxHeight; i++)
		{
			for (int j = 0; j < MaxWidth; j++)
			{
				//Get candy current position by its' center coordinate
				int curMapX = (candies[i][j].GetCurrentX() - 280 + 25) / 50;
				int curMapY = (candies[i][j].GetCurrentY() - 35 + 25) / 50;

				if(curMapX >= 0 && curMapX < MaxWidth && curMapY >= 0 && curMapY < MaxHeight)
					if(candies[i][j].GetStyle() > 0 && (map[curMapY][curMapX] || map[curMapY - 1][curMapX]))
						curPosition[curMapY][curMapX] = &candies[i][j];
					else
						curPosition[i][j] = NULL;
			}
		}
	}

	void GameArea::OnShow()
	{
		scoreBoard.OnShow();

		///////////////////////////////////////////
		// Show gamearea						///
		///////////////////////////////////////////
		for (int i = 0; i < MaxHeight; i++)
		{
			for (int j = 0; j < MaxWidth; j++)
			{
				switch (map[i][j])
				{
				case 0:
					continue;
				case 3:
					singleJelly.SetTopLeft(j * 50 + x, i * 50 + y);
					singleJelly.ShowBitmap();
					break;
				case 4:
					doubleJelly.SetTopLeft(j * 50 + x, i * 50 + y);
					doubleJelly.ShowBitmap();
					break;
				default:
					area.SetTopLeft(j * 50 + x, i * 50 + y);
					area.ShowBitmap();
					break;
				}
			}
		}

		ShowPortal(1);

		///////////////////////////////////////////
		// Show all candy						///
		///////////////////////////////////////////
		for (int i = 0; i < MaxHeight; i++)
		{
			for (int j = 0; j < MaxWidth; j++)
			{
				if (curPosition[i][j] != NULL)
					curPosition[i][j]->OnShow();
			}
		}

		ShowPortal(2);

		for (auto i = blasts.begin(); i != blasts.end(); i++)
			(*i)->OnShow();

		PlayVoiceEffect(-1);
	}

	void GameArea::OnMove()
	{
		UpdateCurPosition();

		PutCandy();			//put candy at apawning area if it's empty

		DropCandy();		//drop if candy hvnt touch the ground/other candy

		TeleportCandy();

		#pragma omp parallel for
		for (int i = 0; i < MaxHeight; i++)
			#pragma omp parallel for
			for (int j = 0; j < MaxWidth; j++)
				if(candies[i][j].GetStyle() > 0)
					candies[i][j].OnMove(initiating);

		int comboCleared = IsDropping() ? 0 : FindCombo();

		OnMoveBlasts();

		if (!comboCleared && !IsDropping())
			OnMoveEnding();

		ReleaseInOrder();

		Delay();
	}

	void GameArea::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		if (nChar == VK_F1) goldFinger = true;
	}

	void GameArea::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		if (nChar == VK_F1) goldFinger = false;
	}

	int GameArea::FindCombo()
	{
		if (releaseSwap)
		{
			ReleaseSwap();
			InitClickedCandy();
			releaseSwap = false;
			return 1;
		}
		if (!initiating && !delayRemove && !removeList.size())
		{

			int candyCleared = ClearCombo();

			if (candyCleared)
			{	//play combo sound
				currentComboSound = currentComboSound > 11 ? 11 : currentComboSound;
				if (*sound) CAudio::Instance()->Play(audioID[currentComboSound++], false);
			}
			else
			{
				if (totalCandyCleared)
				{
					char cc[50];
					sprintf(cc, "comboCleared : %d\n", totalCandyCleared);
					TRACE(cc);
				}
				
				if (totalCandyCleared > 30) PlayVoiceEffect(AUDIO_DIVINE);
				else if (totalCandyCleared >= 24) PlayVoiceEffect(AUDIO_DELICIOUS);
				else if (totalCandyCleared > 12) PlayVoiceEffect(AUDIO_TASTY);
				else if (totalCandyCleared == 12) PlayVoiceEffect(AUDIO_SWEET);
				currentComboSound = totalCandyCleared = 0;
			}

			if (candyCleared && clickedCandies.size() == 2)
			{//If there is a combo after swapping candies, initiate click, -1 moves
				scoreBoard.moves -= 1;
				InitClickedCandy();
			}
			else if (!candyCleared && clickedCandies.size() == 2)
			{ //else swap two candies back to original position
				if (*sound) CAudio::Instance()->Play(AUDIO_NEG_SWAP, false);
				SwapCandy();
				InitClickedCandy();
			}
		}
		else if (!delayRemove && !removeList.size()) ClearCombo();

		return totalCandyCleared;
	}

	void GameArea::OnMoveBlasts()
	{
		for (auto i = blasts.begin(); i != blasts.end();)
		{
			if ((*i)->IsLast())
			{
				delete *i;
				i = blasts.erase(i);
			}
			else
			{
				(*i)->OnMove();
				i++;
			}
		}
	}

	void GameArea::OnLButtonDown(UINT nFlags, CPoint point)
	{}

	void GameArea::OnLButtonUp(UINT nFlags, CPoint point)
	{
		if (!delay && !ending && !IsDropping())
		{
			int column = (point.x - 280) / 50;
			int row = (point.y - 35) / 50;
			if (!map[row][column]) return;

			if (goldFinger && candies[row][column].GetStyle() > 0)
			{	//Gold Finger: developer use only
				int currentPower = candies[row][column].GetPower();
				candies[row][column].SetPower(currentPower == 4 ? 0 : currentPower + 1);
			}
			else
			{
				Candy* clickedCandy = candies[row][column].Click();
				auto candy = find(clickedCandies.begin(), clickedCandies.end(), clickedCandy); //GetCandyIterator

				if (candy == clickedCandies.end())
					clickedCandies.push_back(clickedCandy);
				else clickedCandies.erase(candy);			//If player click a candy twice, initiate click

				if (clickedCandies.size() == 2)
				{	//if two candies clicked, swap if they are neighbour
					if (IsNeighbour(*clickedCandies[0], *clickedCandies[1]))
					{
						SwapCandy();
						if (*sound) CAudio::Instance()->Play(AUDIO_SWAP, false);
						//Release swapPower when each of clickedCandy is superCandy or both are poweredCandy
						if (clickedCandies[0]->GetPower() == 4 || clickedCandies[1]->GetPower() == 4 || (clickedCandies[0]->GetPower() && clickedCandies[1]->GetPower()))
							releaseSwap = true;
					}
					else if (!releaseSwap) InitClickedCandy();
				}
			}
		}
	}

	void GameArea::OnMouseMove(UINT nFlags, CPoint point)
	{

	}

	void GameArea::ShowLoading()
	{
		CMovingBitmap loading;						// 貼上loading圖示
		loading.LoadBitmap(IDB_INGAME_LOADING, RGB(0, 0, 0));
		loading.SetTopLeft(0, 0);
		loading.ShowBitmap();
		CDDraw::BltBackToPrimary();					// 將 Back Plain 貼到螢幕
	}

	void GameArea::ShowPortal(int position)
	{
		vector<pair<CPoint, CPoint>>* portalList = &(*stage)->portalList;	// get portal position

		for (int i = 0; i < (int)portalList->size(); i++)
		{
			if (!map[(*portalList)[i].first.y][(*portalList)[i].first.x] || !map[(*portalList)[i].second.y][(*portalList)[i].second.x])
				continue;

			int toTopLeftX = (*portalList)[i].first.x * 50 + x;
			int toTopLeftY = (*portalList)[i].first.y * 50 + y + 40;
			int fromTopLeftX = (*portalList)[i].second.x * 50 + x;
			int fromTopLeftY = (*portalList)[i].second.y * 50 + y - 5;
			
			if (position == 1)
			{
				portalStart1.SetTopLeft(fromTopLeftX, fromTopLeftY);
				portalStart1.ShowBitmap();
				portalEnd1.SetTopLeft(toTopLeftX, toTopLeftY);
				portalEnd1.ShowBitmap();
			}
			else if (position == 2)
			{
				portalStart2.SetTopLeft(fromTopLeftX, fromTopLeftY);
				portalStart2.ShowBitmap();
				portalEnd2.SetTopLeft(toTopLeftX, toTopLeftY);
				portalEnd2.ShowBitmap();
			}
		}
	}

	void GameArea::InitCandy(bool drop)
	{
		for (auto i = blasts.begin(); i != blasts.end();)
		{
			delete *i;
			blasts.erase(i++);
		}
		for (int i = 0; i < MaxHeight; i++)
		{
			for (int j = 0; j < MaxWidth; j++)
			{
				switch (map[i][j])
				{
				case 0:
					candies[i][j] = Candy(j * 50 + x, i * 50 + y);
					break;

				default:
					int id = drop == true ? 0 : rand() % MAX_RAND_NUM + 1;
					candies[i][j] = Candy(id, j * 50 + x, i * 50 + y);
					break;
				}
			}
		}
		UpdateCurPosition();

		while (!drop && (ClearCombo() || IsDropping()))
		{
			OnMove();
			ShowLoading();
		}

		initiating = false;
	}

	bool GameArea::IsGameOver()
	{
		return !running;
	}
	int GameArea::GetCurrentStage()
	{
		return ((*stage)->GetCurrentStage());
	}

	void GameArea::DropCandy()
	{
		if(DropCandyStraight()) return;
		DropCandySide();
	}

	int GameArea::DropCandyStraight()
	{	
		int total = 0, count = 1;
		
		//Drop in current column
		do
		{
			for (int i = MaxHeight - 1; i >= 0; i--)
				for (int j = 0; j < MaxWidth; j++)
					if (map[i + 1][j] != 0 && candies[i][j].GetStyle() > 0 && !candies[i + 1][j].GetStyle())
					{
						candies[i][j].SetDestination(candies[i][j].GetTopLeftY() + 50);

						candies[i + 1][j] = candies[i][j];
						candies[i][j].SetStyle(0);
						total++;
						count = 1;
					}
		} 
		while (count--);
		return total;
	}

	int GameArea::DropCandySide()
	{	
		int total = 0;
		
		for (int i = 0; i < MaxHeight; i++)
			for (int j = 0; j < MaxWidth; j++)
				if (map[i + 1][j] != 0 && candies[i][j].GetStyle() > 0)
					if (map[i + 1][j - 1] && curPosition[i + 1][j] != NULL && candies[i][j - 1].GetStyle() < 0 && !candies[i + 1][j - 1].GetStyle())
					{
						candies[i][j].SetDestination(candies[i][j].GetTopLeftX() - 50, candies[i][j].GetTopLeftY() + 50);

						candies[i + 1][j - 1] = candies[i][j];
						candies[i][j].SetStyle(0);
						total++;
					}
					else if (map[i + 1][j + 1] && curPosition[i + 1][j] != NULL && candies[i][j + 1].GetStyle() < 0 && !candies[i + 1][j + 1].GetStyle())
					{
						candies[i][j].SetDestination(candies[i][j].GetTopLeftX() + 50, candies[i][j].GetTopLeftY() + 50);

						candies[i + 1][j + 1] = candies[i][j];
						candies[i][j].SetStyle(0);
						total++;
					}

		return total;
	}

	int GameArea::ClearCombo()
	{
		set<Candy*> accumulateCandy;
		int comboDeleted = 0;
		for (int i = 0; i < MaxHeight; i++)
		{
			for (int j = 0; j < MaxWidth; j++)
			{
				if (candies[i][j].GetStyle() <= 0) continue;
				accumulateCandy.insert(&candies[i][j]);						//put the first candy into set
				GetCandies(accumulateCandy, i, j, candies[i][j].GetStyle());//collect all similar candies that follow-up with first
				comboDeleted += DeleteCombo(accumulateCandy);				//delete all combo
			}
		}
		return comboDeleted;
	}

	void GameArea::GetCandies(set<Candy*>& accumulateCandy, int i, int j, int checkStyle)
	{	
		if (candies[i][j].GetPower() == 4) return;

		//Recursive condition: there is(are) same candy(ies) nearby
		int currentStyle = candies[i][j].GetStyle();
		candies[i][j].SetStyle(0);
		if (j + 1 < MaxWidth && candies[i][j + 1].GetStyle() == checkStyle && candies[i][j + 1].GetPower() != 4)
		{//check to the right
			accumulateCandy.insert(&candies[i][j + 1]);
			GetCandies(accumulateCandy, i, j + 1, checkStyle);
		}
		if (i + 1 < MaxHeight && candies[i + 1][j].GetStyle() == checkStyle && candies[i + 1][j].GetPower() != 4)
		{//Check downward
			accumulateCandy.insert(&candies[i + 1][j]);
			GetCandies(accumulateCandy, i + 1, j, checkStyle);
		}
		if (j - 1 >= 0 && candies[i][j - 1].GetStyle() == checkStyle && candies[i][j - 1].GetPower() != 4)
		{//Check to the left
			accumulateCandy.insert(&candies[i][j - 1]);
			GetCandies(accumulateCandy, i, j - 1, checkStyle);
		}
		if (i - 1 >= 0 && candies[i - 1][j].GetStyle() == checkStyle && candies[i - 1][j].GetPower() != 4)
		{//Check upward
			accumulateCandy.insert(&candies[i - 1][j]);
			GetCandies(accumulateCandy, i - 1, j, checkStyle);
		}
		candies[i][j].SetStyle(currentStyle);
	}

	//Functions CompareX() and CompareY() are used by stable_sort
	bool CompareX(Candy* candy1, Candy* candy2)					//
	{															//
		return (candy1->GetTopLeftX() < candy2->GetTopLeftX());	//
	}															//
																//
	bool CompareY(Candy* candy1, Candy* candy2)					//
	{															//
		return candy1->GetTopLeftY() < candy2->GetTopLeftY();	//
	}															//
	//==========================================================//

	int GameArea::DeleteCombo(set<Candy*>&accumulateCandy)
	{
		if (accumulateCandy.size() < 3)
		{	//Pass
			accumulateCandy.clear();
			return 0;
		}

		vector<int> x, y;
		vector<Candy*> toDelete;
		set<Candy*> temp;
		int comboDeleted = 0;
		for (auto i = accumulateCandy.begin(); i != accumulateCandy.end(); i++)
		{
			x.push_back((*i)->GetTopLeftX());
			y.push_back((*i)->GetTopLeftY());
		}
		for (auto i = accumulateCandy.begin(); i != accumulateCandy.end(); i++)
		{	/*delete if more than 3 candies on a vertical line*/
			if (count(x.begin(), x.end(), (*i)->GetTopLeftX()) >= 3)
				toDelete.push_back(*i);
		}
		comboDeleted += RemoveContinuous(toDelete, 'y', &CompareY, temp);
		for (auto i = accumulateCandy.begin(); i != accumulateCandy.end(); i++)
		{	/*delete if more than 3 candies on a horizontal line*/
			if (count(y.begin(), y.end(), (*i)->GetTopLeftY()) >= 3)
				toDelete.push_back(*i);
		}
		comboDeleted += RemoveContinuous(toDelete, 'x', &CompareX, temp);
		accumulateCandy.clear();
		return comboDeleted;
	}

	void GameArea::PutEndingBonus()
	{
		int totalPut = scoreBoard.moves.GetInteger();
		list<Candy*> candy;
		for (int i = 0; i < MaxHeight; i++)
		{
			for (int j = 0; j < MaxWidth; j++)
			{
				if (candies[i][j].GetStyle() > 0 && candies[i][j].GetPower() == 0)
					candy.push_back(&candies[i][j]);
			}
		}
		
		do
		{
			for (auto i = candy.begin(); i != candy.end();)
			{
				if (totalPut > 0 && !(rand() % totalPut))
				{
					endingBonus.push_back(*i);
					i = candy.erase(i);
					totalPut--;
				}
				else i++;
			}
		} while (totalPut > 0);
	}

	int GameArea::RemoveContinuous(vector<Candy*>& toDelete, char axis, bool(*Compare)(Candy*, Candy*), set<Candy*>& temp)
	{
		if (!toDelete.size()) return 0; //pass

		vector<Candy*> line;
		char check = axis == 'x' ? 'y' : 'x';
		int comboDeleted = 0;
		while(1)
		{	
			GetLine(line, toDelete, check);					//collect candies on a same line
			stable_sort(line.begin(), line.end(), Compare);	//When their x/y are same, sort according to y/x

			int count = 1;
			for (unsigned int i = 0; i < line.size() - 1; i++)
			{
				if (line[i]->GetTopLeft(axis) + 50 == line[i + 1]->GetTopLeft(axis)) 
					count++;					//If next candy is follow-up with current, keep counting
				else if (count < 3) count = 1;	//else, if count >= 3 -> combo, or pass
				else
				{
					RemoveContinuous(line, i - (count - 1), i, axis, temp);
					count = 1;
					comboDeleted += count;
				}
			}
			if (count >= 3)
			{
				RemoveContinuous(line, (unsigned)(line.size() - count), (unsigned)(line.size()), axis, temp);
				comboDeleted += count;
			}
			line.clear();
			if (toDelete.size() < 3) break;	//break if there is not enough candies to form a combo
		}
		toDelete.clear();
		return comboDeleted;
	}

	void GameArea::RemoveContinuous(vector<Candy*>& line, unsigned offset, unsigned lineSize, char axis, set<Candy*>& temp)
	{
		bool packCandy = !initiating;
		bool linePower = !initiating && lineSize - offset == 4 ? true : false;
		bool superCandy = !initiating && lineSize - offset > 4 ? true : false;
		for (unsigned int j = offset; j < lineSize; j++)
		{	
			ReleasePower(line[j]);
			if (axis == 'y') temp.insert(line[j]);
			else if(packCandy && find(temp.begin(), temp.end(), line[j]) != temp.end())
			{
				if (!initiating && *sound)CAudio::Instance()->Play(AUDIO_PACK_CREATE, false);
				line[j]->SetPower(3);
				line[j]->Relive();
				superCandy = linePower = packCandy = false;
				continue;
			}

			if (linePower && find(clickedCandies.begin(), clickedCandies.end(), line[j]) != clickedCandies.end())
			{
				if (!initiating && *sound)CAudio::Instance()->Play(AUDIO_LINE_CREATE, false);
				line[j]->Relive();
				line[j]->SetPower(axis == 'x' ? 2 : 1);
				linePower = false;
				continue;
			}

			if (superCandy && find(clickedCandies.begin(), clickedCandies.end(), line[j]) != clickedCandies.end())
			{
				if (!initiating && *sound)CAudio::Instance()->Play(AUDIO_SUPER_CREATE, false);
				line[j]->Relive();
				line[j]->SetPower(4);
				superCandy = false;
				continue;
			}
		}
		if (linePower)
		{
			if (!initiating && *sound)CAudio::Instance()->Play(AUDIO_LINE_CREATE, false);
			line[offset]->SetPower(axis == 'x' ? 2 : 1);
			line[offset]->Relive();
		}
		if (superCandy)
		{
			if (!initiating && *sound) CAudio::Instance()->Play(AUDIO_SUPER_CREATE, false);
			line[offset]->SetPower(4);
			line[offset]->Relive();
		}
	}

	void GameArea::ReleaseInOrder()
	{
		for (auto i = removeList.begin(); i != removeList.end();)
		{
			ReleasePower(*(*i)->begin());
			(*i)->pop_front();
			if (!(*i)->size())
			{
				delete *i;
				i = removeList.erase(i);
			}
			else i++;
			if (gameOver) gameOver = false;
		}
	}

	void GameArea::GetLine(vector<Candy*>& line, vector<Candy*>& toDelete, char check)
	{
		int currentLine = toDelete[0]->GetTopLeft(check);
		for (auto i = toDelete.begin(); i != toDelete.end();)
		{
			if ((*i)->GetTopLeft(check) == currentLine)
			{
				line.push_back(*i);
				i = toDelete.erase(i);
			}
			else i++;
		}
	}

	void GameArea::GetWordBmp(double** size, int ** frame, CMovingBitmap ** word, int audio_id)
	{
		*size = new double(0.2);
		*frame = new int(0);

		switch (audio_id)
		{
		case AUDIO_SWEET:
			*word = &sweet;
			break;
		case AUDIO_TASTY:
			*word = &tasty;
			break;
		case AUDIO_DELICIOUS:
			*word = &delicious;
			break;
		case AUDIO_DIVINE:
			*word = &divine;
			break;
		case AUDIO_SUGAR_CRUSH:
			*word = &sugarCrush;
			break;
		default:
			GAME_ASSERT(0, "Invalid audio id!");
		}
		playingVoice = true;
	}

	void GameArea::GotoGameStateOver(bool result)
	{
		// save result and data to stage after delay, then show result in game state over
		if (delay > 0) delay--;
		else if (result)
		{	// Unlock next stage and save score highest history if win
			if (*sound) CAudio::Instance()->Play(AUDIO_LEVEL_COMPLETE, false);
			(*(stage + 1))->SetUnlock();
			(*stage)->currentScore = scoreBoard.score.GetInteger();
			(*stage)->lastHighScore = scoreBoard.lastHighScore < scoreBoard.score ? scoreBoard.score.GetInteger() : scoreBoard.lastHighScore;
			(*stage)->SetPassOrFail(0);
			running = false;
		}
		else
		{
			if (*sound) CAudio::Instance()->Play(AUDIO_LEVEL_FAIL, false);
			(*stage)->currentScore = scoreBoard.score.GetInteger();
			(*stage)->SetPassOrFail(1);
			running = false;
		}
	}

	int GameArea::PutCandy()
	{	// Spawn candy if spawning area is empty
		int total = 0;
		for (auto i = spawnArea.begin(); i != spawnArea.end(); i++)
			if (curPosition[i->first][i->second] == NULL)
			{
				int id = rand() % MAX_RAND_NUM + 1;	//random type of Candy
				candies[i->first][i->second] = Candy(id, i->second * 50 + x, i->first * 50 + y - 25);
				candies[i->first][i->second].SetDestination(i->first * 50 + y);
				total++;
			}
		return total;
	}

	bool GameArea::IsDropping()
	{
		for (int i = 0; i < MaxHeight; i++)
		{
			for (int j = 0; j < MaxWidth; j++)
				if (map[i][j] != 0 && candies[i][j].IsMoving())
					return true;
		}

		if (blasts.size() || playingVoice || delayRemove) return true;
		return false;
	}

	void GameArea::SwapCandy()
	{
		clickedCandies[0]->SetDestination(clickedCandies[1]->GetCurrentX(), clickedCandies[1]->GetCurrentY());
		clickedCandies[1]->SetDestination(clickedCandies[0]->GetCurrentX(), clickedCandies[0]->GetCurrentY());

		Candy temp = *clickedCandies[0];
		*clickedCandies[0] = *clickedCandies[1];
		*clickedCandies[1] = temp;
	}

	bool GameArea::IsNeighbour(Candy &a, Candy &b)
	{
		bool vertiNeighbour = fabs(a.GetTopLeftX() - b.GetTopLeftX()) == 50 && a.GetTopLeftY() == b.GetTopLeftY();
		bool horztNeighbour = fabs(a.GetTopLeftY() - b.GetTopLeftY()) == 50 && a.GetTopLeftX() == b.GetTopLeftX();
		return vertiNeighbour || horztNeighbour;
	}

	void GameArea::OnMoveEnding()
	{	
		bool result = scoreBoard.IsReachedTarget() || (scoreBoard.score > scoreBoard.oneStar && scoreBoard.mode == 1);	//Win or lose

		// turn all moving step to bonus and release all powered candy if target reached
		if (!ending && (!scoreBoard.moves.GetInteger() || scoreBoard.IsReachedTarget()))
		{	// change current state to ending
			if (result) PlayVoiceEffect(AUDIO_SUGAR_CRUSH);
			PutEndingBonus();
			ending = true;
		}
		else if (ending && running)
		{
			if (endingBonus.size())
			{	// Show bonus powered candy
				(*endingBonus.begin())->SetPower(rand() % 2 + 1);
				endingBonus.pop_front();
				scoreBoard.moves--;
				Sleep(100);
			}
			else if (result)
			{	// remove all powered candy until no powered candy spwan if player win this game
				removeList.push_back(new list<Candy*>);

				for (int i = 0; i < MaxHeight; i++)
				{	//collect all powered candy
					for (int j = 0; j < MaxWidth; j++)
					{
						if (candies[i][j].GetStyle() > 0 && candies[i][j].GetPower() > 0)
							(*removeList.rbegin())->push_back(&candies[i][j]);
					}
				}
				if (!removeList.back()->size())
				{	// to avoid memory leak, delete removelist if no powered candy found
					delete (*removeList.rbegin());
					removeList.pop_back();
				}
				if (!removeList.size() && !gameOver)
				{	// change state to gameOver
					delay = (int)(700.0 / GAME_CYCLE_TIME);
					gameOver = true;
				}
			}
			else if (!result && !gameOver)
			{	// change state to gameOver
				delay = (int)(700.0 / GAME_CYCLE_TIME);
				gameOver = true;
			}

			if (gameOver) GotoGameStateOver(result);
		}
	}

	void GameArea::PlayVoiceEffect(int audio_id)
	{
		static double* size;	//current size of word
		static int* frame;		//current frame of animation
		static CMovingBitmap* word;

		if (audio_id != -1) GetWordBmp(&size, &frame, &word, audio_id); // reset animation's size, frame & bitmap
		
		if (!playingVoice) return;

		//Show words from small to big at the center
		word->SetTopLeft(SIZE_X / 2 - (int)(word->Width() * *size) / 2, SIZE_Y / 2 - (int)(word->Height() * *size) / 2);
		word->ShowBitmap(*size);
		if (*size < 1) *size += 0.1;
		if ((*frame)++ > 30)
		{
			delete size;
			delete frame;
			playingVoice = false;
		}

		if (!(*sound) || (*frame) != 1) return;
		CAudio::Instance()->Play(audio_id, false);
	}

	void GameArea::TeleportCandy()
	{
		vector<pair<CPoint, CPoint>>* portalList = &(*stage)->portalList;	// get portal position

		#pragma omp parallel for
		for (int i = 0; i < (int)portalList->size(); i++)
		{
			Candy* from = curPosition[(*portalList)[i].first.y][(*portalList)[i].first.x];	//get candy of starting point
			Candy* to = curPosition[(*portalList)[i].second.y][(*portalList)[i].second.x];	//get candy of end
			if (from != NULL && !(from->IsMoving()) && to == NULL)
			{	//teleport if there is a still candy at starting point and no candy at the end
				from->SetDestination(from->GetCurrentY() + 24);	//drop into portal

				//create a candy come out from portal
				candies[(*portalList)[i].second.y][(*portalList)[i].second.x] = Candy(from->GetStyle(), (*portalList)[i].second.x * 50 + x, (*portalList)[i].second.y * 50 + y - 25);
				candies[(*portalList)[i].second.y][(*portalList)[i].second.x].SetDestination((*portalList)[i].second.y * 50 + y);
				candies[(*portalList)[i].second.y][(*portalList)[i].second.x].SetPower(from->GetPower());
				candies[(*portalList)[i].second.y][(*portalList)[i].second.x].SetStyle(from->GetStyle());
			}
			if (from != NULL && from->GetCurrentY() >= (*portalList)[i].first.y * 50 + y + 24)
				from->Kill();	//candy disappear when drop into portal
		}
	}

	void GameArea::InitClickedCandy()
	{
		for (auto i = clickedCandies.begin(); i != clickedCandies.end(); i++)
		{
			(*i)->InitClick();
		}
		clickedCandies.clear();
	}

	void GameArea::InitGameArea(int totalJelly)
	{
		//Set Target
		if (scoreBoard.mode == 1)
		{
			if (scoreBoard.lastHighScore < scoreBoard.oneStar)
				scoreBoard.target = (int)scoreBoard.oneStar;
			else if (scoreBoard.lastHighScore < scoreBoard.twoStar)
				scoreBoard.target = (int)scoreBoard.twoStar;
			else if (scoreBoard.lastHighScore < scoreBoard.threeStar)
				scoreBoard.target = (int)scoreBoard.threeStar;
			else scoreBoard.target = (int)scoreBoard.lastHighScore;
		}
		else if (scoreBoard.mode == 2)
		{
			scoreBoard.target = totalJelly;
		}
		else GAME_ASSERT(0, "Game mode unrecognizable!")

		scoreBoard.score = 0;
		initiating = true;
		ending = false;
		running = true;
		gameOver = false;
		delay = 0;
		delayRemove = false;
		releaseSwap = false;
		goldFinger = false;
	}
}