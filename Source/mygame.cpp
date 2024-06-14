/*
 * mygame.cpp: 本檔案儲遊戲本身的class的implementation
 * Copyright (C) 2002-2008 Woei-Kae Chen <wkc@csie.ntut.edu.tw>
 *
 * This file is part of game, a free game development framework for windows.
 *
 * game is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * game is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * History:
 *   2002-03-04 V3.1
 *          Add codes to demostrate the use of CMovingBitmap::ShowBitmap(CMovingBitmap &).
 *	 2004-03-02 V4.0
 *      1. Add CGameStateInit, CGameStateRun, and CGameStateOver to
 *         demonstrate the use of states.
 *      2. Demo the use of CInteger in CGameStateRun.
 *   2005-09-13
 *      Rewrite the codes for CBall and CEraser.
 *   2005-09-20 V4.2Beta1.
 *   2005-09-29 V4.2Beta2.
 *      1. Add codes to display IDC_GAMECURSOR in GameStateRun.
 *   2006-02-08 V4.2
 *      1. Revise sample screens to display in English only.
 *      2. Add code in CGameStateInit to demo the use of PostQuitMessage().
 *      3. Rename OnInitialUpdate() -> OnInit().
 *      4. Fix the bug that OnBeginState() of GameStateInit is not called.
 *      5. Replace AUDIO_CANYON as AUDIO_JELLY.
 *      6. Add help bitmap to CGameStateRun.
 *   2006-09-09 V4.3
 *      1. Rename Move() and Show() as OnMove and OnShow() to emphasize that they are
 *         event driven.
 *   2006-12-30
 *      1. Bug fix: fix a memory leak problem by replacing PostQuitMessage(0) as
 *         PostMessage(AfxGetMainWnd()->m_hWnd, WM_CLOSE,0,0).
 *   2008-02-15 V4.4
 *      1. Add namespace game_framework.
 *      2. Replace the demonstration of animation as a new bouncing ball.
 *      3. Use ShowInitProgress(percent) to display loading progress.
 *   2010-03-23 V4.6
 *      1. Demo MP3 support: use lake.mp3 to replace lake.wav.
*/

#include "stdafx.h"
#include "Resource.h"
#include <mmsystem.h>
#include <ddraw.h>
#include <set>
#include <string.h>
#include <string>
#include "audio.h"
#include "gamelib.h"
#include "mygame.h"
#include <iostream>

namespace game_framework
{
	/////////////////////////////////////////////////////////////////////////////
	// 這個class為遊戲的遊戲開頭畫面物件
	/////////////////////////////////////////////////////////////////////////////

	CGameStateInit::CGameStateInit(CGame* g)
		: CGameState(g)
	{
		playBtnClicked = finishLoaded = onHelp = helpBtnClicked = onAbout = onHowToPlay = onSetting = onCheatPage= false;
		howToPlayPage = 0;
	}

	void CGameStateInit::OnInit()
	{
		ShowLoading();

		//Load background image
		background.LoadBitmap("Bitmaps\\InitBackground.bmp");

		/*================== Load play button ======================*/
		int playBtnBmp[] = { IDB_PLAYBUTTON_1, IDB_PLAYBUTTON_2, IDB_PLAYBUTTON_3, IDB_PLAYBUTTON_4,
							 IDB_PLAYBUTTON_5, IDB_PLAYBUTTON_6, IDB_PLAYBUTTON_7, IDB_PLAYBUTTON_8,
							 IDB_PLAYBUTTON_9, IDB_PLAYBUTTON_10, IDB_PLAYBUTTON_11, IDB_PLAYBUTTON_12
		};

		for (int i = 0; i < 12; i++)
		{
			playButton.AddBitmap(playBtnBmp[i], RGB(0, 0, 0));
		}

		playButton.SetDelayCount(4);
		clickedPlayButton.LoadBitmap("Bitmaps\\PlayButtonClicked.bmp", RGB(0, 0, 0));
		/*=========================================================*/

		/*==================== Load tiffy =========================*/
		int TiffyBitmap[10] = { IDB_TIFFY_0, IDB_TIFFY_1, IDB_TIFFY_2, IDB_TIFFY_3, IDB_TIFFY_4,
								IDB_TIFFY_5, IDB_TIFFY_6, IDB_TIFFY_7, IDB_TIFFY_8, IDB_TIFFY_9
		};

		for (int i = 0; i < 10; i++)
		{
			tiffy.AddBitmap(TiffyBitmap[i], RGB(255, 255, 255));
		}

		for (int i = 8; i > 0; i--)
		{
			tiffy.AddBitmap(TiffyBitmap[i], RGB(255, 255, 255));
		}

		tiffy.SetDelayCount(4);
		/*==========================================================*/

		/*===================== Load Toffee =======================*/
		int ToffeeBitmap[6] = { IDB_TOFFEE_1, IDB_TOFFEE_2, IDB_TOFFEE_3, IDB_TOFFEE_4, IDB_TOFFEE_5, IDB_TOFFEE_6 };

		for (int i = 0; i < 6; i++)
		{
			toffee.AddBitmap(ToffeeBitmap[i], RGB(255, 255, 255));
		}

		for (int i = 5; i > 0; i--)
		{
			toffee.AddBitmap(ToffeeBitmap[i], RGB(255, 255, 255));
		}

		toffee.SetDelayCount(4);
		/*==========================================================*/

		/*================== Load Candy Crush logo =================*/
		int CandyCrush[] = { IDB_CANDY_CRUSH_1, IDB_CANDY_CRUSH_2, IDB_CANDY_CRUSH_3, IDB_CANDY_CRUSH_4, IDB_CANDY_CRUSH_5, IDB_CANDY_CRUSH_6, 
						IDB_CANDY_CRUSH_8, IDB_CANDY_CRUSH_10, IDB_CANDY_CRUSH_12, IDB_CANDY_CRUSH_14, IDB_CANDY_CRUSH_16, IDB_CANDY_CRUSH_18, 
						IDB_CANDY_CRUSH_20, IDB_CANDY_CRUSH_21, IDB_CANDY_CRUSH_22, IDB_CANDY_CRUSH_23,
						IDB_CANDY_CRUSH_22, IDB_CANDY_CRUSH_21, IDB_CANDY_CRUSH_20, IDB_CANDY_CRUSH_19, IDB_CANDY_CRUSH_18, IDB_CANDY_CRUSH_17, 
						IDB_CANDY_CRUSH_16, IDB_CANDY_CRUSH_15, IDB_CANDY_CRUSH_14, IDB_CANDY_CRUSH_13, IDB_CANDY_CRUSH_12, IDB_CANDY_CRUSH_11,
						IDB_CANDY_CRUSH_10, IDB_CANDY_CRUSH_9, IDB_CANDY_CRUSH_7, IDB_CANDY_CRUSH_7, IDB_CANDY_CRUSH_6, 
						IDB_CANDY_CRUSH_7, IDB_CANDY_CRUSH_9,IDB_CANDY_CRUSH_10, IDB_CANDY_CRUSH_11, IDB_CANDY_CRUSH_12, IDB_CANDY_CRUSH_13, 
						IDB_CANDY_CRUSH_14, IDB_CANDY_CRUSH_15, IDB_CANDY_CRUSH_16, IDB_CANDY_CRUSH_17, IDB_CANDY_CRUSH_18, IDB_CANDY_CRUSH_19,
						IDB_CANDY_CRUSH_19, IDB_CANDY_CRUSH_18, IDB_CANDY_CRUSH_17, IDB_CANDY_CRUSH_16, IDB_CANDY_CRUSH_15 };
		for (int i = 0; i < 50; i++) {
			candyCrush.AddBitmap(CandyCrush[i], RGB(255, 255, 255));
		}
		candyCrush.SetDelayCount(1);
		candyCrush.SetCycle(false);
		/*==========================================================*/

		//load audio
		CAudio::Instance()->Load(AUDIO_STAGE, "sounds\\Overworld_Level_Select.mp3");
		CAudio::Instance()->Load(AUDIO_BTN_CLICK, "sounds\\button_press.wav");
		CAudio::Instance()->Load(AUDIO_BTN_RELEASE, "sounds\\button_release.wav");

		//load setting button
		settingButton.AddBitmap("Bitmaps/SettingButton-0.bmp", RGB(0, 0, 0));
		settingButton.AddBitmap("Bitmaps/SettingButton-1.bmp", RGB(0, 0, 0));
		settingButton.AddBitmap("Bitmaps/SettingButton-2.bmp", RGB(0, 0, 0));
		settingButton.AddBitmap("Bitmaps/SettingButton-1.bmp", RGB(0, 0, 0));
		settingButton.SetDelayCount(8);
		settingButtonClicked.LoadBitmap("Bitmaps\\settingButtonClicked.bmp", RGB(0, 0, 0));

		//setting menu background
		settingMenu.LoadBitmap("Bitmaps\\setting_0.bmp", RGB(255, 255, 255));

		/*===================== Load Setting Item =======================*/
		//load music on button
		musicOn.AddBitmap("Bitmaps/MusicOnButton-0.bmp", RGB(0, 0, 0));
		musicOn.AddBitmap("Bitmaps/MusicOnButton-1.bmp", RGB(0, 0, 0));
		musicOn.AddBitmap("Bitmaps/MusicOnButton-2.bmp", RGB(0, 0, 0));
		musicOn.AddBitmap("Bitmaps/MusicOnButton-1.bmp", RGB(0, 0, 0));
		musicOn.SetDelayCount(8);
		musicOnClicked.LoadBitmap("Bitmaps\\MusicOnButtonClicked.bmp", RGB(0, 0, 0));

		//load music off button
		musicOff.AddBitmap("Bitmaps/MusicOffButton-0.bmp", RGB(0, 0, 0));
		musicOff.AddBitmap("Bitmaps/MusicOffButton-1.bmp", RGB(0, 0, 0));
		musicOff.AddBitmap("Bitmaps/MusicOffButton-2.bmp", RGB(0, 0, 0));
		musicOff.AddBitmap("Bitmaps/MusicOffButton-1.bmp", RGB(0, 0, 0));
		musicOff.SetDelayCount(8);
		musicOffClicked.LoadBitmap("Bitmaps\\MusicOffButtonClicked.bmp", RGB(0, 0, 0));

		//loud sound-on button
		soundOn.AddBitmap("Bitmaps/SoundOnButton-0.bmp", RGB(0, 0, 0));
		soundOn.AddBitmap("Bitmaps/SoundOnButton-1.bmp", RGB(0, 0, 0));
		soundOn.AddBitmap("Bitmaps/SoundOnButton-2.bmp", RGB(0, 0, 0));
		soundOn.AddBitmap("Bitmaps/SoundOnButton-1.bmp", RGB(0, 0, 0));
		soundOn.SetDelayCount(8);
		soundOnClicked.LoadBitmap("Bitmaps\\SoundOnButtonClicked.bmp", RGB(0, 0, 0));

		//loud sound-off button
		soundOff.AddBitmap("Bitmaps/SoundOffButton-0.bmp", RGB(0, 0, 0));
		soundOff.AddBitmap("Bitmaps/SoundOffButton-1.bmp", RGB(0, 0, 0));
		soundOff.AddBitmap("Bitmaps/SoundOffButton-2.bmp", RGB(0, 0, 0));
		soundOff.AddBitmap("Bitmaps/SoundOffButton-1.bmp", RGB(0, 0, 0));
		soundOff.SetDelayCount(8);
		soundOffClicked.LoadBitmap("Bitmaps\\SoundOffButtonClicked.bmp", RGB(0, 0, 0));
		/*==========================================================*/

		//load help button
		helpButton.AddBitmap("Bitmaps/HelpButton-0.bmp", RGB(0, 0, 0));
		helpButton.AddBitmap("Bitmaps/HelpButton-1.bmp", RGB(0, 0, 0));
		helpButton.AddBitmap("Bitmaps/HelpButton-1.bmp", RGB(0, 0, 0));
		helpButton.SetDelayCount(8);
		helpButtonClicked.LoadBitmap("Bitmaps/HelpButtonClicked.bmp", RGB(0, 0, 0));

		/*===================== Load Help Item =======================*/
		helpBackground.LoadBitmap("Bitmaps/setting.bmp", RGB(0, 0, 0));
		howToPlayTab.LoadBitmap("Bitmaps/TabHowToPlay.bmp", RGB(255, 255, 255));
		aboutTab.LoadBitmap("Bitmaps/TabAbout.bmp", RGB(255, 255, 255));
		cheatTab.LoadBitmap("Bitmaps/TabCheat.bmp", RGB(255, 255, 255));
		leftButton.LoadBitmap("Bitmaps/Left.bmp", RGB(255, 255, 255));
		rightButton.LoadBitmap("Bitmaps/Right.bmp", RGB(255, 255, 255));
		howToPlay[0].LoadBitmap("Bitmaps/howtoplay1.bmp", RGB(255, 255, 255));
		howToPlay[1].LoadBitmap("Bitmaps/howtoplay2.bmp", RGB(255, 255, 255));
		howToPlay[2].LoadBitmap("Bitmaps/howtoplay3.bmp", RGB(255, 255, 255));
		about.LoadBitmap("Bitmaps/About.bmp", RGB(255, 255, 255));
		cheat.LoadBitmap("Bitmaps/Cheat.bmp", RGB(255, 255, 255));
		aboutButton.LoadBitmap("Bitmaps/AboutButton.bmp", RGB(255, 255, 255));
		howToPlayButton.LoadBitmap("Bitmaps/HowToPlayButton.bmp", RGB(255, 255, 255));
		cheatButton.LoadBitmap("Bitmaps/CheatButton.bmp", RGB(255, 255, 255));
		/*==========================================================*/

		finishLoaded = true;
		OnBeginState();
	}

	void CGameStateInit::OnBeginState()
	{
		if (finishLoaded)
		{
			candyCrush.Reset();	//reset animation of candy crush logo
			playBtnClicked = false;	//reset playbutton stste
			if (music) CAudio::Instance()->Play(AUDIO_STAGE, true);	//play background music
		}
	}

	void CGameStateInit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		const char KEY_ESC = 27;
		const char KEY_SPACE = ' ';

		if (nChar == KEY_SPACE)
			GotoGameState(GAME_STATE_RUN);							// 切換至GAME_STATE_RUN
		else if (nChar == KEY_ESC)									// Demo 關閉遊戲的方法
			PostMessage(AfxGetMainWnd()->m_hWnd, WM_CLOSE, 0, 0);	// 關閉遊戲
	}

	void CGameStateInit::OnLButtonDown(UINT nFlags, CPoint point)
	{
		//point = pointAdjustment(point);
		if (onSetting)
		{
			//setting area
			SettingMenuOnLButtonDown(point);
		}

		//initiate menu
		else
		{
			if (ButtonOnClick(point, playButton))
			{
				if (sound) CAudio::Instance()->Play(AUDIO_BTN_CLICK, false);
				playBtnClicked = true;
			}
			else playBtnClicked = false;

			if (ButtonOnClick(point, helpButton))
			{
				if (sound) CAudio::Instance()->Play(AUDIO_BTN_CLICK, false);
				helpBtnClicked = true;
			}
			else helpBtnClicked = false;
		}
		SettingOnLButtonDown(point);
	}

	void CGameStateInit::OnLButtonUp(UINT nFlags, CPoint point)
	{
		//point = pointAdjustment(point);
		if (onSetting)
		{
			//setting area
			SettingMenuOnLButtonUp(point);
		}
		else if (onHelp) 
		{
			//help area
			HelpMenuOnLButtonUp(point);
		}

		//initiate menu
		else
		{
			if (ButtonOnClick(point, playButton))
			{
				if (sound) CAudio::Instance()->Play(AUDIO_BTN_RELEASE, false);
				GotoGameState(GAME_STATE_MENU);		// 切換至GAME_STATE_RUN
			}
			else playBtnClicked = false;

			if (ButtonOnClick(point,helpButton))
			{
				if (sound) CAudio::Instance()->Play(AUDIO_BTN_RELEASE, false);
				onHelp = onHelp ? false : true;
				onAbout = true;
			}
		    helpBtnClicked = false;
		}
		SettingOnLButtonUp(point);
	}

	void CGameStateInit::OnShow()
	{
		// 貼上背景
		background.SetTopLeft(0, 0);
		background.ShowBitmap();

		//貼上Play Button
		if (playBtnClicked)
		{
			clickedPlayButton.SetTopLeft(SIZE_X / 2 - playButton.Width() / 2, SIZE_Y / 5 * 4 - playButton.Height());
			clickedPlayButton.ShowBitmap();
		}
		else
		{
			playButton.SetTopLeft(SIZE_X / 2 - playButton.Width() / 2, SIZE_Y / 5 * 4 - playButton.Height());
			playButton.OnShow();
		}
		
		//character
		tiffy.SetTopLeft(95, 400);
		tiffy.OnShow();
		toffee.SetTopLeft(700, 60);
		toffee.OnShow();
		candyCrush.SetTopLeft(250, -50);
		candyCrush.OnShow();

		//setting 
		ShowSettingButton();
		ShowSettingMenu();

		//help
		if (helpBtnClicked)
		{
			helpButtonClicked.SetTopLeft(settingButton.Left() - helpButton.Width(), 0);
			helpButtonClicked.ShowBitmap();
		}
		else
		{
			helpButton.SetTopLeft(settingButton.Left() - helpButton.Width(), 0);
			helpButton.OnShow();
		}
		ShowHelpMenu();
	}

	void CGameStateInit::OnMove()
	{
		SettingOnMove(); 
		SettingMenuOnMove();
		candyCrush.OnMove();
		if (!playBtnClicked) playButton.OnMove();
		if (!helpBtnClicked) helpButton.OnMove();
		tiffy.OnMove();
		toffee.OnMove();
	}

	void CGameStateInit::ShowHelpMenu()
	{
		if (onHelp)
		{
			//show menu
			helpBackground.SetTopLeft(SIZE_X / 2 - (helpBackground.Width() / 2), SIZE_Y / 2 - (helpBackground.Height() / 2));
			helpBackground.ShowBitmap();

			if (onAbout)
			{
				//show tab initiate (about tab)
				aboutTab.SetTopLeft(helpBackground.Left() + (helpBackground.Width() - aboutTab.Width()) / 2, helpBackground.Top() + (helpBackground.Height() - aboutTab.Height()) / 2 + 40);
				aboutTab.ShowBitmap();
				//show about page
				about.SetTopLeft(aboutTab.Left() + 3, aboutTab.Top() + 26);
				about.ShowBitmap();
			}
			if (onHowToPlay) 
			{
				//show tab how to play 
				ShowHowToPlay();
			}
			if (onCheatPage)
			{
				//show tab cheat
				cheatTab.SetTopLeft(aboutTab.Left(), aboutTab.Top());
				cheatTab.ShowBitmap();
				//show cheat page
				cheat.SetTopLeft(aboutTab.Left() + 3, aboutTab.Top() + 26);
				cheat.ShowBitmap();
			}
			//set loaction of the tab
			aboutButton.SetTopLeft(aboutTab.Left(), aboutTab.Top());
			howToPlayButton.SetTopLeft(aboutTab.Left()+aboutButton.Width(), aboutTab.Top());
			cheatButton.SetTopLeft(howToPlayButton.Left() + howToPlayButton.Width(), aboutTab.Top());
		}
	}

	void CGameStateInit::ShowHowToPlay()
	{
		//show tab how to play
		howToPlayTab.SetTopLeft(aboutTab.Left(), aboutTab.Top());
		howToPlayTab.ShowBitmap();
		//show initiate how to play page
		howToPlay[howToPlayPage].SetTopLeft(howToPlayTab.Left() + 3, howToPlayTab.Top() + 26);
		howToPlay[howToPlayPage].ShowBitmap();
		//show left right button
		if (howToPlayPage != 0) {
			leftButton.SetTopLeft(howToPlay[howToPlayPage].Left() + 20, howToPlay[howToPlayPage].Top() + howToPlay[howToPlayPage].Height() - leftButton.Height() - 20);
			leftButton.ShowBitmap();
		}
		if (howToPlayPage != 2) {
			rightButton.SetTopLeft(howToPlay[howToPlayPage].Left() + howToPlay[howToPlayPage].Width() - rightButton.Width() - 20, howToPlay[howToPlayPage].Top() + howToPlay[howToPlayPage].Height() - rightButton.Height() - 20);
			rightButton.ShowBitmap();
		}
	}

	void CGameStateInit::HelpMenuOnLButtonUp(CPoint point)
	{
		//on help area
		if (ButtonOnClick(point, helpBackground))
		{
			//about tab
			if (ButtonOnClick(point, aboutButton))
			{
				onAbout = true;
				onHowToPlay = onCheatPage = false;
			}
			//how to play tab
			if (ButtonOnClick(point, howToPlayButton))
			{
				onHowToPlay = true;
				onAbout = onCheatPage = false;
			}
			//cheat tab
			if (ButtonOnClick(point, cheatButton))
			{
				onCheatPage = true;
				onAbout = onHowToPlay = false;
			}
			//left right button
			if (onHowToPlay)
			{
				if (ButtonOnClick(point, leftButton) && howToPlayPage>0)
				{
					howToPlayPage--;
				}
				if (ButtonOnClick(point, rightButton) && howToPlayPage<2)
				{
					howToPlayPage++;
				}
			}
		}
		else 
		{
			onHelp = false;
		}
	}
	
	void CGameStateInit::SetMusic(bool music)
	{
		this->music = music;
		if (music)
			CAudio::Instance()->Play(AUDIO_STAGE, true);
		else
			CAudio::Instance()->Stop(AUDIO_STAGE);
	}

	/////////////////////////////////////////////////////////////////////////////
	// 這個class為遊戲的結束狀態(Game Over)
	/////////////////////////////////////////////////////////////////////////////

	CGameStateOver::CGameStateOver(CGame* g)
		: CGameState(g)
	{
		currentScore.SetType(2);
		currentStage.SetType(3);
		nextBtnClicked = retryBtnClicked = exitBtnClicked = onSetting = false;
	}

	void CGameStateOver::OnMove()
	{
		SettingOnMove();
		SettingMenuOnMove();
		exitButton.OnMove();
		nextButton.OnMove();
		retryButton.OnMove();
	}

	void CGameStateOver::OnBeginState()
	{
		currentStage = current_stage+1;
		currentScore = (int)stages[current_stage]->GetCurrentScore();
		isFail = stages[current_stage]->IsFail();
		nextBtnClicked = retryBtnClicked = false;
		if (music)
		{
			if (isFail) CAudio::Instance()->Play(AUDIO_STATE_FAIL, true);
			else CAudio::Instance()->Play(AUDIO_STATE_COMPLETE, true);
		}
	}

	void CGameStateOver::OnLButtonDown(UINT nFlags, CPoint point)
	{
		if (onSetting)
		{
			SettingMenuOnLButtonDown(point);
		}
		else 
		{
			//Exit Button
			if (ButtonOnClick(point, exitButton))
			{
				if (sound) CAudio::Instance()->Play(AUDIO_BTN_CLICK);
				exitBtnClicked = true;
			}

			//Retry Button
			if (ButtonOnClick(point, retryButton))
			{
				if (sound) CAudio::Instance()->Play(AUDIO_BTN_CLICK);
				retryBtnClicked = true;
			}

			//Next Button
			if (ButtonOnClick(point, nextButton) && !isFail)
			{
				if (sound) CAudio::Instance()->Play(AUDIO_BTN_CLICK);
				nextBtnClicked = true;
			}
		}
		SettingOnLButtonDown(point);
	}

	void CGameStateOver::OnLButtonUp(UINT nFlags, CPoint point)
	{
		if (onSetting)
		{
			SettingMenuOnLButtonUp(point);
		}
		else
		{
			//Exit Button
			if (ButtonOnClick(point, exitButton))
			{
				if (sound) CAudio::Instance()->Play(AUDIO_BTN_RELEASE);
				GotoGameState(GAME_STATE_MENU);		// 切換至GAME_STATE_MENU
				StopAllMusic();
			}

			//Retry Button
			if (ButtonOnClick(point, retryButton))
			{
				if (sound) CAudio::Instance()->Play(AUDIO_BTN_RELEASE);
				gameArea.LoadStage(stages, current_stage);
				GotoGameState(GAME_STATE_RUN);		// 切換至GAME_STATE_RUN
				StopAllMusic();
			}

			//Next Button
			if (ButtonOnClick(point, nextButton) && !isFail && currentStage.GetInteger() != MAX_STAGE )
			{
				if (sound) CAudio::Instance()->Play(AUDIO_BTN_RELEASE);
				current_stage += 1;
				gameArea.LoadStage(stages, current_stage);
				GotoGameState(GAME_STATE_RUN);		// 切換至GAME_STATE_RUN
				StopAllMusic();
			}
			nextBtnClicked = retryBtnClicked = exitBtnClicked = false;
		}
		SettingOnLButtonUp(point);
	}

	void CGameStateOver::OnInit()
	{
		//load background
		backgroundOver.LoadBitmap("Bitmaps/inGameBG1.bmp");

		//load score board
		scoreBoardOver.LoadBitmap("Bitmaps/score_state_over.bmp", RGB(0, 0, 0));

		//load star
		redStar.LoadBitmap("Bitmaps/RedStar.bmp", RGB(251, 230, 239));
		greenStar.LoadBitmap("Bitmaps/GreenStar.bmp", RGB(251, 230, 239));
		yellowStar.LoadBitmap("Bitmaps/YellowStar.bmp", RGB(251, 230, 239));
		emptyStar.LoadBitmap("Bitmaps/ContainerStar.bmp", RGB(251, 230, 239));
		youFailed.LoadBitmap("Bitmaps/Failed.bmp", RGB(251, 230, 239));

		//load button
		exitButton.AddBitmap("Bitmaps/ExitButton-0.bmp", RGB(255, 255, 255));
		exitButton.AddBitmap("Bitmaps/ExitButton-1.bmp", RGB(255, 255, 255));
		exitButton.AddBitmap("Bitmaps/ExitButton-2.bmp", RGB(255, 255, 255));
		exitButton.AddBitmap("Bitmaps/ExitButton-1.bmp", RGB(255, 255, 255));
		exitButton.SetDelayCount(8);
		exitButtonClicked.LoadBitmap("Bitmaps\\ExitButtonClicked.bmp", RGB(255, 255, 255));

		int nextBtn[] = { IDB_NEXTBTN_0, IDB_NEXTBTN_1, IDB_NEXTBTN_2, IDB_NEXTBTN_3, IDB_NEXTBTN_4,
			IDB_NEXTBTN_5, IDB_NEXTBTN_6, IDB_NEXTBTN_7, IDB_NEXTBTN_8, IDB_NEXTBTN_9 };
		int retryBtn[] = { IDB_RETRYBTN_0, IDB_RETRYBTN_1, IDB_RETRYBTN_2, IDB_RETRYBTN_3, IDB_RETRYBTN_4,
			IDB_RETRYBTN_5, IDB_RETRYBTN_6, IDB_RETRYBTN_7, IDB_RETRYBTN_8, IDB_RETRYBTN_9 };

		for (int i = 0; i < 10; i++)
		{
			nextButton.AddBitmap(nextBtn[i], RGB(251, 230, 239));
			retryButton.AddBitmap(retryBtn[i], RGB(251, 230, 239));
		}
		nextButton.SetDelayCount(4);
		retryButton.SetDelayCount(4);

		nextButtonClicked.LoadBitmap("Bitmaps\\NextButtonClicked.bmp", RGB(251, 230, 239));
		retryButtonClicked.LoadBitmap("Bitmaps\\RetryButtonClicked.bmp", RGB(251, 230, 239));

		//Load background music
		CAudio::Instance()->Load(AUDIO_STATE_FAIL, "sounds\\Level_Failed.mp3");
		CAudio::Instance()->Load(AUDIO_STATE_COMPLETE, "sounds\\Level_Complete.mp3");
	}

	int CGameStateOver::GetDigit(int n)
	{
		n = abs(n);
		int digit = 0;
		while (n > 0)
		{
			digit++;
			n /= 10;
		}
		return digit == 0 ? 1 : digit;
	}

	void CGameStateOver::SetMusic(bool music)
	{
		this->music = music;

		if (music)
		{
			if (isFail) CAudio::Instance()->Play(AUDIO_STATE_FAIL, true);
			else CAudio::Instance()->Play(AUDIO_STATE_COMPLETE, true);
		}
		else
		{
			CAudio::Instance()->Stop(AUDIO_STATE_FAIL);
			CAudio::Instance()->Stop(AUDIO_STATE_COMPLETE);
		}
	}

	void CGameStateOver::ShowButtons()
	{
		//set button location
		int exitBtnTopLX = scoreBoardOver.Left() + scoreBoardOver.Width() - exitButton.Width();
		int exitBtnTopLY = scoreBoardOver.Top() + 30;
		int retryBtnTopLX = (backgroundOver.Width() / 2) + (isFail || (current_stage+1 == 15) ? -nextButton.Width() / 2 : 20 - nextButton.Width());
		int retryBtnTopLY = (backgroundOver.Height() / 2) - (scoreBoardOver.Height() / 2) + 530;
		int nextBtnTopLX = (backgroundOver.Width() / 2) - 20;
		int nextBtnTopLY = (backgroundOver.Height() / 2) - (scoreBoardOver.Height() / 2) + 530;

		//Show exit button
		if (exitBtnClicked)
		{
			exitButtonClicked.SetTopLeft(exitBtnTopLX, exitBtnTopLY);
			exitButtonClicked.ShowBitmap();
		}
		else
		{
			exitButton.SetTopLeft(exitBtnTopLX, exitBtnTopLY);
			exitButton.OnShow();
		}

		//Show retry button
		if (retryBtnClicked)
		{
			retryButtonClicked.SetTopLeft(retryBtnTopLX, retryBtnTopLY);
			retryButtonClicked.ShowBitmap();
		}
		else
		{
			retryButton.SetTopLeft(retryBtnTopLX, retryBtnTopLY);
			retryButton.OnShow();
		}

		//Show "Failed" if fail, show next button else
		if (isFail)
		{
			youFailed.SetTopLeft((backgroundOver.Width() / 2) - (youFailed.Width() / 2), (backgroundOver.Height() / 2) - (scoreBoardOver.Height() / 2) + 180);
			youFailed.ShowBitmap();
		}
		else if(current_stage + 1 != 15)
		{
			if (nextBtnClicked)
			{
				nextButtonClicked.SetTopLeft(nextBtnTopLX, nextBtnTopLY);
				nextButtonClicked.ShowBitmap();
			}
			else
			{
				nextButton.SetTopLeft(nextBtnTopLX, nextBtnTopLY);
				nextButton.OnShow();
			}
		}

	}

	void CGameStateOver::ShowStars(int amount, int xStar, int yStar)
	{
		//show 3 star
		if (amount == 3)
		{
			yellowStar.SetTopLeft(xStar, yStar);
			yellowStar.ShowBitmap();
			yellowStar.SetTopLeft(xStar + 110 + 20, yStar - 20);
			yellowStar.ShowBitmap();
			yellowStar.SetTopLeft(xStar + 220 + 40, yStar);
			yellowStar.ShowBitmap();
		}
		//show 1/2 star
		else if (amount >= 1)
		{
			redStar.SetTopLeft(xStar, yStar);
			redStar.ShowBitmap();

			if (amount == 2)
			{
				greenStar.SetTopLeft(xStar + 110 + 20, yStar - 20);
				greenStar.ShowBitmap();
			}
			else {
				emptyStar.SetTopLeft(xStar + 110 + 20, yStar - 20);
				emptyStar.ShowBitmap();
			}
			emptyStar.SetTopLeft(xStar + 220 + 40, yStar);
			emptyStar.ShowBitmap();
		}
		//show 0 star
		else {
			emptyStar.SetTopLeft(xStar, yStar);
			emptyStar.ShowBitmap();
			emptyStar.SetTopLeft(xStar + 110 + 20, yStar - 20);
			emptyStar.ShowBitmap();
			emptyStar.SetTopLeft(xStar + 220 + 40, yStar);
			emptyStar.ShowBitmap();
		}
	}

	void CGameStateOver::StopAllMusic()
	{
		CAudio::Instance()->Stop(AUDIO_STATE_FAIL);
		CAudio::Instance()->Stop(AUDIO_STATE_COMPLETE);
	}

	void CGameStateOver::OnShow()
	{
		//show background
		backgroundOver.SetTopLeft(0, 0);
		backgroundOver.ShowBitmap();

		//setting button
		ShowSettingButton();
		ShowSettingMenu();
		
		//show score board
		scoreBoardOver.SetTopLeft((backgroundOver.Width() / 2) - (scoreBoardOver.Width() / 2), (backgroundOver.Height() / 2) - (scoreBoardOver.Height() / 2));
		scoreBoardOver.ShowBitmap();
		
		//show stage 
		currentStage.SetTopLeft((backgroundOver.Width() / 2) + 60, (backgroundOver.Height() / 2) - (scoreBoardOver.Height() / 2) + 55);
		currentStage.ShowBitmap();

		//show star
		int xStar = (backgroundOver.Width() / 2) - (370 / 2);
		int yStar = (backgroundOver.Height() / 2) - (scoreBoardOver.Height() / 2) + 180;
		if (stages[current_stage]->GetCurrentScore() >= stages[current_stage]->GetScoreThree() && !isFail)
		{	//Show 3 yellow star if current score higher than star three
			ShowStars(3, xStar, yStar);
		}
		else if (stages[current_stage]->GetCurrentScore() >= stages[current_stage]->GetScoreTwo() && !isFail)
		{	//Show 2 star if current score higher than star two
			ShowStars(2, xStar, yStar);
		}
		else if (stages[current_stage]->GetCurrentScore() >= stages[current_stage]->GetScoreOne() && !isFail)
		{	//Show 1 star if current score higher than star one
			ShowStars(1, xStar, yStar);
		}
		else if (stages[current_stage]->GetCurrentScore() < stages[current_stage]->GetScoreOne() && !isFail)
		{	//Show 0 star if current score higher than star one
			ShowStars(0, xStar, yStar);
		}

		//show score
		currentScore.SetTopLeft((backgroundOver.Width() / 2) - (60 * GetDigit(currentScore.GetInteger()) / 2), (backgroundOver.Height() / 2) - (scoreBoardOver.Height() / 2) + 390);
		currentScore.ShowBitmap();
		ShowButtons();
		
	}

	/////////////////////////////////////////////////////////////////////////////
	// 這個class為遊戲的遊戲執行物件，主要的遊戲程式都在這裡
	/////////////////////////////////////////////////////////////////////////////

	CGameStateRun::CGameStateRun(CGame* g) : CGameState(g)
	{}

	CGameStateRun::~CGameStateRun()
	{}

	void CGameStateRun::OnBeginState()
	{
		background.SetTopLeft(0, 0);						// 設定背景的起始座標
		if (music) CAudio::Instance()->Play(AUDIO_JELLY, true);		// 撥放 MIDI
	}

	void CGameStateRun::OnMove()							// 移動遊戲元素
	{
		//
		// 如果希望修改cursor的樣式，則將下面程式的commment取消即可
		//
		// SetCursor(AfxGetApp()->LoadCursor(IDC_GAMECURSOR));
		SettingOnMove();
		SettingMenuOnMove();
		gameArea.OnMove();

		if (gameArea.IsGameOver())
		{
			GotoGameState(GAME_STATE_OVER);
			CAudio::Instance()->Stop(AUDIO_JELLY);
		}
	}

	void CGameStateRun::OnInit()  								// 遊戲的初值及圖形設定
	{
		background.LoadBitmap("Bitmaps\\inGameBG1.bmp");// 載入背景的圖形

		//載入游戲音效
		CAudio::Instance()->Load(AUDIO_JELLY, "sounds\\MovesJellyLevels.mp3");
		CAudio::Instance()->Load(AUDIO_NEG_SWAP, "sounds\\negative_switch_sound1.wav");
		CAudio::Instance()->Load(AUDIO_SWAP, "sounds\\switch_sound1.wav");
		CAudio::Instance()->Load(AUDIO_SUPER_CREATE, "sounds\\colour_bomb_created.wav");
		CAudio::Instance()->Load(AUDIO_LINE_CREATE, "sounds\\striped_candy_created1.wav");
		CAudio::Instance()->Load(AUDIO_PACK_CREATE, "sounds\\wrapped_candy_created1.wav");
		CAudio::Instance()->Load(AUDIO_POWER_ALL, "sounds\\colour_bomb1.wav");
		CAudio::Instance()->Load(AUDIO_SQUARE_REMOVE1, "sounds\\square_removed1.wav");
		CAudio::Instance()->Load(AUDIO_SQUARE_REMOVE2, "sounds\\square_removed2.wav");
		CAudio::Instance()->Load(AUDIO_LINE_BLAST, "sounds\\line_blast1.wav");
		CAudio::Instance()->Load(AUDIO_CANDY_LAND1, "sounds\\candy_land1.wav");
		CAudio::Instance()->Load(AUDIO_CANDY_LAND2, "sounds\\candy_land2.wav");
		CAudio::Instance()->Load(AUDIO_CANDY_LAND3, "sounds\\candy_land3.wav");
		CAudio::Instance()->Load(AUDIO_CANDY_LAND4, "sounds\\candy_land4.wav");
		CAudio::Instance()->Load(AUDIO_SUPER_REMOVE, "sounds\\super_colour_bomb1.wav");
		CAudio::Instance()->Load(AUDIO_SWEET, "sounds\\sweet.wav");
		CAudio::Instance()->Load(AUDIO_TASTY, "sounds\\tasty.wav");
		CAudio::Instance()->Load(AUDIO_DELICIOUS, "sounds\\delicious.wav");
		CAudio::Instance()->Load(AUDIO_DIVINE, "sounds\\divine.wav");
		CAudio::Instance()->Load(AUDIO_SUGAR_CRUSH, "sounds\\sugar_crush.wav");
		CAudio::Instance()->Load(AUDIO_LEVEL_FAIL, "sounds\\level_failed1.wav");
		CAudio::Instance()->Load(AUDIO_LEVEL_COMPLETE, "sounds\\level_completed.wav");

		for (int i = 0; i < 12; i++)
		{
			char sound[30] = { 0 };
			sprintf(sound, "sounds\\combo_sound%d.wav", i + 1);
			CAudio::Instance()->Load(AUDIO_COMBO1 + i, sound);
		}

		gameArea.LoadBitmap();
	}

	void CGameStateRun::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		gameArea.OnKeyDown(nChar, nRepCnt, nFlags);
	}

	void CGameStateRun::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		if (nChar == VK_ESCAPE)
		{
			CAudio::Instance()->Stop(AUDIO_JELLY);
			GotoGameState(GAME_STATE_MENU);
		}

		gameArea.OnKeyUp(nChar, nRepCnt, nFlags);
	}

	void CGameStateRun::OnLButtonDown(UINT nFlags, CPoint point)  // 處理滑鼠的動作
	{
		SettingOnLButtonDown(point);
		if (onSetting)
		{
			SettingMenuOnLButtonDown(point);
		}
		else
		{
			gameArea.OnLButtonDown(nFlags, point);
		}
	}

	void CGameStateRun::OnLButtonUp(UINT nFlags, CPoint point)	// 處理滑鼠的動作
	{
		SettingOnLButtonUp(point);
		if (onSetting)
		{
			SettingMenuOnLButtonUp(point);
		}
		else
		{
			gameArea.OnLButtonUp(nFlags, point);
		}
	}

	void CGameStateRun::OnMouseMove(UINT nFlags, CPoint point)	// 處理滑鼠的動作
	{
		// 沒事。如果需要處理滑鼠移動的話，寫code在這裡
	}

	void CGameStateRun::OnRButtonDown(UINT nFlags, CPoint point)  // 處理滑鼠的動作
	{}

	void CGameStateRun::OnRButtonUp(UINT nFlags, CPoint point)	// 處理滑鼠的動作
	{}


	void CGameStateRun::OnShow()
	{
		background.ShowBitmap();			// 貼上背景圖

		//setting button
		ShowSettingButton();
		ShowSettingMenu();

		gameArea.OnShow();
	}

	void CGameStateRun::SetMusic(bool music)
	{
		this->music = music;

		if (music) 
			CAudio::Instance()->Play(AUDIO_JELLY, true);
		else 
			CAudio::Instance()->Stop(AUDIO_JELLY);
	}

	/////////////////////////////////////////////////////////////////////////////
	// 這個class為遊戲關卡選單(Game menu)
	/////////////////////////////////////////////////////////////////////////////

	CGameStateMenu::CGameStateMenu(CGame *g)
		: CGameState(g), drag(false), mouseDisplayment(0), inertia(0), goldFinger(false)
	{
		IsMovingUp = false; IsMovingDown = false;
		MAX_Y = 0; MIN_Y = -3600;
		sy = -3600;

		int Pos[][2] = { 
			{270,4030},{495,3980},{530,3850},{320,3870},{135,3910},
			{135,3750},{340,3690},{570,3720},{770,3800},{960,3840},
			{1085,3750},{1010,3600},{760,3540},{520,3590},{280,3585},
			{385,3295}, {660,3175}, {425,3150}, {175,3190}, {100,3020},
			{155,2865}, {405,2805}, {635,2835}, {780,3000}, {800,3175},
			{705,3340}, {890,3400}, {1080,3300}, {1090,3135}, {1050,2975} };
		for (int i = 0; i < 30; i++) 
		{
			for (int j = 0; j < 2; j++) 
			{
				StagePos[i][j] = Pos[i][j];
			}
		}
	}

	CGameStateMenu::~CGameStateMenu()
	{
		for (unsigned i = 0; i < stages.size(); i++)
			delete stages[i];
	}

	void CGameStateMenu::OnInit()
	{
		//background
		woodBackgourd.LoadBitmap("Bitmaps\\WoodBackground.bmp");
		menuBackground.LoadBitmap("Bitmaps\\stage_map.bmp");
		comingSoon.LoadBitmap("Bitmaps\\ComingSoon.bmp", RGB(0, 0, 0));
		stageNum.SetType(1);

		int StageButton[5] = { IDB_STAGE_BUTTON_BLUE, IDB_STAGE_BUTTON_RED, IDB_STAGE_BUTTON_GREEN, IDB_STAGE_BUTTON_YELLOW, IDB_STAGE_BUTTON_GREY };
		//unlock icon
		for (int i = 0; i < 5; i++) {
			stageButton[i].LoadBitmap(StageButton[i], RGB(255, 255, 255));
		}

		//star icon
		star1.LoadBitmap("Bitmaps\\SmallRedStar.bmp", RGB(255, 255, 255));
		star2.LoadBitmap("Bitmaps\\SmallGreenStar.bmp", RGB(255, 255, 255));
		star3.LoadBitmap("Bitmaps\\SmallYellowStar.bmp", RGB(255, 255, 255));

		//load stage
		for (int i = 0; i < MAX_STAGE + 1; i++) {
			stages.push_back(new Stage(i + 1));
			stages[i]->LoadStage();
		}
	}

	void CGameStateMenu::OnBeginState()
	{
		goldFinger = false;
	}

	void CGameStateMenu::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		const char KEY_UP = 0x26;
		const char KEY_DOWN = 0x28;
		//menu control with keyboard
		if (nChar == KEY_UP) IsMovingUp = true;
		if (nChar == KEY_DOWN) IsMovingDown = true;

		if (nChar == VK_F1) goldFinger = true;
	}

	void CGameStateMenu::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		const char KEY_UP = 0x26;
		const char KEY_DOWN = 0x28;
		const char KEY_ESC = 27;
		//menu control with keyboard
		if (nChar == KEY_UP) IsMovingUp = false;
		if (nChar == KEY_DOWN) IsMovingDown = false;

		if (nChar == VK_F1) goldFinger = false;

		if (nChar == KEY_ESC)
		{
			CAudio::Instance()->Stop(AUDIO_STAGE);
			GotoGameState(GAME_STATE_INIT);
		}
	}

	void CGameStateMenu::OnLButtonDown(UINT nFlags, CPoint point)
	{
		//point = pointAdjustment(point);
		drag = true;
		SettingOnLButtonDown(point);
		if (onSetting) 
		{
			SettingMenuOnLButtonDown(point);
		}
		else
		{
			clickX = point.x;
			clickY = point.y;
			clickSY = sy;
			char CC[50];
			sprintf(CC, "Click on \n");
			TRACE(CC);
		}
	}

	void CGameStateMenu::OnLButtonUp(UINT nFlags, CPoint point)
	{
		//point = pointAdjustment(point);
		drag = false;
		SettingOnLButtonUp(point);
		if (onSetting)
		{
			//setting button
			SettingMenuOnLButtonUp(point);
		}
		else
		{
			//stage
			int x = point.x;
			int y = point.y - sy;
			for (int i = 0; i < MAX_STAGE ; i++) {
				if (StagePos[i][0] < x && x < (StagePos[i][0] + 60) && StagePos[i][1] < y && (y < StagePos[i][1] + 60))
				{
					if (stages[i]->IsUnlock() || goldFinger)
					{
						current_stage = i;
						gameArea.LoadStage(stages, i);
						CAudio::Instance()->Stop(AUDIO_STAGE);
						GotoGameState(GAME_STATE_RUN);
					}
				}
			}
		}
	}

	void CGameStateMenu::OnMouseMove(UINT nFlags, CPoint point)
	{
		//point = pointAdjustment(point);
		if (drag)
		{
			int displayment = point.y - clickY;
			sy = clickSY + displayment;
			inertia = displayment < 0 ? -20 : 20;
		}
	}

	void CGameStateMenu::OnRButtonDown(UINT nFlags, CPoint point)
	{}

	void CGameStateMenu::OnRButtonUp(UINT nFlags, CPoint point)
	{}

	void CGameStateMenu::SetMovingUp(bool status)
	{
		if (status && sy <= MAX_Y)
			sy += 5;
	}

	void CGameStateMenu::SetMovingDown(bool status)
	{
		if (status && sy >= MIN_Y)
			sy -= 5;
	}

	void CGameStateMenu::OnMove()
	{
		SettingOnMove();
		SettingMenuOnMove();
		SetMovingUp(IsMovingUp);
		SetMovingDown(IsMovingDown);
		if (!drag && inertia > 0) sy += inertia--;
		else if (!drag && inertia < 0) sy += inertia++;
	}

	int CGameStateMenu::GetDigit(int n)
	{
		n = abs(n);
		int digit = 0;
		while (n > 0)
		{
			digit++;
			n /= 10;
		}
		return digit == 0 ? 1 : digit;
	}

	void CGameStateMenu::OnShow()
	{
		//show wood background
		woodBackgourd.SetTopLeft(0, 0);
		woodBackgourd.ShowBitmap();
		//show menu background
		menuBackground.SetTopLeft(40, sy);
		menuBackground.ShowBitmap();

		//show stage map
		if (sy < MAX_Y && sy < MIN_Y)
			sy = -3600;
		if (sy > MAX_Y && sy > MIN_Y)
			sy = 0;
		if (sy > -1000)//adjast here for coming Soon show condition
		{
			comingSoon.SetTopLeft(SIZE_X / 2 - comingSoon.Width() / 2, SIZE_Y / 2 - comingSoon.Height() / 2);
			comingSoon.ShowBitmap();
		}

		//setting button
		ShowSettingButton();
		ShowSettingMenu();

		//show unlock icon
		for (int i = 0; i < MAX_STAGE ; i++)
		{
			int xStar = StagePos[i][0] - 10, xButton = StagePos[i][0] - 5;
			int yStar = StagePos[i][1] + sy + 65, yButton = StagePos[i][1] - 3 + sy;
			stageNum.SetInteger(i + 1);
			if (stages[i]->IsUnlock())
			{
				if (stages[i]->GetLastScoreHistory() >= stages[i]->GetScoreThree())
				{	//Show yellow button if history score higher than star three
					ShowStageButton(3, i, xButton, yButton);
					ShowStars(3, xStar, yStar);
				}
				else if (stages[i]->GetLastScoreHistory() >= stages[i]->GetScoreTwo())
				{	//Show green button if history star three > score higher > star two
					ShowStageButton(2, i, xButton, yButton);
					ShowStars(2, xStar, yStar);
				}
				else if (stages[i]->GetLastScoreHistory() >= stages[i]->GetScoreOne())
				{	//Show red button if star two > history score > star one 
					ShowStageButton(1, i, xButton, yButton);
					ShowStars(1, xStar, yStar);
				}
				else if (stages[i]->GetLastScoreHistory() < stages[i]->GetScoreOne())
				{	//Show blue button if the stage is unlocked but no history score
					ShowStageButton(0, i, xButton, yButton);
				}
			}	//Show gray button if the stage is locked
			else ShowStageButton(4, i, xButton, yButton);
		}
	}

	void CGameStateMenu::SetMusic(bool music)
	{
		this->music = music;

		if (music) 
			CAudio::Instance()->Play(AUDIO_STAGE, true);
		else
			CAudio::Instance()->Stop(AUDIO_STAGE);
	}

	void CGameStateMenu::ShowStageButton(int stageBtn, int stage, int xButton, int yButton)
	{
		stageButton[stageBtn].SetTopLeft(xButton, yButton);
		stageButton[stageBtn].ShowBitmap();

		if (stages[stage]->IsUnlock())
		{
			stageNum.SetTopLeft(xButton + ((stageButton[stageBtn].Width() / 2) - (10 * GetDigit(stage) / 2)), yButton + (stageButton[stageBtn].Height() / 4));
			stageNum.ShowBitmap();
		}
	}

	void CGameStateMenu::ShowStars(int amount, int xStar, int yStar)
	{
		if (amount == 3)
		{
			//show 3 star
			star3.SetTopLeft(xStar, yStar);
			star3.ShowBitmap();
			star3.SetTopLeft(xStar + 30, yStar + 5);
			star3.ShowBitmap();
			star3.SetTopLeft(xStar + 60, yStar);
			star3.ShowBitmap();
		}
		else if (amount >= 1)
		{
			//show 1 star
			star1.SetTopLeft(xStar, yStar);
			star1.ShowBitmap();

			if (amount == 2)
			{
				//show 2 star
				star2.SetTopLeft(xStar + 30, yStar + 5);
				star2.ShowBitmap();
			}
		}
	}

	CPoint pointAdjustment(CPoint point)
	{
		point.y = point.y + 35;
		return point;
	}
}