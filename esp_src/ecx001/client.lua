local http = require 'http'
local sensorjson = "{a:2}";

-- when '\r' is received.
uart.on("data", "\n",
  function(data)
        http.postContent("192.168.0.101", data, "application/json", function(data) 
        --local result = json.decode(data.content)
        --print("the post " .. result.this)
    end)
    if data=="reset\n" then
      --node.restart()
      --uart.on("data") -- unregister callback function
    end
end, 0)
