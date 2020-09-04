
defaultArgs({
    text = "Lorem Dibidabidab!",
    mapColorFrom = 7,
    color = 7,
    lineSpacing = 3,
    letterSpacing = 1,
    font = "sprites/ui/default_font",
    waving = false,
    wavingFrequency = 3,
    wavingSpeed = 5,
    wavingAmplitude = 4
})

function create(text, args)

    setComponents(text, {
        TextView = {
            text = args.text,
            mapColorFrom = args.mapColorFrom,
            mapColorTo = args.color,
            lineSpacing = args.lineSpacing,
            letterSpacing = args.letterSpacing,
            fontSprite = args.font,
            waving = args.waving,
            wavingFrequency = args.wavingFrequency,
            wavingSpeed = args.wavingSpeed,
            wavingAmplitude = args.wavingAmplitude
        },
        UIElement = {}
    })
end
