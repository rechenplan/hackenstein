set_map(8, 8, 1)
set_map(9, 9, 1)

has_ball = true
connected = false

function on_tick(elapsed_time)
end

function on_receive(str)

  local ball = load("return " .. str)()
  print(serialize_table(ball))
  spawn(ball, function() has_ball = true end)

end

function serialize_table(table)

  local table_str = "{"
  for k, v in pairs(table) do
    table_str = table_str .. tostring(k) .. "=" .. tostring(v) .. ","
  end
  table_str = table_str .. "}"
  return table_str

end

function on_key_down(key)

  if key == 32 and has_ball then

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
      bounces = -1,
      bouncy = 1,
      collision_radius = 0.5,
      explosion_size = 0,
    }

    broadcast(serialize_table(ball))
    spawn(ball, function() has_ball = true end)
    has_ball = false

  end

  if string.char(key) == "c" and not connected then
    connected = true
    connect("10.0.0.251", 26001)
  end

end

function on_key_up(key)
end
