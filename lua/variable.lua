Gvar =
{
[1] = nil, [2] = nil, [3] = nil,
[4] = nil, [5] = nil, [6] = nil,
[7] = nil, [8] = nil, [9] = nil,
[10] = nil, [11] = nil, [12] = nil
}

function mReturn(...)
    for i = 1, arg.n do
        if(arg[i] ~= nil) then
            Gvar[i] = arg[i]
        end
    end
end

mGet = function(index)
    return Gvar[index]
end
