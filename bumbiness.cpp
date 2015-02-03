#include <QtCore/QCoreApplication>
#include <cstdio>
#include <cstdlib>
#include <QTime>
#include "bumbiness.h"

static wrpformat wrp;

Bumbiness::Bumbiness(int argc, char *argv[])
{
    DoTime();

    Open_Files(argc, argv);

    Signatures();

    Elevations();

    Textures();

    Write_Elevations();

    Write_Textures();

    Objects();
}

/*

    Creates random bumbiness level for the terrain.

    returns short bumbiness value.

*/
int Bumbiness::RandBumby(short ElevationBumbiness)
{
    // here we randomize the bumbiness.
    short MyRandomBumbiness = 0;

    //(359*rand()/(RAND_MAX+1.0))
    MyRandomBumbiness = (qrand() % ElevationBumbiness);

    //printf ("Yay new RandomBum:%u\n", MyRandomBumbiness);

    // return the new value
    return MyRandomBumbiness;
};


void Bumbiness::DoTime()
{
    // initialize random seed:
    QTime timmy(0, 0, 0);
    qsrand(timmy.secsTo(QTime::currentTime()));
}



void Bumbiness::Open_Files(int argc, char *argv[])
{
    // check that we have enough params
    if (argc < 4)
    {
        printf("Sorry not enough parameters!\n\n\tBumbiness Source.Wrp Destination.Wrp <Bumbiness Number>\n\n");
        exit (1);
    }

    printf ("Source WRP: %s\nDestination WRP: %s\n", argv[1], argv[2]);

    map = fopen (argv[1], "rb");
    if (!map)
    {
        printf ("error in %s\n", argv[1]);
        exit (1);
    }

    printf ("Opened %s\n", argv[1]);

    output = fopen (argv[2], "wb");
    if (!output)
    {
        printf ("error at %s file\n", argv[2]);
        exit (1);
    }

    printf ("Opened %s\n", argv[2]);

    // grab the bumbiness from cmd line
    ElevationBumbiness = atoi(argv[3]);
    printf("Bumbiness: %i\n", ElevationBumbiness);
    // add 1 as if user places like 2m then it only returns 0 and 1 meters (random from 0 to 2..) :)
    ElevationBumbiness += 1;
}


void Bumbiness::Signatures()
{
    fread (sig,4,1,map);
    sig[4] = 0;
    fwrite (sig, 4, 1, output);

    fread(&MapSize,4,1,map);
    fwrite(&MapSize,4,1,output);

    fread(&MapSize,4,1,map);
    fwrite(&MapSize,4,1,output);

    if (MapSize > 4096)
    {
        printf("MapSize %i is too large! exiting!\n",MapSize);
        exit(1);
    };

    printf("Signature: %s\nMapSize: %i\nReading elevations...",sig,MapSize);
}


void Bumbiness::Elevations()
{
    // read elevations
    int x = 0, z = 0;
    for (int zx = 0; zx < MapSize*MapSize; zx++)
    {
        fread(&wrp[x][z].Elevation,sizeof(wrp[x][z].Elevation),1,map);

        if (LowElev > Elevation) LowElev = Elevation;
        if (HighElev < Elevation) HighElev = Elevation;

        x++;
        if (x == MapSize)
        {
            z++; x = 0;
        }
        if (z == MapSize)
        {
            z = 0;
        }
    }
}


void Bumbiness::Textures()
{
    printf(" Done\nReading Textures...");

    // read textures IDs
    int x = 0, z = 0;
    TexIndex = 0;

    for (int tx = 0; tx < MapSize*MapSize; tx++)
    {
        wrp[x][z].TexIndex = 0;
        fread(&wrp[x][z].TexIndex,sizeof(wrp[x][z].TexIndex),1,map);

        x++;
        if (x == MapSize)
        {
            z++;
            x = 0;
        }
        if (z == MapSize)
        {
            z = 0;
        }
    }
}


void Bumbiness::Write_Elevations()
{
    printf(" Done\nBumbing and writing elevations...");

    // write the elevations
    int x = 0, z = 0;
    for (int xx = 0; xx < MapSize*MapSize; xx++)
    {
        /*
        then we write the elevations again with the bumb.
        it has to be the weird x 22 as the terrain elevations are, weird ;)
        */
        wrp[x][z].Elevation += (RandBumby(ElevationBumbiness) * 22);

        fwrite(&wrp[x][z].Elevation,sizeof(wrp[x][z].Elevation),1,output);
        x++;
        if (x == MapSize)
        {
            z++;
            x = 0;
        }
        if (z == MapSize)
        {
            z = 0;
        }
    }
}


void Bumbiness::Write_Textures()
{
    printf(" Done\nWriting texture index...");

    // write the texture indexes
    int x = 0, z = 0;
    for (int cr = 0; cr < MapSize*MapSize; cr++)
    {
        fwrite(&wrp[x][z].TexIndex,sizeof(wrp[x][z].TexIndex),1,output);
        x++;
        if (x == MapSize)
        {
            z++;
            x = 0;
        }
        if (z == MapSize)
        {
            z = 0;
        }
    }

    printf(" Done\nReading Texture names...");

    //textures 32 char length and total of 512
    for (int ix = 0; ix < 512; ix++)
    {
        sig[0] = 0;
        fread(sig,32,1,map);
        fwrite(sig,32,1,output);
    }
}


void Bumbiness::Objects()
{
    printf(" Done\nReading 3dObjects...");

    char dObjName[76];
    float dDir,dDirX = 0,dDirZ = 0;
    long dObjIndex = 0;

    while (!feof(map))
    {
        dDir = 1;
        fread(&dDir,4,1,map);
        fwrite(&dDir,4,1,output);
        dDir = 0;
        fread(&dDir,4,1,map);
        fwrite(&dDir,4,1,output);
        fread(&dDir,4,1,map);
        fwrite(&dDir,4,1,output);
        fread(&dDir,4,1,map);
        fwrite(&dDir,4,1,output);
        dDir = 1;
        fread(&dDir,4,1,map);
        fwrite(&dDir,4,1,output);
        dDir = 0;
        fread(&dDir,4,1,map);
        fwrite(&dDir,4,1,output);
        fread(&dDir,4,1,map);
        fwrite(&dDir,4,1,output);
        fread(&dDir,4,1,map);
        fwrite(&dDir,4,1,output);
        dDir = 1;
        fread(&dDir,4,1,map);
        fwrite(&dDir,4,1,output);

        // X coord
        fread(&dDirX,4,1,map);
        fwrite(&dDirX,4,1,output);

        // here we do our magic for aling to ground
        fread(&dDir,4,1,map);

        // Z coord
        fread(&dDirZ,4,1,map);

        dObjIndex++;
        fread(&dObjIndex,4,1,map);
        // clear the objname variable
        for (int i = 0; i < 76; i++) dObjName[i] = 0;

        fread(dObjName,76,1,map);
        // start
        strlwr(dObjName);
        fwrite(&dDir,4,1,output);
        fwrite(&dDirZ,4,1,output);
        fwrite(&dObjIndex,4,1,output);

        fwrite(dObjName,sizeof(dObjName),1,output);
    };

    printf (" %ld Objects done\n", dObjIndex);
}


void Bumbiness::Close_Files()
{
    fclose(map);
    fclose(output);
}
