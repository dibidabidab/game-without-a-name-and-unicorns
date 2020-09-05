
function fastStartup()
    startupArgs = getGameStartupArgs()


    saveGamePath = startupArgs["--single-player"] or "saves/default_save.dibdab"
    startSinglePlayerSession(saveGamePath)

    username = startupArgs["--username"] or "poopoo"
    joinSession(username, function(declineReason)

        tryCloseGame()
        error("couldn't join session: "..declineReason)
    end)


    levelPath = startupArgs["--level"] or "assets/levels/default_level.lvl"
    loadOrCreateLevel(levelPath)
end
