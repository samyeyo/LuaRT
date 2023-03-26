--
--  LuaRT speech.lua example
--  Text-to-Speech example using sys.COM Object
--

-- Create a SAPI.SpVoice COM Object
local ObjVoice = sys.COM("SAPI.SpVoice") 

-- and call its Speak method, you shoud ear "Hello World"
ObjVoice:Speak("Hello World !") 