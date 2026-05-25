#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_DATA 100
#define K 3
#define ATRIBUT 3
#define MAX_ITER 100

typedef struct
{
    char nama[100];
    // Data asli
    double organisasi;
    double belajar;
    double hobi;
    // Data normalisasi
    double normOrganisasi;
    double normBelajar;
    double normHobi;

    int cluster;
} Mahasiswa;

// Membersihkan data angka
double cleanNumber(char str[])
{
    double a, b;
    // Jika format seperti 4-5
    if (sscanf(str, "%lf-%lf", &a, &b) == 2)
    {
        return (a + b) / 2.0;
    }
    // Ambil angka biasa
    sscanf(str, "%lf", &a);
    return a;
}

// Menghitung Euclidean Distance
double euclidean(double data[], double centroid[])
{
    return sqrt(
        pow(data[0] - centroid[0], 2) +
        pow(data[1] - centroid[1], 2) +
        pow(data[2] - centroid[2], 2));
}

int main()
{
    FILE *fp = fopen("data_kmeans_mahasiswa.csv", "r");

    if (fp == NULL)
    {
        printf("File tidak ditemukan!\n");
        return 1;
    }
    Mahasiswa data[MAX_DATA];

    char line[1000];
    int jumlahData = 0;

    // Skip header CSV
    fgets(line, sizeof(line), fp);
    // Membaca data CSV
    while (fgets(line, sizeof(line), fp))
    {
        char *token;
        token = strtok(line, ","); // Timestamp
        token = strtok(NULL, ","); // Email
        token = strtok(NULL, ","); // Nama

        if (token != NULL)
            strcpy(data[jumlahData].nama, token);
        token = strtok(NULL, ","); // Universitas
        token = strtok(NULL, ","); // Angkatan
        token = strtok(NULL, ","); // Organisasi

        if (token != NULL)
            data[jumlahData].organisasi =
                cleanNumber(token);
        // Belajar
        token = strtok(NULL, ",");
        if (token != NULL)
            data[jumlahData].belajar =
                cleanNumber(token);
        // Hobi
        token = strtok(NULL, ",");
        if (token != NULL)
            data[jumlahData].hobi =
                cleanNumber(token);

        data[jumlahData].cluster = -1;
        jumlahData++;
    }

    fclose(fp);
    // Menentukan min dan max
    double minOrg = data[0].organisasi;
    double maxOrg = data[0].organisasi;

    double minBel = data[0].belajar;
    double maxBel = data[0].belajar;

    double minHobi = data[0].hobi;
    double maxHobi = data[0].hobi;

    for (int i = 0; i < jumlahData; i++)
    {
        if (data[i].organisasi < minOrg)
            minOrg = data[i].organisasi;
        if (data[i].organisasi > maxOrg)
            maxOrg = data[i].organisasi;
        if (data[i].belajar < minBel)
            minBel = data[i].belajar;
        if (data[i].belajar > maxBel)
            maxBel = data[i].belajar;
        if (data[i].hobi < minHobi)
            minHobi = data[i].hobi;
        if (data[i].hobi > maxHobi)
            maxHobi = data[i].hobi;
    }

    // Min-Max Normalization
    for (int i = 0; i < jumlahData; i++)
    {
        if (maxOrg - minOrg == 0)
            data[i].normOrganisasi = 0;
        else
            data[i].normOrganisasi =
                (data[i].organisasi - minOrg) /
                (maxOrg - minOrg);

        if (maxBel - minBel == 0)
            data[i].normBelajar = 0;
        else
            data[i].normBelajar =
                (data[i].belajar - minBel) /
                (maxBel - minBel);

        if (maxHobi - minHobi == 0)
            data[i].normHobi = 0;
        else
            data[i].normHobi =
                (data[i].hobi - minHobi) /
                (maxHobi - minHobi);
    }
    srand(time(NULL));

    // CENTROID AWAL
    double centroid[K][ATRIBUT] = {

        {data[19].normOrganisasi,
         data[19].normBelajar,
         data[19].normHobi},

        {data[14].normOrganisasi,
         data[14].normBelajar,
         data[14].normHobi},

        {data[20].normOrganisasi,
         data[20].normBelajar,
         data[20].normHobi}

    };
    int changed;

    // Proses iterasi K-Means
    for (int iter = 0; iter < MAX_ITER; iter++)
    {
        changed = 0;
        // Menentukan cluster terdekat
        for (int i = 0; i < jumlahData; i++)
        {
            double nilaiData[ATRIBUT] = {
                data[i].normOrganisasi,
                data[i].normBelajar,
                data[i].normHobi};
            double minDist = 999999;

            int clusterBaru = -1;

            for (int j = 0; j < K; j++)
            {
                double dist =
                    euclidean(nilaiData,
                              centroid[j]);
                if (dist < minDist)
                {
                    minDist = dist;
                    clusterBaru = j;
                }
            }
            if (data[i].cluster != clusterBaru)
            {
                changed = 1;
                data[i].cluster = clusterBaru;
            }
        }

        // Update centroid
        double total[K][ATRIBUT] = {0};
        int jumlah[K] = {0};

        for (int i = 0; i < jumlahData; i++)
        {
            int c = data[i].cluster;
            total[c][0] +=
                data[i].normOrganisasi;
            total[c][1] +=
                data[i].normBelajar;
            total[c][2] +=
                data[i].normHobi;
            jumlah[c]++;
        }

        for (int i = 0; i < K; i++)
        {
            if (jumlah[i] != 0)
            {
                centroid[i][0] =
                    total[i][0] / jumlah[i];
                centroid[i][1] =
                    total[i][1] / jumlah[i];
                centroid[i][2] =
                    total[i][2] / jumlah[i];
            }
        }

        // Berhenti jika cluster sudah stabil
        if (!changed)
        {
            break;
        }
    }

    // Menampilkan centroid akhir
    printf("===== CENTROID AKHIR =====\n\n");

    for (int i = 0; i < K; i++)
    {
        printf("Cluster %d\n", i + 1);

        printf("Organisasi : %.2lf\n", centroid[i][0]);
        printf("Belajar    : %.2lf\n", centroid[i][1]);
        printf("Hobi       : %.2lf\n\n", centroid[i][2]);
    }
    // Menampilkan hasil clustering
    printf("\n====== HASIL CLUSTERING ======\n\n");

    for (int i = 0; i < K; i++)
    {
        printf("========== CLUSTER %d ==========\n\n",
               i + 1);
        for (int j = 0; j < jumlahData; j++)
        {
            if (data[j].cluster == i)
            {
                printf("%-25s | Org: %-5.1lf | Belajar: %-5.1lf | Hobi: %-5.1lf\n",
                       data[j].nama,
                       data[j].organisasi,
                       data[j].belajar,
                       data[j].hobi);
            }
        }
        printf("\n");
    }
    return 0;
}
