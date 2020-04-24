
#include "Palette.h"
#include <files/file.h>
#include <utils/string.h>

Palette::Palette(const char *path)
{
    auto str = splitString(File::readString(path), "#\n").at(1);

    auto lines = splitString(str, "\n");

    for (auto &l : lines)
    {
        if (l.empty())
            continue;

        vec3 color;
        int i = 0;

        auto parts = splitString(l, " ");
        for (auto &p : parts)
        {
            if (p.empty())
                continue;

            auto numberAndName = splitString(p, "\t");

            color[i] = atoi(numberAndName.at(0).c_str()) / 255.;

            if (i == 2)
                colors.emplace_back(numberAndName.at(1), color);
            i++;
        }
    }
}

void Palette::save(const char *path)
{
    std::string str = "GIMP Palette\n#\n";

    for (auto &pair : colors)
    {
        for (int i = 0; i < 3; i++)
        {
            auto numberString = std::to_string(int(pair.second[i] * 255));
            str += std::string((i == 0 ? 3 : 4) - numberString.size(), ' ');
            str += numberString;
        }
        str += '\t';
        str += pair.first;
        str += '\n';
    }
    File::writeBinary(path, str);
}

Palettes3D::Palettes3D()
{
    effects.push_back({
        "default",
        {
            asset<Palette>("palettes/default0"),
            asset<Palette>("palettes/default1")
        }
    });
    effects.push_back({
        "test",
        {
            asset<Palette>("palettes/default1"),
            asset<Palette>("palettes/default0")
        }
    });
}

int Palettes3D::effectIndex(const std::string &name) const
{
    int i = 0;
    for (auto &effect : effects)
    {
        if (effect.name == name)
            break;
        i++;
    }
    return i;
}

void Palettes3D::create3DTexture()
{
    int nrOfColors = effects.at(0).lightLevels[0]->colors.size();

    // for some reason I don't know yet nrOfColors must be divisible by 4:
    nrOfColors = (nrOfColors / 4) * 4;
    nrOfColors += 4;

    int
            width = nrOfColors,
            height = LIGHT_LEVELS,
            depth = effects.size();

    std::vector<GLubyte> data(width * height * depth * 3, 0);

    int i = 0;
    for (auto &effect : effects)
    {
        for (auto &p : effect.lightLevels)
        {
            for (int c = 0; c < nrOfColors; c++)
            {
                vec3 color = (p->colors.size() <= c) ? vec3(1, 0, 1) : p->colors.at(c).second;

                for (int j = 0; j < 3; j++)
                    data[i++] = uint8(color[j] * 255);
            }
        }
    }
    assert(i == data.size());
    assert(i == (width * height * depth * 3));
    texture3D = TextureArray::fromByteData(&data[0], GL_RGB, width, height, depth, GL_NEAREST, GL_NEAREST);
}

SharedTexArray Palettes3D::get3DTexture()
{
    bool reloaded = false;

    for (auto &effect : effects)
        for (auto &p : effect.lightLevels)
            if (p.hasReloaded())
                reloaded = true;

    if (changedByEditor || !texture3D || reloaded)
        create3DTexture();
    changedByEditor = false;

    return texture3D;
}
