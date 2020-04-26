#include <iostream>
#include <thread>
#include <vector>
using namespace std;

#include <stdio.h>
#include <Windows.h>

int latime_ecran = 120.6;			//setarea paremetrilor 
int inaltime_ecran = 80;			//consolei
wstring piesa[7];
int latime_careu = 12;				//setarea paremetrilor
int inaltime_careu = 18;			//consolei
unsigned char* careu = nullptr;

int Roteste(int pozitie_x, int pozitie_y, int numarApasari)   //functia care roteste piesele
{
	int rotatie = 0;
	switch (numarApasari % 4)
	{
	case 0:		
		rotatie = pozitie_y * 4 + pozitie_x;
		break;						
									

	case 1:			
		rotatie = 12 + pozitie_y - (pozitie_x * 4);
		break;						
									

	case 2:		
		rotatie = 15 - (pozitie_y * 4) - pozitie_x;
		break;						
									

	case 3:			
		rotatie = 3 - pozitie_y + (pozitie_x * 4);
		break;						
	}								

	return rotatie;
}

bool Incadrare_Piesa(int numarPiesa, int numarRotatie, int numarPozitieX, int numarPozitieY)	//functie care stabileste incadrarea piesei
{																								//avand grija sa nu le suprapuna 
	//celulele care au valoarea mai mare decat 0 inseamna ca sunt ocupate
	for (int i = 0; i< 4; i++)
		for (int j = 0; j< 4; j++)
		{
			int rotatie = Roteste(i, j, numarRotatie);
			int fi = (numarPozitieY + j) * latime_careu + (numarPozitieX + i);
			if (numarPozitieX + i>= 0 && numarPozitieX + i< latime_careu)
			{
				if (numarPozitieY + j>= 0 && numarPozitieY + j< inaltime_careu)
				{
					if (piesa[numarPiesa][rotatie] != L'.' && careu[fi] != 0) //conditie care testeaza daca a avut loc coliziunea
						return false;
				}
			}
		}

	return true;
}

int main()
{
	//cream buffer-ul consolei
	wchar_t* ecran = new wchar_t[latime_ecran * inaltime_ecran];
	for (int i = 0; i < latime_ecran * inaltime_ecran; i++) ecran[i] = L' ';
	HANDLE Consola = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(Consola);
	DWORD biti = 0;

	piesa[0].append(L"..X...X...X...X.");		//creeare pieselor
	piesa[1].append(L"..X..XX...X.....");
	piesa[2].append(L".....XX..XX.....");
	piesa[3].append(L"..X..XX..X......");
	piesa[4].append(L".X...XX...X.....");
	piesa[5].append(L".X...X...XX.....");
	piesa[6].append(L"..X...X..XX.....");

	careu = new unsigned char[latime_careu * inaltime_careu]; //cream careul si ii setam marginile
	for (int i = 0; i< latime_careu; i++)
		for (int j = 0; j< inaltime_careu; j++)
			careu[j * latime_careu + i] = (i == 0 || i == latime_careu - 1 || j == inaltime_careu - 1) ? 9 : 0;

	bool tasta[4];
	int piesa_curenta= 0;
	int rotatie_curenta = 0;
	int pozitie_x = latime_careu / 2;
	int pozitie_y = 0;
	int viteza = 20;
	int viteza_piesa = 0;
	bool coborare = false;
	bool rotire = true;
	int nr_piesa = 0;
	int scor = 0;
	vector<int> vector_linii;
	bool sfarsit_joc = false;

	while (!sfarsit_joc) //Main Loop
	{
		//timpul cu care piesa merge in jos
		this_thread::sleep_for(50ms);
		viteza_piesa++;
		coborare = (viteza_piesa == viteza);

		//intrarea piesei
		for (int k = 0; k < 4; k++)
			tasta[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;

		//functionalitatile sagetilor 
		pozitie_x += (tasta[0] && Incadrare_Piesa(piesa_curenta, rotatie_curenta, pozitie_x + 1, pozitie_y)) ? 1 : 0;
		pozitie_x -= (tasta[1] && Incadrare_Piesa(piesa_curenta, rotatie_curenta, pozitie_x - 1, pozitie_y)) ? 1 : 0;
		pozitie_y += (tasta[2] && Incadrare_Piesa(piesa_curenta, rotatie_curenta, pozitie_x, pozitie_y + 1)) ? 1 : 0;

		//rotatia piesei
		if (tasta[3])
		{
			rotatie_curenta += (rotire && Incadrare_Piesa(piesa_curenta, rotatie_curenta + 1, pozitie_x, pozitie_y)) ? 1 : 0;
			rotire = false;
		}
		else
			rotire = true;

		//forteaza piesa de a cobora mai repede
		if (coborare)
		{
			//viteza creste o data la 50 de piese 
			viteza_piesa = 0;
			nr_piesa++;
			if (nr_piesa % 50 == 0)
				if (viteza >= 10) viteza--;

			//Testeaza daca piesa poate fi mutata in jos
			if (Incadrare_Piesa(piesa_curenta, rotatie_curenta, pozitie_x, pozitie_y + 1))
				pozitie_y++;
			else
			{
				//piesa este blocata atunci cand nu mai poate fi coborata
				for (int i = 0; i< 4; i++)
					for (int j = 0; j< 4; j++)
						if(piesa[piesa_curenta][Roteste(i, j, rotatie_curenta)] != L'.')
							careu[(pozitie_y + j) * latime_careu + (pozitie_x + i)] = piesa_curenta + 1;
				//verifica daca sunt linii complete
				for (int j = 0; j < 4; j++)
					if (pozitie_y + j < inaltime_careu - 1)
					{
						bool linieCompleta = true;
						for (int i = 1; i< latime_careu - 1; i++)
							linieCompleta &= (careu[(pozitie_y + j) * latime_careu + i]) != 0;

						//elimina linia completa
						if (linieCompleta)
						{
							for (int i = 1; i< latime_careu - 1; i++)
								careu[(pozitie_y + j) * latime_careu + i] = 8;
							vector_linii.push_back(pozitie_y + j);
						}
					}

				scor += 25;
				if (!vector_linii.empty())	 scor += (1 << vector_linii.size()) * 100;

				//alege o alta piesa
				pozitie_x = latime_careu / 2;
				pozitie_y = 0;
				rotatie_curenta = 0;
				piesa_curenta = rand() % 7;
				//daca piesa nu incape din momentul in care este lansata, jocul se incheie
				sfarsit_joc = !Incadrare_Piesa(piesa_curenta, rotatie_curenta, pozitie_x, pozitie_y);
			}
		}

		//deseneaza careul
		for (int x = 0; x < latime_careu; x++)
			for (int y = 0; y < inaltime_careu; y++)
				ecran[(y + 2) * latime_ecran + (x + 2)] = L" ABCDEFG=#"[careu[y * latime_careu + x]];

		//deseneaza piesa curenta 
		for (int i = 0; i< 4; i++)
			for (int j= 0; j< 4; j++)
				if (piesa[piesa_curenta][Roteste(i, j, rotatie_curenta)] != L'.')
					ecran[(pozitie_y + j + 2) * latime_ecran + (pozitie_x + i + 2)] = piesa_curenta + 65;

		//deseneaza scorul
		swprintf_s(&ecran[2 * latime_ecran + latime_careu + 6], 16, L"SCORE: %8d", scor);

		//Animare stergerea unei linii complete
		if (!vector_linii.empty())
		{
			WriteConsoleOutputCharacter(Consola, ecran, latime_ecran * inaltime_ecran, { 0,0 }, &biti);
			this_thread::sleep_for(400ms); 

			for (auto& v : vector_linii)
				for (int i = 1; i< latime_careu - 1; i++)
				{
					for (int j = v; j > 0; j--)
						careu[j * latime_careu + i] = careu[(j - 1) * latime_careu + i];
					careu[i] = 0;
				}

			vector_linii.clear();
		}

		WriteConsoleOutputCharacter(Consola, ecran, latime_ecran * inaltime_ecran, { 0,0 }, &biti);
	}
	CloseHandle(Consola);

	//afisarea dupa sfarsitul jocului
	cout << "Game Over!! Scorul dumneavoastra:" << scor << endl;
	system("pause");
	return 0;
}