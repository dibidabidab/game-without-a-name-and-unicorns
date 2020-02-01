
### [Play game in browser](https://hilkojj.github.io/dibidabidab/game.html)

## Usage

Clone this repository.

run `python3 initialize_project.py` to download dependencies.

### Compile for Desktop

`cd desktop`

`cmake .` (only the first time, and everytime you add new files)

`make`

`cd ..`

`./desktop/out/game`


### Compile for HTML/Web

**NOTE**: [install Emscripten first](https://emscripten.org/docs/getting_started/downloads.html)

`cd html`

`emconfigure cmake .` (only the first time, and everytime you add new files)

`make`

`emrun out/game.html`

### Configure GitHub build & deploy automation

This repository contains a Github Workflow which automates the building and deploying of the HTML game to GitHub Pages.

This workflow can be found in `.github/workflows/html_build.yml` and in the Actions tab on GitHub.

This workflow will require a secret access token in order to deploy your game to GitHub pages.

#### Steps to add the secret access token:

- Go to your account settings on GitHub
- Go to 'Developer settings' -> 'Personal access tokens'
- Click 'Generate new token'
- Create a token with the 'repo' scope
- Copy the access token
- Go to 'Secrets' in your repository's settings
- Add a new secret with the name 'ACCESS_TOKEN' and paste the token in the text field
- Trigger the workflow (by commiting something for example) and the game should appear on `https://*your-username*.github.io/*your-repo-name*/game.html`

