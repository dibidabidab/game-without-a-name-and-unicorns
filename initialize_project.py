import subprocess
import urllib.request
import zipfile
import os

GU_REPO = "https://github.com/hilkojj/cpp-game-utils.git"

def command(str):
    subprocess.run(str.split(" "))

def zip(url, dst):
    urllib.request.urlretrieve(url, dst + ".zip")

    zip_ref = zipfile.ZipFile(dst + ".zip", 'r')
    zip_ref.extractall(dst)
    zip_ref.close()

    os.remove(dst + ".zip")

def file(url, dst):
    urllib.request.urlretrieve(url, dst)


# command("git init")
command(f"git submodule add {GU_REPO} gu")
command("git submodule update --init --recursive")

command("python -m pip install --user glad")
command("python -m glad --generator=c --extensions=GL_EXT_texture_compression_s3tc --out-path=gu/external/glad")

zip("https://github.com/glfw/glfw/releases/download/3.2.1/glfw-3.2.1.zip", "gu/external")
zip("https://github.com/g-truc/glm/releases/download/0.9.9.5/glm-0.9.9.5.zip", "gu/external/glm-0.9.9.5")
file("https://github.com/nlohmann/json/releases/download/v3.6.1/json.hpp", "gu/external/json.hpp")
