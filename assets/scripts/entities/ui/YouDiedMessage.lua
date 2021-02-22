

function create(mess)

    setName(mess, "you_died")

    setComponents(mess, {
        UIElement {
            absolutePositioning = true,
            absoluteHorizontalAlign = 0,
            absoluteVerticalAlign = 1
        },
        UIContainer {
            nineSliceSprite = "sprites/ui/you_died_plane",
            fixedHeight = 35,
            centerAlign = true,
            fillRemainingParentWidth = true,
            padding = ivec2(0, 10)
        }
    })

    component.UIContainer.animate(mess, "fixedHeight", 76, .2, "pow2Out", function ()

        local text = createChild(mess)
        applyTemplate(text, "Text", {
            text = "You Died!",
            letterSpacing = 30,
            wavingFrequency = .04,
            wavingSpeed = 20,
            wavingAmplitude = 2,
            waving = true
        })
        component.TextView.animate(text, "letterSpacing", 1, 1., "circleOut")

        if saveGame.timesDied > 1 then

            local nice = createChild(mess)
            setName(nice, "counter")
            setComponents(nice, {
                UIElement {
                    startOnNewLine = true,

                    margin = ivec2(15, 7)
                },
                UIContainer {

                    fixedWidth = 160,
                    fixedHeight = 27
                }
            })


            applyTemplate(createChild(nice), "Text", {
                text = "Again.. For the ",
                color = colors.brick
            })

            local th = "th"
            if saveGame.timesDied == 2 then
                th = "nd"
            elseif saveGame.timesDied == 3 then
                th = "rd"
            elseif saveGame.timesDied == 69 then
                th = "th ;-)"
            end

            applyTemplate(createChild(nice), "Text", {
                text = math.floor(saveGame.timesDied)..th,
                color = colors.rainbow_red
            })
            applyTemplate(createChild(nice), "Text", {
                text = " time..",
                color = colors.brick
            })
        end
    end)

end

