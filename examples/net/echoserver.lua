-- Echo server example
-- Using non blocking sockets and asynchronous tasks
--
-- First run this program in a console : luart echoserver.lua
-- Then start the GUI client : wluart echoclient.lua


local net = require "net"

Server = Object {}

-- Server constructor (socket initialization)
function Server:constructor()
    self.socket = net.Socket("localhost", 5000)
    self.socket.blocking = false
    self.connected = false
    self.clients = {}
end

-- Make the server ready to receive incoming connections
function Server:connect()
    if self.socket:bind() then
        print("Echo server running...\n")
    else
        error("Could not create server connection : "..net.error)
    end
end

function Server:sendall(msg)
    print(msg)
    for client in each(self.clients) do
        await(client:send(msg))
    end
end

-- Method called each time a new client connects
function Server:echo(client)
    print("New client connected from "..client.ip)
    local id = #self.clients+1
    self.clients[id] = client

    -- Asynchronous welcome message sending
    await(client:send("Welcome to Echo Server !\n"))
    print("Welcome msg sent")

    -- Wait forever for new messages from the client
    while true do
        -- wait for a message
        local msg = await(client:recv())

        -- An error occured or the client has disconnected
        if not msg then
            self.clients[id] = nil
            self:sendall(client.ip.." connection lost")
            return
        else
            -- Send the received message to all connected clients
            self:sendall("["..client.ip.."] "..msg)
        end
    end
end

-- Server main loop
async(function(self)
    if not self.connected then
        self:connect()
    end
    while true do
        -- Wait for a new incoming connection..
        local client = await(self.socket:accept())

        -- ...then call the echo() method to manage this new client
        self:echo(client)
        sleep()
    end
end, Server())

waitall()