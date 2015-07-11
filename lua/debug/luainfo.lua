module("sys",package.seeall)
function ShowLuaMemInfo()
    --每半个小时清除一次
    collectgarbage("collect")
    CPP.li("lua memory used : "..collectgarbage("count"))
end
