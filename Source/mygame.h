/*
 * mygame.h: 本檔案儲遊戲本身的class的interface
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
 *	 2004-03-02 V4.0
 *      1. Add CGameStateInit, CGameStateRun, and CGameStateOver to
 *         demonstrate the use of states.
 *   2005-09-13
 *      Rewrite the codes for CBall and CEraser.
 *   2005-09-20 V4.2Beta1.
 *   2005-09-29 V4.2Beta2.
 *   2006-02-08 V4.2
 *      1. Rename OnInitialUpdate() -> OnInit().
 *      2. Replace AUDIO_CANYON as AUDIO_JELLY.
 *      3. Add help bitmap to CGameStateRun.
 *   2006-09-09 V4.3
 *      1. Rename Move() and Show() as OnMove and OnShow() to emphasize that they are
 *         event driven.
 *   2008-02-15 V4.4
 *      1. Add namespace game_framework.
 *      2. Replace the demonstration of animation as a new bouncing ball.
 *      3. Use ShowInitProgress(percent) to display loading progress.
*/

#include "Blast.h"
#include "Candy.h"
#include "Stage.h"
#include "ScoreBoard.h"
#include "GameArea.h"

namespace game_framework {
	/////////////////////////////////////////////////////////////////////////////
	// 這個class為遊戲的遊戲開頭畫面物件
	// 每個Member function的Implementation都要弄懂
	/////////////////////////////////////////////////////////////////////////////

	class CGameStateInit : public CGameState {
	public:
		CGameStateInit(CGame *g);
		void OnInit();  								// 遊戲的初值及圖形設定
		void OnBeginState();							// 設定每次重玩所需的變數
		void OnKeyUp(UINT, UINT, UINT); 				// 處理鍵盤Up的動作
		void OnLButtonDown(UINT nFlags, CPoint point);  // 處理滑鼠的動作
		void OnLButtonUp(UINT nFlags, CPoint point);	// 處理滑鼠的動作
	protected:
		void OnShow();									// 顯示這個狀態的遊戲畫面
		void OnMove();
		void ShowHelpMenu();
		void ShowHowToPlay();
		void HelpMenuOnLButtonUp(CPoint point);
		void SetMusic(bool);

	private:
		CMovingBitmap background, clickedPlayButton, helpButtonClicked;	// Candycrush starter background, play button
		CMovingBitmap helpBackground, aboutTab, howToPlayTab, cheatTab, howToPlay[3], about, cheat,
			leftButton, rightButton, aboutButton, howToPlayButton, cheatButton;
		CAnimation tiffy, playButton, toffee, candyCrush, helpButton;
		bool playBtnClicked, finishLoaded, helpBtnClicked, onHelp, onAbout, onHowToPlay, onCheatPage;
		int howToPlayPage;
	};

	/////////////////////////////////////////////////////////////////////////////
	// 這個class為遊戲的遊戲執行物件，主要的遊戲程式都在這裡
	// 每個Member function的Implementation都要弄懂
	/////////////////////////////////////////////////////////////////////////////


	class CGameStateRun : public CGameState {
	public:
		CGameStateRun(CGame *g);						//Run Default game
		~CGameStateRun();
		void OnBeginState();							// 設定每次重玩所需的變數
		void OnInit();  								// 遊戲的初值及圖形設定
		void OnKeyDown(UINT, UINT, UINT);
		void OnKeyUp(UINT, UINT, UINT);
		void OnLButtonDown(UINT nFlags, CPoint point);  // 處理滑鼠的動作
		void OnLButtonUp(UINT nFlags, CPoint point);	// 處理滑鼠的動作
		void OnMouseMove(UINT nFlags, CPoint point);	// 處理滑鼠的動作 
		void OnRButtonDown(UINT nFlags, CPoint point);  // 處理滑鼠的動作
		void OnRButtonUp(UINT nFlags, CPoint point);	// 處理滑鼠的動作
	protected:
		void OnMove();									// 移動遊戲元素
		void OnShow();									// 顯示這個狀態的遊戲畫面
		void SetMusic(bool);

	private:
		CMovingBitmap	background;	// 背景圖
	};

	/////////////////////////////////////////////////////////////////////////////
	// 這個class為遊戲的結束狀態(Game Over)
	// 每個Member function的Implementation都要弄懂
	/////////////////////////////////////////////////////////////////////////////

	class CGameStateOver : public CGameState {
	public:
		CGameStateOver(CGame *g);
		void OnBeginState();
		void OnLButtonDown(UINT nFlags, CPoint point);			// handle mouse behavior
		void OnLButtonUp(UINT nFlags, CPoint point);			// handle mouse behavior
		void OnInit();
	protected:
		void OnMove();									// 移動遊戲元素
		void OnShow();									// 顯示這個狀態的遊戲畫面
		int GetDigit(int);
		void SetMusic(bool);
	private:
		void ShowButtons();
		void ShowStars(int, int, int);
		void StopAllMusic();

		int counter, stageNum;	// 倒數之計數器
		bool isFail;
		CMovingBitmap backgroundOver;	// 背景圖
		CMovingBitmap scoreBoardOver;
		CInteger currentScore;
		CInteger currentStage;
		CMovingBitmap redStar, greenStar, yellowStar, emptyStar, youFailed;
		CMovingBitmap exitButtonClicked, nextButtonClicked, retryButtonClicked;
		CAnimation exitButton, nextButton, retryButton;
		bool exitBtnClicked, nextBtnClicked, retryBtnClicked;
	};

	class CGameStateMenu : public CGameState
	{
	public:
		CGameStateMenu(CGame *g);
		~CGameStateMenu();
		void OnInit();
		void OnBeginState();
		void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);	// handle keyboard behavior
		void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);	// handle keyboard behavior
		void OnLButtonDown(UINT nFlags, CPoint point);			// handle mouse behavior
		void OnLButtonUp(UINT nFlags, CPoint point);			// handle mouse behavior
		void OnMouseMove(UINT nFlags, CPoint point);			// handle mouse behavior 
		void OnRButtonDown(UINT nFlags, CPoint point);			// handle mouse behavior
		void OnRButtonUp(UINT nFlags, CPoint point);			// handle mouse behavior
		void SetMovingUp(bool status);
		void SetMovingDown(bool status);
		int GetDigit(int n);

	protected:
		void OnMove();
		void OnShow();
		void SetMusic(bool);

	private:
		void ShowStageButton(int, int, int, int);
		void ShowStars(int, int, int);

		CMovingBitmap menuBackground, woodBackgourd, stageButton[5];
		CMovingBitmap star1, star2, star3, comingSoon;
		int sy;
		int MAX_Y, MIN_Y;
		bool IsMovingUp, IsMovingDown, drag;
		bool goldFinger;
		int StagePos[30][2];
		CInteger stageNum;
		LONG clickX, clickY, clickSY;	//save position of mouse and menu when mouse clicked
		int mouseDisplayment, inertia;
	};
	CPoint pointAdjustment(CPoint point);
}