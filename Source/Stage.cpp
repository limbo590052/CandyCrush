#include "stdafx.h"
#include "Resource.h"
#include <mmsystem.h>
#include <ddraw.h>
#include <fstream>
#include <string>
#include "audio.h"
#include "gamelib.h"
#include "Stage.h"

using namespace std;

game_framework::Stage::Stage(int files)
{
	scoreOne = 0; scoreTwo = 0; scoreThree = 0;		//Target score for three star
	vertical = 0; horizontal = 0; pack = 0; chocolate = 0;	//total special candy spawn onInit
	candyType = 0;								//Max candy type in this game
	lastHighScore = 0;							//History highest score
	maxStep = 0;
	mode = 0;
	currentScore = 0;
	currentStage = files;
	isUnlock = 0;
	stageTxt = "Stages\\cnt_stage" + to_string(files) + ".txt";
	hasPortal = false;
}

game_framework::Stage::~Stage()
{
	WriteBack();
}

void game_framework::Stage::LoadStage()
{
	fstream InputStage;
	InputStage.open(stageTxt);
	if (!InputStage.is_open())
	{
		char fileName[200] = "\nOpen file failed! file: ";
		strcat(fileName, stageTxt.c_str());
		strcat(fileName, "\nReason: ");
		GAME_ASSERT(0, strcat(fileName, strerror(errno)));
	}
	string data[13];
	string tempPortal;
	string file;
	//MAP
	for (int i = 0; i < 13; i++) 
	{
		getline(InputStage, file);
		for (int j = 0; j < 20; j++) 
		{
			switch (file[j])
			{
			case '0': map[i][j] = 0; break;		//0 = none, !0 = gameArea
			case '2': map[i][j] = 2; break;		//2 = normalArea
			case '3': map[i][j] = 3; break;		//3 = singleJelly
			case '4': map[i][j] = 4; break;		//4 = doubleJelly
			case '1': map[i][j] = 1; break;		//1 = candy spawning area
			}
		}
	}
	//Other stage information
	for (int i = 2; i < 13; i++) 
	{
		getline(InputStage, file, '\n');
		data[i] = file.substr(0, file.find('\t'));
	}
	//PORTAL
	int portal;
	string portalLoc;
	getline(InputStage, file, '\n');
	portal = stoi(file.substr(0, file.find('\t')));
	if (portal == 1) {
		hasPortal = true;
		getline(InputStage, file, '\n');
		tempPortal = file.substr(0, file.find('\t'));
		portal = stoi(tempPortal.substr(0, tempPortal.find(' ')));
		tempPortal.erase(0, tempPortal.find(' ')+1);
		for (int i = 0; i < portal; i++) {
			CPoint begin, end;
			portalLoc = tempPortal.substr(0, tempPortal.find(' '));
			tempPortal.erase(0, tempPortal.find(' ') + 1);
			begin.x = stoi(portalLoc);
			portalLoc = tempPortal.substr(0, tempPortal.find(' '));
			tempPortal.erase(0, tempPortal.find(' ') + 1);
			begin.y = stoi(portalLoc);
			portalLoc = tempPortal.substr(0, tempPortal.find(' '));
			tempPortal.erase(0, tempPortal.find(' ') + 1);
			end.x = stoi(portalLoc);
			portalLoc = tempPortal.substr(0, tempPortal.find(' '));
			tempPortal.erase(0, tempPortal.find(' ') + 1);
			end.y = stoi(portalLoc);
			portalList.push_back(pair<CPoint, CPoint>(begin, end));
		}
	}
	//LAST SCORE
	getline(InputStage, file, '\n');
	data[0] = file.substr(0, file.find('\t'));
	//IS_UNCLOCK
	getline(InputStage, file, '\n');
	data[1] = file.substr(0, file.find('\t'));

	lastHighScore = stod(data[0]);
	isUnlock = stoi(data[1]);
	scoreOne = stod(data[2]);
	scoreTwo = stod(data[3]);
	scoreThree = stod(data[4]);
	maxStep = stoi(data[5]);
	vertical = stoi(data[6]);
	horizontal = stoi(data[7]);
	pack = stoi(data[8]);
	chocolate = stoi(data[9]);
	candyType = stoi(data[10]);
	mode = stoi(data[11]);
	initcandy = stoi(data[12]);
	InputStage.close();
}

double game_framework::Stage::GetScoreOne()
{
	return scoreOne;
}

double game_framework::Stage::GetScoreTwo()
{
	return scoreTwo;
}

double game_framework::Stage::GetScoreThree()
{
	return scoreThree;
}
double game_framework::Stage::GetLastScoreHistory()
{
	return lastHighScore;
}
bool game_framework::Stage::IsUnlock()
{
	return isUnlock == 1 ? true : false;
}
bool game_framework::Stage::IsFail()
{
	return isFail;
}
void game_framework::Stage::SetPassOrFail(int fail)
{
	isFail = fail == 1 ? 1 : 0;
}
int game_framework::Stage::GetCurrentScore()
{
	return currentScore;
}
int game_framework::Stage::GetCurrentStage()
{
	return currentStage;
}

/*function to re-create the cnt_stg.txt  
without last 2 lines(last score and isunlock*/
void game_framework::Stage::RemoveLine()
{
		ifstream read(stageTxt);
		ofstream myFile;
		string file;
		const char* data = stageTxt.data();
		myFile.open("temp.txt", ofstream::out);
		int line_no = 0, n = 26;
		if (hasPortal)
			n =27;
		while (!read.eof())
		{
			getline(read, file, '\n');
			line_no++;
			if (line_no < n )
			{
				myFile << file;
				myFile << '\n';
			}
		}
		myFile.close();
		read.close();
		remove(data);
		rename("temp.txt", data);
}

void game_framework::Stage::WriteBack()
{
		RemoveLine();
		ofstream myFile(stageTxt, ofstream::app);
		myFile << lastHighScore << "\t#LastScore\n";
		myFile << isUnlock <<"\t#IsUnlock";
		myFile.close();
}
void game_framework::Stage::SetCurrentScore(int score)
{
	currentScore = score;
}

void game_framework::Stage::SetUnlock()
{
	isUnlock = true;
}

