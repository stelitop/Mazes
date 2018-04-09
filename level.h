#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

int amountOfLevels;

struct level
{
    char level[100][100];
    pair<double,double> starting_position = make_pair(0,0);
    pair<int,int> size;
    int itemAmount=0;

    void loadLvl(string name)
    {
        itemAmount = 0;
        string y,ret="";
        ifstream file2;
        file2.open(name);
        int l=0;
        while (getline(file2, y))
        {
            l++;
            if (l == 1)
            {
                int i=0;
                while (y[i]!=';')
                {
                    size.first = size.first*10+y[i]-'0';
                    i++;
                }
                i++;
                while (y[i]!=';')
                {
                    size.second = size.second*10+y[i]-'0';
                    i++;
                }
            }
            else ret += y;
        }

        for (int i=0;i<size.first;i++)
        {
            for (int j=0;j<size.second;j++)
            {
                level[i][j]=ret[i*size.second+j];
                if (level[i][j] == 'S')
                {
                    starting_position.first = i;
                    starting_position.second = j;
                }
                if (level[i][j] == 'A')
                    itemAmount++;
            }
        }
    }

}levels[1000], levelmap;

struct levelDoor
{
    int x;
    int y;
}levelDoors[1000];
int amountDoors;

void loadLevels()
{
    levelmap.loadLvl("levels/map.map");
    levelmap.itemAmount=-1000;
    ifstream file;
    string x;
    file.open("levels/names.nms");
    amountOfLevels=0;
    while (getline(file,x))
    {
        amountOfLevels++;
        string name="";
        int i=0;
        for (;x[i]!=';';i++) name+=x[i];

        name = "levels/"+name;

        levels[amountOfLevels].loadLvl(name);
    }
    ifstream locats;
    int counter=0;
    locats.open("levels/locations.map");
    while (getline(locats,x))
    {
        counter++;
        int i=0,num=0;
        for (;x[i]!=';';i++) num=num*10+x[i]-'0';
        levelDoors[counter].x = num;
        i++;
        num=0;
        for (;x[i]!=';';i++) num=num*10+x[i]-'0';
        levelDoors[counter].y = num;
    }
    amountDoors = counter;
}

void cleanLevels()
{
    for (int i=1;i<=amountOfLevels;i++)
        delete &levels[i];
}

#endif // LEVEL_H_INCLUDED
