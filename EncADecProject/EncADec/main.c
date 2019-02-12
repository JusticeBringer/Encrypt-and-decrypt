#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/**
 * Contine corelatiile fiecarei cifre in imaginea test si indicii tuturor ferestrelor posibile
 */
struct Template
{
    double corelatia;
    unsigned int nrFereastra;

    unsigned int cifra;
    unsigned int ly1;
    unsigned int r1y;
    unsigned int lx1;
    unsigned int r1x;
};

/**
 * Fiind trimisa calea unei imagini se calculeaza padding-ul
 * si se pune intr-un vector continutul imaginii trimise
 * @param cale
 * @return tablou
 */
unsigned int *liniarizareTablou( char *cale )
{
    FILE *fisierlocal = fopen(cale, "rb");

    if (fisierlocal == NULL)
    {
        printf("Fisierul este gol/ Nu a reusit sa fie deschis pentru liniarizarea tabloului");
        return 0;
    }

    fseek(fisierlocal, 18, SEEK_SET);
    unsigned int latime = 0;
    fread(&latime, sizeof(latime), 1, fisierlocal);

    unsigned int inaltime = 0;
    fread(&inaltime, sizeof(inaltime), 1, fisierlocal);

    unsigned int *tablou = calloc(latime * inaltime, sizeof(unsigned int));

    fseek(fisierlocal, 0, SEEK_SET);
    unsigned char *header = calloc(54, sizeof(unsigned char));
    fread(header, 54, 1, fisierlocal);

    //incarc fara rotire...asa cum e ea...linie cu linie
    unsigned int indiceElementeTablou = 1;
    unsigned int i, j;
    unsigned int padding;

    if (latime % 4 != 0)
        padding = 4 - (3 * latime) % 4;
    else
        padding = 0;

    fseek(fisierlocal, 54,SEEK_SET);
    for (i = inaltime; i >= 1; i--)
    {  for (j = 1; j <= latime; j++)
        {
            fread(&tablou[indiceElementeTablou], 3, 1, fisierlocal);
            indiceElementeTablou++;
        }
        fseek(fisierlocal,padding,SEEK_CUR);
    }
    fclose(fisierlocal);

    return tablou;
}

/**
 * Fiind trimisa calea unei imagini se returneaza header-ului ei
 * @param cale
 * @return header
 */
unsigned char *obtineHeader (char *cale)
{
    FILE *fisierlocal = fopen(cale, "rb");

    if (fisierlocal == NULL)
    {
        printf("Fisierul este gol/ Nu a reusit sa fie deschis pentru obtinerea headerului");
        exit(0);
    }

    fseek(fisierlocal, 0, SEEK_SET);
    unsigned char *header = calloc(54, sizeof(unsigned char));
    fread(header, 54 , 1, fisierlocal);

    fclose(fisierlocal);
    return header;
}

/**
 * Salveaza extern o imagine cu ajutorul parametrilor dati
 * @param cale
 * @param tabloulDinMemoriaInterna
 * @param headerCeVine
 */
void salvareInMemorieExterna( char *cale, unsigned int *tabloulDinMemoriaInterna, unsigned char *headerCeVine)
{
    FILE *fisierlocal = fopen(cale, "wb");

    if (fisierlocal == NULL) {
        printf("Fisierul este gol/ Nu a reusit sa fie deschis pentru salvare in memoria externa");
        exit(0);
    }
    unsigned int x = headerCeVine[18];
    unsigned int a, b, c;
    a = (unsigned int) (headerCeVine[19]);
    a <<= 8;
    b = (unsigned int) (headerCeVine[20]);
    b <<= 16;
    c = (unsigned int) (headerCeVine[21]);
    c <<= 24;
    x += a + b + c;

    unsigned int y = headerCeVine[22];
    unsigned int d, e, f;
    d = (unsigned int) (headerCeVine[23]);
    d <<= 8;
    e = (unsigned int) (headerCeVine[24]);
    e <<= 16;
    f = (unsigned int) (headerCeVine[25]);
    f <<= 24;
    y += d + e + f;

    //copii octetii si pun intr-un unsigned int

    unsigned int latime = x;
    unsigned int inaltime = y;

    unsigned int dimensiuneTabloulLiniarizat = latime * inaltime ;

    fwrite(headerCeVine, 1, 54, fisierlocal);

    unsigned int padding = 0;

    if (latime % 4 != 0)
        padding = 4 - (3 * latime) % 4;
    else
        padding = 0;

    unsigned int i = 1, j = 1;
    for( i = 1; i <= dimensiuneTabloulLiniarizat;i++)
    {
        fwrite(&tabloulDinMemoriaInterna[i], 3, 1, fisierlocal);
        if (i % latime == 0)
            fseek(fisierlocal,padding,SEEK_CUR);
    }

    fclose(fisierlocal);
}

/**
 * Functia xor
 * @param n
 * @param seed
 * @return tablou
 */
unsigned int *xorShift32(unsigned int n, unsigned int seed)
{
    unsigned int  r, k;
    unsigned int *tablou = calloc(n + 1, sizeof(int));

    r = seed;

    for( k = 1; k <= n; k++)
    {
        r = r ^ r << 13;
        r = r ^ r >> 17;
        r = r ^ r << 5;
        tablou[k] = r;
    }

    return tablou;
}

/**
 * Genereaza o permutare aleatoare
 * @param n
 * @param tabloulGeneratXor
 * @return tablou
 */
unsigned int *permutare(unsigned int n, unsigned int *tabloulGeneratXor)
{
    unsigned int k = 1, r = 1, aux = 1;
    unsigned int *tablou = calloc( n + 1, sizeof(unsigned int));

    for( k = 1; k <= n; k++)
        tablou[k] = k;

    unsigned int indice = 1;

    for( k = n; k >= 1; k--, indice++ )
    {
        r = tabloulGeneratXor[indice] % k +1;
        aux = tablou[r];
        tablou[r] = tablou[k];
        tablou[k] = aux;
    }

    return tablou;
}

/**
 * Cripteaza o imagine
 * @param caleImagineInitiala
 * @param caleImagineCriptata
 * @param caleCheiaSecreta
 */
void criptareImagine( char *caleImagineInitiala, char *caleImagineCriptata, char *caleCheiaSecreta)
{
    FILE* fisierImgInitiala = fopen(caleImagineInitiala, "rb");

    if (fisierImgInitiala == NULL) {
        printf("Fisierul este gol/ Nu a reusit sa fie deschis pentru criptareImagine");
        exit(0);
    }

    unsigned char *headerCeVine = obtineHeader(caleImagineInitiala);

    unsigned int x = headerCeVine[18];
    unsigned int a, b, c;

    a = (unsigned int) (headerCeVine[19]);
    a <<= 8;
    b = (unsigned int) (headerCeVine[20]);
    b <<= 16;
    c = (unsigned int) (headerCeVine[21]);
    c <<= 24;
    x += a + b + c;

    unsigned int y = headerCeVine[22];
    unsigned int d, e, f;

    d = (unsigned int) (headerCeVine[23]);
    d <<= 8;
    e = (unsigned int) (headerCeVine[24]);
    e <<= 16;
    f = (unsigned int) (headerCeVine[25]);
    f <<= 24;
    y += d + e + f;

    unsigned int dimensiuneTablou = x * y;

    unsigned int *tablouLiniarizat = liniarizareTablou(caleImagineInitiala);

    unsigned int obtineCheiaSecreta1 = 0;
    unsigned int obtineCheiaSecreta2 = 0;

    FILE* fisierCuCheiaSecreta = fopen(caleCheiaSecreta, "r");

    fscanf(fisierCuCheiaSecreta, "%u",&obtineCheiaSecreta1);

    fscanf(fisierCuCheiaSecreta, "%u",&obtineCheiaSecreta2);

    fclose(fisierCuCheiaSecreta);

    unsigned int *genereazaTablouXor2X = NULL;
    genereazaTablouXor2X = xorShift32(dimensiuneTablou * 2 , obtineCheiaSecreta1);

    unsigned int *genereazaTablouPermutare = NULL;
    genereazaTablouPermutare = permutare(dimensiuneTablou, genereazaTablouXor2X);

    unsigned int i;
    unsigned int *tabloulPermutat = calloc(dimensiuneTablou, sizeof(unsigned int));

    for( i = 1; i <= dimensiuneTablou; i = i + 1)
    {
        tabloulPermutat[i] = tablouLiniarizat[genereazaTablouPermutare[i]];
    }

    salvareInMemorieExterna(caleImagineCriptata, tabloulPermutat, headerCeVine);

    unsigned int *tabloulPermutatInvers = calloc(dimensiuneTablou, sizeof(unsigned int));

    for( i = 1; i<= dimensiuneTablou; i++)
        tabloulPermutatInvers[genereazaTablouPermutare[i]] = tablouLiniarizat[i];

    for (i = 1; i <=dimensiuneTablou; i++)
        if( i == 1)
            tabloulPermutat[i] = obtineCheiaSecreta2 ^ tabloulPermutat[i] ^ genereazaTablouXor2X[dimensiuneTablou + 1];
        else
            tabloulPermutat[i] = tabloulPermutat[i-1] ^ tabloulPermutat[i] ^ genereazaTablouXor2X[dimensiuneTablou + i];

    salvareInMemorieExterna(caleImagineCriptata, tabloulPermutat, headerCeVine);

    fclose(fisierCuCheiaSecreta);
    fclose(fisierImgInitiala);
}

/**
 * Decripteaza o imagine
 * @param caleImagineInitiala
 * @param caleImagineCriptata
 * @param caleImagineDecriptata
 * @param caleCheiaSecreta
 */
void decriptareImagine( char *caleImagineInitiala, char *caleImagineCriptata, char *caleImagineDecriptata,char *caleCheiaSecreta )
{

    FILE* fisierImgInitiala = fopen(caleImagineInitiala, "rb");

    if (fisierImgInitiala == NULL) {
        printf("Fisierul este gol/ Nu a reusit sa fie deschis pentru criptareImagine");
        exit(0);
    }

    unsigned char *headerCeVine = obtineHeader(caleImagineInitiala);

    unsigned int x = headerCeVine[18];
    unsigned int a, b, c, i;

    a = (unsigned int) (headerCeVine[19]);
    a <<= 8;
    b = (unsigned int) (headerCeVine[20]);
    b <<= 16;
    c = (unsigned int) (headerCeVine[21]);
    c <<= 24;
    x += a + b + c;

    unsigned int y = headerCeVine[22];
    unsigned int d, e, f;

    d = (unsigned int) (headerCeVine[23]);
    d <<= 8;
    e = (unsigned int) (headerCeVine[24]);
    e <<= 16;
    f = (unsigned int) (headerCeVine[25]);
    f <<= 24;
    y += d + e + f;

    unsigned int dimensiuneTablou = x * y;

    unsigned int *tablouLiniarizat = liniarizareTablou(caleImagineInitiala);

    unsigned int obtineCheiaSecreta1 = 0;
    unsigned int obtineCheiaSecreta2 = 0;

    FILE* fisierCuCheiaSecreta = fopen(caleCheiaSecreta, "r");

    fscanf(fisierCuCheiaSecreta, "%u",&obtineCheiaSecreta1);

    fscanf(fisierCuCheiaSecreta, "%u",&obtineCheiaSecreta2);

    fclose(fisierCuCheiaSecreta);

    unsigned int *genereazaTablouXor2X = NULL;
    genereazaTablouXor2X = xorShift32(dimensiuneTablou * 2 , obtineCheiaSecreta1);

    unsigned int *genereazaTablouPermutare = NULL;
    genereazaTablouPermutare = permutare(dimensiuneTablou, genereazaTablouXor2X);

    unsigned int *tabloulPermutat = calloc(dimensiuneTablou, sizeof(unsigned int));
    for( i = 1; i <= dimensiuneTablou; i++)
        tabloulPermutat[i] = tablouLiniarizat[genereazaTablouPermutare[i]];

    for (i = 1; i <=dimensiuneTablou; i++)
        if( i == 1)
            tabloulPermutat[i] = obtineCheiaSecreta2 ^ tabloulPermutat[i] ^ genereazaTablouXor2X[dimensiuneTablou + 1];
        else
            tabloulPermutat[i] = tabloulPermutat[i-1] ^ tabloulPermutat[i] ^ genereazaTablouXor2X[dimensiuneTablou + i];

    //criptat

    unsigned int *copieTablouPermutat = calloc(dimensiuneTablou, sizeof(unsigned int));
    for( i = 1; i <= dimensiuneTablou; i++)
        copieTablouPermutat[i] = tabloulPermutat[i];

    unsigned int *tabloulCPrim = calloc(dimensiuneTablou, sizeof(unsigned int));
    for (i = 1; i <=dimensiuneTablou; i++)
        if( i == 1)
            tabloulCPrim[i] = obtineCheiaSecreta2 ^ tabloulPermutat[i] ^ genereazaTablouXor2X[dimensiuneTablou + 1];
        else
            tabloulCPrim[i] = tabloulPermutat[i-1] ^ tabloulPermutat[i] ^ genereazaTablouXor2X[dimensiuneTablou + i];

    for( i = 1; i <= dimensiuneTablou; i++)
        copieTablouPermutat[genereazaTablouPermutare[i]] = tabloulCPrim[i];

    salvareInMemorieExterna(caleImagineDecriptata,copieTablouPermutat, headerCeVine);

    fclose(fisierCuCheiaSecreta);
    fclose(fisierImgInitiala);
}

/**
 * Afiseaza valorile testului Chi Patrat
 * @param caleImagineInitiala
 */
void testulChiPatrat( char *caleImagineInitiala )
{
    unsigned char *headerCeVine = obtineHeader(caleImagineInitiala);

    unsigned int x = headerCeVine[18];
    unsigned int a, b, c, i;

    a = (unsigned int) (headerCeVine[19]);
    a <<= 8;
    b = (unsigned int) (headerCeVine[20]);
    b <<= 16;
    c = (unsigned int) (headerCeVine[21]);
    c <<= 24;
    x += a + b + c;

    unsigned int y = headerCeVine[22];
    unsigned int d, e, f;

    d = (unsigned int) (headerCeVine[23]);
    d <<= 8;
    e = (unsigned int) (headerCeVine[24]);
    e <<= 16;
    f = (unsigned int) (headerCeVine[25]);
    f <<= 24;
    y += d + e + f;

    unsigned int dimensiuneTablou = x * y;
    unsigned int *tablouLiniarizat = liniarizareTablou(caleImagineInitiala);

    unsigned int  j = 0, k = 1;
    double sumaRed = 0, sumaGreen = 0, sumaBlue = 0 , fBaraSus = (double)dimensiuneTablou/256;
    unsigned char byteRed = 0, byteGreen = 0, byteBlue = 0;

    // blue
    for( i = 0; i <= 255; i++)
    {
        unsigned int nr = 0;
        for( k = 1; k <= dimensiuneTablou; k++)
        {
            byteBlue = tablouLiniarizat[k];
            if(byteBlue == i)
                nr ++;
        }
        sumaBlue = sumaBlue + (( nr - fBaraSus ) * ( nr - fBaraSus ) ) / fBaraSus;
    }

    // green
    for( i = 0; i <= 255; i++)
    {
        unsigned int nr = 0;
        for( k = 1; k <= dimensiuneTablou; k++)
        {
            byteGreen = tablouLiniarizat[k] >>8;
            if(byteGreen == i)
                nr ++;
        }
        sumaGreen = sumaGreen + (( nr - fBaraSus ) * ( nr - fBaraSus ) ) / fBaraSus;
    }

    // red
    for( i = 0; i <= 255; i++)
    {
        unsigned int nr = 0;
        for( k = 1; k <= dimensiuneTablou; k++)
        {
            byteRed = tablouLiniarizat[k] >>16;
            if(byteRed == i)
                nr ++;
        }
        sumaRed = sumaRed + (( nr - fBaraSus ) * ( nr - fBaraSus ) ) / fBaraSus;
    }

    printf(" albastru %0.2f \n verde %0.2f \n rosu %0.2f", sumaBlue, sumaGreen, sumaRed);
}

/**
 * Returneaza un tablou inversat
 * @param tabloulLiniarizatCeVine
 * @param caleImagineInitiala
 * @return tablou
 */
unsigned int *inversare(unsigned int *tabloulLiniarizatCeVine, char *caleImagineInitiala)
{
    unsigned char *headerCeVine = obtineHeader(caleImagineInitiala);

    unsigned int x = headerCeVine[18];
    unsigned int a, b, c, i;

    a = (unsigned int) (headerCeVine[19]);
    a <<= 8;
    b = (unsigned int) (headerCeVine[20]);
    b <<= 16;
    c = (unsigned int) (headerCeVine[21]);
    c <<= 24;
    x += a + b + c;

    unsigned int y = headerCeVine[22];
    unsigned int d, e, f;

    d = (unsigned int) (headerCeVine[23]);
    d <<= 8;
    e = (unsigned int) (headerCeVine[24]);
    e <<= 16;
    f = (unsigned int) (headerCeVine[25]);
    f <<= 24;
    y += d + e + f;

    unsigned int inaltime = y;
    unsigned int latime = x;

    unsigned int aux = 0, j;

    for( i = 1; i<= inaltime/2; i++)
        for(j = 1; j<= latime; j++)
        {
            aux = tabloulLiniarizatCeVine[i * latime + j];
            tabloulLiniarizatCeVine[i * latime + j] = tabloulLiniarizatCeVine[(inaltime - i + 1) * latime + j];
            tabloulLiniarizatCeVine[(inaltime - i + 1) * latime + j] = aux;
        }

    return tabloulLiniarizatCeVine;
}

/**
 * Deinverseaza elementele unui tablou
 * @param caleImagineInversata
 * @param tablouInversatLiniarizat
 * @return tablou
 */
unsigned int *deInversare(char *caleImagineInversata, unsigned int *tablouInversatLiniarizat)
{
    unsigned char *headerCeVine = obtineHeader(caleImagineInversata);

    unsigned int x = headerCeVine[18];
    unsigned int a, b, c, i;

    a = (unsigned int) (headerCeVine[19]);
    a <<= 8;
    b = (unsigned int) (headerCeVine[20]);
    b <<= 16;
    c = (unsigned int) (headerCeVine[21]);
    c <<= 24;
    x += a + b + c;

    unsigned int y = headerCeVine[22];
    unsigned int d, e, f;

    d = (unsigned int) (headerCeVine[23]);
    d <<= 8;
    e = (unsigned int) (headerCeVine[24]);
    e <<= 16;
    f = (unsigned int) (headerCeVine[25]);
    f <<= 24;
    y += d + e + f;

    unsigned int inaltime = y;
    unsigned int latime = x;

    unsigned int aux   = 0, j;

    for( i = 1; i<= inaltime/2; i++)
        for(j = 1; j<= latime; j++)
        {
            aux = tablouInversatLiniarizat[i * latime + j];
            tablouInversatLiniarizat[i * latime + j] = tablouInversatLiniarizat[(inaltime - i + 1) * latime + j];
            tablouInversatLiniarizat[(inaltime - i + 1) * latime + j] = aux;

        }

    return tablouInversatLiniarizat;
}


int main()
{
    //---------------------------------------------Criptare, decriptare si testul chi patrat----------------------------

    char pathImage1[255];
    char pathImage2[255];
    char pathImageInit[255];
    char secretKeyTxt[255];

    printf("Calea imaginii 1:");
    scanf("%s ", pathImage1);

    printf("Calea imaginii 2:");
    scanf("%s ", pathImage2);

    printf("Calea cheii:");
    scanf("%s ", secretKeyTxt);

    criptareImagine(pathImage1, pathImage2, secretKeyTxt);

    printf("Calea imaginii initiale:");
    scanf("%s ", pathImageInit);

    printf("Calea imaginii criptate:");
    scanf("%s ", pathImage1);

    printf("Calea imaginii decriptate:");
    scanf("%s ", pathImage2);

    printf("Calea cheii:");
    scanf("%s ", secretKeyTxt);

    testulChiPatrat(pathImageInit);
    testulChiPatrat(pathImage1);

    decriptareImagine( pathImageInit, pathImage1, pathImage2, secretKeyTxt);

//---------------------------------------------Criptare, decriptare si testul chi patrat----------------------------

    return 0;
}
