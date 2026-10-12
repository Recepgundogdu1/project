//
// Created by gundo on 12.10.2026.
//
#include "kutuphane.h"
#include <iostream>
#include <string>
using namespace std;

kutuphane::kutuphane(string _kitap_ismi, int _kitap_no, string _yazar, string _musteri_isim) {
 kitap_ismi=_kitap_ismi;
    kitap_no=_kitap_no;
    yazar=_yazar;
    musteri_isim=_musteri_isim;

}

void kutuphane::bilgilerigoster() {
    cout<<kitap_no<<" "<<kitap_ismi<<" "<<yazar<<" "<<musteri_isim<<kutuphaneismi;

}

 void kutuphane::setkutuphaneismi(string  _kutuphaneismi ){
    kutuphaneismi=_kutuphaneismi;
}

string kutuphane::getkutuphaneismi() {
    return kutuphaneismi;
}

kutuphane::kutuphane() {
    kitap_ismi="bos girildi kitap ismi";
    kitap_no=0;
    yazar="bos girildi yazar";
    musteri_isim="bos girildi musteri ismi bos girildi";


}

