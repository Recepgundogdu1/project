//
// Created by gundo on 12.10.2026.
//

#ifndef CPP_KUTUPHANE_H
#define CPP_KUTUPHANE_H
#include <iostream>
#include <string>
using namespace std;
#endif //CPP_KUTUPHANE_H


class kutuphane {
public:
    string kitap_ismi;
    int kitap_no;
    string yazar;
    string musteri_isim;
    kutuphane(string _kitap_ismi,int _kitap_no,string _yazar,string _musteri_isim);
    void bilgilerigoster();
    kutuphane();
    void setkutuphaneismi(string _kutuphaneismi);
    string getkutuphaneismi();
private:
    string kutuphaneismi;

};
