#ifndef LOADTEXTURES_H_INCLUDED
#define LOADTEXTURES_H_INCLUDED

map<string, Texture> textures;
map<string, Texture> playerIcons;

void loadTextures()
{
    vector<pair<string, string>> texs;
    texs.push_back( make_pair("Start", "textures/Levels/Start.png") );
    texs.push_back( make_pair("Empty", "textures/Levels/Empty.png") );
    texs.push_back( make_pair("Wall", "textures/Levels/Wall.png") );
    texs.push_back( make_pair("Item", "textures/Levels/Item.png") );
    texs.push_back( make_pair("Moveable", "textures/Levels/Moveable.png") );
    texs.push_back( make_pair("TunnelUD" , "textures/Levels/UpDown.png"));
    texs.push_back( make_pair("TunnelLR" , "textures/Levels/LeftRight.png"));
    texs.push_back( make_pair("TunnelAll" , "textures/Levels/AllSides.png"));
    texs.push_back( make_pair("Rotating1" , "textures/Levels/Rotating1.png"));
    texs.push_back( make_pair("Rotating2" , "textures/Levels/Rotating2.png"));

    texs.push_back( make_pair("Locked", "texture/Levels/Locked.png"));

    for (unsigned int i=0;i<texs.size();i++)
    {
        Texture add;
        add.loadFromFile(texs[i].second);
        add.setSmooth(true);
        textures[texs[i].first] = add;
    }

    texs.clear();

    texs.push_back(make_pair("Icon1", "textures/PlayerIcons/Player1.png"));
    texs.push_back(make_pair("Icon2", "textures/PlayerIcons/Player2.png"));

    for (unsigned int i=0;i<texs.size();i++)
    {
        Texture add;
        add.loadFromFile(texs[i].second);
        //add.setSmooth(true);
        playerIcons[texs[i].first] = add;
    }
}

void cleanTextures()
{
    for (map<string, Texture>::const_iterator it = textures.begin(); it != textures.end(); it++)
    {
        delete &it->second;
    }
    for (map<string, Texture>::const_iterator it = playerIcons.begin(); it != playerIcons.end(); it++)
    {
        delete &it->second;
    }
}

#endif // LOADTEXTURES_H_INCLUDED
