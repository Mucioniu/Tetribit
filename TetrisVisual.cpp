#include <iostream>
#include <thread>
#include <vector>
using namespace std;

#include <stdio.h>
#include <Windows.h>

int latime_ecran = 120.6;			
int inaltime_ecran = 80;		
wstring piesa[7];
int latime_careu = 12;
int inaltime_careu = 18;
unsigned char* careu = nullptr;

int Roteste(int pozitie_x, int pozitie_y, int numarApasari)
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

bool Incadrare_Piesa(int numarPiesa, int numarRotatie, int numarPozitieX, int numarPozitieY)
{
	for (int i = 0; i< 4; i++)
		for (int j = 0; j< 4; j++)
		{
			int rotatie = Roteste(i, j, numarRotatie);
			int fi = (numarPozitieY + j) * latime_careu + (numarPozitieX + i);
			if (numarPozitieX + i>= 0 && numarPozitieX + i< latime_careu)
			{
				if (numarPozitieY + j>= 0 && numarPozitieY + j< inaltime_careu)
				{
					if (piesa[numarPiesa][rotatie] != L'.' && careu[fi] != 0)
						return false;
				}
			}
		}

	return true;
}

int main()
{
	wchar_t* ecran = new wchar_t[latime_ecran * inaltime_ecran];
	for (int i = 0; i < latime_ecran * inaltime_ecran; i++) ecran[i] = L' ';
	HANDLE Consola = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(Consola);
	DWORD biti = 0;

	piesa[0].append(L"..X...X...X...X.");
	piesa[1].append(L"..X..XX...X.....");
	piesa[2].append(L".....XX..XX.....");
	piesa[3].append(L"..X..XX..X......");
	piesa[4].append(L".X...XX...X.....");
	piesa[5].append(L".X...X...XX.....");
	piesa[6].append(L"..X...X..XX.....");

	careu = new unsigned char[latime_careu * inaltime_careu];
	for (int x = 0; x < latime_careu; x++)
		for (int y = 0; y < inaltime_careu; y++)
			careu[y * latime_careu + x] = (x == 0 || x == latime_careu - 1 || y == inaltime_careu - 1) ? 9 : 0;

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

	while (!sfarsit_joc)
	{
		this_thread::sleep_for(50ms);
		viteza_piesa++;
		coborare = (viteza_piesa == viteza);

		for (int k = 0; k < 4; k++)
			tasta[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;

		pozitie_x += (tasta[0] && Incadrare_Piesa(piesa_curenta, rotatie_curenta, pozitie_x + 1, pozitie_y)) ? 1 : 0;
		pozitie_x -= (tasta[1] && Incadrare_Piesa(piesa_curenta, rotatie_curenta, pozitie_x - 1, pozitie_y)) ? 1 : 0;
		pozitie_y += (tasta[2] && Incadrare_Piesa(piesa_curenta, rotatie_curenta, pozitie_x, pozitie_y + 1)) ? 1 : 0;

		if (tasta[3])
		{
			rotatie_curenta += (rotire && Incadrare_Piesa(piesa_curenta, rotatie_curenta + 1, pozitie_x, pozitie_y)) ? 1 : 0;
			rotire = false;
		}
		else
			rotire = true;

		if (coborare)
		{
			viteza_piesa = 0;
			nr_piesa++;
			if (nr_piesa % 50 == 0)
				if (viteza >= 10) viteza--;

			if (Incadrare_Piesa(piesa_curenta, rotatie_curenta, pozitie_x, pozitie_y + 1))
				pozitie_y++;
			else
			{
				for (int px = 0; px < 4; px++)
					for (int py = 0; py < 4; py++)
						if(piesa[piesa_curenta][Roteste(px, py, rotatie_curenta)] != L'.')
							careu[(pozitie_y + py) * latime_careu + (pozitie_x + px)] = piesa_curenta + 1;

				for (int py = 0; py < 4; py++)
					if (pozitie_y + py < inaltime_careu - 1)
					{
						bool bLine = true;
						for (int px = 1; px < latime_careu - 1; px++)
							bLine &= (careu[(pozitie_y + py) * latime_careu + px]) != 0;

						if (bLine)
						{
							for (int px = 1; px < latime_careu - 1; px++)
								careu[(pozitie_y + py) * latime_careu + px] = 8;
							vector_linii.push_back(pozitie_y + py);
						}
					}

				scor += 25;
				if (!vector_linii.empty())	 scor += (1 << vector_linii.size()) * 100;

				pozitie_x = latime_careu / 2;
				pozitie_y = 0;
				rotatie_curenta = 0;
				piesa_curenta = rand() % 7;
				sfarsit_joc = !Incadrare_Piesa(piesa_curenta, rotatie_curenta, pozitie_x, pozitie_y);
			}
		}

		for (int x = 0; x < latime_careu; x++)
			for (int y = 0; y < inaltime_careu; y++)
				ecran[(y + 2) * latime_ecran + (x + 2)] = L" ABCDEFG=#"[careu[y * latime_careu + x]];

		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
				if (piesa[piesa_curenta][Roteste(px, py, rotatie_curenta)] != L'.')
					ecran[(pozitie_y + py + 2) * latime_ecran + (pozitie_x + px + 2)] = piesa_curenta + 65;

		swprintf_s(&ecran[2 * latime_ecran + latime_careu + 6], 16, L"SCORE: %8d", scor);

		if (!vector_linii.empty())
		{
			WriteConsoleOutputCharacter(Consola, ecran, latime_ecran * inaltime_ecran, { 0,0 }, &biti);
			this_thread::sleep_for(400ms); 

			for (auto& v : vector_linii)
				for (int px = 1; px < latime_careu - 1; px++)
				{
					for (int py = v; py > 0; py--)
						careu[py * latime_careu + px] = careu[(py - 1) * latime_careu + px];
					careu[px] = 0;
				}

			vector_linii.clear();
		}

		WriteConsoleOutputCharacter(Consola, ecran, latime_ecran * inaltime_ecran, { 0,0 }, &biti);
	}
	CloseHandle(Consola);
	cout << "Game Over!! Scorul dumneavoastra:" << scor << endl;
	system("pause");
	return 0;
}