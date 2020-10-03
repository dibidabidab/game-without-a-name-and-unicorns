
A game about unicorns and shooting arrows, made using my own [game engine](https://github.com/dibidabidab/dibidab-engine).

#### [Download for Linux](https://dibidabidab.github.io/dibidab-engine/game-linux.zip)
#### [Download for Windows](https://dibidabidab.github.io/dibidab-engine/game-windows.zip)
#### [Play game in browser*](https://dibidabidab.github.io/dibidab-engine/game.html)
<sup>*less peformance, prefer the linux or windows download instead.</sup>

[![Build & deploy to gh-pages](https://github.com/hilkojj/dibidabidab/workflows/Build%20&%20deploy%20to%20gh-pages/badge.svg)](https://github.com/hilkojj/dibidabidab/actions)
[![BCH compliance](https://bettercodehub.com/edge/badge/hilkojj/dibidabidab?branch=master)](https://bettercodehub.com/)

## Usage

Make sure you have [Git LFS](https://git-lfs.github.com/) installed, then clone this repo.

Do `git submodule update --init --recursive` to clone submodules


##### Note for windows:
After compiling, make sure you move `OpenAL32.dll` to the working directory (the game might not launch without). 

You can find it somewhere in `desktop/out/Release/bin/....` or in a similar directory (I hope Windows has a proper search function). 

### Open in CLion
- open `desktop/CMakeLists.txt`
- change the project root from `./desktop` to `./`
- in your debug configuration, set the working directory to `./` as well, otherwise the assets cannot be found

### Compile for Desktop

`cd desktop`

`cmake .` (or `cmake . -DCMAKE_BUILD_TYPE=Release`)

`make -j8` (or `cmake --build . -j8 --config Release`)

`cd ..`

`./desktop/out/game` (or `./desktop/out/Release/game.exe`)

### Compile for HTML/Web

**NOTE**: [install Emscripten first](https://emscripten.org/docs/getting_started/downloads.html)

`cd html`

`emconfigure cmake .` (only the first time, and everytime you add new files)

`make -j8`

`emrun out/game.html`

### Configure GitHub build & deploy automation

This repository contains a Github Workflow which automates the building and deploying of the game to GitHub Pages.
The workflow will generate the following files:
- `game.html` and asset files (directly playable in browser)
- `game-linux.zip`
- `game-windows.zip`

This workflow can be found in `.github/workflows/` and in the Actions tab on GitHub.

This workflow will require a secret access token in order to deploy the game to GitHub pages.

#### Steps to add the secret access token:

- Go to your account settings on GitHub
- Go to 'Developer settings' -> 'Personal access tokens'
- Click 'Generate new token'
- Create a token with the 'repo' scope
- Copy the access token
- Go to 'Secrets' in your repository's settings
- Add a new secret with the name 'ACCESS_TOKEN' and paste the token in the text field
- Trigger the workflow (by commiting something for example) and the game should appear on `https://*your-username*.github.io/*your-repo-name*/game.html`

