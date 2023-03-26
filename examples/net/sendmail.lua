local net = require "net"
local console = require "console"

console.clear()

console.writecolor("lightcyan","G")
console.writecolor("lightred","M")
console.writecolor("lightyellow","A")
console.writecolor("lightcyan","I")
console.writecolor("lightgreen","L")
console.writeln(" TLS mail sending\n")


console.color = "cyan"
local username = sys.Buffer(console.readln("Enter your gmail account (example: name.surname): ")):encode("base64")
console.echo = '*'
local password = sys.Buffer(console.readln("Enter your gmail account password: ")):encode("base64")
console.echo = true
local to = console.readln("Enter the mail recipient: ")

local from = sys.Buffer(username, "base64").."@gmail.com"


local sock = net.Socket("smtp.gmail.com", 587)
function smtp(cmd, msg)
	if cmd == nil then
		if sock:connect() == true then
			console.color = "green"
			console.writeln("\n------------------------------------------------------------")
		end
	else
		sock:send(cmd.."\r\n")
	end
	if msg ~= false then
		console.write(sock:recv())
	end
end

smtp()
smtp("EHLO "..net.ip)
smtp("STARTTLS")
sock:starttls()
smtp("AUTH LOGIN")
smtp(username)
smtp(password)
smtp("MAIL FROM:<"..from..">")
smtp("RCPT TO:<"..to..">")
smtp("DATA")
smtp('From: "tsam" <'..from..'>', false)
smtp('To: "You" <'..to..'>', false)
smtp("Subject: This is a test\r\n", false)
smtp("Hello World !\r\n.")
console.writeln("------------------------------------------------------------")