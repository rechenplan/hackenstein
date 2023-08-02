function map_load(map)
  map_set(map, 8, 8, 1)
end

function collision(player, object)
end

function key_down(key)
  broadcast(string.char(key))
end

function key_up(key)
end

function player_init(player)
end

function player_cleanup(player)
end

function receive(message)
end
