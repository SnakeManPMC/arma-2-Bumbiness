#ifndef BUMBINESS_H
#define BUMBINESS_H

#include <QtCore/QCoreApplication>
#include <cstdio>
#include <cstdlib>

typedef struct
{
    short Elevation;
    short TexIndex;
} wrpformat[4096][4096];

class Bumbiness
{
public:
    Bumbiness(int argc, char *argv[]);
    int RandBumby(short ElevationBumbiness);
    void DoTime();
    void Open_Files(int argc, char *argv[]);
    void Signatures();
    void Elevations();
    void Textures();
    void Write_Elevations();
    void Write_Textures();
    void Objects();
    void Close_Files();

private:
    FILE *map;
    FILE *output;
    char sig[33];
    short Elevation, LowElev, HighElev, TexIndex;
    short ElevationBumbiness;
    int MapSize;
};

#endif // BUMBINESS_H
