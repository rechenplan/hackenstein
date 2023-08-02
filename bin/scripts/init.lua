function map_load(map)
end

function collision(player, object)
end

function key_down(key)
  print("You sent '" .. string.char(key) .."'")
  broadcast(string.char(key))
end

function key_up(key)
end

function update(player, elapsed_time)
end

function player_init(player)
end

function player_cleanup(player)
end

function receive(message)
  print("You received '" .. message .. "'")
end
