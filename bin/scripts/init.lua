set_map(8, 8, 1)
set_map(9, 9, 1)

has_balls = 1
connected = false
console = { "hackenstein 0.1", "press ` to begin entering commands", "", "> " }
hacking = false

function on_tick(elapsed_time)
  cls()
  for i = 1, 4 do
    printxy(console[i], 0, i * 8)
  end
end

function on_connect(id)
  print("connection " .. tostring(id))
end

function on_disconnect(id)
  print("disconnect " .. tostring(id))
end

function on_receive(id, str)

  local ball = load("return " .. str)()
  print(serialize_table(ball))
  spawn(ball, function() has_balls = has_balls + 1 end)

end

function on_key_down(key)

  if key == 32 then

    local pos = get_position()
    local rot = get_rotation()

    local ball = {
      position_x = pos.x + 2 * math.cos(rot),
      position_y = pos.y + 2 * math.sin(rot),
      position_z = 0.35,
      velocity_x = 5 * math.cos(rot),
      velocity_y = 5 * math.sin(rot),
      velocity_z = 1,
      rotation = rot,
      friction = 1,
      height = 0.25,
      width = 0.25,
      color = 0x10a010,
      bounces = 0,
      bouncy = 1,
      collision_radius = 0.5,
      explosion_size = 0,
    }

    broadcast(serialize_table(ball))
    spawn(ball, function() print("Ouch!") end)

  end

  if string.char(key) == "c" and not connected then
    connect("10.0.0.251", 26001)
  end

end

function on_key_up(key)
end

function serialize_table(table)

  local table_str = "{"
  for k, v in pairs(table) do
    table_str = table_str .. tostring(k) .. "=" .. tostring(v) .. ","
  end
  table_str = table_str .. "}"
  return table_str

end

