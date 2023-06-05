#define _CRT_SECURE_NO_WARNINGS

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

int x[100], y[100], noktaSayisi;
const int alanX = 1000;
const int alanY = 1000;
const int fontBuyuklugu = 18;

typedef ALLEGRO_COLOR Renk_t;

struct enUzakXY
{//Merkez noktasina en uzak noktanin x ve y koordinatlar�n� tutan struct.
	int enUzakX;
	int enUzakY;
};

struct Agirlik
{//Merkez noktasinin x ve y koordinatlarini ve merkezin en uzak nokta ile aras�ndaki mesafeyi tutan struct.
	float agirlikX;
	float agirlikY;
	float yariCap;
	struct enUzakXY;
};

void veriAl() {
	//Kullanici tarafindan dosyaya girilen verilen al�n��� ve pointer arac�l��� ile bir dizi tutulu�u.
	FILE* dosya = fopen("sayilar.txt", "r");
	int k = 0;
	if (dosya == NULL) {
		printf("Dosya bulunamadi");
	}
	else {
		printf("Dosya bulundu\n");

		while (!feof(dosya))
		{
			fscanf(dosya, "%d %d", &x[k], &y[k]);
			k++;
		}
	}
	noktaSayisi = k;
	fclose(dosya);
}

struct Agirlik agirlikOrtalamasi() {
	//T�m noktalar�n x ve y a��rl�k ortalamalar�n� bularak rastgele bir merkez noktas�n�n olu�turulu�u.
	struct Agirlik agirlik;
	int agirlikX = 0, agirlikY = 0;
	for (int i = 0; i < noktaSayisi; i++) {
		agirlikX = agirlikX + x[i];
		agirlikY = agirlikY + y[i];
	}

	agirlik.agirlikX = agirlikX / noktaSayisi;
	agirlik.agirlikY = agirlikY / noktaSayisi;

	return agirlik;
}

int olcekOlusturma() {
	//En b�y�k girdiye g�re dinamik �l�eklendirme.
	int olcek, tut = x[0];
	for (int i = 0; i < noktaSayisi; i++) {
		if (abs(x[i]) > abs(tut))
			tut = x[i];
		if (abs(y[i]) > abs(tut))
			tut = y[i];
	}
	olcek = alanX / (abs(tut) * 4);
	return olcek;
}

int pixelSayisi(int olcek) {
	//Her koordinat ekseni aras�ndaki pixel miktari.
	int artisMiktari;

	if (olcek > 50)
		artisMiktari = 1;
	else if (olcek > 20)
		artisMiktari = 2;
	else if (olcek > 10)
		artisMiktari = 4;
	else if (olcek > 5)
		artisMiktari = 10;
	else if (olcek > 2)
		artisMiktari = 20;
	else
		artisMiktari = 50;

	return artisMiktari;
}

float mesafeHesapla(float x1, float x2, float y1, float y2) {
	// 2 nokta aras� mesafe hesaplama.
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

struct Agirlik yaricapHesapla() {
	struct Agirlik agirlik = agirlikOrtalamasi(noktaSayisi, x, y);

	int k = 0, s = 0, kontrol = 1;
	float tut, mesafe;

	if (noktaSayisi < 2) 
	{
		agirlik.yariCap = 0;
		agirlik.enUzakX = x[0];
		agirlik.enUzakY = y[0];
		return agirlik;
	}

	while (kontrol) {

		tut = mesafeHesapla(x[0], agirlik.agirlikX, y[0], agirlik.agirlikY); // Merkeze en uzak noktay� ilk nokta olarak kabul etme
		agirlik.enUzakX = x[0], agirlik.enUzakY = y[0]; // Merkeze en uzak noktanin x ve y koordinatlar�n� ilk nokta olarak kabul etme
		for (int i = 0; i < noktaSayisi - 1; i++)
		{
			if (mesafeHesapla(x[i + 1], agirlik.agirlikX, y[i + 1], agirlik.agirlikY) > tut)
			{
				tut = mesafeHesapla(x[i + 1], agirlik.agirlikX, y[i + 1], agirlik.agirlikY); // Yeni merkeze en uzak mesafeyi belirleme.
				agirlik.enUzakX = x[i + 1]; // Yeni en uzak x koordinat�n� belirleme.
				agirlik.enUzakY = y[i + 1]; // Yeni en uzak y koordinat�n� belirleme.
			}
		}

		agirlik.agirlikX = (k * (agirlik.agirlikX) + agirlik.enUzakX) / (k + 1); // Merkez x noktasinin olu�turulmas�.
		agirlik.agirlikY = (k * (agirlik.agirlikY) + agirlik.enUzakY) / (k + 1); // Merkez y noktasinin olu�turulmas�.

		agirlik.yariCap = mesafeHesapla(agirlik.enUzakX, agirlik.agirlikX, agirlik.enUzakY, agirlik.agirlikY); // En uzak nokta ile merkez noktasinin arasindaki mesafenin hesaplanmas�.

		for (int i = 0; i < noktaSayisi - 1; i++) // D�ng�y� sonland�racak �art�n sa�lanmas�.
		{
			for (int j = i + 1; j < noktaSayisi; j++)
			{
				float p = mesafeHesapla(x[i], x[j], y[i], y[j]);
				float cap = agirlik.yariCap * 2.0;

				if (p == cap && k > 1000) // E�er �ember �zerindeki herhangi 2 nokta aras�ndaki mesafe �apa e�itse d�ng�y� durdur.
					kontrol = 0;

				mesafe = mesafeHesapla(x[j], agirlik.agirlikX, y[j], agirlik.agirlikY);
				if (mesafe == agirlik.yariCap && k > 1000) // �ember �zerindeki noktalar�n sayisi
					s++;
			}
		}
		if (s >= 3) // �ember �zerinde 3 ve 3'ten fazla nokta varsa d�ng�y� durdur.
			kontrol = 0;

		s = 0;
		k++;

		//A�a��daki yorum sat�r�n�n a��klamas�..Program�n ad�m ad�m ideal �emberi �izdirmesinin g�sterili�i.

		/*int olcek = olcekOlusturma(x, y, noktaSayisi);
		al_draw_circle(agirlik.agirlikX * olcek + alanX / 2, -agirlik.agirlikY * olcek + alanY/2, agirlik.yariCap * olcek , al_map_rgb(255, 255, 255), 1);
		al_draw_circle(agirlik.agirlikX * olcek + alanX / 2, -agirlik.agirlikY * olcek + alanY / 2, 5, al_map_rgb(255, 0, 0), 1);
		al_rest(0.1);
		al_flip_display();*/
	}
	return agirlik;
}

void cemberCiz(int olcek) {

	
	al_init_ttf_addon();                     //Allegro eklentilerinin eklenmesi.
	al_init_font_addon();
	al_init_primitives_addon();

	ALLEGRO_FONT* font = al_load_ttf_font("arial.ttf", fontBuyuklugu, 0);  //Fontun olu�turulmas�, fon tipi ve b�y�kl���n�n belirlenmesi.

	Renk_t izgaraRenk = al_map_rgb(50, 50, 50);
	Renk_t duzlemRenk = al_map_rgb(238, 233, 233);
	Renk_t fontRenk = al_map_rgb(238, 233, 233);     //Renk k�sayollar�.
	Renk_t ucgenRenk = al_map_rgb(255, 255, 255);

	al_clear_to_color(al_map_rgb(0, 0, 0));      //Ekran arkaplan rengi.

	int artisMiktari = pixelSayisi(olcek);       //Koordinat d�zlemi �zerinde sayilar�n artis miktar�.

	//Koordinat d�zleminin �izili�i.
	al_draw_line(0, alanY / 2, alanX, alanY / 2, duzlemRenk, 2);
	al_draw_line(alanX / 2, 0, alanX / 2, alanY, duzlemRenk, 2);

	//Izgaralar�n �izili�i.
	for (int i = 0; i < (alanX / 2) / olcek + 1; i++) {
		al_draw_line(0, (alanY / 2) + i * olcek, alanX, (alanY / 2) + i * olcek, izgaraRenk, 1);
		al_draw_line(0, (alanY / 2) - i * olcek, alanX, (alanY / 2) - i * olcek, izgaraRenk, 1);
	}

	for (int i = 0; i < (alanY / 2) / olcek + 1; i++) {
		al_draw_line(i * olcek + alanX / 2, 0, i * olcek + alanX / 2, alanY, izgaraRenk, 1);
		al_draw_line(-i * olcek + alanX / 2, 0, -i * olcek + alanX / 2, alanY, izgaraRenk, 1);
	}

	char yazdir[5];

	for (int i = artisMiktari; i <= (alanX / 2) / olcek; i += artisMiktari) {
		//Koordinat d�zlemi �zerindeki say�lar�n yazd�r���.
		sprintf(yazdir, "%d", i);
		al_draw_text(font, fontRenk, alanX / 2 - (-i) * olcek - fontBuyuklugu / 2, alanY / 2, 0, yazdir);
		al_draw_text(font, fontRenk, 7 + alanX / 2, alanY / 2 - i * olcek - fontBuyuklugu / 2, 0, yazdir);

		sprintf(yazdir, "%d", -i);
		al_draw_text(font, fontRenk, alanX / 2 - i * olcek - fontBuyuklugu / 2, alanY / 2, 0, yazdir);
		al_draw_text(font, fontRenk, 2 + alanX / 2, alanY / 2 - (-i) * olcek - fontBuyuklugu / 2, 0, yazdir);
	}
	//Koordinat d�zlemi u�lar�ndaki ��genlerin olu�turulmas�.
	al_draw_filled_triangle(alanX / 2, 0, alanX / 2 + 10, 10, alanX / 2 - 10, 10, ucgenRenk);
	al_draw_filled_triangle(alanX / 2, alanY, alanX / 2 - 10, alanY - 10, alanX / 2 + 10, alanY - 10, ucgenRenk);
	al_draw_filled_triangle(0, alanY / 2, 10, alanY / 2 + 10, 10, alanY / 2 - 10, ucgenRenk);
	al_draw_filled_triangle(alanX, alanY / 2, alanX - 10, alanY / 2 + 10, alanX - 10, alanY / 2 - 10, ucgenRenk);

	//Kullan�c� taraf�ndan girilen noktalar�n ekrana yaz�l���.
	for (int i = 0; i < noktaSayisi; i++)
	{
		al_draw_filled_circle(x[i] * (olcek)+alanX / 2, y[i] * (-olcek) + alanY / 2, 6, al_map_rgb(0, 255, 255));
	}

	struct Agirlik agirlik = yaricapHesapla(x, y, noktaSayisi);
	float yariCap = agirlik.yariCap;

	//En k���k �emberin �izili�i.
	al_draw_circle(agirlik.agirlikX * olcek + alanX / 2, -agirlik.agirlikY * olcek + alanY / 2, yariCap * olcek, al_map_rgb(238, 238, 0), 3);

	//Merkez noktas�n�n �izili�i.
	al_draw_filled_circle(agirlik.agirlikX * olcek + alanX / 2, -agirlik.agirlikY * olcek + alanY / 2, 5, al_map_rgb(255, 0, 0));

	//Yar��ap�n �izili�i.
	al_draw_line(agirlik.agirlikX * olcek + alanX / 2, -agirlik.agirlikY * olcek + alanY / 2, (agirlik.enUzakX * olcek + alanX / 2), (-agirlik.enUzakY * olcek + alanY / 2), al_map_rgb(255, 0, 0), 2);

	//Yar��ap ve merkez noktas�n�n yaz�l���.
	char yariCapYazdir[25];
	sprintf(yariCapYazdir, "Yaricap = %.2f", (agirlik.yariCap));
	al_draw_text(font, al_map_rgb(255, 0, 0), 50, 50, 0, yariCapYazdir);
	printf("Yaricap = %.4f\n", (agirlik.yariCap));
	al_draw_text(font, al_map_rgb(255, 0, 0), (agirlik.agirlikX * olcek + alanX / 2 + agirlik.enUzakX * olcek + alanX / 2) / 2, (-agirlik.agirlikY * olcek + alanY / 2 - agirlik.enUzakY * olcek + alanY / 2) / 2, 0, "r");

	char merkezNoktasiYazdir[50];
	sprintf(merkezNoktasiYazdir, "Merkez noktasi = (%.2f,%.2f)", (agirlik.agirlikX), (agirlik.agirlikY));
	al_draw_text(font, al_map_rgb(255, 0, 0), 50, 75, 0, merkezNoktasiYazdir);
	printf("Merkez noktasi = %.2f,%.2f \n", (agirlik.agirlikX), (agirlik.agirlikY));
	al_flip_display();
}

void splineCiz(int olcek) {
	int i = 0;
	float t = 0;
	while (i != noktaSayisi - 1)
	{
		t += 0.002; // t'yi her d�ng�de k���k bir miktar artt�r�l��� ve noktalar�n ekran �zerinde s�k g�r�nt�lenmesini sa�lanmas�.
		if (t >= 1) // t'yi 0 ile 1 aras�nda s�n�rland�rma
		{
			i++; // t 1'e her ula�t���nda di�er 2 nokta aras�nda e�rinin �izilmesini sa�lama.
			t = 0;
		}
		// �ki noktan�n, bir ba�ka 2 noktay� kontrol noktas� kabul ederek, e�rinin her bir pikselinin x ve y koordinat�n�n hesaplan���.
		float tx = 0.5 * ((2 * x[(i + 1) % noktaSayisi]) + (-x[i % noktaSayisi] + x[(i + 2) % noktaSayisi]) * t + (2 * x[i % noktaSayisi] - 5 * x[(i + 1) % noktaSayisi] + 4 * x[(i + 2) % noktaSayisi] - x[(i + 3) % noktaSayisi]) * t * t + (-x[i % noktaSayisi] + 3 * x[(i + 1) % noktaSayisi] - 3 * x[(i + 2) % noktaSayisi] + x[(i + 3) % noktaSayisi]) * t * t * t);
		float ty = 0.5 * ((2 * y[(i + 1) % noktaSayisi]) + (-y[i % noktaSayisi] + y[(i + 2) % noktaSayisi]) * t + (2 * y[i % noktaSayisi] - 5 * y[(i + 1) % noktaSayisi] + 4 * y[(i + 2) % noktaSayisi] - y[(i + 3) % noktaSayisi]) * t * t + (-y[i % noktaSayisi] + 3 * y[(i + 1) % noktaSayisi] - 3 * y[(i + 2) % noktaSayisi] + y[(i + 3) % noktaSayisi]) * t * t * t);

		al_draw_filled_circle(tx * olcek + alanX / 2, -ty * olcek + alanY / 2, 2, al_map_rgb(255, 170, 212));//Noktan�n ekrana �izili�i
		
	}
	al_flip_display();

}

int fareIslemleri(int olcek) {

	printf("\nEkran uzerinden herhangi bir yere tiklayarak yeni bir nokta olusturabilirsiniz.\n");
	printf("Fare tekerlegini kullanarak yaklasma ve uzaklasma islemi yapabilirsiniz.\n");
	while (1) {

		al_init();
		al_install_mouse();

		ALLEGRO_EVENT_QUEUE* queue = NULL;
		queue = al_create_event_queue();

		al_register_event_source(queue, al_get_mouse_event_source());
		ALLEGRO_EVENT event;
		al_wait_for_event(queue, &event);
		int t = 0;
		if (event.mouse.dz) {
			t = event.mouse.dz;       // Fare tekerle�i ile �l�ek �zerinde de�i�iklik yapabilmesini sa�layan k�s�m.
			if (t > 0) {
				olcek += t * 1;
				t = 0;
				return olcek;
			}
			else if (t < 0) {
				olcek += t * 1;
				t = 0;
				return olcek;
			}
		}
			
		if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			noktaSayisi++;
			x[noktaSayisi - 1] = (event.mouse.x - alanX / 2) / olcek;    // Kullan�c�n�n fare ile giri� yapabilmesini sa�layan k�s�m.
			y[noktaSayisi - 1] = -(event.mouse.y - alanY / 2) / olcek;
			return olcek; 
		}
	}
}

void noktaYazdir() {
	for (int i = 0; i < noktaSayisi; i++) {
		printf("%d. nokta = {%d,%d}\n", i + 1, x[i], y[i]);
	}
}

int main()
{
	al_init();
	veriAl();
	if (noktaSayisi < 2) {
		printf("Yetersiz koordinat bilgisi...\nLutfen dosyaya nokta girisi yapiniz.\n");
		system("pause");
	}
	ALLEGRO_DISPLAY* display = NULL;        //Ekran�n olu�turulmas�.
	display = al_create_display(alanX, alanX);   //Ekran geni�li�inin belirlenmesi.
	int olcek = olcekOlusturma();
	while (1) {
		agirlikOrtalamasi();
		noktaYazdir();
		cemberCiz(olcek);
		splineCiz(olcek);
		olcek = fareIslemleri(olcek);
		system("cls");
	}
}
