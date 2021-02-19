
function addTo(entity, shortSounds)

    onEntityEvent(entity, "Attacked", function(attack)

        local se = createEntity()

        local ss = component.SoundSpeaker.getFor(se)

        local sounds = {}

        if component.Health.getFor(entity).currHealth == 0 then

            --if shortSounds then
            --    sounds = {"sounds/blood/med0", "sounds/blood/med1", "sounds/blood/med2"}
            --else
                sounds = {"sounds/blood/long"}
            --end

        elseif shortSounds then
            sounds = {"sounds/blood/short0", "sounds/blood/short1"}
        else
            sounds = {"sounds/blood/med0", "sounds/blood/med1", "sounds/blood/med2"}
        end

        ss.sound = sounds[math.random(1, #sounds)]

        ss.pitch = math.random() * .4 + .9
        ss.volume = math.random() * .2 + 2.5

        ss.looping = false

        component.DespawnAfter.getFor(se).time = 2
    end)


end
