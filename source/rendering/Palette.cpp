
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
