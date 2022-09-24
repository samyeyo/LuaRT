--
--  luaRT lang.lua example
--  Hello World in multiple languages
--

local console = require 'console'

function title(msg)
	local length = math.floor((console.width-string.len(msg))/2)
	console.clear("black")
	console.bgcolor = "lightblue"
	console.writecolor("blue", string.rep(string.char(0xE2, 0x96, 0x80), console.width))
	console.writecolor("brightwhite", string.rep(' ', length)..msg)
	console.write(string.rep(' ', console.width-length-string.len(msg)))
	console.bgcolor = "blue"
	console.writecolor("lightblue", string.rep(string.char(0xE2, 0x96, 0x80), console.width))
	console.bgcolor = "black"
	console.writeln("\n")
end

title("Hello World in different languages")

local hellos = {
	Afrikaans ="Hello Wêreld",
	Albanian ="Përshendetje Botë",
	Amharic ="ሰላም ልዑል",
	Arabic ="مرحبا بالعالم",
	Armenian ="Բարեւ աշխարհ",
	Basque ="Kaixo Mundua",
	Belarussian ="Прывітанне Сусвет",
	Bengali ="ওহে বিশ্ব",
	Bulgarian ="Здравей свят",
	Catalan ="Hola món",
	Chichewa ="Moni Dziko Lapansi",
	Chinese ="你好世界",
	Croatian ="Pozdrav svijete",
	Czech ="Ahoj světe",
	Danish ="Hej Verden",
	Dutch ="Hallo Wereld",
	English ="Hello World",
	Estonian ="Tere maailm",
	Finnish ="Hei maailma",
	French ="Bonjour monde",
	Frisian ="Hallo wrâld",
	Georgian ="გამარჯობა მსოფლიო",
	German ="Hallo Welt",
	Greek ="Γειά σου Κόσμε",
	Hausa ="Sannu Duniya",
	Hebrew ="שלום עולם",
	Hindi ="नमस्ते दुनिया",
	Hungarian ="Helló Világ",
	Icelandic ="Halló heimur",
	Igbo ="Ndewo Ụwa",
	Indonesian ="Halo Dunia",
	Italian ="Ciao mondo",
	Japanese ="こんにちは世界",
	Kazakh ="Сәлем Әлем",
	Khmer ="សួស្តី​ពិភពលោក",
	Kyrgyz ="Салам дүйнө",
	Lao ="ສະ​ບາຍ​ດີ​ຊາວ​ໂລກ",
	Latvian ="Sveika pasaule",
	Lithuanian ="Labas pasauli",
	Luxemburgish ="Moien Welt",
	Macedonian ="Здраво свету",
	Malay ="Hai dunia",
	Malayalam ="ഹലോ വേൾഡ്",
	Mongolian ="Сайн уу дэлхий",
	Myanmar ="မင်္ဂလာပါကမ္ဘာလောက",
	Nepali ="नमस्कार संसार",
	Norwegian ="Hei Verden",
	Pashto ="سلام نړی",
	Persian ="سلام دنیا",
	Polish ="Witaj świecie",
	Portuguese ="Olá Mundo",
	Punjabi ="ਸਤਿ ਸ੍ਰੀ ਅਕਾਲ ਦੁਨਿਆ",
	Romanian ="Salut Lume",
	Russian ="Привет мир",
	Gaelic ="Hàlo a Shaoghail",
	Serbian ="Здраво Свете",
	Sesotho ="Lefatše Lumela",
	Sinhala ="හෙලෝ වර්ල්ඩ්",
	Slovenian ="Pozdravljen svet",
	Spanish ="¡Hola Mundo", 
	Sundanese ="Halo Dunya",
	Swahili ="Salamu Dunia",
	Swedish ="Hej världen",
	Tajik ="Салом Ҷаҳон",
	Thai ="สวัสดีชาวโลก",
	Turkish ="Selam Dünya",
	Ukrainian ="Привіт Світ",
	Uzbek ="Salom Dunyo",
	Vietnamese ="Chào thế giới",
	Welsh ="Helo Byd",
	Xhosa ="Molo Lizwe",
	Yiddish ="העלא וועלט",
	Yoruba ="Mo ki O Ile Aiye",
	Zulu ="Sawubona Mhlaba"
}

while(true) do
	console.writecolor("white", "\nEnter a language (type 'list' for available choices) : ") 
	console.color = "cyan"
	local lang = console.readln():lower()
	if lang == "list"then
		console.color = "lightyellow"
		for l in pairs(hellos) do
			console.writeln("\t"..l)
		end
	else 
		local hello = hellos[lang:capitalize()] or false
		if hello then
			console.writecolor("brightwhite", "\n\tHello world in "..lang..": ")
			console.writecolor("lightyellow", hello.."\n");
		else
			console.writecolor("lightred", "\tUnknown language !\n")
		end
	end
end