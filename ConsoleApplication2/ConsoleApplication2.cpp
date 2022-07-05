// ConsoleApplication2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <time.h>
#include <atlstr.h>
#include <algorithm>    // std::sort

#include <curl/curl.h>
#include <json/json.h>

#define ILOSC_TEST 8

clock_t                                 start, koniec;                              //Czas pobierania danych
std::string                             url_crypto = "";
int                                     ilosc_par_lista = 0;
std::vector<std::string>                kryptowaluty_wszystkie;
std::vector<std::string>                kryptowaluty_lista;
std::vector<std::vector<std::string>>   cale_dane;
std::vector<std::string>                para;
std::string                             dane_wejsciowe[ILOSC_TEST] = { "BTCUSDT","ETHUSDT","SOLUSDT","ADAUSDT","OPUSDT","ENSUSDT","PAXGUSDT","AAVEUSDT" };

void inicjuj()
{
    for (int j = 0; j < ILOSC_TEST; j++)
    {
        para.clear();
        //std::cout << dane_wejsciowe[j] << "-->" << j << std::endl;
        para.push_back(dane_wejsciowe[j]);
        para.push_back("0.00");
        cale_dane.push_back(para);
    }
}                                                                                           //Funkcja do testow                                                                       
void stworz_url()                                                                           
{
    std::string url("https://api.binance.com/api/v3/ticker/price?symbols=[");
    for (int i = 0; i < ILOSC_TEST; i++)
    {
        if (i > 0) { url = url + ","; }
        url = url + "\"" + cale_dane[i][0] + "\"";
    }
    url_crypto = url + "]";
}                                                                                           //Stworz url do pobierania listy wybranych crypto
bool pobierz_dane(std::string text);                                                        //Pobiera dane dla konkretnych wybranych crypto
bool pobierz_dane_lista();                                                                  //Tylko raz na poczatku pobiera cala liste wszystkich crypto
void porzadkuj_liste(std::vector<std::string> lista);                                       //Usun koncowki "USDT","BTC","ETH" itp i powtorzenia 
bool sprawdz_powtorzenia(std::vector<std::string> lista1, std::string text, int ilosc_i);   //sprawdza powtorzenia na nowo tworzonej liscie


int main()
{
    bool czy_pobrano = false;
    bool czy_pobrano_liste = false;
    inicjuj();
    stworz_url();
    while (!czy_pobrano_liste) { czy_pobrano_liste = pobierz_dane_lista(); }
    kryptowaluty_lista = kryptowaluty_wszystkie;
    porzadkuj_liste(kryptowaluty_lista);
    std::sort(kryptowaluty_lista.begin(), kryptowaluty_lista.end());
    std::cout << "Hello World!\n";
    for (int i = 0; i < ilosc_par_lista; i++)
    {
        std::cout << kryptowaluty_lista[i] << std::endl;
        //std::cout << cale_dane[i][0] << " -> " << cale_dane[i][1] << std::endl;
    }
    std::cout << "ilosc pozycji: " << ilosc_par_lista << std::endl;
    start = clock();
    while (!czy_pobrano) { czy_pobrano = pobierz_dane(url_crypto); }
    koniec = clock();
    std::cout << "Czas pobrania: " << (koniec - start)*1000 / CLOCKS_PER_SEC << "ms" << std::endl;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

namespace
{
    std::size_t callback(
        const char* in,
        std::size_t size,
        std::size_t num,
        std::string* out)
    {
        const std::size_t totalBytes(size * num);
        out->append(in, totalBytes);
        return totalBytes;
    }
}

bool pobierz_dane(std::string text)
{
    //MessageBox(NULL, L"A0", L"A0", MB_OK);

    const std::string url=text;
    CURL* curl = curl_easy_init();

    // Set remote URL.
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // Don't bother trying IPv6, which would increase DNS resolution time.
    curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);

    // Don't wait forever, time out after 1 seconds.
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1);

    // Follow HTTP redirects if necessary.
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    // Hook up data handling function.
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);

    // Response information.
    //long httpCode(0);
    std::unique_ptr<std::string> httpData(new std::string());

    // Hook up data container (will be passed as the last parameter to the
    // callback handling function).  Can be any pointer type, since it will
    // internally be passed as a void pointer.
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData.get());

    // Run our HTTP GET command, capture the HTTP response code, and clean up.
    curl_easy_perform(curl);
    //curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    curl_easy_cleanup(curl);
    Json::Value jsonData;
    Json::Reader jsonReader;

    int ilosc = 0;

    if (jsonReader.parse(*httpData.get(), jsonData))
    {
        //MessageBox(NULL, L"A", L"A", MB_OK);
        for (Json::Value::ArrayIndex i = 0; i != jsonData.size(); i++)
        {
            std::cout << jsonData[i]["symbol"].asString() << "  -->  " << jsonData[i]["price"].asString() << std::endl;
            ilosc++;
            /*for (int j = 0; j < ilosc_par; j++)
            {
                if (jsonData[i]["symbol"].asString() == kryptowaluty_pary[j])
                {

                    zm_text_wiersz[0] = jsonData[i]["symbol"].asString();
                    zm_text_wiersz[1] = jsonData[i]["price"].asString();
                    wynik[ilosc] = zm_text_wiersz;
                    ilosc++;
                    kryptowaluty_pary.erase(kryptowaluty_pary.begin() + j);
                    j = ilosc_par;
                    ilosc_par--;
                }
            }*/
        }
        std::cout << "Wszystkie pary: " << ilosc << std::endl;
        return true;
        //kryptowaluty_pary.
        /*koniec = clock();
        kryptowaluty_pary.resize(kryptowaluty_pary_zapas.size());
        kryptowaluty_pary = kryptowaluty_pary_zapas;
        ilosc_par = ilosc_par_zapas;
        return true;*/
    }
    else
    {
        /*koniec = clock();
        kryptowaluty_pary.resize(kryptowaluty_pary_zapas.size());
        kryptowaluty_pary = kryptowaluty_pary_zapas;
        ilosc_par = ilosc_par_zapas;*/
        return false;
    }
}
bool pobierz_dane_lista()
{
    const std::string url("https://api.binance.com/api/v3/ticker/price");
    CURL* curl = curl_easy_init();

    // Set remote URL.
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // Don't bother trying IPv6, which would increase DNS resolution time.
    curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);

    // Don't wait forever, time out after 1 seconds.
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1);

    // Follow HTTP redirects if necessary.
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    // Hook up data handling function.
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);

    // Response information.
    //long httpCode(0);
    std::unique_ptr<std::string> httpData(new std::string());

    // Hook up data container (will be passed as the last parameter to the
    // callback handling function).  Can be any pointer type, since it will
    // internally be passed as a void pointer.
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData.get());

    // Run our HTTP GET command, capture the HTTP response code, and clean up.
    curl_easy_perform(curl);
    //curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    curl_easy_cleanup(curl);
    Json::Value jsonData;
    Json::Reader jsonReader;

    int ilosc = 0;

    if (jsonReader.parse(*httpData.get(), jsonData))
    {
        //MessageBox(NULL, L"A", L"A", MB_OK);
        for (Json::Value::ArrayIndex i = 0; i != jsonData.size(); i++)
        {
            kryptowaluty_wszystkie.push_back(jsonData[i]["symbol"].asString());
            ilosc++;
            /*for (int j = 0; j < ilosc_par; j++)
            {
                if (jsonData[i]["symbol"].asString() == kryptowaluty_pary[j])
                {

                    zm_text_wiersz[0] = jsonData[i]["symbol"].asString();
                    zm_text_wiersz[1] = jsonData[i]["price"].asString();
                    wynik[ilosc] = zm_text_wiersz;
                    ilosc++;
                    kryptowaluty_pary.erase(kryptowaluty_pary.begin() + j);
                    j = ilosc_par;
                    ilosc_par--;
                }
            }*/
        }
        ilosc_par_lista = ilosc;
        std::cout << "Wszystkie pary kryptowalut: " << ilosc << std::endl;
        return true;
        //kryptowaluty_pary.
        /*koniec = clock();
        kryptowaluty_pary.resize(kryptowaluty_pary_zapas.size());
        kryptowaluty_pary = kryptowaluty_pary_zapas;
        ilosc_par = ilosc_par_zapas;
        return true;*/
    }
    else
    {
        /*koniec = clock();
        kryptowaluty_pary.resize(kryptowaluty_pary_zapas.size());
        kryptowaluty_pary = kryptowaluty_pary_zapas;
        ilosc_par = ilosc_par_zapas;*/
        return false;
    }
}
void porzadkuj_liste(std::vector<std::string> lista)
{
    std::vector<std::string> tymczasowy;
    int szukaj = 0;
    int ilosc_par_nowe = 0;
    for (int i = 0; i < ilosc_par_lista; i++)   //usun "DOWN" "UP" "AUCTION" "BULL" "BEAR"
    {
        szukaj = lista[i].find("DOWN");
        if (szukaj > 0) { lista[i]=""; }
        szukaj = lista[i].find("UP");
        if (szukaj > 0) { lista[i]=""; }
        szukaj = lista[i].find("BULL");
        if (szukaj > 0) { lista[i]=""; }
        szukaj = lista[i].find("BEAR");
        if (szukaj > 0) { lista[i]=""; }
        szukaj = lista[i].find("AUCTION");
        if (szukaj > 0) { lista[i]=""; }
    }
    for (int i = 0; i < ilosc_par_lista; i++)   //Petla USDT - usun koncowke
    {
        szukaj = lista[i].find("USDT");
        if (szukaj > 0) 
        { 
            lista[i].replace(szukaj, 4, ""); 
            tymczasowy.push_back(lista[i]); 
            ilosc_par_nowe++; 
        }
    }
    for (int i = 0; i < ilosc_par_lista; i++)   //Petla BTC - usun koncowke
    {
        szukaj = lista[i].find("BTC");
        if (szukaj > 0) 
        { 
            lista[i].replace(szukaj, 4, "");
            if (sprawdz_powtorzenia(tymczasowy, lista[i], ilosc_par_nowe))
            {
                tymczasowy.push_back(lista[i]);
                ilosc_par_nowe++;
            }
        }
    }
    for (int i = 0; i < ilosc_par_lista; i++)   //Petla ETH - usun koncowke
    {
        szukaj = lista[i].find("ETH");
        if (szukaj > 0) 
        { 
            lista[i].replace(szukaj, 4, ""); 
            if (sprawdz_powtorzenia(tymczasowy, lista[i], ilosc_par_nowe))
            {
                tymczasowy.push_back(lista[i]);
                ilosc_par_nowe++;
            }
        }
    }
    kryptowaluty_lista.clear();
    ilosc_par_lista = ilosc_par_nowe;
    kryptowaluty_lista = tymczasowy;
}
bool sprawdz_powtorzenia(std::vector<std::string> lista1, std::string text, int ilosc_i)
{
    int zmienna = -1;
    for (int i = 0; i < ilosc_i; i++)
    {
        zmienna = lista1[i].find(text);
        if (zmienna >= 0) { return false; i = ilosc_i; }
    }
    return true;
}
