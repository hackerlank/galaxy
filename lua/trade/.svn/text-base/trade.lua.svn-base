module("trade", package.seeall)
local taskList = require("lua.trade.task")

function AcceptTask(playerlevel)

    for k,v in ipairs(taskList) do
        if playerlevel >= v.low and playerlevel <= v.up then
            mReturn(v.start, v.complete)
            return true
        end
    end

    return false

end

function completeTask(lv, reward, pass, times)

    local xmjy = lv * 100
    --local xmgx = reward * 0.1
    local jinbi = 0

    if lv < 61 then
      jinbi = 40000
    elseif lv < 81 then
      jinbi = 60000
    else
      jinbi = 90000
    end

	if(reward > pass) then
		jinbi = jinbi + (reward - pass) * 0.15
	end

    if(times > 2) then
        xmjy = xmjy * 0.3
        --xmgx *= 0.3
        jinbi = jinbi * 0.3
    end

    mReturn(xmjy, jinbi)
    return true
end
