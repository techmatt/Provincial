struct StemmerInfo;
class Stemmer
{
public:
    static void Stem(String &s);

private:
    static int cons(StemmerInfo *z, int i);
    static int m(StemmerInfo *z);
    static int vowelinstem(StemmerInfo *z);
    static int doublec(StemmerInfo *z, int j);
    static int cvc(StemmerInfo *z, int i);
    static void setto(StemmerInfo *z, char * s);
    static int ends(StemmerInfo *z, char * s);
    static void r(StemmerInfo *z, char * s);
    static void step1ab(StemmerInfo *z);
    static void step1c(StemmerInfo *z);
    static void step2(StemmerInfo *z);
    static void step3(StemmerInfo *z);
    static void step4(StemmerInfo *z);
    static void step5(StemmerInfo *z);
};
