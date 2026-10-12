#include <iostream>
#include <string>
#include "kutuphane.h"

using namespace std;



int main() {
    kutuphane kitap1;
    kutuphane kitap2("aile mutlulugu",21,"tolstoy","mehmet");
    kutuphane kitap3;

    kitap1.setkutuphaneismi("mus alparslan");

   kitap1.kitap_ismi="denemeler";
    kitap1.yazar="montaigane";
    kitap1.kitap_no=232;
    kitap1.musteri_isim="dogan";

    kitap2.bilgilerigoster();
    cout<<endl;
    kitap1.bilgilerigoster();
    cout<<endl;
    kitap3.bilgilerigoster();


    return 0;
}





