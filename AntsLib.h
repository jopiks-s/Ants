#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <stdlib.h>

using namespace std;

float a=1,b=2, evaporation=0.7, MaxPheromon=5, MinPheromon=0.3, StartRoadPheromon=100;          //a-length, b-pher

float ParticleLiveTime = 5, ParticleStep=10;												//pheromonParticle
sf::Color ParticleColor(255,0,0);
int MaxParticle = 1000;

int Q = 800;

class Road;
class Dot;
class DotsNet;
Road* RoadOfAttachedDots(Dot* d1, Dot* d2);

class PheromonParticle
{
public:
	sf::CircleShape PheromonShape;
	float LiveRemain;
	PheromonParticle(sf::Vector2i loc, int r, sf::Color Col) :LiveRemain(ParticleLiveTime+rand()%3)
	{
		PheromonShape.setRadius(r);
		PheromonShape.setOrigin(r, r);
		PheromonShape.setPosition(loc.x, loc.y);
		PheromonShape.setFillColor(Col);
	}
	bool ParticleTick(float deltaTime, sf::RenderWindow *window)
	{
		LiveRemain -= deltaTime;
		if (LiveRemain <= 0)
			return false;

		sf::Color Col = PheromonShape.getFillColor();
		Col.a = 255 * LiveRemain / ParticleLiveTime;
		PheromonShape.setFillColor(Col);
		window->draw(PheromonShape);
		return true;
	}
};

class Road
{
public:
	std::vector<sf::Vector2i> RoadCoords;
	std::vector<int> RoadDotsIndex;
	sf::RectangleShape RoadShape;
	float Pheromon, length;
	Road(sf::Vector2i Start, sf::Vector2i End, int StartInd, int EndInd,
		int RoadWidth, float Pheromon) :Pheromon(Pheromon)
	{
		RoadCoords.push_back(Start);
		RoadCoords.push_back(End);

		RoadDotsIndex.push_back(StartInd);
		RoadDotsIndex.push_back(EndInd);

		length = sqrt(pow(Start.x - End.x, 2) + pow(Start.y - End.y, 2));

		RoadShape.setSize(sf::Vector2f(RoadWidth, length));
		RoadShape.setOrigin(RoadWidth / 2, 0);

		sf::Vector2f dir((End.x - Start.x) / length, (End.y - Start.y) / length);
		float cosFi = dir.x / sqrt(pow(dir.x, 2) + pow(dir.y, 2));
		float angle = acos(cosFi) * 180 / 3.14;
		if (End.y > Start.y)
			angle *= -1;
		angle = 270-angle;

		RoadShape.setPosition(Start.x, Start.y);
		RoadShape.rotate(angle);
	}
};

class Dot
{
public:
	int radius, loc[2], index;
	sf::CircleShape DotShape;
	std::vector<Road*> AttachedRoads;
	sf::Text Indextxt;

	Dot(int x, int y, int ind, int r, sf::Color FillCol=sf::Color(0), sf::Text txt=sf::Text())
		:loc{x, y}, radius(r), index(ind), Indextxt(txt)
	{
		DotShape.setRadius(radius);
		DotShape.setOrigin(radius, radius);
		DotShape.setPosition(sf::Vector2f(loc[0], loc[1]));
		DotShape.setFillColor(FillCol);
		Indextxt.setString(std::to_string(index));
		Indextxt.setOrigin(Indextxt.getLocalBounds().left, Indextxt.getLocalBounds().top);
		Vector2f txtshift(Indextxt.getLocalBounds().width/2, Indextxt.getLocalBounds().height/2);
		Indextxt.setPosition(loc[0] - txtshift.x, loc[1] + txtshift.y - r);
	}

	void DrawDot(sf::RenderWindow* window)
	{
		window->draw(DotShape);
		window->draw(Indextxt);
	}
	void SetPosition(sf::Vector2i Pos)
	{
		loc[0] = Pos.x;
		loc[1] = Pos.y;
		DotShape.setPosition(sf::Vector2f(Pos.x, Pos.y));
		Vector2f txtshift(Indextxt.getLocalBounds().width / 2, Indextxt.getLocalBounds().height / 2);
		Indextxt.setPosition(loc[0] - txtshift.x, loc[1] + txtshift.y - DotShape.getRadius());
	}
	void ChangeIndex(int NewInd)
	{
		index = NewInd;
		Indextxt.setString(std::to_string(index));
	}
};

class DotsNet
{
public:
	float BestDistance=0;
	int DotsNum, AntsNum;
	vector<Dot*> DotsArr;
	vector<Road*> RoadArr;
	vector<PheromonParticle*> ParticleArr;
	sf::Color DotsCol, RoadCol;

	DotsNet(int DotsNum = 0, sf::Vector2i StartLoc = sf::Vector2i(0, 0),
		sf::Vector2f DotSpace = sf::Vector2f(0, 0),
		int AntsN=10,
		sf::Color DotsC = sf::Color(0, 0, 0), 
		sf::Color RoadC = sf::Color(0, 0, 0))
		: DotsCol(DotsC), RoadCol(RoadC), DotsNum(DotsNum), AntsNum(AntsN)
	{
		for (int x = 0; x < DotsNum/3; x++)
		{
			for (int y = 0; y < 3; y++)
			{
				Dot* dot=new Dot(StartLoc.x + x * DotSpace.x + rand() % 60 - 30, StartLoc.y + y * DotSpace.y+rand()%60-30, y+x*3, 30);
				dot->DotShape.setFillColor(DotsC);
				DotsArr.push_back(dot);
			}
		}
		GenerateRoad();	
	}
	DotsNet(vector<Dot*> Dots, 
		int AntsN = 10,
		sf::Color DotsC = sf::Color(0, 0, 0),
		sf::Color RoadC = sf::Color(9,105,162,200))
		: DotsCol(DotsC), RoadCol(RoadC), DotsNum(DotsNum), AntsNum(AntsN)
	{
		DotsArr = Dots;
		GenerateRoad();
	}
	void DrawNet(float deltaTime, sf::RenderWindow *window)
	{
		for (int i = 0; i < RoadArr.size(); i++)
		{
			sf::Color Col = RoadArr[i]->RoadShape.getFillColor();
			Col.a *=evaporation;
			if (Col.a < 0.05 * 255)
				Col.a = 0.05 * 255;
			RoadArr[i]->RoadShape.setFillColor(Col);
			window->draw(RoadArr[i]->RoadShape);
		}	
		for (int i = 0; i < ParticleArr.size(); i++)
			if (!ParticleArr[i]->ParticleTick(deltaTime, window))
				ParticleArr.erase(ParticleArr.begin() + i);
		for (int i = 0; i < DotsArr.size(); i++)
			DotsArr[i]->DrawDot(window);
	}

	void GenerateRoad()
	{
		if (!DotsArr.size())
			return;
		for (int Main = 0; Main < DotsArr.size() - 1; Main++)
		{
			sf::Vector2i ThisLoc(DotsArr[Main]->loc[0], DotsArr[Main]->loc[1]);
			for (int Other = DotsArr.size()-1; Other >Main; Other--)
			{
				sf::Vector2i OtherLoc(DotsArr[Other]->loc[0], DotsArr[Other]->loc[1]);
				Road* road=new Road(ThisLoc, OtherLoc, Main, Other, 5, StartRoadPheromon);
				road->RoadShape.setFillColor(RoadCol);
				DotsArr[Main]->AttachedRoads.push_back(road);
				DotsArr.at(Other)->AttachedRoads.push_back(road);
				RoadArr.push_back(road);
			}
		}
	}

	float Iterate()
	{
		if (DotsArr.size() == 0 || RoadArr.size() == 0)
			return 0;
		float dist=0;
		std::vector<Road*> Path;
		std::vector<int> IndexedPath;
		std::map<Dot*, int> UnvalidDots;
		Dot* CurrentDot = DotsArr[rand() % DotsArr.size()];
		Dot* StartDot = CurrentDot;
		UnvalidDots[CurrentDot] = 0;
		IndexedPath.push_back(CurrentDot->index);

		for (int transition = 0; transition < DotsArr.size(); transition++)
		{
			std::map<Dot*, float> Probabilitys;
			std::map<Dot*, float>::iterator it = Probabilitys.begin();

			if (transition == DotsArr.size() - 1)
				Probabilitys[StartDot] = 1;
			

			for (int i = 0; i < DotsArr.size(); i++)
			{
				if (DotsArr[i]==CurrentDot)
					continue;
				if (UnvalidDots.find(DotsArr[i]) != UnvalidDots.end())
					continue;

				Road *GeneralRoad = RoadOfAttachedDots(CurrentDot, DotsArr[i]);
				if (!GeneralRoad)
					continue;

				float Probability = pow(GeneralRoad->Pheromon, b) * pow(Q / GeneralRoad->length, a);
				Probabilitys.insert(std::make_pair(DotsArr[i], Probability));
			}

			float sum=0, prevSum=0;
			for (it; it != Probabilitys.end(); it++)
				sum += it->second;
			it = Probabilitys.begin();
			int RandVal = rand() % 100;

			for (int count=0; it != Probabilitys.end(); it++, count++)
			{
				if (prevSum<=RandVal&&RandVal<prevSum+100*it->second/sum)
				{
					Road* SelectedRoad = RoadOfAttachedDots(CurrentDot, it->first);

				/*	int ParticleNum = RoadArr[count]->length / ParticleStep;
					if (!(ParticleArr.size() + ParticleNum > MaxParticle))
					{
						sf::Vector2i Start(CurrentDot->loc[0], CurrentDot->loc[1]);
						sf::Vector2i deltaCoord(it->first->loc[0] - Start.x, it->first->loc[1] - Start.y);
						for (int k = 0; k < ParticleNum; k++)
						{
							sf::Vector2i PartLoc(Start.x+rand()%8-4 + k * deltaCoord.x / ParticleNum, Start.y+ rand() % 8 - 4 + k * deltaCoord.y / ParticleNum);
							PheromonParticle* Particle = new PheromonParticle(PartLoc, 4, ParticleColor);
							ParticleArr.push_back(Particle);
						}
					}*/
				/*else
					{
						ParticleNum = 5;
						sf::Vector2i Start(CurrentDot->loc[0], CurrentDot->loc[1]);
						sf::Vector2i deltaCoord(it->first->loc[0] - Start.x, it->first->loc[1] - Start.y);
						for (int k = 0; k < ParticleNum; k++)
						{
							sf::Vector2i PartLoc(Start.x + rand() % 5 - 2 + k * deltaCoord.x / ParticleNum, Start.y + rand() % 5 - 2 + k * deltaCoord.y / ParticleNum);
							PheromonParticle* Particle = new PheromonParticle(PartLoc, 4, ParticleColor);
							ParticleArr[rand()%ParticleArr.size()]=Particle;
						}
					}*/

					dist += SelectedRoad->length;
					UnvalidDots.insert(std::make_pair(it->first, 0));
					Path.push_back(SelectedRoad);
					CurrentDot = it->first;
					IndexedPath.push_back(CurrentDot->index);
					break;
				}
				prevSum+=100*it->second/sum;
			}
		}	

		float PherAdd = Q / dist;
		for (int i = 0; i < Path.size(); i++)
		{
			Path[i]->Pheromon += PherAdd;
			sf::Color Col = Path[i]->RoadShape.getFillColor();
			Col.a += PherAdd * 25;
			if (Col.a > 255)
				Col.a = 255;
			Path[i]->RoadShape.setFillColor(Col);
		}
		return dist;
	}

	

	void ReleaseAnts()
	{
		for (int i = 0; i < AntsNum; i++)
		{
			float res = Iterate();
			if (res < BestDistance)
				BestDistance = res;
		}
		for (int i = 0; i < RoadArr.size(); i++)
		{
			RoadArr[i]->Pheromon *= evaporation;
			if (RoadArr[i]->Pheromon < MinPheromon)
				RoadArr[i]->Pheromon = MinPheromon;
		}
			
	}

	std::vector<float> GetPheromonInfo()
	{
		std::vector<float> out;
		for (int i = 0; i < RoadArr.size(); i++)
			out.push_back(RoadArr[i]->Pheromon);
		return out;
	}

	float GetPosibleBestWay()
	{
		return 0;
	}
};

Road* RoadOfAttachedDots(Dot* d1, Dot* d2)
{
	if (d1 == d2)
		return nullptr;
	for (int i = 0; i < d1->AttachedRoads.size(); i++)
		for (int k = 0; k < d2->AttachedRoads.size(); k++)
			if (d1->AttachedRoads[i] == d2->AttachedRoads[k])
				return d1->AttachedRoads[i];
				
	return nullptr;
}

int FindRoad(sf::Vector2i StartLoc, sf::Vector2i EndLoc, std::vector<Road*>* RoadBuff)
{
	for (int j = 0; j < RoadBuff->size(); j++)
		if ((RoadBuff->at(j)->RoadCoords[0] == StartLoc && RoadBuff->at(j)->RoadCoords[1] == EndLoc)
			||
			(RoadBuff->at(j)->RoadCoords[0] == EndLoc && RoadBuff->at(j)->RoadCoords[1] == StartLoc))
			return j;
	return -1;
}