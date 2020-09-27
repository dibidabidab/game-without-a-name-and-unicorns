
defaultArgs({
    text = "Lorem Dibidabidab!",
    mapColorFrom = 7,
    color = 7,
    lineSpacing = 3,
    letterSpacing = 1,
    font = "sprites/ui/default_font",
    waving = false,
    wavingFrequency = .09,
    wavingSpeed = 10,
    wavingAmplitude = 3
})

function create(text, args)

    setComponents(text, {
        TextView {
            text = args.text,
            mapColorFrom = args.mapColorFrom,
            mapColorTo = args.color,
            letterSpacing = args.letterSpacing,
            fontSprite = args.font,
            waving = args.waving,
            wavingFrequency = args.wavingFrequency,
            wavingSpeed = args.wavingSpeed,
            wavingAmplitude = args.wavingAmplitude
        },
        UIElement {
            lineSpacing = args.lineSpacing
        }
    })
end
