-- Your Wifi connection data
local SSID = "spot_name"
local SSID_PASSWORD = "defaultpass"

print("set up wifi mode")
wifi.setmode(wifi.STATION)
wifi.sta.config(SSID,SSID_PASSWORD)
wifi.sta.connect()
tmr.alarm(1,1000,1,function()
  if wifi.sta.getip() == nil then
    print("IP unavailable, waiting")
  else
    tmr.stop(1)
    print("IP:"..wifi.sta.getip())
  dofile("client.lua")
  end
end)