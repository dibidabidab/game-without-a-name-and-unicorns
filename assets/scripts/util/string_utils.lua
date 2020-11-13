
function camelCaseToSpaces(str)
    return str:gsub('(%S)(%u)', '%1 %2')
end
