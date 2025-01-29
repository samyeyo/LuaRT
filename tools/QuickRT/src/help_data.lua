return {
	['each(iterable) iterator'] = {
	desc = [[Iterate on a table or Object over values only.]],
	addr = 'https:\\\\www.luart.org\\doc\\base\\each.html',
	params = {
		{
			name = [[iterable]],
			desc = [[ A table or an Object to iterate on. ]],
			color = 'green'}, nil} },
	['basic module'] = {
	desc = [[The basic module in Lua provides provides core functions to Lua such as garbage collection control, coroutines, iterators... Luart enhances the basic module with the following elements :]],
	addr = 'https:\\\\www.luart.org\\doc\\base\\index.html',
	params = {} },
	['is(var, object)'] = {
	desc = [[Check if the variable inherits from the specified Object.]],
	addr = 'https:\\\\www.luart.org\\doc\\base\\is.html',
	params = {
		{
			name = [[var]],
			desc = [[ A variable for which we want to check inheritance. That variable can be of any type. ]],
			color = 'green'}, 
		{
			name = [[object]],
			desc = [[ An Object (as defined by the Object function) ]],
			color = 'brightwhite'}, nil} },
	['Object(from)'] = {
	desc = [[Declare a new Object type using the from prototype. The Object function implements single inheritance object oriented programming in Lua.]],
	addr = 'https:\\\\www.luart.org\\doc\\base\\object.html',
	params = {
		{
			name = [[from]],
			desc = [[ A table or an other previously declred Object that serves as a prototype to define a new custom type. ]],
			color = 'brightwhite'}, nil} },
	['type(var)'] = {
	desc = [[Returns the type of a variable.]],
	addr = 'https:\\\\www.luart.org\\doc\\base\\type.html',
	params = {
		{
			name = [[var]],
			desc = [[ A variable to check its type. ]],
			color = 'brightwhite'}, nil} },
	['console.bgcolor read/write property'] = {
	desc = [[The console.bgcolor property sets or gets the current console background color. The color is represented by a string. Possible values are : 
        - "black" 
        - "blue" 
        - "green" 
        - "cyan" 
        - "red" 
        - "purple" 
        - "yellow" 
        - "white" 
        - "gray" 
        - "lightblue" 
        - "lightgreen" 
        - "lightcyan" 
        - "lightred" 
        - "lightpurple" 
        - "lightyellow" 
        - "brightwhite"]],
	addr = 'https:\\\\www.luart.org\\doc\\console\\bgcolor.html',
	params = {} },
	['console.clear(color)'] = {
	desc = [[Clears the console screen (equivalent of the famous DOS command cls).]],
	addr = 'https:\\\\www.luart.org\\doc\\console\\clear.html',
	params = {
		{
			name = [[color]],
			desc = [[ An optional string indicating the color to clear the console screen with. See console.bgcolor property to list the possible colors. If the color is not provided, use the current background color. Once the console is cleared, the function revert back to the previous background color. ]],
			color = 'lightyellow'}, nil} },
	['console.color read/write property'] = {
	desc = [[The console.color property sets or gets the current console text color. The color is represented by a string Possible values are : 
        - "black" 
        - "blue" 
        - "green" 
        - "cyan" 
        - "red" 
        - "purple" 
        - "yellow" 
        - "white" 
        - "gray" 
        - "lightblue" 
        - "lightgreen" 
        - "lightcyan" 
        - "lightred" 
        - "lightpurple" 
        - "lightyellow" 
        - "brightwhite"]],
	addr = 'https:\\\\www.luart.org\\doc\\console\\color.html',
	params = {} },
	['console.cursor readwrite property'] = {
	desc = [[The console.cursor property is a readwrite property that indicate if the console cursor is visible or not.]],
	addr = 'https:\\\\www.luart.org\\doc\\console\\cursor.html',
	params = {} },
	['console.echo read/write property'] = {
	desc = [[The console.echo property is a boolean value that sets/gets the current console ECHO mode.A console.echo property set to true means that each character entered in stdin will be written to the console stdout file. It's the default mode. Set that property to false if you don't want to print to the console the entered text (for example, when typing passwords). Set that property to a single char string to print to the console that character when typing on the keyboard.]],
	addr = 'https:\\\\www.luart.org\\doc\\console\\echo.html',
	params = {} },
	['console.font read/write property'] = {
	desc = [[Get the current console font, a string value that represent the font name. To set the console.font property, you can assign a string that represent either a font name of an installed system font, or a font file (*.ttf, *.fon...). A File can also be provided, representing a font file.]],
	addr = 'https:\\\\www.luart.org\\doc\\console\\font.html',
	params = {} },
	['console.font read/write property'] = {
	desc = [[Get or set the console font size, a number that represent the font size in pixels. If you use Windows Terminal as console, please note that it does not permit to change console font size programmatically, so setting this property will have no effect.]],
	addr = 'https:\\\\www.luart.org\\doc\\console\\fontsize.html',
	params = {} },
	['console.fullscreen read/write property'] = {
	desc = [[Gets/sets the console fullscreen mode (really fullscreen without the window frame and title bar). To go fullscreen, set the property to true To revert back to normal mode (original size with window frame and title bar) set the property to false. The user can revert back to normal mode by pressing ALT+RETURN.]],
	addr = 'https:\\\\www.luart.org\\doc\\console\\fullscreen.html',
	params = {} },
	['console.height readonly property'] = {
	desc = [[The console.height property gets the current console height in character. That value depends on the current console window size.]],
	addr = 'https:\\\\www.luart.org\\doc\\console\\height.html',
	params = {} },
	['console module'] = {
	desc = [[The console module provides properties and functions for input/ouput facilities, such as printing colored text, reading keys,...Luart uses UTF8 as for strings, with some limits (use of Windows Terminal or Powershell is preferred as cmd.exe is not 100% compatible with UTF8 output).]],
	addr = 'https:\\\\www.luart.org\\doc\\console\\index.html',
	params = {} },
	['console.inverse()'] = {
	desc = [[Swaps the text color and the background color of the console until another call to this function.]],
	addr = 'https:\\\\www.luart.org\\doc\\console\\inverse.html',
	params = {} },
	['console.keyhit readonly property'] = {
	desc = [[The console.keyhit is a readonly property that returns a boolean value indicating true if a key has been pressed (in that case, a call to console.readchar() will return immediately with the key pressed).]],
	addr = 'https:\\\\www.luart.org\\doc\\console\\keyhit.html',
	params = {} },
	['console.locate(column, line)'] = {
	desc = [[Position the console cursor to the specified location on the console. The position 1,1 represent the upper left corner in the console.]],
	addr = 'https:\\\\www.luart.org\\doc\\console\\locate.html',
	params = {
		{
			name = [[column]],
			desc = [[ A number representing the horizontal position of the cursor from 1 to console.width ]],
			color = 'lightyellow'}, 
		{
			name = [[line]],
			desc = [[ A number representing the vertical position of the cursor from 1 to console.height ]],
			color = 'lightyellow'}, nil} },
	['console.open()'] = {
	desc = [[Opens a console if it does not exists (in Desktop version, when using wluart.exe).]],
	addr = 'https:\\\\www.luart.org\\doc\\console\\open.html',
	params = {} },
	['console.read(length)'] = {
	desc = [[Reads from the console using console.stdin. The same result can be obtained using console.stdin:read()]],
	addr = 'https:\\\\www.luart.org\\doc\\console\\read.html',
	params = {
		{
			name = [[length]],
			desc = [[ The number of characters to read from the console. ]],
			color = 'lightyellow'}, nil} },
	['console.readchar()'] = {
	desc = [[Reads a key from the console and returns a corresponding character if possible. That function can read special keys (Function keys, arrows...).]],
	addr = 'https:\\\\www.luart.org\\doc\\console\\readchar.html',
	params = {} },
	['console.readln()'] = {
	desc = [[Reads a line from the console (until a the ENTER/RETURN key has been pressed). The same result can be obtained using console.stdin:readln().]],
	addr = 'https:\\\\www.luart.org\\doc\\console\\readln.html',
	params = {} },
	['console.readmouse()'] = {
	desc = [[Wait for the mouse to move or click inside the console.]],
	addr = 'https:\\\\www.luart.org\\doc\\console\\readmouse.html',
	params = {} },
	['console.reset()'] = {
	desc = [[Resets the console to the default text and background colors. That function is always called before the program terminates.]],
	addr = 'https:\\\\www.luart.org\\doc\\console\\reset.html',
	params = {} },
	['console.stderr read/write property'] = {
	desc = [[The console.stderr property gets the current standard console error output file. It's a File value]],
	addr = 'https:\\\\www.luart.org\\doc\\console\\stderr.html',
	params = {} },
	['console.stdin read/write property'] = {
	desc = [[The console.stdin property gets the current standard console input file. It's a File value.]],
	addr = 'https:\\\\www.luart.org\\doc\\console\\stdin.html',
	params = {} },
	['console.stdout read/write property'] = {
	desc = [[The console.stdout property gets the current standard console output file. It's a File value with the same methods and properties.]],
	addr = 'https:\\\\www.luart.org\\doc\\console\\stdout.html',
	params = {} },
	['console.title read/write property'] = {
	desc = [[The console.title gets/sets the console window title.]],
	addr = 'https:\\\\www.luart.org\\doc\\console\\title.html',
	params = {} },
	['console.width readonly property'] = {
	desc = [[The console.width property gets the current console width in character. That value depends on the current console window size.]],
	addr = 'https:\\\\www.luart.org\\doc\\console\\width.html',
	params = {} },
	['console.write(str1, str2, str3...)'] = {
	desc = [[Writes to the console using console.stdout. Contrary to console.stdout:write(), console.write() can write more than one string, each string is separated with a space character.]],
	addr = 'https:\\\\www.luart.org\\doc\\console\\write.html',
	params = {
		{
			name = [[str1, str2,...]],
			desc = [[ The strings to write to the console. If the argument is not a string, the function converts it internally using the tostring function. The function does not return to next line after printing the characters. If you want to add a newline character, use console.writeln() instead. ]],
			color = 'lightpurple'}, nil} },
	['console.writecolor(color, str1, str2, str3...)'] = {
	desc = [[Writes colored output to the console using console.stdout. The function change the color of the text (not its background). Then the function set back the text color to the previous one. Console.write() can write more than one string, each string is separated with a space character.]],
	addr = 'https:\\\\www.luart.org\\doc\\console\\writecolor.html',
	params = {
		{
			name = [[color]],
			desc = [[ A string that represents the color of the printed text. Possible values are : 
        - "black" 
        - "blue" 
        - "green" 
        - "cyan" 
        - "red" 
        - "purple" 
        - "yellow" 
        - "white" 
        - "gray" 
        - "lightblue" 
        - "lightgreen" 
        - "lightcyan" 
        - "lightred" 
        - "lightpurple" 
        - "lightyellow" 
        - "brightwhite" ]],
			color = 'lightpurple'}, 
		{
			name = [[str1, str2,...]],
			desc = [[ The strings to write to the console. If the argument is not a string, the function converts it internally using the tostring function. The function does not return to next line after printing the characters. ]],
			color = 'lightpurple'}, nil} },
	['console.writeln(str1, str2, str3...)'] = {
	desc = [[Writes to the console using console.stdout, adding a last newline character. Contrary to console.stdout:writeln(), console.writeln() can write more than one string, each string is separated with a space character.]],
	addr = 'https:\\\\www.luart.org\\doc\\console\\writeln.html',
	params = {
		{
			name = [[str1, str2,...]],
			desc = [[ Optional strings to write to the console. If the argument is not a string, the function converts it internally using the tostring function. Calling the function without any argument writes "\n" ]],
			color = 'lightpurple'}, nil} },
	['console.x read/write property'] = {
	desc = [[The console.x property gets or sets the current cursor column position.]],
	addr = 'https:\\\\www.luart.org\\doc\\console\\x.html',
	params = {} },
	['console.y read/write property'] = {
	desc = [[The console.y property gets or sets the current cursor line position.]],
	addr = 'https:\\\\www.luart.org\\doc\\console\\y.html',
	params = {} },
	['crypto.algorithm read/write property'] = {
	desc = [[The crypto.algorithm property gets/sets the algorithm used for the crypto.encrypt() and crypto.decrypt() functions. Valid algorithms are : 
        - "aes128" : for AES 128 cipher (default) 
        - "aes256" : for AES 256 cipher]],
	addr = 'https:\\\\www.luart.org\\doc\\crypto\\algorithm.html',
	params = {} },
	['Cipher.algorithm readonly property'] = {
	desc = [[The Cipher.algorithm gets the current algorithm used for encryption/decryption. The algorithm is set upon Cipher construction (See Cipher.constructor()).]],
	addr = 'https:\\\\www.luart.org\\doc\\crypto\\cipher-algorithm.html',
	params = {} },
	['Cipher.constructor(algorithm, key, iv , mode) constructor'] = {
	desc = [[The Cipher constructor returns a Cipher value representing a cryptographic algorithm to be used for encryption/decryption, with the provided key.]],
	addr = 'https:\\\\www.luart.org\\doc\\crypto\\cipher-constructor.html',
	params = {
		{
			name = [[algorithm]],
			desc = [[ A string representing the algorithm to be used for encryption/decryption : 
        - "aes128" : advanced encryption standard with 128 bits (16 bytes) key length (block cipher) 
        - "aes192" : advanced encryption standard with 192 bits (24 bytes) key length (block cipher) 
        - "aes256" : advanced encryption standard with 256 bits (32 bytes) key length (block cipher) 
        - "des" : data encryption standard with 56 bits (7 bytes) key length (block cipher) 
        - "2des" : double data encryption standard with 112 bits (14 bytes) key length (block cipher) 
        - "3des" : triple data encryption standard with 168 bits (21 bytes) key length (block cipher) 
        - "rc2" : Rivest cipher 2 with 40 to 128 bits (5...16 bytes) key length (block cipher) 
        - "rc4" : Rivest cipher 4 with 40 to 128 bits (5...16 bytes) key length (stream cipher) ]],
			color = 'lightpurple'}, 
		{
			name = [[key]],
			desc = [[ A string or a Buffer value, containing the key. If the key is longer than the expected key length, only the first given bytes are used. An error occurs if the key length is too small. ]],
			color = 'lightpurple'}, 
		{
			name = [[iv]],
			desc = [[ An optional string or Buffer value, containing an initialization vector. ]],
			color = 'lightpurple'}, 
		{
			name = [[mode]],
			desc = [[ An optional string to specify the cipher mode : 
        - "cbc" : cipher block chaining mode (the default for block algorithms) 
        - "cfb" : cipher feedback mode 
        - "ecb" : electronic codebook mode 
        - "cts" : ciphertext stealing mode ]],
			color = 'lightpurple'}, nil} },
	['Cipher:decrypt(data) method'] = {
	desc = [[Decrypt the specified data.]],
	addr = 'https:\\\\www.luart.org\\doc\\crypto\\cipher-decrypt.html',
	params = {
		{
			name = [[data]],
			desc = [[ A Buffer or a string containing the data to be decrypted. The Cipher object uses an internal decryption buffer, so that sucessive calls to this method appends the decrypted data to this internal buffer. That permits to decrypt data from various streams easily.Use the property Cipher.decrypted to retrieve the whole decrypted data. ]],
			color = 'gray'}, nil} },
	['Cipher.decrypted readonly property'] = {
	desc = [[The Cipher.decrypted property gets Buffer value, containing the data decrypted previously by the Cipher:decrypt() method.]],
	addr = 'https:\\\\www.luart.org\\doc\\crypto\\cipher-decrypted.html',
	params = {} },
	['Cipher:encrypt(data) method'] = {
	desc = [[Encrypt the specified data.]],
	addr = 'https:\\\\www.luart.org\\doc\\crypto\\cipher-encrypt.html',
	params = {
		{
			name = [[data]],
			desc = [[ A Buffer or a string containing the data to be encrypted. The Cipher object uses an internal encryption buffer, so that sucessive calls to this method appends the encrypted data to this internal buffer. That permits to encrypt data from various streams easily.Use the property Cipher.encrypted to retrieve the whole encrypted data. ]],
			color = 'gray'}, nil} },
	['Cipher.encrypted readonly property'] = {
	desc = [[The Cipher.decrypted property gets a Buffer value, containing the data encrypted previously by the Cipher:encrypt() method.]],
	addr = 'https:\\\\www.luart.org\\doc\\crypto\\cipher-encrypted.html',
	params = {} },
	['Cipher:reset() method'] = {
	desc = [[Empty and reset the Cipher internal encryption and decryption buffers, to work with new data.]],
	addr = 'https:\\\\www.luart.org\\doc\\crypto\\cipher-reset.html',
	params = {} },
	['crypto.Cipher'] = {
	desc = [[The crypto.Cipher Object provides cryptography encryption/decryption capabilities.]],
	addr = 'https:\\\\www.luart.org\\doc\\crypto\\cipher.html',
	params = {} },
	['crypto.crc32(data)'] = {
	desc = [[Calculate the Cyclic Redundancy Check (CRC) value of the given data..]],
	addr = 'https:\\\\www.luart.org\\doc\\crypto\\crc32.html',
	params = {
		{
			name = [[data]],
			desc = [[ A string or a Buffer. ]],
			color = 'lightpurple'}, nil} },
	['crypto.decrypt(data, key, init , mode)'] = {
	desc = [[Decrypts the data with the specified key, using the current crypto.algorithm.]],
	addr = 'https:\\\\www.luart.org\\doc\\crypto\\decrypt.html',
	params = {
		{
			name = [[data]],
			desc = [[ A string or a Buffer to be decrypted. ]],
			color = 'lightpurple'}, 
		{
			name = [[key]],
			desc = [[ A string or a Buffer to be used as the key for decryption. ]],
			color = 'lightpurple'}, 
		{
			name = [[init]],
			desc = [[ An optional string or a Buffer to be used as vector initialization. ]],
			color = 'lightpurple'}, 
		{
			name = [[mode]],
			desc = [[ An optional string that specify the mode that was used for encryption: 
        - "cbc" : For cipher block chainin mode (default). 
        - "cfb" : For 8bit cipher feedback mode. 
        - "ecb" : For electronic book mode. 
        - "cts" : For ciphertext stealing mode. ]],
			color = 'lightpurple'}, nil} },
	['crypto.encrypt(data, key, init , mode)'] = {
	desc = [[Encrypts the data with the specified key, using the current crypto.algorithm.]],
	addr = 'https:\\\\www.luart.org\\doc\\crypto\\encrypt.html',
	params = {
		{
			name = [[data]],
			desc = [[ A string or a Buffer to be encrypted. ]],
			color = 'lightpurple'}, 
		{
			name = [[key]],
			desc = [[ A string or a Buffer to be used as the key for encryption. ]],
			color = 'lightpurple'}, 
		{
			name = [[init]],
			desc = [[ An optional string or a Buffer to be used as vector initialization.If not provided, a default zero bytes vector is used. ]],
			color = 'lightpurple'}, 
		{
			name = [[mode]],
			desc = [[ An optional string that specify the encryption mode : 
        - "cbc" : For cipher block chainin mode (default). 
        - "cfb" : For 8bit cipher feedback mode. 
        - "ecb" : For electronic book mode. 
        - "cts" : For ciphertext stealing mode. ]],
			color = 'lightpurple'}, nil} },
	['crypto.generate(length)'] = {
	desc = [[Generate binary data with cryptographically random bytes.]],
	addr = 'https:\\\\www.luart.org\\doc\\crypto\\generate.html',
	params = {
		{
			name = [[length]],
			desc = [[ A number representing the length in bytes of the data to be generated. ]],
			color = 'lightyellow'}, nil} },
	['crypto.hash(algorithm, data)'] = {
	desc = [[Create a hash digest of data, using the specified algorithm.]],
	addr = 'https:\\\\www.luart.org\\doc\\crypto\\hash.html',
	params = {
		{
			name = [[algorithm]],
			desc = [[ A string representing the hash algorithm to use : 
        - "md5" for MD5 hash algorithm 
        - "sha1" for SHA1 hash algorithm 
        - "sha256" for SHA256 hash algorithm 
        - "sha384" for SHA384 hash algorithm 
        - "sha512" for SHA512 hash algorithm ]],
			color = 'lightpurple'}, 
		{
			name = [[data]],
			desc = [[ A string or a Buffer that contains the data to hash. ]],
			color = 'lightpurple'}, nil} },
	['crypto module'] = {
	desc = [[The crypto module provides properties and functions for cryptography facilities, such as hashing functions and encryption/decryption for various algorithms.]],
	addr = 'https:\\\\www.luart.org\\doc\\crypto\\index.html',
	params = {} },
	[''] = {
	desc = [[Here you will find the essentials to take your first steps with Luart, regardless of whether you are an experienced developer or a beginner. With Luart, you can use and learn the Lua programming language easily thanks to its specific implementation on Windows. It only takes a few lines of code to make a simple app, so it's time to get started!]],
	addr = 'https:\\\\www.luart.org\\doc\\index.html',
	params = {} },
	['Luart standard modules'] = {
	desc = [[Luart includes a specific standard library that enhances capabilities for Lua development on Windows operating systems. Please click on the following modules to get more help : sys console net zip crypto ui]],
	addr = 'https:\\\\www.luart.org\\doc\\modules.html',
	params = {} },
	['net.adapters(family)iterator'] = {
	desc = [[Iterator function to retrieve installed network adapters on the Windows operating system, using the IPv4 or IPv6 address family. family A string representing the IP family address returned at each iteration : "IPv4" or "IPv6".]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\adapters.html',
	params = {
		{
			name = [[family]],
			desc = [[ A string representing the IP family address returned at each iteration : "IPv4" or "IPv6". ]],
			color = 'lightpurple'}, nil} },
	['net.error readonly property'] = {
	desc = [[The net.error property gets the last network error message.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\error.html',
	params = {} },
	['Ftp.active read/write property'] = {
	desc = [[The Ftp.active property gets or sets the current FTP mode : true to use FTP active mode and false to use FTP passive mode.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\ftp-active.html',
	params = {} },
	['Ftp:close() method'] = {
	desc = [[Close the Ftp client connection. Use Ftp:open() to open a new connection. Remember to close the connection when no more needed. Ftp:close() is automatically called before garbage collection of the Ftp object, but this behaviour cannot be predicted]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\ftp-close.html',
	params = {} },
	['Ftp:command(cmd) method'] = {
	desc = [[Sends and executes the provided command to the FTP server.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\ftp-command.html',
	params = {
		{
			name = [[cmd]],
			desc = [[ A string that contains the command to execute. ]],
			color = 'lightpurple'}, nil} },
	['net.Ftp(url, username, password) constructor'] = {
	desc = [[The Ftp constructor returns a Ftp value representing a FTP client and connects it to the FTP server at the given URL, using provided authentication.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\ftp-constructor.html',
	params = {
		{
			name = [[url]],
			desc = [[ A string representing a valid URL to connect to the FTP server. ]],
			color = 'lightpurple'}, 
		{
			name = [[username]],
			desc = [[ A string representing the username (use "anonymous" for anonymous access if supported by the server). ]],
			color = 'lightpurple'}, 
		{
			name = [[password]],
			desc = [[ A string representing the password (use any string for anonymous access). By default, Ftp connection is open in passive mode. See the Ftp.active property. ]],
			color = 'lightpurple'}, nil} },
	['Ftp.currentdir read/write property'] = {
	desc = [[The Ftp.currentdir property gets or sets the current directory on the FTP server as a string.Returns nil if the value could not be retrieved (get more information with the net.error property).]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\ftp-currentdir.html',
	params = {} },
	['Ftp:download(remote_file) method'] = {
	desc = [[Download a remote file from the connected HTTP server, to the current directory.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\ftp-download.html',
	params = {
		{
			name = [[remote_file]],
			desc = [[ A string that contains the name of the file to be retrieved. ]],
			color = 'lightpurple'}, nil} },
	['Ftp.hostname readonly property'] = {
	desc = [[The Ftp.hostname property gets the current hostname string used for connection to the FTP server or nil if connection have been closed.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\ftp-hostname.html',
	params = {} },
	['Ftp:list(search_path) methoditerator'] = {
	desc = [[Iterates over directory content inside the specified search path.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\ftp-list.html',
	params = {
		{
			name = [[search_path]],
			desc = [[ An optional string that contains a valid directory path or file name. The string can contain wildcards, but no blank spaces are allowed.If not specified, iterate over all current directory contents. ]],
			color = 'lightpurple'}, nil} },
	['Ftp:makedir(path) method'] = {
	desc = [[Creates a new directory on the FTP server.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\ftp-makedir.html',
	params = {
		{
			name = [[path]],
			desc = [[ A string that contains the name of the directory to be created. This can be either a fully qualified path or a name relative to the current FTP directory. ]],
			color = 'lightpurple'}, nil} },
	['Ftp:movefile(path, newpath) method'] = {
	desc = [[Moves the specified file from the FTP server.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\ftp-movefile.html',
	params = {
		{
			name = [[path]],
			desc = [[ A string that contains the file path (partially or fully qualified file names relative to the current FTP directory can be used). ]],
			color = 'lightpurple'}, 
		{
			name = [[newpath]],
			desc = [[ A string that contains he new file path (partially or fully qualified file names relative to the current FTP directory can be used). ]],
			color = 'lightpurple'}, nil} },
	['Ftp:open(url, username, password) method'] = {
	desc = [[Connect the Ftp client to a new URL, closing the current connection.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\ftp-open.html',
	params = {
		{
			name = [[url]],
			desc = [[ A string representing a valid URL to connect to the FTP server. ]],
			color = 'lightpurple'}, 
		{
			name = [[username]],
			desc = [[ A string representing the username (use "anonymous" for anonymous access if supported by the server). ]],
			color = 'lightpurple'}, 
		{
			name = [[password]],
			desc = [[ A string representing the password (use any string for anonymous access). ]],
			color = 'lightpurple'}, nil} },
	['Ftp.port readonly property'] = {
	desc = [[The Ftp.port property gets the current port used for connection to the FTP server or nil if connection have been closed.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\ftp-port.html',
	params = {} },
	['Ftp:removedir(path) method'] = {
	desc = [[Removes the specified directory from the FTP server.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\ftp-removedir.html',
	params = {
		{
			name = [[path]],
			desc = [[ A string that contains the name of the directory to be removed. This can be either a fully qualified path or a name relative to the current FTP directory. ]],
			color = 'lightpurple'}, nil} },
	['Ftp:removefile(path)method'] = {
	desc = [[Removes the specified file from the FTP server.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\ftp-removefile.html',
	params = {
		{
			name = [[path]],
			desc = [[ A string that contains the file path to be removed. ]],
			color = 'lightpurple'}, nil} },
	['Ftp:upload(file, remote_name) method'] = {
	desc = [[Upload a file to the connected FTP server.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\ftp-upload.html',
	params = {
		{
			name = [[file]],
			desc = [[ A string or a File object representing the file to be uploaded to the server. ]],
			color = 'brightwhite'}, 
		{
			name = [[remote_name]],
			desc = [[ An optional string that contains the destination path and filename on the server.If not provided, the file will be uploaded on the current FTP directory (See Ftp.currentdir) using the same filename. ]],
			color = 'lightpurple'}, nil} },
	['net.Ftp'] = {
	desc = [[The Ftp Object is a client implementaton of the File Transfer Protocol.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\ftp.html',
	params = {} },
	['Http:close() method'] = {
	desc = [[Close the Http client connection. Use Http:open() to open a new connection. Remember to close the connection when no more needed. Http:close() is automatically called before garbage collection of the Http object, but this behaviour cannot be predicted]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\http-close.html',
	params = {} },
	['net.Http(url, username, password) constructor'] = {
	desc = [[The Http constructor returns a Http value representing a HTTP client and connect it to the HTTP server at the given URL, using optional basic authentication.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\http-constructor.html',
	params = {
		{
			name = [[url]],
			desc = [[ A string representing a valid URL to connect to the HTTP server. ]],
			color = 'lightpurple'}, 
		{
			name = [[username]],
			desc = [[ An optional string representing a username for basic HTTP authentication. ]],
			color = 'lightpurple'}, 
		{
			name = [[password]],
			desc = [[ An optional string representing a password for basic HTTP authentication. ]],
			color = 'lightpurple'}, nil} },
	['Http:get(uri) method'] = {
	desc = [[Send a GET request to the connected HTTP server for a specific resource, and return the server response.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\http-get.html',
	params = {
		{
			name = [[uri]],
			desc = [[ A string representing the resource to get on the server. ]],
			color = 'lightpurple'}, nil} },
	['Http.headers readonly property'] = {
	desc = [[The Http.headers property gets a string that contains the headers of the response returned by the HTTP server after the last request or nil if no request have been sent.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\http-headers.html',
	params = {} },
	['Http.hostname readonly property'] = {
	desc = [[The Http.hostname property gets the current hostname string used for connection to the HTTP server or nil if connection have been closed.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\http-hostname.html',
	params = {} },
	['Http.mime readonly property'] = {
	desc = [[The Http.mime property gets the mime type string returned by the HTTP server after a request or nil if no request have been previously sent.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\http-mime.html',
	params = {} },
	['Http:open(url, username, password) method'] = {
	desc = [[Connect the Http client to a new URL, closing the current connection.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\http-open.html',
	params = {
		{
			name = [[url]],
			desc = [[ A string representing a valid URL to the HTTP server. ]],
			color = 'lightpurple'}, 
		{
			name = [[username]],
			desc = [[ An optional string representing a username for basic HTTP authentication. ]],
			color = 'lightpurple'}, 
		{
			name = [[password]],
			desc = [[ An optional string representing a password for basic HTTP authentication. ]],
			color = 'lightpurple'}, nil} },
	['Http.port readonly property'] = {
	desc = [[The Http.port property gets the port number used for connection to the HTTP server or nil if connection have been closed.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\http-port.html',
	params = {} },
	['Http:post(uri, parameters)'] = {
	desc = [[Send a POST request to the specified resource, and return the server response.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\http-post.html',
	params = {
		{
			name = [[uri]],
			desc = [[ A string representing the resource one the server to POST parameters to. ]],
			color = 'lightpurple'}, 
		{
			name = [[parameters]],
			desc = [[ A table representing the parameters.Each key in the table is a string, representing the parameter name.Table values represent the value for the corresponding parameter. Only string or File object are accepted values. ]],
			color = 'green'}, nil} },
	['Http.scheme readonly property'] = {
	desc = [[The Http.scheme property gets the current scheme string used for connection to the HTTP server : "http", "https" or nil if connection have been closed.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\http-scheme.html',
	params = {} },
	['Http.status readonly property'] = {
	desc = [[The Http.status property gets the status string returned by the HTTP server after a request or nil if no request have been sent.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\http-status.html',
	params = {} },
	['Http.statuscode readonly property'] = {
	desc = [[The Http.statuscode property gets the status code number returned by the HTTP server after a request or nil if no request have been sent.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\http-statuscode.html',
	params = {} },
	['net.Http'] = {
	desc = [[The Http Object is a client implementaton of the HyperText Transfer Protocol.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\http.html',
	params = {} },
	['net module'] = {
	desc = [[The net module provides properties and functions for networking facilities, such as creating sockets, checking connection, downloading files...]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\index.html',
	params = {} },
	['net.ip readonly property'] = {
	desc = [[The net.ip property gets the current local IPv4 address as a string, used by the Windows operating system to connect to Internet. A valid Internet connection must be available, or returns false otherwise.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\ip.html',
	params = {} },
	['net.isalive readonly property'] = {
	desc = [[The net.isalive property checks if a network connection is available, returning true or false otherwise.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\isalive.html',
	params = {} },
	['net.publicip readonly property'] = {
	desc = [[The net.publicip property gets the current public IPv4 address as a string. A valid Internet connection must be available, or returns false otherwise.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\publicip.html',
	params = {} },
	['net.resolve(hostname, protocol)'] = {
	desc = [[Resolves the given hostname to an IP address from the DNS record.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\resolve.html',
	params = {
		{
			name = [[hostname]],
			desc = [[ The string to indicate the hostname to resolve, for example "luart.org". ]],
			color = 'lightpurple'}, 
		{
			name = [[family]],
			desc = [[ An optional string to indicate the IP address family to retrieve from the DNS record : 
        - "ipv4" : an IPv4 address will be returned (default) 
        - "ipv6" : an IPv6 address will be returned ]],
			color = 'lightpurple'}, nil} },
	['net.reverse(ip)'] = {
	desc = [[Returns the fully qualified domain from an IP address (reverse DNS lookup).]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\reverse.html',
	params = {
		{
			name = [[ip]],
			desc = [[ The IPv4 or IPv6 address to perform the reverse DNS lookup. ]],
			color = 'lightpurple'}, nil} },
	['net.select(socket_array, timeout)'] = {
	desc = [[Check for network events (readability, writability and errors) on the provided array of Sockets, by updating their corresponding properties Socket.canread, Socket.canwrite, Socket.failed.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\select.html',
	params = {
		{
			name = [[socket_array]],
			desc = [[ A table containing an array of Sockets to check for network events. ]],
			color = 'lightpurple'}, 
		{
			name = [[timeout]],
			desc = [[ An optional number to indicate the maximum time for select to wait for network events, in microseconds. If that argument is not provided, the function wait until a network event occurs. ]],
			color = 'lightyellow'}, nil} },
	['Socket : accept() method'] = {
	desc = [[Accept a connection. The socket should have been set to listen for incoming connections with a previous Socket:bind() call. When used in blocking mode, the method won't return until a connection is present.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\socket-accept.html',
	params = {} },
	['Socket : bind(backlog) method'] = {
	desc = [[Bind the Socket and listen for incoming connection.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\socket-bind.html',
	params = {
		{
			name = [[backlog]],
			desc = [[ An optional integer representing the maximum length of the queue of pending connections (defaults to 5). ]],
			color = 'lightyellow'}, nil} },
	['Socket.blocking read/write property'] = {
	desc = [[The Socket.blocking property sets or checks if the Socket is in blocking mode. A true value means the Socket is in blocking mode, meaning that all Socket operations (such as Socket:recv() or Socket:send()) will block until they have terminated, or false otherwise. By default, all created Socket Object have the property Socket.blocking set to true]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\socket-blocking.html',
	params = {} },
	['Socket.canread readonly property'] = {
	desc = [[The Socket.canread gets the readability of a Socket after a net.select() call.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\socket-canread.html',
	params = {} },
	['Socket.canwrite readonly property'] = {
	desc = [[]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\socket-canwrite.html',
	params = {} },
	['Socket : close() method'] = {
	desc = [[Closes the Socket and the bounded connection. The Socket should not be used anymore. When the Socket is garbage collected, the close method is called automatically]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\socket-close.html',
	params = {} },
	['Socket : connect() method'] = {
	desc = [[Connect to the remote Socket.ip address. Used in blocking mode, the methods wait until the connection is established, or returns immediately otherwise.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\socket-connect.html',
	params = {} },
	['net.Socket(host, port, family) constructor'] = {
	desc = [[The Socket constructor returns a Socket value representing a Windows TCP socket to be bound to a specific IP address or hostname on the given port number. In case of hostname, a DNS query is sent to be resolved to a valid IP of the given family.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\socket-constructor.html',
	params = {
		{
			name = [[host]],
			desc = [[ A string representing an IP address (family is autodetected) or a hostname (IP is autoresolved to the given family) ]],
			color = 'lightpurple'}, 
		{
			name = [[port]],
			desc = [[ The port number the Socket will be bound to. ]],
			color = 'lightyellow'}, 
		{
			name = [[family]],
			desc = [[ An optional string to specify the address family when resolving the provided hostname. 
        - "ipv4" : an IPv4 address will be used when resolving the hostname (default) 
        - "ipv6" : an IPv6 address will be used when resolving the hostname ]],
			color = 'lightpurple'}, nil} },
	['Socket.failed readonly property'] = {
	desc = [[]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\socket-failed.html',
	params = {} },
	['Socket.family readonly property'] = {
	desc = [[The Socket.family property gets the address family "ipv4" or "ipv6"]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\socket-family.html',
	params = {} },
	['Socket.ip readonly property'] = {
	desc = [[The Socket.ip property gets the IP address the socket is bound to.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\socket-ip.html',
	params = {} },
	['Socket : peek() method'] = {
	desc = [[Get how many bytes are available for reading on the Socket connection with a single Socket:recv() call.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\socket-peek.html',
	params = {} },
	['Socket.port readonly property'] = {
	desc = [[The Socket.port property gets the port number used to create the Socket.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\socket-port.html',
	params = {} },
	['Socket : recv(size) method'] = {
	desc = [[Receive data from the Socket. In blocking mode, the method wait until some data is available for reading.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\socket-recv.html',
	params = {
		{
			name = [[size]],
			desc = [[ An optional integer representing the maximum number of bytes to read from the connection, defaults to 1024. ]],
			color = 'lightyellow'}, nil} },
	['Socket : send(data) method'] = {
	desc = [[Send the data to the Socket. In blocking mode, the method can block if the length of the data is larger than the maximum allowed by the transport.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\socket-send.html',
	params = {
		{
			name = [[data]],
			desc = [[ A Buffer or a string containing the data to be sent. If another type of variable is used, it is internaly converted to a string before sending. ]],
			color = 'gray'}, nil} },
	['Socket : sendall(data) method'] = {
	desc = [[Send all the data to the Socket. In blocking mode, the method can block if the length of the data is larger than the maximum allowed by the transport.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\socket-sendall.html',
	params = {
		{
			name = [[data]],
			desc = [[ A Buffer or a string containing the data to be sent. If another type of variable is used, it is internaly converted to a string before sending. ]],
			color = 'gray'}, nil} },
	['Socket : shutdown() method'] = {
	desc = [[Shuts down the Socket. Further sends or reveives are disallowed, and any pending data is discarded. Call this method if you want to stop using the connection immediately, because Socket:close() can take some time to effectively close the connection.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\socket-shutdown.html',
	params = {} },
	['Socket : starttls(certificate, password) method'] = {
	desc = [[Enable transport level security (TLS -formerly known as SSL) on a Socket connection. The connection must have been initialized with a previous Socket:bind() or Socket:connect() call. Any further calls to Socket:send() or Socket:recv() will be encrypted/decrypted, respectively.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\socket-starttls.html',
	params = {
		{
			name = [[certificate]],
			desc = [[ An optional string representing the certificate to verify the identity of the other peer. If none is provided, uses the certificate from the server. The string can be : 
        - A server name of a certificate already installed in the current user/Windows certificate store 
        - A path to a .pfx file (PKCS #12), that contains cryptographic information (certificates and private keys). Its contents can be cryptographically protected with passwords. ]],
			color = 'lightpurple'}, 
		{
			name = [[password]],
			desc = [[ An optional string representing the password that must be provided when using a .pfx file . ]],
			color = 'lightpurple'}, nil} },
	['net.Socket'] = {
	desc = [[The net.Socket Object provides access to the underlying Windows TCP socket implementation.]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\socket.html',
	params = {} },
	['net.urlparse(url)'] = {
	desc = [[Parse a URL into 4 components, corresponding to the structure of a URL : scheme :// hostname / path ? parameters]],
	addr = 'https:\\\\www.luart.org\\doc\\net\\urlparse.html',
	params = {
		{
			name = [[url]],
			desc = [[ A string representing a URL to parse, for example "https://www.luart.org/index.html". ]],
			color = 'lightpurple'}, nil} },
	['string.capitalize(str)'] = {
	desc = [[Capitalize the string : All letters are lowered except the first character of the string.]],
	addr = 'https:\\\\www.luart.org\\doc\\string\\capitalize.html',
	params = {
		{
			name = [[str]],
			desc = [[ A string that will be capitalized. ]],
			color = 'lightpurple'}, nil} },
	['string module'] = {
	desc = [[Strings in Luart are considered as containers for UTF8 characters, where in standard Lua, strings are containers for bytes, as explained in the Compatibility section The string module in Luart provides generic functions for string manipulation, such as finding and extracting substrings, and pattern matching.That means that all standard Lua strings functions can be used with UTF8 encoded strings using the standard Lua string functions : see the Lua 5.4 String module reference.The string module in Luart adds or modifies the following elements :]],
	addr = 'https:\\\\www.luart.org\\doc\\string\\index.html',
	params = {} },
	['Stringiterable'] = {
	desc = [[Strings in Luart are iterable through the Luart helper function each, returning the next character in the string with each loop iteration. Each iteration returns a string containing the next character.]],
	addr = 'https:\\\\www.luart.org\\doc\\string\\iterator.html',
	params = {} },
	['#Stringlength operator'] = {
	desc = [[The length operator # has the same functionality as in standard Lua. As stated in the Lua 5.4 programming manual (see The Length Operator), the returned length of a string is its number of bytes. This behaviour is respected in Luart to preserve compatibility with standard Lua with ASCII characters. To get the length in characters of a string in Luart, do not use the # operator, use string.len() instead]],
	addr = 'https:\\\\www.luart.org\\doc\\string\\length.html',
	params = {} },
	['string.lower(str)'] = {
	desc = [[Converts all uppercase characters in a string into lowercase characters.]],
	addr = 'https:\\\\www.luart.org\\doc\\string\\lower.html',
	params = {
		{
			name = [[str]],
			desc = [[ The string to be used to produce a string with lowercase characters. ]],
			color = 'lightpurple'}, nil} },
	['string.search(str, substring, start)'] = {
	desc = [[Search for the first occurence of a substring, using plain text (no patterns). Faster than standard string.find]],
	addr = 'https:\\\\www.luart.org\\doc\\string\\search.html',
	params = {
		{
			name = [[str]],
			desc = [[ The string to search. ]],
			color = 'lightpurple'}, 
		{
			name = [[substr]],
			desc = [[ The sub-string to to search for. ]],
			color = 'lightpurple'}, 
		{
			name = [[start]],
			desc = [[ An optional number indicating where to start the search. Its default value is 1 and can be negative. ]],
			color = 'lightyellow'}, nil} },
	['string.similarity (str1, str2)'] = {
	desc = [[Calculate the similarity between two strings, using Levenshtein distance algorithm.]],
	addr = 'https:\\\\www.luart.org\\doc\\string\\similarity.html',
	params = {
		{
			name = [[str1, str2]],
			desc = [[ The strings to be evaluated for similiarity. ]],
			color = 'lightpurple'}, nil} },
	['string.upper(str)'] = {
	desc = [[Converts all lowercase characters in a string into uppercase characters.]],
	addr = 'https:\\\\www.luart.org\\doc\\string\\upper.html',
	params = {
		{
			name = [[str]],
			desc = [[ The string to be used to produce a string with uppercase characters. ]],
			color = 'lightpurple'}, nil} },
	['Buffer : append (var, encoding) method'] = {
	desc = [[Append binary data to the Buffer with the specified content.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Buffer-append.html',
	params = {
		{
			name = [[var]],
			desc = [[ A variable used to append data the Buffer. Only number, string and table values are permitted with the following behavior : 
        - number : Append the Buffer with the specified size. All the appended bytes are initialized to zero. 
        - string : Append the Buffer with the specified string. An optional encoding argument can specify how to interpret the string (see below) 
        - table : Append the Buffer with the specified table. The table must contain only sequential byte values (for example { 20, 33, 75 }) ]],
			color = 'brightwhite'}, 
		{
			name = [[encoding]],
			desc = [[ An optional string used to specify the encoding when appending the Buffer with a string value : 
        - "utf8" : Append the Buffer with a UTF8 string encoding, or as raw binary data (default) 
        - "unicode" : Append the Buffer with an UNICODE (UCS-2 LE) encoded string 
        - "base64" : Append the Buffer with a base64 encoded string 
        - "hex" : Append the Buffer with a hexadecimal encoded string ]],
			color = 'lightpurple'}, nil} },
	['Buffer : compress() method'] = {
	desc = [[Compress the Buffer to another binary Buffer, using the DEFLATE compression algorithm.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Buffer-compress.html',
	params = {} },
	['Buffer .. concatenation'] = {
	desc = [[Buffer concatenation allow to append two buffers to each other, or to concatenate a Buffer with a string (using default UTF8 encoding).The result of the concatenation is a third Buffer.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Buffer-concat.html',
	params = {} },
	['sys.Buffer (var, encoding) constructor'] = {
	desc = [[The Buffer constructor returns a Buffer value representing a sequence of raw bytes in memory, initialized with the var argument. The Buffer value can then be accessed, modified, resized, converted from/to string, compressed, decoded...Prefer to use Buffer instead of strings when manipulating binary data. Remember that Luart strings are UTF8 encoded.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Buffer-constructor.html',
	params = {
		{
			name = [[var]],
			desc = [[ A variable used to initialize the Buffer. Only number, string and table values are permitted with the following behavior : 
        - number : Initialize the Buffer with the specified size. All bytes are initialized to zero. 
        - string : Initialize the Buffer with the specified string. An optional encoding argument can specify how to decode the string (see below) 
        - table : Initialize the Buffer with the specified table. The table must contain only sequential byte values (for example { 20, 33, 75 }) ]],
			color = 'brightwhite'}, 
		{
			name = [[encoding]],
			desc = [[ An optional string used to specify the decoding method when initializing the Buffer with a string value : 
        - "utf8" : Initialize the Buffer with a UTF8 encoded string, or as raw binary data (default) 
        - "unicode" : Initialize the Buffer with an UNICODE (UCS-2 LE) encoded string 
        - "base64" : Initialize the Buffer with a base64 encoded string 
        - "hex" : Initialize the Buffer with a hexadecimal encoded string ]],
			color = 'lightpurple'}, nil} },
	['Buffer:contains(pattern)method'] = {
	desc = [[Checks if the Buffer contains the whole byte pattern.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Buffer-contains.html',
	params = {
		{
			name = [[pattern]],
			desc = [[ A string or Buffer value that contains the byte sequence to search for. ]],
			color = 'gray'}, nil} },
	['Buffer : decompress()method'] = {
	desc = [[Decompress the Buffer to another binary Buffer, using the DEFLATE decompression algorithm.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Buffer-decompress.html',
	params = {} },
	['Buffer : encode (encoding) method'] = {
	desc = [[Returns a string, encoded from the Buffer according to the provided encoding.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Buffer-encode.html',
	params = {
		{
			name = [[encoding]],
			desc = [[ A string that specifies the encoding method to convert the binary Buffer to string : 
        - "utf8" : uses the UTF8 encoding (default) 
        - "unicode" : uses the UNICODE (UCS-2 LE) encoding 
        - "base64" : uses base64 encoding 
        - "hex" : uses hexadecimal encoding ]],
			color = 'lightpurple'}, nil} },
	['Buffer == equality'] = {
	desc = [[Buffer equality allow to compare buffers. A buffer will be equal to another when their sizes and contents are the same.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Buffer-eq.html',
	params = {} },
	['Buffer : from (var, encoding) method'] = {
	desc = [[Reinitialize the Buffer with the specified content.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Buffer-from.html',
	params = {
		{
			name = [[var]],
			desc = [[ A variable used to initialize the Buffer. Only number, string and table values are permitted with the following behavior : 
        - number : Initialize the Buffer with the specified size. All bytes are initialized to zero. 
        - string : Initialize the Buffer with the specified string. An optional encoding argument can specify how to interpret the string (see below) 
        - table : Initialize the Buffer with the specified table. The table must contain only sequential byte values (for example { 20, 33, 75 }) ]],
			color = 'brightwhite'}, 
		{
			name = [[encoding]],
			desc = [[ An optional string used to specify the encoding when initializing the Buffer with a string value : 
        - "utf8" : Initialize the Buffer with a UTF8 string encoding, or as raw binary data (default) 
        - "unicode" : Initialize the Buffer with an UNICODE (UCS-2 LE) encoded string 
        - "base64" : Initialize the Buffer with a base64 encoded string 
        - "hex" : Initialize the Buffer with a hexadecimal encoded string ]],
			color = 'lightpurple'}, nil} },
	['Buffer indexation'] = {
	desc = [[Buffer indexing allow to get/set individual byte value.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Buffer-index.html',
	params = {} },
	['Bufferiterable'] = {
	desc = [[Buffers are iterable through the Luart helper function a href="../base/each.html">each, returning the next byte in the Buffer with each loop iteration. Each iteration returns a number containing the next byte.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Buffer-iterator.html',
	params = {} },
	['Buffer : pack (format, value1, value2,...) method'] = {
	desc = [[Reinitialize the Buffer with serialized values.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Buffer-pack.html',
	params = {
		{
			name = [[format]],
			desc = [[ A format string to specify the binary layout to serialize the values. See Lua programming reference "string.pack" for available sequence of conversions options. ]],
			color = 'lightpurple'}, 
		{
			name = [[values1, values2, ...]],
			desc = [[ The values to serialize in binary format. ]],
			color = 'brightwhite'}, nil} },
	['Buffer.size read/write property'] = {
	desc = [[The Buffer.size property get or set the current Buffer size. The property permits to extend or shrink the Buffer. The length operator # can be used to get the Buffer size]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Buffer-size.html',
	params = {} },
	['Buffer : sub (start, end) method'] = {
	desc = [[Extracts part of the Buffer like the function string:sub.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Buffer-sub.html',
	params = {
		{
			name = [[start]],
			desc = [[ An index of the Buffer defining the start of the sub-Buffer. That index can be negative (starting to count from the end of the Buffer). ]],
			color = 'lightyellow'}, 
		{
			name = [[end]],
			desc = [[ An optional index that defines where the sub-Buffer ends. That index can be negative (starting to count from the end of the Buffer). When that argument is absent, it is assumed to be equal to -1 (end of the Buffer). ]],
			color = 'lightyellow'}, nil} },
	['tostring(Buffer) string conversion'] = {
	desc = [[A Buffer can be converted to a string (with the default UTF8 decoder) using the standard Lua function tostring() Please use the Buffer:encode() function to convert the binary data with another string decoder]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Buffer-tostring.html',
	params = {} },
	['Buffer : unpack (format) method'] = {
	desc = [[Returns values serialized inside the Buffer, using the provided format.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Buffer-unpack.html',
	params = {
		{
			name = [[format]],
			desc = [[ A format string to specify the binary layout to serialize the values. See Lua programming reference "string.pack" for available sequence of conversions options. ]],
			color = 'lightpurple'}, nil} },
	['sys.Buffer'] = {
	desc = [[Buffer is an Object representation of a sequence of bytes in memory. It permits lower level access to memory with automatic garbage collection of the allocated memory.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\buffer.html',
	params = {} },
	['sys.clipboard read/write property'] = {
	desc = [[The sys.clipboard property retrieve/set a string from/to the Windows clipboard content. That property returns nil if the clipboard is empty.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\clipboard.html',
	params = {} },
	['sys.clock()'] = {
	desc = [[Returns the current elapsed time, that can be used for time-interval measurements.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\clock.html',
	params = {} },
	['sys.cmd(command, hidden)'] = {
	desc = [[Executes an operating-system command.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\cmd.html',
	params = {
		{
			name = [[command]],
			desc = [[ A string that contains the command to send to the command interpreter. ]],
			color = 'lightpurple'}, 
		{
			name = [[hidden]],
			desc = [[ An optional boolean value indicating if the command execute in a hidden window, defaulting to true. ]],
			color = 'lightyellow'}, nil} },
	['sys.currentdir read/write property'] = {
	desc = [[The sys.currentdir property get or set a string representing the current working directory.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\currentdir.html',
	params = {} },
	['Datetime > < == comparison'] = {
	desc = [[Datetime comparison allow to compare two Datetime values to each other. To compare two Datetime values, just use standard comparison operators <, =, or ==.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Datetime-comparison.html',
	params = {} },
	['Datetime .. concatenation'] = {
	desc = [[Datetime concatenation allow to concatenate a Datetime value with another string, using the default locale short format for both the date and time part.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Datetime-concat.html',
	params = {} },
	['sys.Datetime (moment) constructor'] = {
	desc = [[The Datetime constructor returns a Datetime value representing a moment, including a date part and a time part.The Datetime value is initialized with the moment argument. If that argument is omitted, the constructor returns the current local date and time. The Datetime value can then be accessed, modified, converted to string,... The format string depends on the current user locale, using the standard short format.For example, for the date part, the constructor use mm/dd/yyyy for English but dd/mm/yyyy for French localePlease note that using the Datetime constructor, the maximum allowed value for the year is 9999. If needed, you can change the year to a higher value with an assignment to the year property (which allows a maximum value of 30827)]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Datetime-constructor.html',
	params = {
		{
			name = [[moment]],
			desc = [[ An optional string used to specify the Datetime value, using the current short date format depending on the user current locale. ]],
			color = 'lightpurple'}, nil} },
	['Datetime.date readonly property'] = {
	desc = [[The Datetime.date property returns a string representing the "date part" of the moment, in the current local short format.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Datetime-date.html',
	params = {} },
	['Datetime.day read/write property'] = {
	desc = [[The Datetime.day property returns a number representing the day of the month. Valid values are from 1 through 31. Setting the day property with a value lower or higher will throw an error.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Datetime-day.html',
	params = {} },
	['Datetime.dayname readonly property'] = {
	desc = [[The Datetime.dayname property returns a string representing the name of the day of the moment, in the current local.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Datetime-dayname.html',
	params = {} },
	['Datetime : format (dateformat, timeformat) method'] = {
	desc = [[Formats the Datetime value as a string.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Datetime-format.html',
	params = {
		{
			name = [[dateformat]],
			desc = [[ An optional string representing the format for the date part. For only formatting the time part, use nil in place. The format tokens accepted are : 
        - Year : "y" or "yy" for short format of the Year, and "yyyy" for the long format. 
        - Month :"M" or "MM" for short format of the Month, and "MMMM" for the long format. 
        - Day : "d" or "dd" for short format of the Day, and "dddd" for the long format. ]],
			color = 'lightpurple'}, 
		{
			name = [[timeformat]],
			desc = [[ An optional string representing the format for the time part. For only formatting the date part, omit that argument. The format tokens accepted are : 
        - Hours (12-hour clock): "h", or "hh" with a leading zero. 
        - Hours (24-hour clock): "H, or "HH" with aleading zero. 
        - Minutes :"m" or "mm" with a leading zero. 
        - Seconds : "s" or "ss" with a leading zero 
        - Time marker : "t or "tt" for AM/PM time marker. ]],
			color = 'lightpurple'}, nil} },
	['Datetime.hour read/write property'] = {
	desc = [[The Datetime.hour property returns a number representing the hour of the moment. Valid values are from 0 through 23. Setting the hour property with a value lower or higher will throw an error.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Datetime-hour.html',
	params = {} },
	['Datetime : interval (from, expressed) method'] = {
	desc = [[Returns an interval from the from value and the Datetime value, expressed by default in days.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Datetime-interval.html',
	params = {
		{
			name = [[from]],
			desc = [[ A Datetime value from where the interval will be calculated, to the current Datetime value. ]],
			color = 'brightwhite'}, 
		{
			name = [[expressed]],
			desc = [[ An optional string that indicates how the interval will be expressed (by default, in days). The following strings are accepted : 
        - "seconds" 
        - "minutes" 
        - "hours" 
        - "days" 
        - "months" 
        - "years" ]],
			color = 'lightpurple'}, nil} },
	['Datetime.milliseconds read/write property'] = {
	desc = [[The Datetime.milliseconds property returns a number representing the milliseconds of the moment. Valid values are from 0 through 999. Setting the milliseconds property with a value lower or higher will throw an error.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Datetime-milliseconds.html',
	params = {} },
	['Datetime.minute read/write property'] = {
	desc = [[The Datetime.minute property returns a number representing the minutes of the moment. Valid values are from 0 through 59. Setting the minute property with a value lower or higher will throw an error.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Datetime-minute.html',
	params = {} },
	['Datetime.month read/write property'] = {
	desc = [[The Datetime.month property returns a number representing the month of the moment. Valid values are from 1 through 12. Setting the month property with a value lower or higher will throw an error. 
        - January 
        - February 
        - March 
        - April 
        - May 
        - June 
        - July 
        - August 
        - September 
        - October 
        - November 
        - December]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Datetime-month.html',
	params = {} },
	['Datetime.monthname readonly property'] = {
	desc = [[The Datetime.monthname property returns a string representing the name of the month of the moment, in the current local.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Datetime-monthname.html',
	params = {} },
	['Datetime.second read/write property'] = {
	desc = [[The Datetime.second property returns a number representing the second of the moment. Valid values are from 0 through 59. Setting the second property with a value lower or higher will throw an error.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Datetime-second.html',
	params = {} },
	['Datetime.time readonly property'] = {
	desc = [[The Datetime.time property returns a string representing the "time part" of the moment, in the current local short format.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Datetime-time.html',
	params = {} },
	['tostring(Datetime) string conversion'] = {
	desc = [[A Datetime can be converted to a string (with the default locale user short format for both the date and time part) using the standard Lua function tostring() Please use the Datetime:format() function to get better control on the formatting.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Datetime-tostring.html',
	params = {} },
	['Datetime.weekday read/write property'] = {
	desc = [[The Datetime.weekday property returns a number representing the day of the week. Valid values are from 1 through 7. 
        - Sunday 
        - Monday 
        - Tuesday 
        - Wednesday 
        - Thursday 
        - Friday 
        - Saturday Setting the weekday property with a value lower or higher will throw an error.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Datetime-weekday.html',
	params = {} },
	['Datetime.year read/write property'] = {
	desc = [[The Datetime.year property returns a number representing the year of the moment. Valid values are from 1601 through 30827. Setting the year property with a value lower or higher will throw an error.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Datetime-year.html',
	params = {} },
	['sys.Datetime'] = {
	desc = [[Datetime is an Object representation of a moment, a combination of a date and a time.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\datetime.html',
	params = {} },
	['Directory.accessed read/write property'] = {
	desc = [[The Directory.accessed property set or get a Datetime value indicating when the folder has been last accessed. The folder must exist physically on the disk.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Directory-accessed.html',
	params = {} },
	['sys.Directory (path) constructor'] = {
	desc = [[The Directory constructor returns a Directory value representing a folder on the specified path.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Directory-constructor.html',
	params = {} },
	['Directory.created read/write property'] = {
	desc = [[The Directory.created property set or get a Datetime value indicating when the folder has been created. The folder must exist physically on the disk.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Directory-created.html',
	params = {} },
	['Directory.exists readonly property'] = {
	desc = [[The Directory.exists property returns a boolean value indicating if the folder exists physically on the drive or not.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Directory-exists.html',
	params = {} },
	['Directory.fullpath readonly property'] = {
	desc = [[The Directory.fullpath property returns a string representing the full path to the folder. The fullpath is set during the call to the Directory constructor, relative to the current working directory at this time.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Directory-fullpath.html',
	params = {} },
	['Directory.hidden read/write property'] = {
	desc = [[The Directory.hidden property set or get a boolean value indicating if the folder is hidden or not. The folder must exist physically on the disk.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Directory-hidden.html',
	params = {} },
	['Directoryiterable'] = {
	desc = [[Directory values are iterable through the Luart helper function each, returning the next entry in the folder with each loop iteration. The folder must exist to iterate through its content. Each iteration returns either a File or a Directory value.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Directory-iterator.html',
	params = {} },
	['Directory : list (filter) method iterable'] = {
	desc = [[Filters the folder content and returns an iterator.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Directory-list.html',
	params = {
		{
			name = [[filter]],
			desc = [[ A string used to filter folder content. The string can include wildcard characters : an asterisk (*) for one or more characters march or a question mark (?) for single character match. ]],
			color = 'lightpurple'}, nil} },
	['Directory : make() method'] = {
	desc = [[Create a folder physically on the disk if it doesn't exists.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Directory-make.html',
	params = {} },
	['Directory.modified read/write property'] = {
	desc = [[The Directory.modified property set or get a Datetime value indicating when the folder has been last modified. The folder must exist physically on the disk.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Directory-modified.html',
	params = {} },
	['Directory : move (path) method'] = {
	desc = [[Moves the folder physically on the disk and all contained files and subdirectories, to the specified path.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Directory-move.html',
	params = {
		{
			name = [[path]],
			desc = [[ A string representing the destination path. ]],
			color = 'lightpurple'}, nil} },
	['Directory.name readonly property'] = {
	desc = [[The Directory.name property returns a string representing the folder name (the last part of the fullpath).]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Directory-name.html',
	params = {} },
	['Directory.parent readonly property'] = {
	desc = [[The Directory.parent property returns a Directory representing the parent folder. If the folder is already a root folder, the property returns nil]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Directory-parent.html',
	params = {} },
	['Directory : remove() method'] = {
	desc = [[Deletes the folder physically on the disk if the folder is empty.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Directory-remove.html',
	params = {} },
	['Directory : removeall() method'] = {
	desc = [[Deletes the folder physically on the disk and all contained files and subdirectories.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Directory-removeall.html',
	params = {} },
	['sys.Directory'] = {
	desc = [[Directory is an Object representation of a folder, that may exist or not.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\directory.html',
	params = {} },
	['sys.env readonly property'] = {
	desc = [[Get a table representing environment variables.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\env.html',
	params = {} },
	['sys.error readonly property'] = {
	desc = [[The sys.error property returns a string representing the last Windows error message or nil if no error occured.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\error.html',
	params = {} },
	['sys.exit(status)'] = {
	desc = [[Terminates the program.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\exit.html',
	params = {
		{
			name = [[status]],
			desc = [[ An optionnal number returned to the operating system. Default value is zero. By convention, a value of zero means that the program completed successfully. A value of -1 means that an error occured. ]],
			color = 'lightyellow'}, nil} },
	['File.accessed read/write property'] = {
	desc = [[The File.accessed property set or get a Datetime value indicating when the file has been last accessed. The file must exist physically on the disk. If the file do not exist, this property returns nil and setting its value will have no effect.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\File-accessed.html',
	params = {} },
	['File : close() method'] = {
	desc = [[Closes the file previously opened for read/write operations. Any further read/write operations will not work.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\File-close.html',
	params = {} },
	['sys.File (filename) constructor'] = {
	desc = [[The File constructor returns a File value, representing an abstraction of a file (that may or may not exist).]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\File-constructor.html',
	params = {} },
	['File : copy(filename) method'] = {
	desc = [[Copies the file physically on the disk to the specified filename.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\File-copy.html',
	params = {
		{
			name = [[filename]],
			desc = [[ An optional string representing the destination filename, or, use the same filename if omitted. ]],
			color = 'lightpurple'}, nil} },
	['File.created read/write property'] = {
	desc = [[The File.created property set or get a Datetime value indicating when the file has been created. The file must exist physically on the disk. If the file do not exist, this property returns nil and setting its value will have no effect.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\File-created.html',
	params = {} },
	['File.directory readonly property'] = {
	desc = [[The File.directory property returns a Directory representing the folder that contains the file.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\File-directory.html',
	params = {} },
	['File.encoding readonly property'] = {
	desc = [[The File.encoding property returns the current file encoding. File should have been opened before to determine it's encoding or the property defaults to "binary" otherwise. File encoding is one of those strings :
        - "binary" encoding : the File has no encoding, using raw bytes for read/write operations.
        - "utf8" encoding : the File is UTF8 encoded, using UTF8 characters for read/write operations.
        - "unicode" encoding : the File is UNICODE (UCS-2 LE) encoded, using UNICODE characters for read/write operations.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\File-encoding.html',
	params = {} },
	['File.eof readonly property'] = {
	desc = [[The File.eof property returns a boolean value indicating if the File position has reached the end of file or not. The file must have been opened before using the property.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\File-eof.html',
	params = {} },
	['File.exists readonly property'] = {
	desc = [[The File.exists property returns a boolean value indicating if the File exists physically on the drive or not.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\File-exists.html',
	params = {} },
	['File.extension readonly property'] = {
	desc = [[The File.extension property returns a string representing the extension of the File, including the "." character.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\File-extension.html',
	params = {} },
	['File : flush() method'] = {
	desc = [[Ensures that all recently written data is saved in the file.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\File-flush.html',
	params = {} },
	['File.fullpath readonly property'] = {
	desc = [[The File.fullpath property returns a string representing the full path of the File. The fullpath is set during the call to the File constructor, relative to the current working directory at this time.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\File-fullpath.html',
	params = {} },
	['File.hidden read/write property'] = {
	desc = [[The File.hidden property set or get a boolean value indicating if the File is hidden or not. The file must exist physically on the disk.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\File-hidden.html',
	params = {} },
	['File.lines readonly property iterable'] = {
	desc = [[The File.lines iterator returns the next line in the File with each loop iteration. File should have been opened before in "read" mode. The line omits the leading end of line character marker.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\File-lines.html',
	params = {} },
	['File.modified read/write property'] = {
	desc = [[The File.modified property set or get a Datetime value indicating when the file has been last modified. The file must exist physically on the disk. If the file do not exist, this property returns nil and setting its value will have no effect.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\File-modified.html',
	params = {} },
	['File : move (path) method'] = {
	desc = [[Moves the file physically on the disk to the specified path.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\File-move.html',
	params = {
		{
			name = [[path]],
			desc = [[ A string representing the destination path. ]],
			color = 'lightpurple'}, nil} },
	['File.name readonly property'] = {
	desc = [[The File.name property returns a string representing the file name and extension of the File.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\File-name.html',
	params = {} },
	['File : open (mode, encoding) method'] = {
	desc = [[Open the file for read/write operations.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\File-open.html',
	params = {
		{
			name = [[mode]],
			desc = [[ An optional string that indicates the kind of access when opening the file. 
        - "read": the file will be opened for read operations, it's the default if neither mode nor encoding are specified.
        - "write": the file will be opened for write operations, erasing any data already present.
        - "append": the file will be opened for write operations, keeping the current data and starting writing at the end.
        - "read/write": the file will be opened for read and write operations. ]],
			color = 'lightpurple'}, 
		{
			name = [[encoding]],
			desc = [[ An optional string that indicates the encoding for read/write operations. By default, encoding is autodetected using Byte Mark Order (BOM) if not provided. 
        - "binary" encoding : the file has no encoding, using raw bytes for read/write operations.
        - "utf8" encoding : the File is UTF8 encoded, using UTF8 characters for read/write operations.
        - "unicode" encoding : the File is UNICODE (UCS-2 LE) encoded, using UNICODE characters for read/write operations. In "write" mode, Byte Order Mark (BOM) is written to the file when "utf8" or "unicode" encoding is provided. ]],
			color = 'lightpurple'}, nil} },
	['File.parent readonly property'] = {
	desc = [[The File.parent property returns a Directory representing the folder that contains the file. If the folder is already a root folder, the property returns nil]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\File-parent.html',
	params = {} },
	['File.path readonly property'] = {
	desc = [[The File.path property returns a string representing the path of the file (fullpath without the filename). The path is set during the call to the File constructor, relative to the current working directory at this time.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\File-path.html',
	params = {} },
	['File.position read/write property'] = {
	desc = [[The File.position property get or set the current file position, as a number. The file must have been opened before using this property. File position always starts at 1. File position interpretation depends on the file encoding.
        - In "binary" encoding : the position is expressed in bytes.
        - In "utf8" or "unicode" encoding : the position is expressed in characters (and you don't have to care about BOM).]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\File-position.html',
	params = {} },
	['File : read (length) method'] = {
	desc = [[Reads data from the file.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\File-read.html',
	params = {
		{
			name = [[length]],
			desc = [[ An optional number indicating the number of characters (or bytes in binary encoding) to read.When no value is provided, the method read all data from the current position until the end of file is reached. ]],
			color = 'lightyellow'}, nil} },
	['File : readln() method'] = {
	desc = [[Reads data from the file until end of line is encountered.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\File-readln.html',
	params = {} },
	['File.readonly read/write property'] = {
	desc = [[The File.readonly property set or get a boolean value indicating if the File is readonly or not. The file must exist physically on the disk.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\File-readonly.html',
	params = {} },
	['File : remove() method'] = {
	desc = [[Deletes the file physically on the disk.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\File-remove.html',
	params = {} },
	['File.temporary read/write property'] = {
	desc = [[The File.temporary property set or get a boolean value indicating if the File is considered temporary or not (meaning that cache data will not be written to disk once the File is closed). The file must exist physically on the disk.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\File-temporary.html',
	params = {} },
	['File : write (data) method'] = {
	desc = [[Writes data to the file.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\File-write.html',
	params = {
		{
			name = [[data]],
			desc = [[ A variable converted to a string value to be written to the file. ]],
			color = 'lightpurple'}, nil} },
	['File : writeln (data) method'] = {
	desc = [[Writes data to the file, adding a terminal end of line character.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\File-writeln.html',
	params = {
		{
			name = [[data]],
			desc = [[ A variable converted to a string value to be written to the file. ]],
			color = 'lightpurple'}, nil} },
	['sys.File'] = {
	desc = [[File is an Object representation of a file that may, or may not physically exists.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\file.html',
	params = {} },
	['sys.halt(mode)'] = {
	desc = [[Shutdown the operating system.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\halt.html',
	params = {
		{
			name = [[mode]],
			desc = [[ An optional string that indicates the shutdown method to use. If this parameter is not specified, the default value "shutdown" is used. "logoff" : close the current Windows session "shutdown" : shutdown the computer "reboot" : reboot the computer "force" : do not wait for other applications to terminate ]],
			color = 'lightpurple'}, nil} },
	['sys module'] = {
	desc = [[The sys module provides specific Objects, properties and functions to interact with the operating system. Note that the sys module is preloaded by default (no need to require it).]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\index.html',
	params = {} },
	['Pipe : close() method'] = {
	desc = [[Closes the Pipe and the associated process. Any further read/write/readerror operations won't achieve.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Pipe-close.html',
	params = {} },
	['sys.Pipe (cmd) constructor'] = {
	desc = [[The Pipe constructor executes a process and returns a Pipe value to interact with it.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Pipe-constructor.html',
	params = {
		{
			name = [[cmd]],
			desc = [[ A string representing the command to execute. ]],
			color = 'lightpurple'}, nil} },
	['Pipe : read (delay) method'] = {
	desc = [[Reads from the Pipe (from the standard output of the executed process).]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Pipe-read.html',
	params = {
		{
			name = [[delay]],
			desc = [[ An optional number representing the time to wait in milliseconds, before reading the Pipe. ]],
			color = 'lightyellow'}, nil} },
	['Pipe : readerror (delay) method'] = {
	desc = [[Reads an error message from the Pipe (from the standard error of the executed process).]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Pipe-readerror.html',
	params = {
		{
			name = [[delay]],
			desc = [[ An optional number representing the time to wait in milliseconds, before reading the Pipe. ]],
			color = 'lightyellow'}, nil} },
	['Pipe : write (data) method'] = {
	desc = [[Writes data to the Pipe (into the standard input of the executed process).]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\Pipe-write.html',
	params = {
		{
			name = [[data]],
			desc = [[ A variable converted to a string value to be written to the Pipe. ]],
			color = 'lightpurple'}, nil} },
	['sys.Pipe'] = {
	desc = [[Pipe is an Object representation of inter-process communication when executing a command. It gives easy redirection of the executed process standard output, standard input and standard error.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\pipe.html',
	params = {} },
	['sys.registry.delete(rootkey, subkey, value)'] = {
	desc = [[Delete a value or a subkey from the Windows registry.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\registry-delete.html',
	params = {
		{
			name = [[rootkey]],
			desc = [[ The root registry key that contains the subkey, must be one of the following string values : 
        - "HKEY_CLASSES_ROOT" : contains data related to applications, shortcuts, and file extension associations 
        - "HKEY_CURRENT_CONFIG" : contains the current hardware configuration settings 
        - "HKEY_CURRENT_USER" : contains data specific to each user with a logon account 
        - "HKEY_LOCAL_MACHINE" : contains all the computer-specific information about the hardware installed, software settings, and other information 
        - "HKEY_USERS" : contains information about all the users who log on to the computer ]],
			color = 'lightpurple'}, 
		{
			name = [[subkey]],
			desc = [[ A string that contains the path to the sub key. ]],
			color = 'lightpurple'}, nil} },
	['sys.registry.read(rootkey, subkey, value, expand)'] = {
	desc = [[Read a value from the given key in the Windows registry.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\registry-read.html',
	params = {
		{
			name = [[rootkey]],
			desc = [[ The root registry key that contains the subkey, must be one of the following string values : 
        - "HKEY_CLASSES_ROOT" : contains data related to applications, shortcuts, and file extension associations 
        - "HKEY_CURRENT_CONFIG" : contains the current hardware configuration settings 
        - "HKEY_CURRENT_USER" : contains data specific to each user with a logon account 
        - "HKEY_LOCAL_MACHINE" : contains all the computer-specific information about the hardware installed, software settings, and other information 
        - "HKEY_USERS" : contains information about all the users who log on to the computer ]],
			color = 'lightpurple'}, 
		{
			name = [[subkey]],
			desc = [[ A string that contains the path to the wanted sub key. ]],
			color = 'lightpurple'}, 
		{
			name = [[value]],
			desc = [[ An optional string that contains the name of the value that we want to retrieve from the registry. If omitted or nil or empty, retrieve the unnamed value associated with the key. ]],
			color = 'lightpurple'}, 
		{
			name = [[expand]],
			desc = [[ An optional boolean value. If set to true and the data read from the registry represents a string, all references to environment variables will be expanded. References to environment variables are between % chars. ]],
			color = 'lightyellow'}, nil} },
	['sys.registry.write(rootkey, subkey, value | nil, data, expand)'] = {
	desc = [[Write a value to the given key in the Windows registry.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\registry-write.html',
	params = {
		{
			name = [[rootkey]],
			desc = [[ The root registry key that contains the subkey, must be one of the following string values : 
        - "HKEY_CLASSES_ROOT" : contains data related to applications, shortcuts, and file extension associations 
        - "HKEY_CURRENT_CONFIG" : contains the current hardware configuration settings 
        - "HKEY_CURRENT_USER" : contains data specific to each user with a logon account 
        - "HKEY_LOCAL_MACHINE" : contains all the computer-specific information about the hardware installed, software settings, and other information 
        - "HKEY_USERS" : contains information about all the users who log on to the computer ]],
			color = 'lightpurple'}, 
		{
			name = [[subkey]],
			desc = [[ A string that contains the path to the wanted sub key. ]],
			color = 'lightpurple'}, 
		{
			name = [[A string that contains the name of the value that we want to write in the registry. If nil, writes the unnamed value associated with the key. data]],
			desc = [[ The data to be written to the registry. Can be any of the following values : 
        - string : writes the string content in the registry. 
        - number : writes the number as a double-word or quad-word in the registry. 
        - Buffer : writes all the buffer as binary data in the registry 
        - table : writes the strings in the table as a multi-string value in the registry. 
        - nil : writes a nil value in the registry. ]],
			color = 'brightwhite'}, 
		{
			name = [[expand]],
			desc = [[ An optional boolean value. If set to true and the data to write to the registry is a string, all references to environment variables will be expanded once the value is read. References to environment variables are between % chars. ]],
			color = 'lightyellow'}, nil} },
	[''] = {
	desc = [[]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\result.html',
	params = {} },
	['sys.sleep(delay)'] = {
	desc = [[Suspend the program execution until the delay elapse.]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\sleep.html',
	params = {
		{
			name = [[delay]],
			desc = [[ The delay for which program execution will be suspended, in milliseconds. Delay is optional, with a default 1 millisecond program suspension. Please note that the use of the sleep function may alter user experience in luart desktop applications beacause ui events are not fired until the delay elapses. ]],
			color = 'lightyellow'}, nil} },
	['sys.tempfile(prefix)'] = {
	desc = [[Returns a temporary File Object]],
	addr = 'https:\\\\www.luart.org\\doc\\sys\\tempfile.html',
	params = {
		{
			name = [[prefix]],
			desc = [[ An optional string that contains the first 3 letters of the temporary filename. ]],
			color = 'lightpurple'}, nil} },
	['Button:align(alignment) method'] = {
	desc = [[Change Button position and size to be aligned relative to its parent.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Button-align.html',
	params = {
		{
			name = [[alignment]],
			desc = [[ A string that specifies the Button alignment : 
        - "all" : the Button will be aligned along all the parent borders (and will cover the entire parent client area). 
        - "bottom" : the Button will be aligned along the bottom border of the parent, preserving its height. 
        - "top" : the Button will be aligned along the the top border of the parent, preserving its height. 
        - "right" : the Button will be aligned along the right border of the parent, preserving its width. 
        - "left" : the Button will be aligned along the left border of the parent, preserving its width. ]],
			color = 'lightpurple'}, nil} },
	['Button:autosize() method'] = {
	desc = [[Resize the Button automatically to accommodate its content.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Button-autosize.html',
	params = {} },
	['Button:center() method'] = {
	desc = [[Centers the Button on the parent widget.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Button-center.html',
	params = {} },
	['Button.constructor(parent, caption, x , y , width , height) constructor'] = {
	desc = [[The Button constructor returns a Button value representing a push button on the parent object.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Button-constructor.html',
	params = {
		{
			name = [[parent]],
			desc = [[ An object that will own the Button. Parent objects can be any of Window, Groupbox and Tab ]],
			color = 'brightwhite'}, 
		{
			name = [[caption]],
			desc = [[ A string representing the Button's text. ]],
			color = 'lightpurple'}, 
		{
			name = [[x]],
			desc = [[ An optional number that indicates the Button horizontal position, in pixels. Zero means the left border of the parent. ]],
			color = 'lightyellow'}, 
		{
			name = [[y]],
			desc = [[ An optional number that indicates the Button vertical position, in pixels. Zero means the top border of the parent. ]],
			color = 'lightyellow'}, 
		{
			name = [[width]],
			desc = [[ An optional number that indicates the Button width in pixels, autosized to fit text content if omitted. ]],
			color = 'lightyellow'}, 
		{
			name = [[height]],
			desc = [[ An optional number that indicates the Button height in pixels, autosized to fit text content if omitted. ]],
			color = 'lightyellow'}, nil} },
	['Button.cursor read/write property'] = {
	desc = [[Get or set the image of the mouse cursor as it hovers over the Button. Mouse cursors are represented by strings : 
        - "arrow" : the default mouse cursor. 
        - "cross" : a crosshair cursor. 
        - "working" : the standard arrow with a small hourglass. 
        - "hand" : a hand cursor. 
        - "help" : the standard arrow with a small question mark. 
        - "ibeam" : the default I-Beam text cursor. 
        - "forbidden" : a slashed circle cursor. 
        - "cardinal" : a four-pointed arrow pointing up, down, right, and left. 
        - "horizontal" : a double-pointed arrow pointing right and left. 
        - "vertical" : a double-pointed arrow pointing up and down. 
        - "leftdiagonal" : a double-pointed arrow pointing topright down to the left. 
        - "rightdiagonal" : a double-pointed arrow pointing topleft down to the right. 
        - "up" : an arrow pointing up. 
        - "wait" : a hourglass cursor. 
        - "none" : no cursor is displayed. The button.cursor property affects only the Button area.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Button-cursor.html',
	params = {} },
	['Button.enabled read/write property'] = {
	desc = [[Get or set the button ability to respond to mouse, and any other events. When set to false, disable the Button (the user will not be able to interact with it), and button's events won't be fired anymore.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Button-enabled.html',
	params = {} },
	['Button.font read/write property'] = {
	desc = [[Get the Button font, a string value that represent the font name. To set the Button.font property, you can assign a string that represent either a font name of an installed system font, or a font file (*.ttf, *.fon...). A File can also be provided, representing a font file. Note that only the font family is changed. The font style and font size are not affected (see the Button.fontstyle and the Button.fontsize properties). By default, a Button uses the same font as its parent widget Any change in the Button.font, Button.fontstyle and Button.fontsize properties results in an automatic resizing of the Button so that the content is displayed correctly]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Button-font.html',
	params = {} },
	['Button.fontsize read/write property'] = {
	desc = [[Get or set the button font size, a number that represent the font size in pixels. Any change in the Button.font, Button.fontstyle and Button.fontsize properties results in an automatic resizing of the Button so that the content is displayed correctly]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Button-fontsize.html',
	params = {} },
	['Button.fontstyle read/write property'] = {
	desc = [[Get or set the button font style, a table value that contains the following keys/values : 
        - "italic" : set to true if the font is in italic. 
        - "underline" : set to true if the font is underlined. 
        - "strike" : set to true if the font is striked. 
        - "thin" : set to true if the font is thin. 
        - "bold" : set to true if the font is bold. 
        - "heavy" : set to true if the font is heavy. Any change in the Button.font, Button.fontstyle and Button.fontsize properties results in an automatic resizing of the Button so that the content is displayed correctly]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Button-fontstyle.html',
	params = {} },
	['Button.height read/write property'] = {
	desc = [[Get or set the height of the Button area. The height starts from 0 (top side of the Button) and increase to the bottom direction.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Button-height.html',
	params = {} },
	['Button:hide() method'] = {
	desc = [[Hide and deactivate the Button (events can no longer be fired).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Button-hide.html',
	params = {} },
	['Button:loadicon(path, index) method'] = {
	desc = [[Loads a Button icon, displayed to the left of the Button caption.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Button-loadicon.html',
	params = {
		{
			name = [[path]],
			desc = [[ Represent any of the following possible icon locations : 
        - A string which represents the path to an ".ico" icon file, or gets the icon associated with the provided file/directory. 
        - A Widget object, whose icon will be used by the Button. 
        - A Directory or File object, representing an ".ico" file, or gets the icon associated with the provided file/directory. 
        - A Buffer object, whose binary content will represent the icon. ]],
			color = 'brightwhite'}, 
		{
			name = [[index]],
			desc = [[ The icon index, a optional number starting from 1, that represent the icon to select. ]],
			color = 'lightyellow'}, nil} },
	['Button:onClick() event'] = {
	desc = [[This event is fired when the user has clicked on the Button.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Button-onclick.html',
	params = {} },
	['Button:onContext() event'] = {
	desc = [[This event is fired when the user has clicked on the Button with the right mouse button.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Button-oncontext.html',
	params = {} },
	['Button:onCreate() event'] = {
	desc = [[This event is fired when the Button object has just been created (just after the Button:constructor() call). This event is particularly interesting when you want to inherit from a widget object, or to initialize its properties.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Button-oncreate.html',
	params = {} },
	['Button:onHide() event'] = {
	desc = [[This event is fired when the Button is hidden (with a call to Button:hide() or setting the Button.visible property to false).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Button-onhide.html',
	params = {} },
	['Button:onHover (x, y) event'] = {
	desc = [[This event is fired when the user moves the mouse pointer over the Button.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Button-onhover.html',
	params = {
		{
			name = [[x]],
			desc = [[ The horizontal position of the mouse in the Button area (zero means the left border of the widget). ]],
			color = 'lightyellow'}, 
		{
			name = [[y]],
			desc = [[ The vertical position of the mouse in the Button area (zero means the top border of the widget). ]],
			color = 'lightyellow'}, nil} },
	['Button:onLeave() event'] = {
	desc = [[This event is fired when the mouse cursor leaves the Button.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Button-onleave.html',
	params = {} },
	['Button:onShow() event'] = {
	desc = [[This event is fired when the Button is shown (with a call to Button:show() or setting the Button.visible property to true).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Button-onshow.html',
	params = {} },
	['Button.parent readonly property'] = {
	desc = [[Get the parent widget that owns the Button.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Button-parent.html',
	params = {} },
	['Button:show() method'] = {
	desc = [[Show and activate the Button (events can now be fired).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Button-show.html',
	params = {} },
	['Button.text read/write property'] = {
	desc = [[Get or set the Button caption as a string.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Button-text.html',
	params = {} },
	['Button.tooltip read/write property'] = {
	desc = [[Get or set the tooltip text that appears when the user moves the mouse over the control, represented as a string value. Set the property to an empty string to remove the tooltip message.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Button-tooltip.html',
	params = {} },
	['Button.visible read/write property'] = {
	desc = [[Get or set the button visibility on screen, a true value means that the Button is shown, a false value means that the Button is hidden. The button.visible property is also affected by the Button:show() and Button:hide() methods.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Button-visible.html',
	params = {} },
	['Button.width read/write property'] = {
	desc = [[Get or set the Button area width. The width starts from 0 (left side of the Button) and increases to the right direction.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Button-width.html',
	params = {} },
	['Button.x read/write property'] = {
	desc = [[Get or set the Button horizontal position. The horizontal position start from 0 (left side of the parent widget) and increase to the right (right side of the parent widget).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Button-x.html',
	params = {} },
	['Button.y read/write property'] = {
	desc = [[Get or set the Button vertical position. The vertical position start from 0 (top side of the parent widget) and increase to the bottom side of the parent widget.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Button-y.html',
	params = {} },
	['ui.Button'] = {
	desc = [[Button is an Object representation of a push button.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\button.html',
	params = {} },
	['Calendar:align(alignment) method'] = {
	desc = [[Change Calendar position and size to be aligned relative to its parent.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Calendar-align.html',
	params = {
		{
			name = [[alignment]],
			desc = [[ A string that specifies the Calendar alignment : 
        - "all" : the Calendar will be aligned along all the parent borders (and will cover the entire parent client area). 
        - "bottom" : the Calendar will be aligned along the bottom border of the parent, preserving its height. 
        - "top" : the Calendar will be aligned along the the top border of the parent, preserving its height. 
        - "right" : the Calendar will be aligned along the right border of the parent, preserving its width. 
        - "left" : the Calendar will be aligned along the left border of the parent, preserving its width. ]],
			color = 'lightpurple'}, nil} },
	['Calendar:center() method'] = {
	desc = [[Centers the Calendar on the parent widget.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Calendar-center.html',
	params = {} },
	['Calendar.constructor(parent, x , y , width , height) constructor'] = {
	desc = [[The Calendar constructor returns a Calendar value representing a month calendar to choose a date.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Calendar-constructor.html',
	params = {
		{
			name = [[parent]],
			desc = [[ An object that will own the Calendar. Parent objects can be any of Window, Groupbox and Tab ]],
			color = 'brightwhite'}, 
		{
			name = [[x]],
			desc = [[ An optional number that indicates the Calendar horizontal position, in pixels. Zero means the left border of the parent. ]],
			color = 'lightyellow'}, 
		{
			name = [[y]],
			desc = [[ An optional number that indicates the Calendar vertical position, in pixels. Zero means the top border of the parent. ]],
			color = 'lightyellow'}, 
		{
			name = [[width]],
			desc = [[ An optional number that indicates the Calendar width in pixels, autosized to fit text content if omitted. ]],
			color = 'lightyellow'}, 
		{
			name = [[height]],
			desc = [[ An optional number that indicates the Calendar height in pixels, autosized to fit text content if omitted. ]],
			color = 'lightyellow'}, nil} },
	['Calendar.cursor read/write property'] = {
	desc = [[Get or set the image of the mouse cursor as it hovers over the Calendar. Mouse cursors are represented by strings : 
        - "arrow" : the default mouse cursor. 
        - "cross" : a crosshair cursor. 
        - "working" : the standard arrow with a small hourglass. 
        - "hand" : a hand cursor. 
        - "help" : the standard arrow with a small question mark. 
        - "ibeam" : the default I-Beam text cursor. 
        - "forbidden" : a slashed circle cursor. 
        - "cardinal" : a four-pointed arrow pointing up, down, right, and left. 
        - "horizontal" : a double-pointed arrow pointing right and left. 
        - "vertical" : a double-pointed arrow pointing up and down. 
        - "leftdiagonal" : a double-pointed arrow pointing topright down to the left. 
        - "rightdiagonal" : a double-pointed arrow pointing topleft down to the right. 
        - "up" : an arrow pointing up. 
        - "wait" : a hourglass cursor. 
        - "none" : no cursor is displayed. The calendar.cursor property affects only the Calendar area.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Calendar-cursor.html',
	params = {} },
	['Calendar.enabled read/write property'] = {
	desc = [[Get or set the calendar ability to respond to mouse, and any other events. When set to false, disable the Calendar (the user will not be able to interact with it), and calendar's events won't be fired anymore.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Calendar-enabled.html',
	params = {} },
	['Calendar.height read/write property'] = {
	desc = [[Get or set the height of the Calendar area. The height starts from 0 (top side of the Calendar) and increase to the bottom direction.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Calendar-height.html',
	params = {} },
	['Calendar:hide() method'] = {
	desc = [[Hide and deactivate the Calendar (events can no longer be fired).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Calendar-hide.html',
	params = {} },
	['Calendar:onCreate() event'] = {
	desc = [[This event is fired when the Calendar object has just been created (just after the Calendar:constructor() call). This event is particularly interesting when you want to inherit from a widget object, or to initialize its properties.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Calendar-oncreate.html',
	params = {} },
	['Calendar:onHide() event'] = {
	desc = [[This event is fired when the Calendar is hidden (with a call to Calendar:hide() or setting the Calendar.visible property to false).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Calendar-onhide.html',
	params = {} },
	['Calendar:onHover (x, y) event'] = {
	desc = [[This event is fired when the user moves the mouse pointer over the Calendar.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Calendar-onhover.html',
	params = {
		{
			name = [[x]],
			desc = [[ The horizontal position of the mouse in the Calendar area (zero means the left border of the widget). ]],
			color = 'lightyellow'}, 
		{
			name = [[y]],
			desc = [[ The vertical position of the mouse in the Calendar area (zero means the top border of the widget). ]],
			color = 'lightyellow'}, nil} },
	['Calendar:onLeave() event'] = {
	desc = [[This event is fired when the mouse cursor leaves the Calendar.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Calendar-onleave.html',
	params = {} },
	['Calendar:onSelect(date) event'] = {
	desc = [[This event is fired when the user has choosen a date on the Calendar.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Calendar-onselect.html',
	params = {} },
	['Calendar:onShow() event'] = {
	desc = [[This event is fired when the Calendar is shown (with a call to Calendar:show() or setting the Calendar.visible property to true).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Calendar-onshow.html',
	params = {} },
	['Calendar.parent readonly property'] = {
	desc = [[Get the parent object that owns the Calendar.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Calendar-parent.html',
	params = {} },
	['Calendar.selected read/write property'] = {
	desc = [[Get or set the current selected date on the Calendar. The selected date is a Datetime value. When the Calendar widget is created, the default selected date is the current day.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Calendar-selected.html',
	params = {} },
	['Calendar:show() method'] = {
	desc = [[Show and activate the Calendar (events can now be fired).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Calendar-show.html',
	params = {} },
	['Calendar.tooltip read/write property'] = {
	desc = [[Get or set the tooltip text that appears when the user moves the mouse over the Calendar, represented as a string value. Set the property to an empty string to remove the tooltip message.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Calendar-tooltip.html',
	params = {} },
	['Calendar.visible read/write property'] = {
	desc = [[Get or set the calendar visibility on screen, a true value means that the Calendar is shown, a false value means that the Calendar is hidden. The calendar.visible property is also affected by the Calendar:show() and Calendar:hide() methods.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Calendar-visible.html',
	params = {} },
	['Calendar.width read/write property'] = {
	desc = [[Get or set the Calendar area width. The width starts from 0 (left side of the Calendar) and increases to the right direction.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Calendar-width.html',
	params = {} },
	['Calendar.x read/write property'] = {
	desc = [[Get or set the Calendar horizontal position. The horizontal position start from 0 (left side of the parent widget) and increase to the right (right side of the parent widget).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Calendar-x.html',
	params = {} },
	['Calendar.y read/write property'] = {
	desc = [[Get or set the Calendar vertical position. The vertical position start from 0 (top side of the parent widget) and increase to the bottom side of the parent widget.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Calendar-y.html',
	params = {} },
	['ui.Calendar'] = {
	desc = [[Calendar is an Object representation of a month calendar.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\calendar.html',
	params = {} },
	['Checkbox:align(alignment) method'] = {
	desc = [[Change Checkbox position and size to be aligned relative to its parent.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Checkbox-align.html',
	params = {
		{
			name = [[alignment]],
			desc = [[ A string that specifies the Checkbox alignment : 
        - "all" : the Checkbox will be aligned along all the parent borders (and will cover the entire parent client area). 
        - "bottom" : the Checkbox will be aligned along the bottom border of the parent, preserving its height. 
        - "top" : the Checkbox will be aligned along the the top border of the parent, preserving its height. 
        - "right" : the Checkbox will be aligned along the right border of the parent, preserving its width. 
        - "top" : the Checkbox will be aligned along the left border of the parent, preserving its width. ]],
			color = 'lightpurple'}, nil} },
	['Checkbox:autosize() method'] = {
	desc = [[Resize the Checkbox automatically to accommodate its content.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Checkbox-autosize.html',
	params = {} },
	['Checkbox:center() method'] = {
	desc = [[Centers the Checkbox on the parent widget.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Checkbox-center.html',
	params = {} },
	['Checkbox.checked read/write property'] = {
	desc = [[Set or check wether the Checkbox item is selected.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Checkbox-checked.html',
	params = {} },
	['Checkbox.constructor(parent, caption, x , y , width , height) constructor'] = {
	desc = [[The Checkbox constructor returns a Checkbox value representing a push checkbox on the parent object. Selecting one Checkbox will not affect any other, as Checkboxes are non-exclusive]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Checkbox-constructor.html',
	params = {
		{
			name = [[parent]],
			desc = [[ An object that will own the Checkbox. Parent objects can be any of Window, Groupbox and Tab ]],
			color = 'brightwhite'}, 
		{
			name = [[caption]],
			desc = [[ A string representing the Checkbox's text. ]],
			color = 'lightpurple'}, 
		{
			name = [[x]],
			desc = [[ An optional number that indicates the Checkbox horizontal position, in pixels. Zero means the left border of the parent. ]],
			color = 'lightyellow'}, 
		{
			name = [[y]],
			desc = [[ An optional number that indicates the Checkbox vertical position, in pixels. Zero means the top border of the parent. ]],
			color = 'lightyellow'}, 
		{
			name = [[width]],
			desc = [[ An optional number that indicates the Checkbox width in pixels, autosized to fit text content if omitted. ]],
			color = 'lightyellow'}, 
		{
			name = [[height]],
			desc = [[ An optional number that indicates the Checkbox height in pixels, autosized to fit text content if omitted. ]],
			color = 'lightyellow'}, nil} },
	['Checkbox.cursor read/write property'] = {
	desc = [[Get or set the image of the mouse cursor as it hovers over the Checkbox. Mouse cursors are represented by strings : 
        - "arrow" : the default mouse cursor. 
        - "cross" : a crosshair cursor. 
        - "working" : the standard arrow with a small hourglass. 
        - "hand" : a hand cursor. 
        - "help" : the standard arrow with a small question mark. 
        - "ibeam" : the default I-Beam text cursor. 
        - "forbidden" : a slashed circle cursor. 
        - "cardinal" : a four-pointed arrow pointing up, down, right, and left. 
        - "horizontal" : a double-pointed arrow pointing right and left. 
        - "vertical" : a double-pointed arrow pointing up and down. 
        - "leftdiagonal" : a double-pointed arrow pointing topright down to the left. 
        - "rightdiagonal" : a double-pointed arrow pointing topleft down to the right. 
        - "up" : an arrow pointing up. 
        - "wait" : a hourglass cursor. 
        - "none" : no cursor is displayed. The checkbox.cursor property affects only the Checkbox area.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Checkbox-cursor.html',
	params = {} },
	['Checkbox.enabled read/write property'] = {
	desc = [[Get or set the checkbox ability to respond to mouse, and any other events. When set to false, disable the Checkbox (the user will not be able to interact with it), and checkbox's events won't be fired anymore.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Checkbox-enabled.html',
	params = {} },
	['Checkbox.font read/write property'] = {
	desc = [[Get the Checkbox font, a string value that represent the font name. To set the Checkbox.font property, you can assign a string that represent either a font name of an installed system font, or a font file (*.ttf, *.fon...). A File can also be provided, representing a font file. Note that only the font family is changed. The font style and font size are not affected (see the Checkbox.fontstyle and the Checkbox.fontsize properties). By default, a Checkbox uses the same font as its parent widget Any change in the Checkbox.font, Checkbox.fontstyle and Checkbox.fontsize properties results in an automatic resizing of the Checkbox so that the content is displayed correctly]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Checkbox-font.html',
	params = {} },
	['Checkbox.fontsize read/write property'] = {
	desc = [[Get or set the checkbox font size, a number that represent the font size in pixels. Any change in the Checkbox.font, Checkbox.fontstyle and Checkbox.fontsize properties results in an automatic resizing of the Checkbox so that the content is displayed correctly]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Checkbox-fontsize.html',
	params = {} },
	['Checkbox.fontstyle read/write property'] = {
	desc = [[Get or set the checkbox font style, a table value that contains the following keys/values : 
        - "italic" : set to true if the font is in italic. 
        - "underline" : set to true if the font is underlined. 
        - "strike" : set to true if the font is striked. 
        - "thin" : set to true if the font is thin. 
        - "bold" : set to true if the font is bold. 
        - "heavy" : set to true if the font is heavy. Any change in the Checkbox.font, Checkbox.fontstyle and Checkbox.fontsize properties results in an automatic resizing of the Checkbox so that the content is displayed correctly]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Checkbox-fontstyle.html',
	params = {} },
	['Checkbox.height read/write property'] = {
	desc = [[Get or set the height of the Checkbox area. The height starts from 0 (top side of the Checkbox) and increase to the bottom direction.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Checkbox-height.html',
	params = {} },
	['Checkbox:hide() method'] = {
	desc = [[Hide and deactivate the Checkbox (events can no longer be fired).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Checkbox-hide.html',
	params = {} },
	['Checkbox:onClick() event'] = {
	desc = [[This event is fired when the user has clicked on the Checkbox.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Checkbox-onclick.html',
	params = {} },
	['Checkbox:onContext() event'] = {
	desc = [[This event is fired when the user has clicked on the Checkbox with the right mouse checkbox.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Checkbox-oncontext.html',
	params = {} },
	['Checkbox:onCreate() event'] = {
	desc = [[This event is fired when the Checkbox object has just been created (just after the Checkbox:constructor() call). This event is particularly interesting when you want to inherit from a widget object, or to initialize its properties.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Checkbox-oncreate.html',
	params = {} },
	['Checkbox:onHide() event'] = {
	desc = [[This event is fired when the Checkbox is hidden (with a call to Checkbox:hide() or setting the Checkbox.visible property to false).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Checkbox-onhide.html',
	params = {} },
	['Checkbox:onHover (x, y) event'] = {
	desc = [[This event is fired when the user moves the mouse pointer over the Checkbox.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Checkbox-onhover.html',
	params = {
		{
			name = [[x]],
			desc = [[ The horizontal position of the mouse in the Checkbox area (zero means the left border of the widget). ]],
			color = 'lightyellow'}, 
		{
			name = [[y]],
			desc = [[ The vertical position of the mouse in the Checkbox area (zero means the top border of the widget). ]],
			color = 'lightyellow'}, nil} },
	['Checkbox:onLeave() event'] = {
	desc = [[This event is fired when the mouse cursor leaves the Checkbox.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Checkbox-onleave.html',
	params = {} },
	['Checkbox:onShow() event'] = {
	desc = [[This event is fired when the Checkbox is shown (with a call to Checkbox:show() or setting the Checkbox.visible property to true).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Checkbox-onshow.html',
	params = {} },
	['Checkbox.parent readonly property'] = {
	desc = [[Get the parent widget that owns the Checkbox.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Checkbox-parent.html',
	params = {} },
	['Checkbox:show() method'] = {
	desc = [[Show and activate the Checkbox (events can now be fired).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Checkbox-show.html',
	params = {} },
	['Checkbox.text read/write property'] = {
	desc = [[Get or set the Checkbox caption as a string.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Checkbox-text.html',
	params = {} },
	['Checkbox.tooltip read/write property'] = {
	desc = [[Get or set the tooltip text that appears when the user moves the mouse over the Checkbox, represented as a string value. Set the property to an empty string to remove the tooltip message.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Checkbox-tooltip.html',
	params = {} },
	['Checkbox.visible read/write property'] = {
	desc = [[Get or set the checkbox visibility on screen, a true value means that the Checkbox is shown, a false value means that the Checkbox is hidden. The checkbox.visible property is also affected by the Checkbox:show() and Checkbox:hide() methods.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Checkbox-visible.html',
	params = {} },
	['Checkbox.width read/write property'] = {
	desc = [[Get or set the Checkbox area width. The width starts from 0 (left side of the Checkbox) and increases to the right direction.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Checkbox-width.html',
	params = {} },
	['Checkbox.x read/write property'] = {
	desc = [[Get or set the Checkbox horizontal position. The horizontal position start from 0 (left side of the parent widget) and increase to the right (right side of the parent widget).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Checkbox-x.html',
	params = {} },
	['Checkbox.y read/write property'] = {
	desc = [[Get or set the Checkbox vertical position. The vertical position start from 0 (top side of the parent widget) and increase to the bottom side of the parent widget.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Checkbox-y.html',
	params = {} },
	['ui.Checkbox'] = {
	desc = [[Checkbox is an Object representation of a check box that can be checked or unchecked.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\checkbox.html',
	params = {} },
	['ui.colordialog(color)'] = {
	desc = [[Displays a dialog box for the user to choose a color.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\colordialog.html',
	params = {
		{
			name = [[color]],
			desc = [[ An optional number that indicates the current color in the dialog. ]],
			color = 'lightyellow'}, nil} },
	['Combobox:add(itemtext) method'] = {
	desc = [[Add an item to the Combobox list. itemtext The string to be added to the Combobox list. These items can later be accessed using the Combobox.items property, as ComboItem objects.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Combobox-add.html',
	params = {} },
	['Combobox:align(alignment) method'] = {
	desc = [[Change Combobox position and size to be aligned relative to its parent.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Combobox-align.html',
	params = {
		{
			name = [[alignment]],
			desc = [[ A string that specifies the Combobox alignment : 
        - "all" : the Combobox will be aligned along all the parent borders (and will cover the entire parent client area). 
        - "bottom" : the Combobox will be aligned along the bottom border of the parent, preserving its height. 
        - "top" : the Combobox will be aligned along the the top border of the parent, preserving its height. 
        - "right" : the Combobox will be aligned along the right border of the parent, preserving its width. 
        - "left" : the Combobox will be aligned along the left border of the parent, preserving its width. ]],
			color = 'lightpurple'}, nil} },
	['Combobox:center() method'] = {
	desc = [[Centers the Combobox on the parent widget.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Combobox-center.html',
	params = {} },
	['Combobox:clear() method'] = {
	desc = [[Remove all items from the Combobox.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Combobox-clear.html',
	params = {} },
	['Combobox.constructor(parent, items, x , y , width , height) constructor'] = {
	desc = [[The Combobox constructor returns a Combobox value representing a text box combined with a scrollable list.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Combobox-constructor.html',
	params = {
		{
			name = [[parent]],
			desc = [[ An object that will own the Combobox. Parent objects can be any of Window, Groupbox and Tab ]],
			color = 'brightwhite'}, 
		{
			name = [[items]],
			desc = [[ A table containing a list of strings (each string will be inserted in the list). ]],
			color = 'green'}, 
		{
			name = [[x]],
			desc = [[ An optional number that indicates the Combobox horizontal position, in pixels. Zero means the left border of the parent. ]],
			color = 'lightyellow'}, 
		{
			name = [[y]],
			desc = [[ An optional number that indicates the Combobox vertical position, in pixels. Zero means the top border of the parent. ]],
			color = 'lightyellow'}, 
		{
			name = [[width]],
			desc = [[ An optional number that indicates the Combobox width in pixels. ]],
			color = 'lightyellow'}, 
		{
			name = [[height]],
			desc = [[ An optional number that indicates the Combobox height in pixels. ]],
			color = 'lightyellow'}, nil} },
	['Combobox.count readonly property'] = {
	desc = [[Get the current items count in the Combobox.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Combobox-count.html',
	params = {} },
	['Combobox.cursor read/write property'] = {
	desc = [[Get or set the image of the mouse cursor as it hovers over the Combobox. Mouse cursors are represented by strings : 
        - "arrow" : the default mouse cursor. 
        - "cross" : a crosshair cursor. 
        - "working" : the standard arrow with a small hourglass. 
        - "hand" : a hand cursor. 
        - "help" : the standard arrow with a small question mark. 
        - "ibeam" : the default I-Beam text cursor. 
        - "forbidden" : a slashed circle cursor. 
        - "cardinal" : a four-pointed arrow pointing up, down, right, and left. 
        - "horizontal" : a double-pointed arrow pointing right and left. 
        - "vertical" : a double-pointed arrow pointing up and down. 
        - "leftdiagonal" : a double-pointed arrow pointing topright down to the left. 
        - "rightdiagonal" : a double-pointed arrow pointing topleft down to the right. 
        - "up" : an arrow pointing up. 
        - "wait" : a hourglass cursor. 
        - "none" : no cursor is displayed. The combobox.cursor property affects only the Combobox area.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Combobox-cursor.html',
	params = {} },
	['Combobox.enabled read/write property'] = {
	desc = [[Get or set the combobox ability to respond to mouse, and any other events. When set to false, disable the Combobox (the user will not be able to interact with it), and combobox's events won't be fired anymore.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Combobox-enabled.html',
	params = {} },
	['Combobox.font read/write property'] = {
	desc = [[Get the Combobox font, a string value that represent the font name. To set the Combobox.font property, you can assign a string that represent either a font name of an installed system font, or a font file (*.ttf, *.fon...). A File can also be provided, representing a font file. Note that only the font family is changed. The font style and font size are not affected (see the Combobox.fontstyle and the Combobox.fontsize properties). 
        - By default, an Combobox uses the same font as its parent widget 
        - Any change in the Combobox.font, Combobox.fontstyle and Combobox.fontsize properties results in an automatic resizing of the Combobox so that the content is displayed correctly]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Combobox-font.html',
	params = {} },
	['Combobox.fontsize read/write property'] = {
	desc = [[Get or set the Combobox font size, a number that represent the font size in pixels. Any change in the Combobox.font, Combobox.fontstyle and Combobox.fontsize properties results in an automatic resizing of the Combobox so that the content is displayed correctly]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Combobox-fontsize.html',
	params = {} },
	['Combobox.fontstyle read/write property'] = {
	desc = [[Get or set the Combobox font style, a table value that contains the following keys/values : 
        - "italic" : set to true if the font is in italic. 
        - "underline" : set to true if the font is underlined. 
        - "strike" : set to true if the font is striked. 
        - "thin" : set to true if the font is thin. 
        - "bold" : set to true if the font is bold. 
        - "heavy" : set to true if the font is heavy.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Combobox-fontstyle.html',
	params = {} },
	['Combobox.height read/write property'] = {
	desc = [[Get or set the height of the Combobox. The height starts from 0 (top side of the Combobox) and increase to the bottom direction. Setting Combobox.height does not impact its visual aspect, as this widget's height depends on its Combobox.fontsize property.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Combobox-height.html',
	params = {} },
	['Combobox:hide() method'] = {
	desc = [[Hide and deactivate the Combobox (events can no longer be fired).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Combobox-hide.html',
	params = {} },
	['Combobox.items read/write propertyiterable'] = {
	desc = [[Provides access to the list of the Combobox items.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Combobox-items.html',
	params = {} },
	['Combobox:onChange() event'] = {
	desc = [[This event is fired when the user has modified the Combobox text.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Combobox-onchange.html',
	params = {} },
	['Combobox:onCreate() event'] = {
	desc = [[This event is fired when the Combobox object has just been created (just after the Combobox:constructor() call). This event is particularly interesting when you want to inherit from a widget object, or to initialize its properties.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Combobox-oncreate.html',
	params = {} },
	['Combobox:onHide() event'] = {
	desc = [[This event is fired when the Combobox is hidden (with a call to Combobox:hide() or setting the Combobox.visible property to false).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Combobox-onhide.html',
	params = {} },
	['Combobox:onSelect(ComboItem) event'] = {
	desc = [[Event fired when the user has choosen an item in the Combobox list.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Combobox-onselect.html',
	params = {} },
	['Combobox:onShow() event'] = {
	desc = [[This event is fired when the Combobox is shown (with a call to Combobox:show() or setting the Combobox.visible property to true).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Combobox-onshow.html',
	params = {} },
	['Combobox.parent readonly property'] = {
	desc = [[Get the parent object that owns the Combobox.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Combobox-parent.html',
	params = {} },
	['Combobox:remove(index | item) method'] = {
	desc = [[Remove an item from the Combobox. index | item A number, representing the item index or the ComboItem to be removed.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Combobox-remove.html',
	params = {} },
	['Combobox.selected readonly property'] = {
	desc = [[Get or set the current selected ComboItem or nil if no selection has been made.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Combobox-selected.html',
	params = {} },
	['Combobox:show() method'] = {
	desc = [[Show and activate the Combobox (events can now be fired).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Combobox-show.html',
	params = {} },
	['Combobox.style read/write property'] = {
	desc = [[Get or set the current style, a string value, that specifies the Combobox appearance : 
        - "text" : Combobox will display only text items (default). 
        - "icons" : Combobox will display text items with their icons, using ComboItem:loadicon(). When setting the style property from "icons" to "text" all previously loaded icons are preserved.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Combobox-style.html',
	params = {} },
	['Combobox.text read/write property'] = {
	desc = [[Get or set the current Combobox text content, as a string.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Combobox-text.html',
	params = {} },
	['Combobox.visible read/write property'] = {
	desc = [[Get or set the Combobox visibility on screen, a true value means that the Combobox is shown, a false value means that the Combobox is hidden. The combobox.visible property is also affected by the Combobox:show() and Combobox:hide() methods.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Combobox-visible.html',
	params = {} },
	['Combobox.width read/write property'] = {
	desc = [[Get or set the Combobox width. The width starts from 0 (left side of the Combobox) and increases to the right direction.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Combobox-width.html',
	params = {} },
	['Combobox.x read/write property'] = {
	desc = [[Get or set the Combobox horizontal position. The horizontal position start from 0 (left side of the parent widget) and increase to the right (right side of the parent widget).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Combobox-x.html',
	params = {} },
	['Combobox.y read/write property'] = {
	desc = [[Get or set the Combobox vertical position. The vertical position start from 0 (top side of the parent widget) and increase to the bottom side of the parent widget.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Combobox-y.html',
	params = {} },
	['ui.Combobox'] = {
	desc = [[Combobox is an Object representation of an edit box combiend with a scrollable list.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\combobox.html',
	params = {} },
	['ui.confirm (msg , title)'] = {
	desc = [[Displays a confirmation message dialog box with a question-mark icon, and waits for the user to click the Yes, No or Cancel button.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\confirm.html',
	params = {
		{
			name = [[msg]],
			desc = [[ A string that indicates the message to display to the user. ]],
			color = 'lightpurple'}, 
		{
			name = [[title]],
			desc = [[ An optional string that indicates the title of the message dialog box (defaulting to "Confirmation") ]],
			color = 'lightpurple'}, nil} },
	['ui.dirdialog (prompt)'] = {
	desc = [[Displays a dialog box for the user to choose an existing directory.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\dirdialog.html',
	params = {
		{
			name = [[prompt]],
			desc = [[ An optional string that gives the user indication for the selection. ]],
			color = 'lightpurple'}, nil} },
	['Edit:align(alignment) method'] = {
	desc = [[Change Edit position and size to be aligned relative to its parent.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-align.html',
	params = {
		{
			name = [[alignment]],
			desc = [[ A string that specifies the Edit alignment : 
        - "all" : the Edit will be aligned along all the parent borders (and will cover the entire parent client area). 
        - "bottom" : the Edit will be aligned along the bottom border of the parent, preserving its height. 
        - "top" : the Edit will be aligned along the the top border of the parent, preserving its height. 
        - "right" : the Edit will be aligned along the right border of the parent, preserving its width. 
        - "left" : the Edit will be aligned along the left border of the parent, preserving its width. ]],
			color = 'lightpurple'}, nil} },
	['Edit:append(text)method'] = {
	desc = [[Append text to Edit content, just after the last character position.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-append.html',
	params = {
		{
			name = [[text]],
			desc = [[ A string, representing the text to append to the Edit content. ]],
			color = 'lightpurple'}, nil} },
	['Edit.bgcolor read/write property'] = {
	desc = [[Get or set the global Edit background color. The background color is represented by a number, an RGB value (one byte per primary color). A RGB color can be represented as an hexadecimal number : 0xRRGGBB , RR meaning a 8bit hexadecimal red value, and so on.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-bgcolor.html',
	params = {} },
	['Edit.canredo readonly property'] = {
	desc = [[Get a boolean value, indicating wether the user can redo the last Edit operation.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-canredo.html',
	params = {} },
	['Edit.canundo readonly property'] = {
	desc = [[Get a boolean value, indicating wether the user can undo the last Edit operation.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-canundo.html',
	params = {} },
	['Edit.caret read/write property'] = {
	desc = [[Get or set the Edit current caret position. The caret position starts from 1 (before the first character) and ends after the last character. Set the caret position to 0 to set it at the last character (without the need to get the Edit's content size). 
        - Caret position is expressed in characters : the caret in position 1 is just before the first character. 
        - Use caret position 0 to reach end of the Edit's content.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-caret.html',
	params = {} },
	['Edit:center() method'] = {
	desc = [[Centers the Edit on the parent widget.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-center.html',
	params = {} },
	['Edit.color read/write property'] = {
	desc = [[Get or set the current Edit text color, that apply to all text content. The color is represented by a number, an RGB value (one byte per primary color). A RGB color can be represented as an hexadecimal number : 0xRRGGBB , RR meaning a 8bit hexadecimal red value, and so on.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-color.html',
	params = {} },
	['Edit.constructor(parent, text, x , y , width , height) constructor'] = {
	desc = [[The Edit constructor returns an Edit value representing a Windows RichEdit control on the parent object.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-constructor.html',
	params = {
		{
			name = [[parent]],
			desc = [[ An object that will own the Edit. Parent objects can be any of Window, Groupbox and Tab ]],
			color = 'brightwhite'}, 
		{
			name = [[text]],
			desc = [[ A string representing the Edit's text content. ]],
			color = 'lightpurple'}, 
		{
			name = [[x]],
			desc = [[ An optional number that indicates the Edit horizontal position, in pixels. Zero means the left border of the parent. ]],
			color = 'lightyellow'}, 
		{
			name = [[y]],
			desc = [[ An optional number that indicates the Edit vertical position, in pixels. Zero means the top border of the parent. ]],
			color = 'lightyellow'}, 
		{
			name = [[width]],
			desc = [[ An optional number that indicates the Edit width in pixels. ]],
			color = 'lightyellow'}, 
		{
			name = [[height]],
			desc = [[ An optional number that indicates the Edit height in pixels. ]],
			color = 'lightyellow'}, nil} },
	['Edit:copy() method'] = {
	desc = [[Copies the Edit current selection to the Windows clipboard. If no current selection exists, nothing is copied to the clipboard.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-copy.html',
	params = {} },
	['Edit:cut() method'] = {
	desc = [[Cut the Edit current selected text to the clipboard. If no current selection exists, nothing is cut to the clipboard.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-cut.html',
	params = {} },
	['Edit.enabled read/write property'] = {
	desc = [[Get or set the edit ability to respond to mouse, and any other events. When set to false, disable the Edit (the user will not be able to interact with it), and edit's events won't be fired anymore.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-enabled.html',
	params = {} },
	['Edit.font read/write property'] = {
	desc = [[Get the Edit font, a string value that represent the font name. To set the Edit.font property, you can assign a string that represent either a font name of an installed system font, or a font file (*.ttf, *.fon...). A File can also be provided, representing a font file. Note that only the font family is changed. The font style and font size are not affected (see the Edit.fontstyle and the Edit.fontsize properties). By default, an Edit uses the same font as its parent widget]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-font.html',
	params = {} },
	['Edit.fontsize read/write property'] = {
	desc = [[Get or set the Edit font size, a number that represent the font size in pixels.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-fontsize.html',
	params = {} },
	['Edit.fontstyle read/write property'] = {
	desc = [[Get or set the edit font style, a table value that contains the following keys/values : 
        - "italic" : set to true if the font is in italic. 
        - "underline" : set to true if the font is underlined. 
        - "strike" : set to true if the font is striked. 
        - "thin" : set to true if the font is thin. 
        - "bold" : set to true if the font is bold. 
        - "heavy" : set to true if the font is heavy.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-fontstyle.html',
	params = {} },
	['Edit.height read/write property'] = {
	desc = [[Get or set the height of the Edit. The height starts from 0 (top side of the Edit) and increase to the bottom direction. The content of the Edit may be hidden if its size is too small. The horizontal and vertical scrollbars are automatically managed so that Edit content can still be accessed.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-height.html',
	params = {} },
	['Edit:hide() method'] = {
	desc = [[Hide and deactivate the Edit (events can no longer be fired).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-hide.html',
	params = {} },
	['Edit.line read/write property'] = {
	desc = [[Get or set the Edit current line position. The line position starts from 1 and grows until the last line of Edit content. Setting this property will move the caret position at the start of the line. If this property is set with a line number higher than the last line, the caret stays at the current position.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-line.html',
	params = {} },
	['Edit.lines read/write property iterable'] = {
	desc = [[Returns a proxy table to get/set the lines of text in the Edit. You can use this property : 
        - linesline_number : gets or sets the line at index line_number. The line index start from 1 and grows until the last line of text. 
        - #edit.lines : gets the line count of the Edit. 
        - You can use each(edit.lines) or ipairs(edit.lines) to iterate through each line of the Edit text. Edit.lines is a proxy table and should not be used for anything other than setting/getting lines.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-lines.html',
	params = {} },
	['Edit:load(file | filename, isrichtext)method'] = {
	desc = [[Fills the Edit content with a specified file.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-load.html',
	params = {
		{
			name = [[File | filename]],
			desc = [[ A File object or a filename string, representing the file whose content will fill the Edit. ]],
			color = 'lightpurple'}, 
		{
			name = [[isrichtext]],
			desc = [[ An optional boolean value indicating wether the file content is in RichText format, or not. ]],
			color = 'lightyellow'}, nil} },
	['Edit.modified read/write property'] = {
	desc = [[Get or set a boolean value, indicating wether the Edit has been modified.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-modified.html',
	params = {} },
	['Edit:onCaret() event'] = {
	desc = [[This event is fired when the Edit cursor position has changed.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-oncaret.html',
	params = {} },
	['Edit:onChange() event'] = {
	desc = [[This event is fired when the user has modified the Edit text content or format. Use the onChange event carefully: changing Edit content text or format during this event will put your program in an infinite loop.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-onchange.html',
	params = {} },
	['Edit:onCreate() event'] = {
	desc = [[This event is fired when the Edit object has just been created (just after the Edit:constructor() call). This event is particularly interesting when you want to inherit from a widget object, or to initialize its properties.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-oncreate.html',
	params = {} },
	['Edit:onHide() event'] = {
	desc = [[This event is fired when the Edit is hidden (with a call to Edit:hide() or setting the Edit.visible property to false).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-onhide.html',
	params = {} },
	['Edit:onSelect(start, end) event'] = {
	desc = [[Event fired when the user has made a selection in the Edit Widget.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-onselect.html',
	params = {
		{
			name = [[start]],
			desc = [[ The character position at the start of the selection. ]],
			color = 'lightyellow'}, 
		{
			name = [[end]],
			desc = [[ The character position at the end of the selection. 
        - Selection and caret position are expressed in characters : the cursor in position 1 is just before the first character. 
        - Selection includes everything if it starts at position 1 and ends at position 0. 
        - The selection is empty if its end and start are the same. ]],
			color = 'lightyellow'}, nil} },
	['Edit:onShow() event'] = {
	desc = [[This event is fired when the Edit is shown (with a call to Edit:show() or setting the Edit.visible property to true).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-onshow.html',
	params = {} },
	['Edit.parent readonly property'] = {
	desc = [[Get the parent object that owns the Edit.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-parent.html',
	params = {} },
	['Edit:copy() method'] = {
	desc = [[Paste the Windows clipboard content to the Edit object.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-paste.html',
	params = {} },
	['Edit.readonly read/write property'] = {
	desc = [[Get or set the the readonly mode of the Edit. A true value, indicate that the user cannot enter any new text content.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-readonly.html',
	params = {} },
	['Edit:redo() method'] = {
	desc = [[redoes the last edit operation on the Edit.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-redo.html',
	params = {} },
	['Edit.richtext read/write property'] = {
	desc = [[Determines if the Edit control use Rich Text Format (the default) or plain text (no format at all), a true value means that the Edit uses Rich Text Format. In order to assign a value to this property, the Edit control must be empty.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-richtext.html',
	params = {} },
	['Edit:save(file | filename, isrichtext , encoding)method'] = {
	desc = [[Saves the Edit content to a specified file.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-save.html',
	params = {
		{
			name = [[File | filename]],
			desc = [[ A File object or a filename string, representing the file whose content will fill the Edit. ]],
			color = 'lightpurple'}, 
		{
			name = [[isrichtext]],
			desc = [[ An optional boolean value indicating wether the file content is in RichText format, or not. ]],
			color = 'lightyellow'}, 
		{
			name = [[encoding]],
			desc = [[ An optional string indicating the desired file encoding (See File:open() for possible encoding values). If omitted, uses default raw binary encoding. ]],
			color = 'lightpurple'}, nil} },
	['Edit:searchdown(findtext, iswholeword , matchcase)method'] = {
	desc = [[Search text withins the Edit content, forward from the current caret position.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-searchdown.html',
	params = {
		{
			name = [[findtext]],
			desc = [[ A string value, representing the text to search for. ]],
			color = 'lightpurple'}, 
		{
			name = [[iswholeword]],
			desc = [[ A boolean value, indicating to search only for whole words that match the search text. ]],
			color = 'lightyellow'}, 
		{
			name = [[matchcase]],
			desc = [[ A boolean value, indicating that the search operation is case-sensitive. ]],
			color = 'lightyellow'}, nil} },
	['Edit:searchup(findtext, iswholeword , matchcase)method'] = {
	desc = [[Search text withins the Edit content, backward from the current caret position.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-searchup.html',
	params = {
		{
			name = [[findtext]],
			desc = [[ A string value, representing the text to search for. ]],
			color = 'lightpurple'}, 
		{
			name = [[iswholeword]],
			desc = [[ A boolean value, indicating to search only for whole words that match the search text. ]],
			color = 'lightyellow'}, 
		{
			name = [[matchcase]],
			desc = [[ A boolean value, indicating that the search operation is case-sensitive. ]],
			color = 'lightyellow'}, nil} },
	['Edit.selection readonly property'] = {
	desc = [[Returns a proxy table to get/set the current Edit selection properties. You can get or set the following fields : 
        - selection.from: the starting character position of the current selection (1 means the caret is just before the first character) 
        - selection.to: the ending character position of the current selection (0 means the caret is after the last character) 
        - selection.text: the text of the current selection 
        - selection.color: the text color (a number RGB value) of the current selection (nil means default color) 
        - selection.bgcolor: the background color (a number RGB value) of the current selection (nil means default color) 
        - selection.font: the font name of the current selection 
        - selection.fontsize: the font size, in pixels, of the current selection 
        - selection.fontstyle: the fontstyle of the current selection 
        - You can use #edit.selection to get current selection length. Edit.selection is a proxy table and should not be used for anything other than setting/getting field values.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-selection.html',
	params = {} },
	['Edit:show() method'] = {
	desc = [[Show and activate the Edit (events can now be fired).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-show.html',
	params = {} },
	['Edit.text read/write property'] = {
	desc = [[Get or set the Edit text content as a string. Using the Edit.text property is not efficient for large content. Instead, use Edit:load() and Edit:savle() or Edit:append().]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-text.html',
	params = {} },
	['Edit.textlength readonly property'] = {
	desc = [[Get the length of the Edit text content, expressed as a number of characters.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-textlength.html',
	params = {} },
	['Edit.tooltip read/write property'] = {
	desc = [[Get or set the tooltip text that appears when the user moves the mouse over the control, represented as a string value. Set the property to an empty string to remove the tooltip message.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-tooltip.html',
	params = {} },
	['Edit:undo() method'] = {
	desc = [[Undoes the last edit operation on the Edit.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-undo.html',
	params = {} },
	['Edit.visible read/write property'] = {
	desc = [[Get or set the edit visibility on screen, a true value means that the Edit is shown, a false value means that the Edit is hidden. The edit.visible property is also affected by the Edit:show() and Edit:hide() methods.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-visible.html',
	params = {} },
	['Edit.width read/write property'] = {
	desc = [[Get or set the Edit width. The width starts from 0 (left side of the Edit) and increases to the right direction.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-width.html',
	params = {} },
	['Edit.wordwrap read/write property'] = {
	desc = [[Determines if the Edit control inserts soft carriage returns so that the text wraps to the right, a true value means that the Edit text wraps to the right.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-wordwrap.html',
	params = {} },
	['Edit.x read/write property'] = {
	desc = [[Get or set the Edit horizontal position. The horizontal position start from 0 (left side of the parent widget) and increase to the right (right side of the parent widget).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-x.html',
	params = {} },
	['Edit.y read/write property'] = {
	desc = [[Get or set the Edit vertical position. The vertical position start from 0 (top side of the parent widget) and increase to the bottom side of the parent widget.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Edit-y.html',
	params = {} },
	['ui.Edit'] = {
	desc = [[Edit is an Object representation of a Windows RichEdit control.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\edit.html',
	params = {} },
	['Entry:align(alignment) method'] = {
	desc = [[Change Entry position and size to be aligned relative to its parent.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-align.html',
	params = {
		{
			name = [[alignment]],
			desc = [[ A string that specifies the Entry alignment : 
        - "all" : the Entry will be aligned along all the parent borders (and will cover the entire parent client area). 
        - "bottom" : the Entry will be aligned along the bottom border of the parent, preserving its height. 
        - "top" : the Entry will be aligned along the the top border of the parent, preserving its height. 
        - "right" : the Entry will be aligned along the right border of the parent, preserving its width. 
        - "left" : the Entry will be aligned along the left border of the parent, preserving its width. ]],
			color = 'lightpurple'}, nil} },
	['Entry:autosize() method'] = {
	desc = [[Resize the Entry automatically to accommodate its contents.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-autosize.html',
	params = {} },
	['Entry.canundo readonly property'] = {
	desc = [[Get a boolean value, indicating wether the user can undo the last Entry operation.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-canundo.html',
	params = {} },
	['Entry:center() method'] = {
	desc = [[Centers the Entry on the parent widget.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-center.html',
	params = {} },
	['Entry.constructor(parent, caption, x , y , width , height) constructor'] = {
	desc = [[The Entry constructor returns a Entry value representing a static text area on the parent object.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-constructor.html',
	params = {
		{
			name = [[parent]],
			desc = [[ An object that will own the Entry. Parent objects can be any of Window, Groupbox and Tab ]],
			color = 'brightwhite'}, 
		{
			name = [[caption]],
			desc = [[ A string representing the Entry's text. ]],
			color = 'lightpurple'}, 
		{
			name = [[x]],
			desc = [[ An optional number that indicates the Entry horizontal position, in pixels. Zero means the left border of the parent. ]],
			color = 'lightyellow'}, 
		{
			name = [[y]],
			desc = [[ An optional number that indicates the Entry vertical position, in pixels. Zero means the top border of the parent. ]],
			color = 'lightyellow'}, 
		{
			name = [[width]],
			desc = [[ An optional number that indicates the Entry width in pixels, autosized to fit text content if omitted. ]],
			color = 'lightyellow'}, 
		{
			name = [[height]],
			desc = [[ An optional number that indicates the Entry height in pixels, autosized to fit text content if omitted. ]],
			color = 'lightyellow'}, nil} },
	['Entry:copy() method'] = {
	desc = [[Copies the current Entry text to the clipboard.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-copy.html',
	params = {} },
	['Entry.cursor read/write property'] = {
	desc = [[Get or set the image of the mouse cursor as it hovers over the Entry. Mouse cursors are represented by strings : 
        - "arrow" : the default mouse cursor. 
        - "cross" : a crosshair cursor. 
        - "working" : the standard arrow with a small hourglass. 
        - "hand" : a hand cursor. 
        - "help" : the standard arrow with a small question mark. 
        - "ibeam" : the default I-Beam text cursor. 
        - "forbidden" : a slashed circle cursor. 
        - "cardinal" : a four-pointed arrow pointing up, down, right, and left. 
        - "horizontal" : a double-pointed arrow pointing right and left. 
        - "vertical" : a double-pointed arrow pointing up and down. 
        - "leftdiagonal" : a double-pointed arrow pointing topright down to the left. 
        - "rightdiagonal" : a double-pointed arrow pointing topleft down to the right. 
        - "up" : an arrow pointing up. 
        - "wait" : a hourglass cursor. 
        - "none" : no cursor is displayed. The entry.cursor property affects only the Entry area.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-cursor.html',
	params = {} },
	['Entry:cut() method'] = {
	desc = [[Cut the current Entry text to the clipboard.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-cut.html',
	params = {} },
	['Entry.enabled read/write property'] = {
	desc = [[Get or set the entry ability to respond to mouse, and any other events. When set to false, disable the Entry (the user will not be able to interact with it), and entry's events won't be fired anymore.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-enabled.html',
	params = {} },
	['Entry.font read/write property'] = {
	desc = [[Get the Entry font, a string value that represent the font name. To set the Entry.font property, you can assign a string that represent either a font name of an installed system font, or a font file (*.ttf, *.fon...). A File can also be provided, representing a font file. Note that only the font family is changed. The font style and font size are not affected (see the Entry.fontstyle and the Entry.fontsize properties). 
        - By default, an Entry uses the same font as its parent widget 
        - Any change in the Entry.font, Entry.fontstyle and Entry.fontsize properties results in an automatic resizing of the Entry so that the content is displayed correctly]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-font.html',
	params = {} },
	['Entry.fontsize read/write property'] = {
	desc = [[Get or set the Entry font size, a number that represent the font size in pixels. Any change in the Entry.font, Entry.fontstyle and Entry.fontsize properties results in an automatic resizing of the Entry so that the content is displayed correctly]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-fontsize.html',
	params = {} },
	['Entry.fontstyle read/write property'] = {
	desc = [[Get or set the Entry font style, a table value that contains the following keys/values : 
        - "italic" : set to true if the font is in italic. 
        - "underline" : set to true if the font is underlined. 
        - "strike" : set to true if the font is striked. 
        - "thin" : set to true if the font is thin. 
        - "bold" : set to true if the font is bold. 
        - "heavy" : set to true if the font is heavy. Any change in the Entry.font, Entry.fontstyle and Entry.fontsize properties results in an automatic resizing of the Entry so that the content is displayed correctly]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-fontstyle.html',
	params = {} },
	['Entry.height read/write property'] = {
	desc = [[Get or set the height of the Entry. The height starts from 0 (top side of the Entry) and increase to the bottom direction.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-height.html',
	params = {} },
	['Entry:hide() method'] = {
	desc = [[Hide and deactivate the Entry (events can no longer be fired).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-hide.html',
	params = {} },
	['Entry.masked read/write property'] = {
	desc = [[Get or set a boolean value, indicating wether the user input is visible or hidden by "*" characters.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-masked.html',
	params = {} },
	['Entry.modified read/write property'] = {
	desc = [[Get or set a boolean value, indicating wether the Entry has been modified.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-modified.html',
	params = {} },
	['Entry:onChange() event'] = {
	desc = [[This event is fired when the user has modified the Entry text.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-onchange.html',
	params = {} },
	['Entry:onCreate() event'] = {
	desc = [[This event is fired when the Entry object has just been created (just after the Entry:constructor() call). This event is particularly interesting when you want to inherit from a widget object, or to initialize its properties.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-oncreate.html',
	params = {} },
	['Entry:onHide() event'] = {
	desc = [[This event is fired when the Entry is hidden (with a call to Entry:hide() or setting the Entry.visible property to false).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-onhide.html',
	params = {} },
	['Entry:onHover (x, y) event'] = {
	desc = [[This event is fired when the user moves the mouse pointer over the Entry.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-onhover.html',
	params = {
		{
			name = [[x]],
			desc = [[ The horizontal position of the mouse in the Entry area (zero means the left border of the widget). ]],
			color = 'lightyellow'}, 
		{
			name = [[y]],
			desc = [[ The vertical position of the mouse in the Entry area (zero means the top border of the widget). ]],
			color = 'lightyellow'}, nil} },
	['Entry:onLeave() event'] = {
	desc = [[This event is fired when the mouse cursor leaves the Entry.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-onleave.html',
	params = {} },
	['Entry:onSelect() event'] = {
	desc = [[Event fired when the user has pressed the ENTER or RETURN key.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-onselect.html',
	params = {} },
	['Entry:onShow() event'] = {
	desc = [[This event is fired when the Entry is shown (with a call to Entry:show() or setting the Entry.visible property to true).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-onshow.html',
	params = {} },
	['Entry.parent readonly property'] = {
	desc = [[Get the parent object that owns the Entry.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-parent.html',
	params = {} },
	['Entry:copy() method'] = {
	desc = [[Pastes the current Entry text from the clipboard.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-paste.html',
	params = {} },
	['Entry:show() method'] = {
	desc = [[Show and activate the Entry (events can now be fired).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-show.html',
	params = {} },
	['Entry.text read/write property'] = {
	desc = [[Get or set the Entry text content as a string. After a change in the Entry.text property, the Entry object do not autosize.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-text.html',
	params = {} },
	['Entry.textalign read/write property'] = {
	desc = [[Get or set the Entry text alignment, a string value that represent how the text will be displayed. 
        - "left" : the text is aligned to the left edge of the Entry (the default). 
        - "right" : the text is aligned to the right edge of the Entry. 
        - "center" : the text is aligned in the center of the Entry area.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-textalign.html',
	params = {} },
	['Entry.textlimit read/write property'] = {
	desc = [[Get or set a number value, indicating the maximum length of the text entered. Setting the property with a value of 0, permits a maximum of 2ï> characters.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-textlimit.html',
	params = {} },
	['Entry.tooltip read/write property'] = {
	desc = [[Get or set the tooltip text that appears when the user moves the mouse over the control, represented as a string value. Set the property to an empty string to remove the tooltip message.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-tooltip.html',
	params = {} },
	['Entry:undo() method'] = {
	desc = [[Undoes the last edit operation on the Entry.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-undo.html',
	params = {} },
	['Entry.visible read/write property'] = {
	desc = [[Get or set the entry visibility on screen, a true value means that the Entry is shown, a false value means that the Entry is hidden. The entry.visible property is also affected by the Entry:show() and Entry:hide() methods.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-visible.html',
	params = {} },
	['Entry.width read/write property'] = {
	desc = [[Get or set the Entry width. The width starts from 0 (left side of the Entry) and increases to the right direction.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-width.html',
	params = {} },
	['Entry.x read/write property'] = {
	desc = [[Get or set the Entry horizontal position. The horizontal position start from 0 (left side of the parent widget) and increase to the right (right side of the parent widget).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-x.html',
	params = {} },
	['Entry.y read/write property'] = {
	desc = [[Get or set the Entry vertical position. The vertical position start from 0 (top side of the parent widget) and increase to the bottom side of the parent widget.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Entry-y.html',
	params = {} },
	['ui.Entry'] = {
	desc = [[Entry is an Object representation of a single line text field.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\entry.html',
	params = {} },
	['ui.error (msg , title)'] = {
	desc = [[Displays an error message dialog box with a red-cross icon, and waits for the user to click the OK button.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\error.html',
	params = {
		{
			name = [[msg]],
			desc = [[ A string that indicates the error message to display to the user. ]],
			color = 'lightpurple'}, 
		{
			name = [[title]],
			desc = [[ An optional string that indicates the title of the message dialog box (defaulting to "Error") ]],
			color = 'lightpurple'}, nil} },
	['ui.fontdialog(widget, color)'] = {
	desc = [[Displays a dialog box for the user to choose a Windows system font.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\fontdialog.html',
	params = {
		{
			name = [[widget]],
			desc = [[ An optional widget whose font will initialize dialog's fields.If an Edit widget is provided, the current selection font is used to initialize dialog's fields. ]],
			color = 'green'}, 
		{
			name = [[color]],
			desc = [[ An optional number representing a RGB color value, to initialize the dialog color field. ]],
			color = 'lightyellow'}, nil} },
	['Groupbox:align(alignment) method'] = {
	desc = [[Change Groupbox position and size to be aligned relative to its parent.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Groupbox-align.html',
	params = {
		{
			name = [[alignment]],
			desc = [[ A string that specifies the Groupbox alignment : 
        - "all" : the Groupbox will be aligned along all the parent borders (and will cover the entire parent client area). 
        - "bottom" : the Groupbox will be aligned along the bottom border of the parent, preserving its height. 
        - "top" : the Groupbox will be aligned along the the top border of the parent, preserving its height. 
        - "right" : the Groupbox will be aligned along the right border of the parent, preserving its width. 
        - "left" : the Groupbox will be aligned along the left border of the parent, preserving its width. ]],
			color = 'lightpurple'}, nil} },
	['Groupbox:center() method'] = {
	desc = [[Centers the Groupbox on the parent widget.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Groupbox-center.html',
	params = {} },
	['Groupbox.constructor(parent, caption, x , y , width , height) constructor'] = {
	desc = [[The Groupbox constructor returns a Groupbox value representing a group of related controls. Whenever another widget is placed within a Groupbox, that groupbox becomes its parent.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Groupbox-constructor.html',
	params = {
		{
			name = [[parent]],
			desc = [[ An object that will own the Groupbox. Parent objects can be any of Window, Groupbox and Tab ]],
			color = 'brightwhite'}, 
		{
			name = [[caption]],
			desc = [[ A string representing the Groupbox's text. ]],
			color = 'lightpurple'}, 
		{
			name = [[x]],
			desc = [[ An optional number that indicates the Groupbox horizontal position, in pixels. Zero means the left border of the parent. ]],
			color = 'lightyellow'}, 
		{
			name = [[y]],
			desc = [[ An optional number that indicates the Groupbox vertical position, in pixels. Zero means the top border of the parent. ]],
			color = 'lightyellow'}, 
		{
			name = [[width]],
			desc = [[ An optional number that indicates the Groupbox width in pixels, autosized to fit text content if omitted. ]],
			color = 'lightyellow'}, 
		{
			name = [[height]],
			desc = [[ An optional number that indicates the Groupbox height in pixels, autosized to fit text content if omitted. ]],
			color = 'lightyellow'}, nil} },
	['Groupbox.enabled read/write property'] = {
	desc = [[Get or set the groupbox ability and all its child widgets to respond to mouse, and any other events. When set to false, disable the Groupbox (the user will not be able to interact with the child widgets within it), and groupbox's events won't be fired anymore.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Groupbox-enabled.html',
	params = {} },
	['Groupbox.font read/write property'] = {
	desc = [[Get the Groupbox font, a string value that represent the font name. To set the Groupbox.font property, you can assign a string that represent either a font name of an installed system font, or a font file (*.ttf, *.fon...). A File can also be provided, representing a font file. Note that only the font family is changed. The font style and font size are not affected (see the Groupbox.fontstyle and the Groupbox.fontsize properties). Any change in the Groupbox.font, Groupbox.fontstyle and Groupbox.fontsize properties applies to all contained widgets]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Groupbox-font.html',
	params = {} },
	['Groupbox.fontsize read/write property'] = {
	desc = [[Get or set the groupbox font size, a number that represent the font size in pixels. Any change in the Groupbox.font, Groupbox.fontstyle and Groupbox.fontsize properties applies to all contained widgets]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Groupbox-fontsize.html',
	params = {} },
	['Groupbox.fontstyle read/write property'] = {
	desc = [[Get or set the groupbox font style, a table value that contains the following keys/values : 
        - "italic" : set to true if the font is in italic. 
        - "underline" : set to true if the font is underlined. 
        - "strike" : set to true if the font is striked. 
        - "thin" : set to true if the font is thin. 
        - "bold" : set to true if the font is bold. 
        - "heavy" : set to true if the font is heavy. Any change in the Groupbox.font, Groupbox.fontstyle and Groupbox.fontsize properties applies to all contained widgets]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Groupbox-fontstyle.html',
	params = {} },
	['Groupbox.height read/write property'] = {
	desc = [[Get or set the height of the Groupbox area. The height starts from 0 (top side of the Groupbox) and increase to the bottom direction.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Groupbox-height.html',
	params = {} },
	['Groupbox:hide() method'] = {
	desc = [[Hide and deactivate the Groupbox (events can no longer be fired).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Groupbox-hide.html',
	params = {} },
	['Groupbox:onCreate() event'] = {
	desc = [[This event is fired when the Groupbox object has just been created (just after the Groupbox:constructor() call). This event is particularly interesting when you want to inherit from a widget object, or to initialize its properties.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Groupbox-oncreate.html',
	params = {} },
	['Groupbox:onHide() event'] = {
	desc = [[This event is fired when the Groupbox is hidden (with a call to Groupbox:hide() or setting the Groupbox.visible property to false).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Groupbox-onhide.html',
	params = {} },
	['Groupbox:onShow() event'] = {
	desc = [[This event is fired when the Groupbox is shown (with a call to Groupbox:show() or setting the Groupbox.visible property to true).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Groupbox-onshow.html',
	params = {} },
	['Groupbox.parent readonly property'] = {
	desc = [[Get the parent object that owns the Groupbox.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Groupbox-parent.html',
	params = {} },
	['Groupbox:show() method'] = {
	desc = [[Show and activate the Groupbox (events can now be fired).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Groupbox-show.html',
	params = {} },
	['Groupbox.text read/write property'] = {
	desc = [[Get or set the Groupbox text as a string, that appear as a title in its top left border.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Groupbox-text.html',
	params = {} },
	['Groupbox.visible read/write property'] = {
	desc = [[Get or set the groupbox visibility on screen, a true value means that the Groupbox is shown, a false value means that the Groupbox is hidden. The groupbox.visible property is also affected by the Groupbox:show() and Groupbox:hide() methods.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Groupbox-visible.html',
	params = {} },
	['Groupbox.width read/write property'] = {
	desc = [[Get or set the Groupbox area width. The width starts from 0 (left side of the Groupbox) and increases to the right direction.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Groupbox-width.html',
	params = {} },
	['Groupbox.x read/write property'] = {
	desc = [[Get or set the Groupbox horizontal position. The horizontal position start from 0 (left side of the parent widget) and increase to the right (right side of the parent widget).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Groupbox-x.html',
	params = {} },
	['Groupbox.y read/write property'] = {
	desc = [[Get or set the Groupbox vertical position. The vertical position start from 0 (top side of the parent widget) and increase to the bottom side of the parent widget.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Groupbox-y.html',
	params = {} },
	['ui.Groupbox'] = {
	desc = [[Groupbox is an Object representation of a static text caption.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\groupbox.html',
	params = {} },
	['ui module'] = {
	desc = [[The ui module provides Objects, and functions to build native Windows GUI applications, known as Desktop applications. This module is only available with the Desktop Luartinterpreter wluart.exe]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\index.html',
	params = {} },
	['ui.info (msg , title)'] = {
	desc = [[Displays an information message dialog box with an icon consisting of a lowercase letter i in a circle, and waits for the user to click the OK button.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\info.html',
	params = {
		{
			name = [[msg]],
			desc = [[ A string that indicates the message to display to the user. ]],
			color = 'lightpurple'}, 
		{
			name = [[title]],
			desc = [[ An optional string that indicates the title of the message dialog box (defaulting to "Information") ]],
			color = 'lightpurple'}, nil} },
	['TabItem.enabled read/write properties'] = {
	desc = [[Get or set the TabItem content ability to respond to mouse, and any other events. When set to false, disable the TabItem (the user will not be able to interact with its content), and TabItem childs controls events won't be fired anymore.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Item-enabled.html',
	params = {} },
	['ListItem.index, TabItem.index, ComboItem.index read/write properties'] = {
	desc = [[Get or set the item position, a number starting from 1 and increase until the total count of items.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Item-index.html',
	params = {} },
	['ListItem:loadicon, TabItem:loadicon, ComboItem:loadicon(path, index) method'] = {
	desc = [[Loads an item icon, displayed to the left of the item's text.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Item-loadicon.html',
	params = {
		{
			name = [[path]],
			desc = [[ Represent any of the following possible icon locations : 
        - A string which represents the path to an ".ico" icon file, or gets the icon associated with the provided file/directory. 
        - A Widget object, whose icon will be used by the item. 
        - A Directory or File object, representing an ".ico" file, or gets the icon associated with the provided file/directory. 
        - A Buffer object, whose binary content will represent the icon. ]],
			color = 'brightwhite'}, 
		{
			name = [[index]],
			desc = [[ The icon index, a optional number starting from 1, that represent the icon to select. ]],
			color = 'lightyellow'}, nil} },
	['ListItem.owner, TabItem.owner, ComboItem.owner readonly properties'] = {
	desc = [[Get the Object that owns the item. Items are ephemeral objects that are instantiated by their owner objects, with the "items" property. It is not recommended that you store them in a table or use them globally since they may refer to items that have been deleted.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Item-owner.html',
	params = {} },
	['ListItem:remove, TabItem:remove, ComboItem:remove() method'] = {
	desc = [[Removes the item from the Tab, List or Combobox it belongs to.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Item-remove.html',
	params = {} },
	['ListItem.text, TabItem.text, ComboItem.text read/write properties'] = {
	desc = [[Get or set the item caption as a string.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Item-text.html',
	params = {} },
	['ListItem ComboItem TabItem'] = {
	desc = [[ListItem, ComboItem, and TabItem are an Object representation of an existing item in a List, Combobox, or Tab respectively. As these objects have no constructor, they cannot be instantiated by programming. Only their owners can create them. Items are ephemeral objects that are instantiated by their owner objects, with the "items" property. It is not recommended that you store them in a table or use them globally since they may refer to items that have been deleted.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\item.html',
	params = {} },
	['Label:align(alignment) method'] = {
	desc = [[Change Label position and size to be aligned relative to its parent.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Label-align.html',
	params = {
		{
			name = [[alignment]],
			desc = [[ A string that specifies the Label alignment : 
        - "all" : the Label will be aligned along all the parent borders (and will cover the entire parent client area). 
        - "bottom" : the Label will be aligned along the bottom border of the parent, preserving its height. 
        - "top" : the Label will be aligned along the the top border of the parent, preserving its height. 
        - "right" : the Label will be aligned along the right border of the parent, preserving its width. 
        - "left" : the Label will be aligned along the left border of the parent, preserving its width. ]],
			color = 'lightpurple'}, nil} },
	['Label:autosize() method'] = {
	desc = [[Resize the Label automatically to accommodate its contents.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Label-autosize.html',
	params = {} },
	['Label:center() method'] = {
	desc = [[Centers the Label on the parent widget.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Label-center.html',
	params = {} },
	['Label.constructor(parent, caption, x , y , width , height) constructor'] = {
	desc = [[The Label constructor returns a Label value representing a static text area on the parent object.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Label-constructor.html',
	params = {
		{
			name = [[parent]],
			desc = [[ An object that will own the Label. Parent objects can be any of Window, Groupbox and Tab ]],
			color = 'brightwhite'}, 
		{
			name = [[caption]],
			desc = [[ A string representing the Label's text. ]],
			color = 'lightpurple'}, 
		{
			name = [[x]],
			desc = [[ An optional number that indicates the Label horizontal position, in pixels. Zero means the left border of the parent. ]],
			color = 'lightyellow'}, 
		{
			name = [[y]],
			desc = [[ An optional number that indicates the Label vertical position, in pixels. Zero means the top border of the parent. ]],
			color = 'lightyellow'}, 
		{
			name = [[width]],
			desc = [[ An optional number that indicates the Label width in pixels, autosized to fit text content if omitted. ]],
			color = 'lightyellow'}, 
		{
			name = [[height]],
			desc = [[ An optional number that indicates the Label height in pixels, autosized to fit text content if omitted. ]],
			color = 'lightyellow'}, nil} },
	['Label.cursor read/write property'] = {
	desc = [[Get or set the image of the mouse cursor as it hovers over the Label. Mouse cursors are represented by strings : 
        - "arrow" : the default mouse cursor. 
        - "cross" : a crosshair cursor. 
        - "working" : the standard arrow with a small hourglass. 
        - "hand" : a hand cursor. 
        - "help" : the standard arrow with a small question mark. 
        - "ibeam" : the default I-Beam text cursor. 
        - "forbidden" : a slashed circle cursor. 
        - "cardinal" : a four-pointed arrow pointing up, down, right, and left. 
        - "horizontal" : a double-pointed arrow pointing right and left. 
        - "vertical" : a double-pointed arrow pointing up and down. 
        - "leftdiagonal" : a double-pointed arrow pointing topright down to the left. 
        - "rightdiagonal" : a double-pointed arrow pointing topleft down to the right. 
        - "up" : an arrow pointing up. 
        - "wait" : a hourglass cursor. 
        - "none" : no cursor is displayed. The label.cursor property affects only the Label area.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Label-cursor.html',
	params = {} },
	['Label.enabled read/write property'] = {
	desc = [[Get or set the label ability to respond to mouse, and any other events. When set to false, disable the Label (the user will not be able to interact with it), and label's events won't be fired anymore.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Label-enabled.html',
	params = {} },
	['Label.font read/write property'] = {
	desc = [[Get the Label font, a string value that represent the font name. To set the Label.font property, you can assign a string that represent either a font name of an installed system font, or a font file (*.ttf, *.fon...). A File can also be provided, representing a font file. Note that only the font family is changed. The font style and font size are not affected (see the Label.fontstyle and the Label.fontsize properties). 
        - By default, a Label uses the same font as its parent widget 
        - Any change in the Label.font, Label.fontstyle and Label.fontsize properties results in an automatic resizing of the Label so that the content is displayed correctly]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Label-font.html',
	params = {} },
	['Label.fontsize read/write property'] = {
	desc = [[Get or set the label font size, a number that represent the font size in pixels. Any change in the Label.font, Label.fontstyle and Label.fontsize properties results in an automatic resizing of the Label so that the content is displayed correctly]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Label-fontsize.html',
	params = {} },
	['Label.fontstyle read/write property'] = {
	desc = [[Get or set the label font style, a table value that contains the following keys/values : 
        - "italic" : set to true if the font is in italic. 
        - "underline" : set to true if the font is underlined. 
        - "strike" : set to true if the font is striked. 
        - "thin" : set to true if the font is thin. 
        - "bold" : set to true if the font is bold. 
        - "heavy" : set to true if the font is heavy. Any change in the Label.font, Label.fontstyle and Label.fontsize properties results in an automatic resizing of the Label so that the content is displayed correctly]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Label-fontstyle.html',
	params = {} },
	['Label.height read/write property'] = {
	desc = [[Get or set the height of the Label area. The height starts from 0 (top side of the Label) and increase to the bottom direction.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Label-height.html',
	params = {} },
	['Label:hide() method'] = {
	desc = [[Hide and deactivate the Label (events can no longer be fired).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Label-hide.html',
	params = {} },
	['Label:onClick() event'] = {
	desc = [[This event is fired when the user has clicked on the Label.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Label-onclick.html',
	params = {} },
	['Label:onContext() event'] = {
	desc = [[This event is fired when the user has clicked on the label with the right mouse button.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Label-oncontext.html',
	params = {} },
	['Label:onCreate() event'] = {
	desc = [[This event is fired when the Label object has just been created (just after the Label:constructor() call). This event is particularly interesting when you want to inherit from a widget object, or to initialize its properties.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Label-oncreate.html',
	params = {} },
	['Label:onHide() event'] = {
	desc = [[This event is fired when the Label is hidden (with a call to Label:hide() or setting the Label.visible property to false).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Label-onhide.html',
	params = {} },
	['Label:onHover (x, y) event'] = {
	desc = [[This event is fired when the user moves the mouse pointer over the Label.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Label-onhover.html',
	params = {
		{
			name = [[x]],
			desc = [[ The horizontal position of the mouse in the Label area (zero means the left border of the widget). ]],
			color = 'lightyellow'}, 
		{
			name = [[y]],
			desc = [[ The vertical position of the mouse in the Label area (zero means the top border of the widget). ]],
			color = 'lightyellow'}, nil} },
	['Label:onLeave() event'] = {
	desc = [[This event is fired when the mouse cursor leaves the Label.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Label-onleave.html',
	params = {} },
	['Label:onShow() event'] = {
	desc = [[This event is fired when the Label is shown (with a call to Label:show() or setting the Label.visible property to true).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Label-onshow.html',
	params = {} },
	['Label.parent readonly property'] = {
	desc = [[Get the parent object that owns the Label.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Label-parent.html',
	params = {} },
	['Label:show() method'] = {
	desc = [[Show and activate the Label (events can now be fired).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Label-show.html',
	params = {} },
	['Label.text read/write property'] = {
	desc = [[Get or set the Label text content as a string.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Label-text.html',
	params = {} },
	['Label.textalign read/write property'] = {
	desc = [[Get or set the Label text alignment, a string value that represent how the text will be displayed. 
        - "left" : the text is aligned to the left edge of the Label (the default). 
        - "right" : the text is aligned to the right edge of the Label. 
        - "center" : the text is aligned in the center of the Label area.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Label-textalign.html',
	params = {} },
	['Label.tooltip read/write property'] = {
	desc = [[Get or set the tooltip text that appears when the user moves the mouse over the control, represented as a string value. Set the property to an empty string to remove the tooltip message.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Label-tooltip.html',
	params = {} },
	['Label.visible read/write property'] = {
	desc = [[Get or set the label visibility on screen, a true value means that the Label is shown, a false value means that the Label is hidden. The label.visible property is also affected by the Label:show() and Label:hide() methods.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Label-visible.html',
	params = {} },
	['Label.width read/write property'] = {
	desc = [[Get or set the Label area width. The width starts from 0 (left side of the Label) and increases to the right direction.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Label-width.html',
	params = {} },
	['Label.x read/write property'] = {
	desc = [[Get or set the Label horizontal position. The horizontal position start from 0 (left side of the parent widget) and increase to the right (right side of the parent widget).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Label-x.html',
	params = {} },
	['Label.y read/write property'] = {
	desc = [[Get or set the Label vertical position. The vertical position start from 0 (top side of the parent widget) and increase to the bottom side of the parent widget.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Label-y.html',
	params = {} },
	['ui.Label'] = {
	desc = [[Label is an Object representation of a static text caption.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\label.html',
	params = {} },
	['List:add(itemtext) method'] = {
	desc = [[Add an item to the List list. itemtext The string to be added to the List list. These items can later be accessed using the List.items property, as ListItem objects.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-add.html',
	params = {} },
	['List:align(alignment) method'] = {
	desc = [[Change List position and size to be aligned relative to its parent.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-align.html',
	params = {
		{
			name = [[alignment]],
			desc = [[ A string that specifies the List alignment : 
        - "all" : the List will be aligned along all the parent borders (and will cover the entire parent client area). 
        - "bottom" : the List will be aligned along the bottom border of the parent, preserving its height. 
        - "top" : the List will be aligned along the the top border of the parent, preserving its height. 
        - "right" : the List will be aligned along the right border of the parent, preserving its width. 
        - "left" : the List will be aligned along the left border of the parent, preserving its width. ]],
			color = 'lightpurple'}, nil} },
	['List:center() method'] = {
	desc = [[Centers the List on the parent widget.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-center.html',
	params = {} },
	['List:clear() method'] = {
	desc = [[Remove all items from the List.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-clear.html',
	params = {} },
	['List.constructor(parent, items, x , y , width , height) constructor'] = {
	desc = [[The List constructor returns a List value representing a scrollable list of items.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-constructor.html',
	params = {
		{
			name = [[parent]],
			desc = [[ An object that will own the List. Parent objects can be any of Window, Groupbox and Tab ]],
			color = 'brightwhite'}, 
		{
			name = [[items]],
			desc = [[ A table containing a list of strings (each string will be inserted in the List). ]],
			color = 'green'}, 
		{
			name = [[x]],
			desc = [[ An optional number that indicates the List horizontal position, in pixels. Zero means the left border of the parent. ]],
			color = 'lightyellow'}, 
		{
			name = [[y]],
			desc = [[ An optional number that indicates the List vertical position, in pixels. Zero means the top border of the parent. ]],
			color = 'lightyellow'}, 
		{
			name = [[width]],
			desc = [[ An optional number that indicates the List width in pixels. ]],
			color = 'lightyellow'}, 
		{
			name = [[height]],
			desc = [[ An optional number that indicates the List height in pixels. ]],
			color = 'lightyellow'}, nil} },
	['List.count readonly property'] = {
	desc = [[Get the current items count in the List.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-count.html',
	params = {} },
	['List.cursor read/write property'] = {
	desc = [[Get or set the image of the mouse cursor as it hovers over the List. Mouse cursors are represented by strings : 
        - "arrow" : the default mouse cursor. 
        - "cross" : a crosshair cursor. 
        - "working" : the standard arrow with a small hourglass. 
        - "hand" : a hand cursor. 
        - "help" : the standard arrow with a small question mark. 
        - "ibeam" : the default I-Beam text cursor. 
        - "forbidden" : a slashed circle cursor. 
        - "cardinal" : a four-pointed arrow pointing up, down, right, and left. 
        - "horizontal" : a double-pointed arrow pointing right and left. 
        - "vertical" : a double-pointed arrow pointing up and down. 
        - "leftdiagonal" : a double-pointed arrow pointing topright down to the left. 
        - "rightdiagonal" : a double-pointed arrow pointing topleft down to the right. 
        - "up" : an arrow pointing up. 
        - "wait" : a hourglass cursor. 
        - "none" : no cursor is displayed. The list.cursor property affects only the List area.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-cursor.html',
	params = {} },
	['List.enabled read/write property'] = {
	desc = [[Get or set the list ability to respond to mouse, and any other events. When set to false, disable the List (the user will not be able to interact with it), and list's events won't be fired anymore.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-enabled.html',
	params = {} },
	['List.font read/write property'] = {
	desc = [[Get the List font, a string value that represent the font name. To set the List.font property, you can assign a string that represent either a font name of an installed system font, or a font file (*.ttf, *.fon...). A File can also be provided, representing a font file. Note that only the font family is changed. The font style and font size are not affected (see the List.fontstyle and the List.fontsize properties). By default, an List uses the same font as its parent widget]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-font.html',
	params = {} },
	['List.fontsize read/write property'] = {
	desc = [[Get or set the List font size, a number that represent the font size in pixels.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-fontsize.html',
	params = {} },
	['List.fontstyle read/write property'] = {
	desc = [[Get or set the List font style, a table value that contains the following keys/values : 
        - "italic" : set to true if the font is in italic. 
        - "underline" : set to true if the font is underlined. 
        - "strike" : set to true if the font is striked. 
        - "thin" : set to true if the font is thin. 
        - "bold" : set to true if the font is bold. 
        - "heavy" : set to true if the font is heavy.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-fontstyle.html',
	params = {} },
	['List.height read/write property'] = {
	desc = [[Get or set the height of the List. The height starts from 0 (top side of the List) and increase to the bottom direction.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-height.html',
	params = {} },
	['List:hide() method'] = {
	desc = [[Hide and deactivate the List (events can no longer be fired).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-hide.html',
	params = {} },
	['List.items read/write propertyiterable'] = {
	desc = [[Provides access to the List items.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-items.html',
	params = {} },
	['List:onChange() event'] = {
	desc = [[This event is fired when items have been modified in the List.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-onchange.html',
	params = {} },
	['List:onContext(item) event'] = {
	desc = [[This event is fired when a right-click occured on the List.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-oncontext.html',
	params = {} },
	['List:onCreate() event'] = {
	desc = [[This event is fired when the List object has just been created (just after the List:constructor() call). This event is particularly interesting when you want to inherit from a widget object, or to initialize its properties.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-oncreate.html',
	params = {} },
	['List:onDoubleClick(ListItem) event'] = {
	desc = [[Event fired when the user has double-clicked on an item in the List.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-ondoubleclick.html',
	params = {} },
	['List:onHide() event'] = {
	desc = [[This event is fired when the List is hidden (with a call to List:hide() or setting the List.visible property to false).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-onhide.html',
	params = {} },
	['List:onHover() event'] = {
	desc = [[This event is fired when the user moves the mouse pointer over the List.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-onhover.html',
	params = {
		{
			name = [[x]],
			desc = [[ The horizontal position of the mouse in the List area (zero means the left border of the widget). ]],
			color = 'lightyellow'}, 
		{
			name = [[y]],
			desc = [[ The vertical position of the mouse in the List area (zero means the top border of the widget). ]],
			color = 'lightyellow'}, nil} },
	['List:onLeave() event'] = {
	desc = [[This event is fired when the mouse cursor leaves the List.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-onleave.html',
	params = {} },
	['List:onSelect(ListItem) event'] = {
	desc = [[Event fired when the user has choosen an item in the List.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-onselect.html',
	params = {} },
	['List:onShow() event'] = {
	desc = [[This event is fired when the List is shown (with a call to List:show() or setting the List.visible property to true).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-onshow.html',
	params = {} },
	['List.parent readonly property'] = {
	desc = [[Get the parent object that owns the List.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-parent.html',
	params = {} },
	['List:remove(index | item) method'] = {
	desc = [[Remove an item from the List. index | item A number, representing the item index or the ListItem to be removed.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-remove.html',
	params = {} },
	['List.selected readonly property'] = {
	desc = [[Get or set the current selected ListItem or nil if no selection has been made.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-selected.html',
	params = {} },
	['List:show() method'] = {
	desc = [[Show and activate the List (events can now be fired).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-show.html',
	params = {} },
	['List:sort(direction) method'] = {
	desc = [[Sort the List items.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-sort.html',
	params = {
		{
			name = [[direction]],
			desc = [[ A string that specifies the sort direction : 
        - "none" : No sorting. 
        - "ascend" : the List will be sorted in alphabetic order 
        - "descend" : the List will be sorted in inverse alphabetic order. ]],
			color = 'lightpurple'}, nil} },
	['List.style read/write property'] = {
	desc = [[Get or set the current style, a string value, that specifies the List appearance : 
        - "text" : List will display only text items (default). 
        - "icons" : List will display text items with their icons, using ListItem:loadicon(). When setting the style property from "icons" to "text" all previously loaded icons are preserved.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-style.html',
	params = {} },
	['List.visible read/write property'] = {
	desc = [[Get or set the List visibility on screen, a true value means that the List is shown, a false value means that the List is hidden. The list.visible property is also affected by the List:show() and List:hide() methods.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-visible.html',
	params = {} },
	['List.width read/write property'] = {
	desc = [[Get or set the List width. The width starts from 0 (left side of the List) and increases to the right direction.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-width.html',
	params = {} },
	['List.x read/write property'] = {
	desc = [[Get or set the List horizontal position. The horizontal position start from 0 (left side of the parent widget) and increase to the right (right side of the parent widget).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-x.html',
	params = {} },
	['List.y read/write property'] = {
	desc = [[Get or set the List vertical position. The vertical position start from 0 (top side of the parent widget) and increase to the bottom side of the parent widget.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\List-y.html',
	params = {} },
	['ui.List'] = {
	desc = [[List is an Object representation of a single line text field.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\list.html',
	params = {} },
	['Menu:add(itemtext) method'] = {
	desc = [[Add a new item to the Menu. itemtext The string to be added to the Menu. These items can later be accessed using the Menu.items property, as MenuItems objects.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Menu-add.html',
	params = {} },
	['Menu:clear() method'] = {
	desc = [[Remove all items from the Menu. An empty Window menu will not be drawn. Adding items to an empty Window menu will draw it again.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Menu-clear.html',
	params = {} },
	['Menu.constructor(itemtext1, itemtext2...) constructor'] = {
	desc = [[The Menu constructor returns a Menu value representing a popup or window menu.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Menu-constructor.html',
	params = {
		{
			name = [[itemtext1...]],
			desc = [[ A list of optional strings that represent items captions in the Menu. ]],
			color = 'lightpurple'}, nil} },
	['Menu.count readonly property'] = {
	desc = [[Get the current count of menu items.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Menu-count.html',
	params = {} },
	['Menu:insert(index, caption , submenu) method'] = {
	desc = [[Insert an item in the Menu, at the specified index, with an optional submenu. index A number representing the item position in the Menu. caption An optional string representing the item caption. submenu An optional item submenu Menu object.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Menu-insert.html',
	params = {
		{
			name = [[index]],
			desc = [[ A number representing the item position in the Menu. ]],
			color = 'lightyellow'}, 
		{
			name = [[caption]],
			desc = [[ An optional string representing the item caption. ]],
			color = 'lightpurple'}, 
		{
			name = [[submenu]],
			desc = [[ An optional item submenu Menu object. ]],
			color = 'brightwhite'}, nil} },
	['Menu.items read/write propertyiterable'] = {
	desc = [[Provides access to the Menu items.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Menu-items.html',
	params = {} },
	['Menu:onClick(item) event'] = {
	desc = [[This event is fired when the user has clicked on a Menu item. This event will not be fired if the clicked MenuItem already has an onClick() event handler, that will be called instead.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Menu-onclick.html',
	params = {
		{
			name = [[item]],
			desc = [[ The Menuitem that have been clicked. ]],
			color = 'brightwhite'}, nil} },
	['Menu:remove(index | item) method'] = {
	desc = [[Remove an existing item from the Menu. index | item A number representing the item index, or the ListItem to be removed.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Menu-remove.html',
	params = {
		{
			name = [[index | item]],
			desc = [[ A number representing the item index, or the ListItem to be removed. ]],
			color = 'lightyellow'}, nil} },
	['ui.Menu'] = {
	desc = [[Menu is an Object representation of a popup or window menu.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\menu.html',
	params = {} },
	['MenuItem.checked read/write properties'] = {
	desc = [[Get or set checked status of the Menu item. When set to true, place a checked mark to the left of the Menu item text.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Menuitem-checked.html',
	params = {} },
	['MenuItem.enabled read/write properties'] = {
	desc = [[Get or set the Menu item ability to respond to mouse click. When set to false, disable the Menu item.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Menuitem-enabled.html',
	params = {} },
	['MenuItem.index read/write properties'] = {
	desc = [[Get or set the Menu item position, a number starting from 1 and increase until the total count of MenuItems.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Menuitem-index.html',
	params = {} },
	['MenuItem:loadicon(path, index) method'] = {
	desc = [[Loads an menuitem icon, displayed to the left of the Menu item's text.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Menuitem-loadicon.html',
	params = {
		{
			name = [[path]],
			desc = [[ Represent any of the following possible icon locations : 
        - A string which represents the path to an ".ico" icon file, or gets the icon associated with the provided file/directory. 
        - A Widget object, whose icon will be used by the MenuItem. 
        - A Directory or File object, representing an ".ico" file, or gets the icon associated with the provided file/directory. 
        - A Buffer object, whose binary content will represent the icon. ]],
			color = 'brightwhite'}, 
		{
			name = [[index]],
			desc = [[ The icon index, a optional number starting from 1, that represent the icon to select. ]],
			color = 'lightyellow'}, nil} },
	['MenuItem:onClick() event'] = {
	desc = [[This event is fired when the user has clicked on a Menu item. If defined, this event will be fired instead of the Menu:onClick() event.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Menuitem-onclick.html',
	params = {} },
	['MenuItem:remove() method'] = {
	desc = [[Removes the menu item from the Menu it belongs to.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Menuitem-remove.html',
	params = {} },
	['MenuItem.submenu read/write properties'] = {
	desc = [[Get or set the Menu item submenu as a Menu object. Often called cascading menu, a submenu is a secondary menu displayed within a MenuItem when the user clicks on it.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Menuitem-submenu.html',
	params = {} },
	['MenuItem.text read/write properties'] = {
	desc = [[Get or set the menu item caption as a string.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Menuitem-text.html',
	params = {} },
	['Menuitem'] = {
	desc = [[Menuitem is an Object representation of an existing item in a Menu. As these objects have no constructor, they cannot be instantiated by programming. Only their owners can create them. Menuitems are ephemeral objects that are instantiated by their owner objects, with the "items" property. It is not recommended that you store them in a table or use them globally since they may refer to menu items that have been removed.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\menuitem.html',
	params = {} },
	['ui.mousepos()'] = {
	desc = [[Get the current mouse position on the screen.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\mousepos.html',
	params = {} },
	['ui.msg (msg , title)'] = {
	desc = [[Displays a message dialog box, and waits for the user to click the OK button.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\msg.html',
	params = {
		{
			name = [[msg]],
			desc = [[ A string that indicates the message to display to the user. ]],
			color = 'lightpurple'}, 
		{
			name = [[title]],
			desc = [[ An optional string that indicates the title of the message dialog box (defaulting to "Message") ]],
			color = 'lightpurple'}, nil} },
	['ui.opendialog (title , allow_mutiple_selection , filters)'] = {
	desc = [[Displays a dialog box for the user to choose a file or set of files to be opened. The file(s) must exist physically on the drive.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\opendialog.html',
	params = {
		{
			name = [[title]],
			desc = [[ An optional string that indicates the title of the dialog box (use the default system title if not specified). ]],
			color = 'lightpurple'}, 
		{
			name = [[allow_multiple_selection]],
			desc = [[ An optional boolean value that indicates if the dialog allow multiple files selection or not. ]],
			color = 'lightyellow'}, 
		{
			name = [[filters]],
			desc = [[ An optional string that indicates filters. A filter is used to display only files with a specified pattern. If omitted, the dialog shows all files. A filter is composed of a and a field, each separated by the character "|". For example, the string "All files (*.*)|*.*|Text files (*.txt)|*.txt" defines 2 filters : 
        - A filter named "All files (*.*)" that correspond to the search pattern "*.*" 
        - A filter named "Text files (*.txt)" that correspond to the search pattern "*.txt" ]],
			color = 'lightpurple'}, nil} },
	['Picture:align(alignment) method'] = {
	desc = [[Change Picture position and size to be aligned relative to its parent.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Picture-align.html',
	params = {
		{
			name = [[alignment]],
			desc = [[ A string that specifies the Picture alignment : 
        - "all" : the Picture will be aligned along all the parent borders (and will cover the entire parent client area). 
        - "bottom" : the Picture will be aligned along the bottom border of the parent, preserving its height. 
        - "top" : the Picture will be aligned along the the top border of the parent, preserving its height. 
        - "right" : the Picture will be aligned along the right border of the parent, preserving its width. 
        - "left" : the Picture will be aligned along the left border of the parent, preserving its width. ]],
			color = 'lightpurple'}, nil} },
	['Picture:center() method'] = {
	desc = [[Centers the Picture on the parent widget.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Picture-center.html',
	params = {} },
	['Picture.constructor(parent, image, x , y , width , height) constructor'] = {
	desc = [[The Picture constructor returns a Picture object representing a static image on the parent object.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Picture-constructor.html',
	params = {
		{
			name = [[parent]],
			desc = [[ An object that will own the Picture. Parent objects can be any of Window, Groupbox and Tab ]],
			color = 'brightwhite'}, 
		{
			name = [[image]],
			desc = [[ A File object or a filename string, representing the image to be loaded. ]],
			color = 'lightpurple'}, 
		{
			name = [[x]],
			desc = [[ An optional number that indicates the Picture horizontal position, in pixels. Zero means the left border of the parent. ]],
			color = 'lightyellow'}, 
		{
			name = [[y]],
			desc = [[ An optional number that indicates the Picture vertical position, in pixels. Zero means the top border of the parent. ]],
			color = 'lightyellow'}, 
		{
			name = [[width]],
			desc = [[ An optional number that indicates the Picture width in pixels, autosized to fit text content if omitted. Defaults to the image width if not provided. ]],
			color = 'lightyellow'}, 
		{
			name = [[height]],
			desc = [[ An optional number that indicates the Picture height in pixels, autosized to fit text content if omitted. Defaults to the image height if not provided. ]],
			color = 'lightyellow'}, nil} },
	['Picture.cursor read/write property'] = {
	desc = [[Get or set the image of the mouse cursor as it hovers over the Picture. Mouse cursors are represented by strings : 
        - "arrow" : the default mouse cursor. 
        - "cross" : a crosshair cursor. 
        - "working" : the standard arrow with a small hourglass. 
        - "hand" : a hand cursor. 
        - "help" : the standard arrow with a small question mark. 
        - "ibeam" : the default I-Beam text cursor. 
        - "forbidden" : a slashed circle cursor. 
        - "cardinal" : a four-pointed arrow pointing up, down, right, and left. 
        - "horizontal" : a double-pointed arrow pointing right and left. 
        - "vertical" : a double-pointed arrow pointing up and down. 
        - "leftdiagonal" : a double-pointed arrow pointing topright down to the left. 
        - "rightdiagonal" : a double-pointed arrow pointing topleft down to the right. 
        - "up" : an arrow pointing up. 
        - "wait" : a hourglass cursor. 
        - "none" : no cursor is displayed. The picture.cursor property affects only the Picture area.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Picture-cursor.html',
	params = {} },
	['Picture.enabled read/write property'] = {
	desc = [[Get or set the picture ability to respond to mouse, and any other events. When set to false, disable the Picture (the user will not be able to interact with it), and picture's events won't be fired anymore.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Picture-enabled.html',
	params = {} },
	['Picture.height read/write property'] = {
	desc = [[Get or set the height of the Picture area. The height starts from 0 (top side of the Picture) and increase to the bottom direction. When changing the Picture.height property, the image expands/shrinks accordingly.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Picture-height.html',
	params = {} },
	['Picture:hide() method'] = {
	desc = [[Hide and deactivate the Picture (events can no longer be fired).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Picture-hide.html',
	params = {} },
	['Picture:load(file) method'] = {
	desc = [[Load an image on the Picture object.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Picture-load.html',
	params = {
		{
			name = [[file]],
			desc = [[ A File object or a filename string, representing the image to be loaded. ]],
			color = 'lightpurple'}, nil} },
	['Picture:onClick() event'] = {
	desc = [[This event is fired when the user has clicked on the Picture.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Picture-onclick.html',
	params = {} },
	['Picture:onContext() event'] = {
	desc = [[This event is fired when the user has clicked on the picture with the right mouse button.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Picture-oncontext.html',
	params = {} },
	['Picture:onCreate() event'] = {
	desc = [[This event is fired when the Picture object has just been created (just after the Picture:constructor() call). This event is particularly interesting when you want to inherit from a widget object, or to initialize its properties.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Picture-oncreate.html',
	params = {} },
	['Picture:onDoubleClick() event'] = {
	desc = [[This event is fired when the user has doubleclicked on the Picture.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Picture-ondoubleclick.html',
	params = {} },
	['Picture:onHide() event'] = {
	desc = [[This event is fired when the Picture is hidden (with a call to Picture:hide() or setting the Picture.visible property to false).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Picture-onhide.html',
	params = {} },
	['Picture:onHover (x, y) event'] = {
	desc = [[This event is fired when the user moves the mouse pointer over the Picture.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Picture-onhover.html',
	params = {
		{
			name = [[x]],
			desc = [[ The horizontal position of the mouse in the Picture area (zero means the left border of the widget). ]],
			color = 'lightyellow'}, 
		{
			name = [[y]],
			desc = [[ The vertical position of the mouse in the Picture area (zero means the top border of the widget). ]],
			color = 'lightyellow'}, nil} },
	['Picture:onLeave() event'] = {
	desc = [[This event is fired when the mouse cursor leaves the Picture.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Picture-onleave.html',
	params = {} },
	['Picture:onShow() event'] = {
	desc = [[This event is fired when the Picture is shown (with a call to Picture:show() or setting the Picture.visible property to true).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Picture-onshow.html',
	params = {} },
	['Picture.parent readonly property'] = {
	desc = [[Get the parent object that owns the Picture.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Picture-parent.html',
	params = {} },
	['Picture:resize(factor) method'] = {
	desc = [[Resize the Picture.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Picture-resize.html',
	params = {
		{
			name = [[factor]],
			desc = [[ A number representing the factor to apply on the Picture's width and height (proportional resizing). Use a factor of 1 to get the original size back. ]],
			color = 'lightyellow'}, nil} },
	['Picture:save(file) method'] = {
	desc = [[Save the Picture image on disk.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Picture-save.html',
	params = {
		{
			name = [[file]],
			desc = [[ A File object or a filename string, representing the image to be saved. ]],
			color = 'lightpurple'}, nil} },
	['Picture:show() method'] = {
	desc = [[Show and activate the Picture (events can now be fired).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Picture-show.html',
	params = {} },
	['Picture.tooltip read/write property'] = {
	desc = [[Get or set the tooltip text that appears when the user moves the mouse over the control, represented as a string value. Set the property to an empty string to remove the tooltip message.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Picture-tooltip.html',
	params = {} },
	['Picture.visible read/write property'] = {
	desc = [[Get or set the picture visibility on screen, a true value means that the Picture is shown, a false value means that the Picture is hidden. The picture.visible property is also affected by the Picture:show() and Picture:hide() methods.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Picture-visible.html',
	params = {} },
	['Picture.width read/write property'] = {
	desc = [[Get or set the Picture area width. The width starts from 0 (left side of the Picture) and increases to the right direction. When changing the Picture.width property, the image expands/shrinks accordingly.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Picture-width.html',
	params = {} },
	['Picture.x read/write property'] = {
	desc = [[Get or set the Picture horizontal position. The horizontal position start from 0 (left side of the parent widget) and increase to the right (right side of the parent widget).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Picture-x.html',
	params = {} },
	['Picture.y read/write property'] = {
	desc = [[Get or set the Picture vertical position. The vertical position start from 0 (top side of the parent widget) and increase to the bottom side of the parent widget.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Picture-y.html',
	params = {} },
	['ui.Picture'] = {
	desc = [[Picture is an Object representation of a static image.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\picture.html',
	params = {} },
	['Radiobutton:align(alignment) method'] = {
	desc = [[Change Radiobutton position and size to be aligned relative to its parent.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Radiobutton-align.html',
	params = {
		{
			name = [[alignment]],
			desc = [[ A string that specifies the Radiobutton alignment : 
        - "all" : the Radiobutton will be aligned along all the parent borders (and will cover the entire parent client area). 
        - "bottom" : the Radiobutton will be aligned along the bottom border of the parent, preserving its height. 
        - "top" : the Radiobutton will be aligned along the the top border of the parent, preserving its height. 
        - "right" : the Radiobutton will be aligned along the right border of the parent, preserving its width. 
        - "top" : the Radiobutton will be aligned along the left border of the parent, preserving its width. ]],
			color = 'lightpurple'}, nil} },
	['Radiobutton:autosize() method'] = {
	desc = [[Resize the Radiobutton automatically to accommodate its content.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Radiobutton-autosize.html',
	params = {} },
	['Radiobutton:center() method'] = {
	desc = [[Centers the Radiobutton on the parent widget.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Radiobutton-center.html',
	params = {} },
	['Radiobutton.checked read/write property'] = {
	desc = [[Set or check wether the Radiobutton item is selected. 
        - A Radiobutton is always used in groups, grouped by their parent object.
        - Radiobuttons are exclusive in the same group : selecting one deselects the others.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Radiobutton-checked.html',
	params = {} },
	['Radiobutton.constructor(parent, caption, x , y , width , height) constructor'] = {
	desc = [[The Radiobutton constructor returns a Radiobutton value representing a push radiobutton on the parent object. 
        - A Radiobutton is always used in groups, grouped by their parent object.
        - Radiobuttons are exclusive in the same group : selecting one deselects the others.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Radiobutton-constructor.html',
	params = {
		{
			name = [[parent]],
			desc = [[ An object that will own the Radiobutton. Parent objects can be any of Window, Groupbox and Tab ]],
			color = 'brightwhite'}, 
		{
			name = [[caption]],
			desc = [[ A string representing the Radiobutton's text. ]],
			color = 'lightpurple'}, 
		{
			name = [[x]],
			desc = [[ An optional number that indicates the Radiobutton horizontal position, in pixels. Zero means the left border of the parent. ]],
			color = 'lightyellow'}, 
		{
			name = [[y]],
			desc = [[ An optional number that indicates the Radiobutton vertical position, in pixels. Zero means the top border of the parent. ]],
			color = 'lightyellow'}, 
		{
			name = [[width]],
			desc = [[ An optional number that indicates the Radiobutton width in pixels, autosized to fit text content if omitted. ]],
			color = 'lightyellow'}, 
		{
			name = [[height]],
			desc = [[ An optional number that indicates the Radiobutton height in pixels, autosized to fit text content if omitted. ]],
			color = 'lightyellow'}, nil} },
	['Radiobutton.cursor read/write property'] = {
	desc = [[Get or set the image of the mouse cursor as it hovers over the Radiobutton. Mouse cursors are represented by strings : 
        - "arrow" : the default mouse cursor. 
        - "cross" : a crosshair cursor. 
        - "working" : the standard arrow with a small hourglass. 
        - "hand" : a hand cursor. 
        - "help" : the standard arrow with a small question mark. 
        - "ibeam" : the default I-Beam text cursor. 
        - "forbidden" : a slashed circle cursor. 
        - "cardinal" : a four-pointed arrow pointing up, down, right, and left. 
        - "horizontal" : a double-pointed arrow pointing right and left. 
        - "vertical" : a double-pointed arrow pointing up and down. 
        - "leftdiagonal" : a double-pointed arrow pointing topright down to the left. 
        - "rightdiagonal" : a double-pointed arrow pointing topleft down to the right. 
        - "up" : an arrow pointing up. 
        - "wait" : a hourglass cursor. 
        - "none" : no cursor is displayed. The radiobutton.cursor property affects only the Radiobutton area.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Radiobutton-cursor.html',
	params = {} },
	['Radiobutton.enabled read/write property'] = {
	desc = [[Get or set the radiobutton ability to respond to mouse, and any other events. When set to false, disable the Radiobutton (the user will not be able to interact with it), and radiobutton's events won't be fired anymore.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Radiobutton-enabled.html',
	params = {} },
	['Radiobutton.font read/write property'] = {
	desc = [[Get the Radiobutton font, a string value that represent the font name. To set the Radiobutton.font property, you can assign a string that represent either a font name of an installed system font, or a font file (*.ttf, *.fon...). A File can also be provided, representing a font file. Note that only the font family is changed. The font style and font size are not affected (see the Radiobutton.fontstyle and the Radiobutton.fontsize properties). By default, a Radiobutton uses the same font as its parent widget Any change in the Radiobutton.font, Radiobutton.fontstyle and Radiobutton.fontsize properties results in an automatic resizing of the Radiobutton so that the content is displayed correctly]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Radiobutton-font.html',
	params = {} },
	['Radiobutton.fontsize read/write property'] = {
	desc = [[Get or set the radiobutton font size, a number that represent the font size in pixels. Any change in the Radiobutton.font, Radiobutton.fontstyle and Radiobutton.fontsize properties results in an automatic resizing of the Radiobutton so that the content is displayed correctly]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Radiobutton-fontsize.html',
	params = {} },
	['Radiobutton.fontstyle read/write property'] = {
	desc = [[Get or set the radiobutton font style, a table value that contains the following keys/values : 
        - "italic" : set to true if the font is in italic. 
        - "underline" : set to true if the font is underlined. 
        - "strike" : set to true if the font is striked. 
        - "thin" : set to true if the font is thin. 
        - "bold" : set to true if the font is bold. 
        - "heavy" : set to true if the font is heavy. Any change in the Radiobutton.font, Radiobutton.fontstyle and Radiobutton.fontsize properties results in an automatic resizing of the Radiobutton so that the content is displayed correctly]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Radiobutton-fontstyle.html',
	params = {} },
	['Radiobutton.height read/write property'] = {
	desc = [[Get or set the height of the Radiobutton area. The height starts from 0 (top side of the Radiobutton) and increase to the bottom direction.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Radiobutton-height.html',
	params = {} },
	['Radiobutton:hide() method'] = {
	desc = [[Hide and deactivate the Radiobutton (events can no longer be fired).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Radiobutton-hide.html',
	params = {} },
	['Radiobutton:onClick() event'] = {
	desc = [[This event is fired when the user has clicked on the Radiobutton.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Radiobutton-onclick.html',
	params = {} },
	['Radiobutton:onContext() event'] = {
	desc = [[This event is fired when the user has clicked on the Radiobutton with the right mouse radiobutton.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Radiobutton-oncontext.html',
	params = {} },
	['Radiobutton:onCreate() event'] = {
	desc = [[This event is fired when the Radiobutton object has just been created (just after the Radiobutton:constructor() call). This event is particularly interesting when you want to inherit from a widget object, or to initialize its properties.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Radiobutton-oncreate.html',
	params = {} },
	['Radiobutton:onHide() event'] = {
	desc = [[This event is fired when the Radiobutton is hidden (with a call to Radiobutton:hide() or setting the Radiobutton.visible property to false).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Radiobutton-onhide.html',
	params = {} },
	['Radiobutton:onHover (x, y) event'] = {
	desc = [[This event is fired when the user moves the mouse pointer over the Radiobutton.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Radiobutton-onhover.html',
	params = {
		{
			name = [[x]],
			desc = [[ The horizontal position of the mouse in the Radiobutton area (zero means the left border of the widget). ]],
			color = 'lightyellow'}, 
		{
			name = [[y]],
			desc = [[ The vertical position of the mouse in the Radiobutton area (zero means the top border of the widget). ]],
			color = 'lightyellow'}, nil} },
	['Radiobutton:onLeave() event'] = {
	desc = [[This event is fired when the mouse cursor leaves the Radiobutton.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Radiobutton-onleave.html',
	params = {} },
	['Radiobutton:onShow() event'] = {
	desc = [[This event is fired when the Radiobutton is shown (with a call to Radiobutton:show() or setting the Radiobutton.visible property to true).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Radiobutton-onshow.html',
	params = {} },
	['Radiobutton.parent readonly property'] = {
	desc = [[Get the parent widget that owns the Radiobutton.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Radiobutton-parent.html',
	params = {} },
	['Radiobutton:show() method'] = {
	desc = [[Show and activate the Radiobutton (events can now be fired).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Radiobutton-show.html',
	params = {} },
	['Radiobutton.text read/write property'] = {
	desc = [[Get or set the Radiobutton caption as a string.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Radiobutton-text.html',
	params = {} },
	['Radiobutton.tooltip read/write property'] = {
	desc = [[Get or set the tooltip text that appears when the user moves the mouse over the Radiobutton, represented as a string value. Set the property to an empty string to remove the tooltip message.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Radiobutton-tooltip.html',
	params = {} },
	['Radiobutton.visible read/write property'] = {
	desc = [[Get or set the radiobutton visibility on screen, a true value means that the Radiobutton is shown, a false value means that the Radiobutton is hidden. The radiobutton.visible property is also affected by the Radiobutton:show() and Radiobutton:hide() methods.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Radiobutton-visible.html',
	params = {} },
	['Radiobutton.width read/write property'] = {
	desc = [[Get or set the Radiobutton area width. The width starts from 0 (left side of the Radiobutton) and increases to the right direction.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Radiobutton-width.html',
	params = {} },
	['Radiobutton.x read/write property'] = {
	desc = [[Get or set the Radiobutton horizontal position. The horizontal position start from 0 (left side of the parent widget) and increase to the right (right side of the parent widget).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Radiobutton-x.html',
	params = {} },
	['Radiobutton.y read/write property'] = {
	desc = [[Get or set the Radiobutton vertical position. The vertical position start from 0 (top side of the parent widget) and increase to the bottom side of the parent widget.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Radiobutton-y.html',
	params = {} },
	['ui.Radiobutton'] = {
	desc = [[Radiobutton is an Object representation of an exclusive option that can be checked or unchecked.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\radiobutton.html',
	params = {} },
	['ui.remove(widget)'] = {
	desc = [[Removes a widget. Once a widget is removed, it should not be accessed anymore and will be garbagecollected at the next garbage collection step.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\remove.html',
	params = {
		{
			name = [[widget]],
			desc = [[ The widget to be removed. ]],
			color = 'brightwhite'}, nil} },
	['ui.savedialog (title , allow_mutiple_selection , filters)'] = {
	desc = [[Displays a dialog box for the user to specify the drive, directory, and name of a file to save.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\savedialog.html',
	params = {
		{
			name = [[title]],
			desc = [[ An optional string that indicates the title of the dialog box (use the default system title if not specified). ]],
			color = 'lightpurple'}, 
		{
			name = [[allow_multiple_selection]],
			desc = [[ An optional boolean value that indicates if the dialog allow multiple files selection or not. ]],
			color = 'lightyellow'}, 
		{
			name = [[filters]],
			desc = [[ An optional string that indicates filters. A filter is used to display only files with a specified pattern. If omitted, the dialog shows all files. A filter is composed of a and a field, each separated by the character "|". For example, the string "All files (*.*)|*.*|Text files (*.txt)|*.txt" defines 2 filters : 
        - A filter named "All files (*.*)" that correspond to the search pattern "*.*" 
        - A filter named "Text files (*.txt)" that correspond to the search pattern "*.txt" ]],
			color = 'lightpurple'}, nil} },
	['Tab:add(itemtext) method'] = {
	desc = [[Add a new item to the Tab. itemtext The string to be added to the Tab. These items can later be accessed using the Tab.items property, as TabItems objects.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tab-add.html',
	params = {} },
	['Tab:align(alignment) method'] = {
	desc = [[Change Tab position and size to be aligned relative to its parent.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tab-align.html',
	params = {
		{
			name = [[alignment]],
			desc = [[ A string that specifies the Tab alignment : 
        - "all" : the Tab will be aligned along all the parent borders (and will cover the entire parent client area). 
        - "bottom" : the Tab will be aligned along the bottom border of the parent, preserving its height. 
        - "top" : the Tab will be aligned along the the top border of the parent, preserving its height. 
        - "right" : the Tab will be aligned along the right border of the parent, preserving its width. 
        - "left" : the Tab will be aligned along the left border of the parent, preserving its width. ]],
			color = 'lightpurple'}, nil} },
	['Tab:center() method'] = {
	desc = [[Centers the Tab on the parent widget.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tab-center.html',
	params = {} },
	['Tab:clear() method'] = {
	desc = [[Remove all items from the Tab.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tab-clear.html',
	params = {} },
	['Tab.constructor(parent, items, x , y , width , height) constructor'] = {
	desc = [[The Tab constructor returns a Tab value representing a notebook widget, which manages multiple views with associated tabs.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tab-constructor.html',
	params = {
		{
			name = [[parent]],
			desc = [[ An object that will own the Tab. Parent objects can be any of Window, Groupbox and Tab ]],
			color = 'brightwhite'}, 
		{
			name = [[items]],
			desc = [[ A table containing strings (each string will be inserted in the Tab). ]],
			color = 'green'}, 
		{
			name = [[x]],
			desc = [[ An optional number that indicates the Tab horizontal position, in pixels. Zero means the left border of the parent. ]],
			color = 'lightyellow'}, 
		{
			name = [[y]],
			desc = [[ An optional number that indicates the Tab vertical position, in pixels. Zero means the top border of the parent. ]],
			color = 'lightyellow'}, 
		{
			name = [[width]],
			desc = [[ An optional number that indicates the Tab width in pixels. ]],
			color = 'lightyellow'}, 
		{
			name = [[height]],
			desc = [[ An optional number that indicates the Tab height in pixels. ]],
			color = 'lightyellow'}, nil} },
	['Tab.count readonly property'] = {
	desc = [[Get the current items count in the Tab.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tab-count.html',
	params = {} },
	['Tab.cursor read/write property'] = {
	desc = [[Get or set the image of the mouse cursor as it hovers over the Tab. Mouse cursors are represented by strings : 
        - "arrow" : the default mouse cursor. 
        - "cross" : a crosshair cursor. 
        - "working" : the standard arrow with a small hourglass. 
        - "hand" : a hand cursor. 
        - "help" : the standard arrow with a small question mark. 
        - "ibeam" : the default I-Beam text cursor. 
        - "forbidden" : a slashed circle cursor. 
        - "cardinal" : a four-pointed arrow pointing up, down, right, and left. 
        - "horizontal" : a double-pointed arrow pointing right and left. 
        - "vertical" : a double-pointed arrow pointing up and down. 
        - "leftdiagonal" : a double-pointed arrow pointing topright down to the left. 
        - "rightdiagonal" : a double-pointed arrow pointing topleft down to the right. 
        - "up" : an arrow pointing up. 
        - "wait" : a hourglass cursor. 
        - "none" : no cursor is displayed. The tab.cursor property affects only the Tab area.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tab-cursor.html',
	params = {} },
	['Tab.enabled read/write property'] = {
	desc = [[Get or set the tab ability to respond to mouse, and any other events. When set to false, disable the Tab (the user will not be able to interact with it), and tab's events won't be fired anymore. When a Tab is disabled, the user cannot select Tabitems anymore, and any child controls]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tab-enabled.html',
	params = {} },
	['Tab.font read/write property'] = {
	desc = [[Get the Tab font, a string value that represent the font name. To set the Tab.font property, you can assign a string that represent either a font name of an installed system font, or a font file (*.ttf, *.fon...). A File can also be provided, representing a font file. Note that only the font family is changed. The font style and font size are not affected (see the Tab.fontstyle and the Tab.fontsize properties). By default, an Tab uses the same font as its parent widget]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tab-font.html',
	params = {} },
	['Tab.fontsize read/write property'] = {
	desc = [[Get or set the Tab font size, a number that represent the font size in pixels.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tab-fontsize.html',
	params = {} },
	['Tab.fontstyle read/write property'] = {
	desc = [[Get or set the Tab font style, a table value that contains the following keys/values : 
        - "italic" : set to true if the font is in italic. 
        - "underline" : set to true if the font is underlined. 
        - "strike" : set to true if the font is striked. 
        - "thin" : set to true if the font is thin. 
        - "bold" : set to true if the font is bold. 
        - "heavy" : set to true if the font is heavy.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tab-fontstyle.html',
	params = {} },
	['Tab.height read/write property'] = {
	desc = [[Get or set the height of the Tab. The height starts from 0 (top side of the Tab) and increase to the bottom direction.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tab-height.html',
	params = {} },
	['Tab:hide() method'] = {
	desc = [[Hide and deactivate the Tab (events can no longer be fired).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tab-hide.html',
	params = {} },
	['Tab.items read/write propertyiterable'] = {
	desc = [[Provides access to the Tab items.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tab-items.html',
	params = {} },
	['Tab:onContext(item) event'] = {
	desc = [[This event is fired when a right-click occured on the Tab.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tab-oncontext.html',
	params = {} },
	['Tab:onCreate() event'] = {
	desc = [[This event is fired when the Tab object has just been created (just after the Tab:constructor() call). This event is particularly interesting when you want to inherit from a widget object, or to initialize its properties.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tab-oncreate.html',
	params = {} },
	['Tab:onHide() event'] = {
	desc = [[This event is fired when the Tab is hidden (with a call to Tab:hide() or setting the Tab.visible property to false).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tab-onhide.html',
	params = {} },
	['Tab:onHover() event'] = {
	desc = [[This event is fired when the user moves the mouse pointer over the Tab.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tab-onhover.html',
	params = {
		{
			name = [[x]],
			desc = [[ The horizontal position of the mouse in the Tab area (zero means the left border of the widget). ]],
			color = 'lightyellow'}, 
		{
			name = [[y]],
			desc = [[ The vertical position of the mouse in the Tab area (zero means the top border of the widget). ]],
			color = 'lightyellow'}, nil} },
	['Tab:onLeave() event'] = {
	desc = [[This event is fired when the mouse cursor leaves the Tab.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tab-onleave.html',
	params = {} },
	['Tab:onSelect(TabItem) event'] = {
	desc = [[Event fired when the user has choosen an item in the Tab.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tab-onselect.html',
	params = {} },
	['Tab:onShow() event'] = {
	desc = [[This event is fired when the Tab is shown (with a call to Tab:show() or setting the Tab.visible property to true).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tab-onshow.html',
	params = {} },
	['Tab.parent readonly property'] = {
	desc = [[Get the parent object that owns the Tab.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tab-parent.html',
	params = {} },
	['Tab:remove(index | item) method'] = {
	desc = [[Remove an item from the Tab. index | item A number, representing the item index or the TabItem to be removed.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tab-remove.html',
	params = {} },
	['Tab.selected readonly property'] = {
	desc = [[Get or set the current selected TabItem or nil if no selection has been made.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tab-selected.html',
	params = {} },
	['Tab:show() method'] = {
	desc = [[Show and activate the Tab (events can now be fired).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tab-show.html',
	params = {} },
	['Tab.style read/write property'] = {
	desc = [[Get or set the current style, a string value, that specifies the Tab appearance : 
        - "text" : Tree will display only text items (default). 
        - "icons" : Tree will display text items with their icons, using Tabtem:loadicon(). When setting the style property from "icons" to "text" all previously loaded icons are preserved.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tab-style.html',
	params = {} },
	['Tab.visible read/write property'] = {
	desc = [[Get or set the Tab visibility on screen, a true value means that the Tab is shown, a false value means that the Tab is hidden. The tab.visible property is also affected by the Tab:show() and Tab:hide() methods.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tab-visible.html',
	params = {} },
	['Tab.width read/write property'] = {
	desc = [[Get or set the Tab width. The width starts from 0 (left side of the Tab) and increases to the right direction.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tab-width.html',
	params = {} },
	['Tab.x read/write property'] = {
	desc = [[Get or set the Tab horizontal position. The horizontal position start from 0 (left side of the parent widget) and increase to the right (right side of the parent widget).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tab-x.html',
	params = {} },
	['Tab.y read/write property'] = {
	desc = [[Get or set the Tab vertical position. The vertical position start from 0 (top side of the parent widget) and increase to the bottom side of the parent widget.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tab-y.html',
	params = {} },
	['ui.Tab'] = {
	desc = [[Tab is an Object representation of a notebook, which manages multiple views with associated tabs.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\tab.html',
	params = {} },
	['Tree:add(itemtext) method'] = {
	desc = [[Add an item to the Tree.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-add.html',
	params = {} },
	['Tree:align(alignment) method'] = {
	desc = [[Change Tree position and size to be aligned relative to its parent.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-align.html',
	params = {
		{
			name = [[alignment]],
			desc = [[ A string that specifies the Tree alignment : 
        - "all" : the Tree will be aligned along all the parent borders (and will cover the entire parent client area). 
        - "bottom" : the Tree will be aligned along the bottom border of the parent, preserving its height. 
        - "top" : the Tree will be aligned along the the top border of the parent, preserving its height. 
        - "right" : the Tree will be aligned along the right border of the parent, preserving its width. 
        - "left" : the Tree will be aligned along the left border of the parent, preserving its width. ]],
			color = 'lightpurple'}, nil} },
	['Tree:center() method'] = {
	desc = [[Centers the Tree on the parent widget.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-center.html',
	params = {} },
	['Tree:clear() method'] = {
	desc = [[Remove all items and subitems from the Tree.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-clear.html',
	params = {} },
	['Tree.constructor(parent, items, x , y , width , height) constructor'] = {
	desc = [[The Tree constructor returns a Tree value representing a scrollable tree of items.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-constructor.html',
	params = {
		{
			name = [[parent]],
			desc = [[ An object that will own the Tree. Parent objects can be any of Window, Groupbox and Tab ]],
			color = 'brightwhite'}, 
		{
			name = [[items]],
			desc = [[ A table containing a list of strings with or without tables values (each string will be inserted in the Tree, tables represent subitems). ]],
			color = 'green'}, 
		{
			name = [[x]],
			desc = [[ An optional number that indicates the Tree horizontal position, in pixels. Zero means the left border of the parent. ]],
			color = 'lightyellow'}, 
		{
			name = [[y]],
			desc = [[ An optional number that indicates the Tree vertical position, in pixels. Zero means the top border of the parent. ]],
			color = 'lightyellow'}, 
		{
			name = [[width]],
			desc = [[ An optional number that indicates the Tree width in pixels. ]],
			color = 'lightyellow'}, 
		{
			name = [[height]],
			desc = [[ An optional number that indicates the Tree height in pixels. ]],
			color = 'lightyellow'}, nil} },
	['Tree.count readonly property'] = {
	desc = [[Get the current root items count in the Tree, subitems not included.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-count.html',
	params = {} },
	['Tree.cursor read/write property'] = {
	desc = [[Get or set the image of the mouse cursor as it hovers over the Tree. Mouse cursors are represented by strings : 
        - "arrow" : the default mouse cursor. 
        - "cross" : a crosshair cursor. 
        - "working" : the standard arrow with a small hourglass. 
        - "hand" : a hand cursor. 
        - "help" : the standard arrow with a small question mark. 
        - "ibeam" : the default I-Beam text cursor. 
        - "forbidden" : a slashed circle cursor. 
        - "cardinal" : a four-pointed arrow pointing up, down, right, and left. 
        - "horizontal" : a double-pointed arrow pointing right and left. 
        - "vertical" : a double-pointed arrow pointing up and down. 
        - "leftdiagonal" : a double-pointed arrow pointing topright down to the left. 
        - "rightdiagonal" : a double-pointed arrow pointing topleft down to the right. 
        - "up" : an arrow pointing up. 
        - "wait" : a hourglass cursor. 
        - "none" : no cursor is displayed. The tree.cursor property affects only the Tree area.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-cursor.html',
	params = {} },
	['Tree.enabled read/write property'] = {
	desc = [[Get or set the tree ability to respond to mouse, and any other events. When set to false, disable the Tree (the user will not be able to interact with it), and tree's events won't be fired anymore.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-enabled.html',
	params = {} },
	['Tree.font read/write property'] = {
	desc = [[Get the Tree font, a string value that represent the font name. To set the Tree.font property, you can assign a string that represent either a font name of an installed system font, or a font file (*.ttf, *.fon...). A File can also be provided, representing a font file. Note that only the font family is changed. The font style and font size are not affected (see the Tree.fontstyle and the Tree.fontsize properties). By default, an Tree uses the same font as its parent widget]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-font.html',
	params = {} },
	['Tree.fontsize read/write property'] = {
	desc = [[Get or set the Tree font size, a number that represent the font size in pixels.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-fontsize.html',
	params = {} },
	['Tree.fontstyle read/write property'] = {
	desc = [[Get or set the Tree font style, a table value that contains the following keys/values : 
        - "italic" : set to true if the font is in italic. 
        - "underline" : set to true if the font is underlined. 
        - "strike" : set to true if the font is striked. 
        - "thin" : set to true if the font is thin. 
        - "bold" : set to true if the font is bold. 
        - "heavy" : set to true if the font is heavy.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-fontstyle.html',
	params = {} },
	['Tree.height read/write property'] = {
	desc = [[Get or set the height of the Tree. The height starts from 0 (top side of the Tree) and increase to the bottom direction.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-height.html',
	params = {} },
	['Tree:hide() method'] = {
	desc = [[Hide and deactivate the Tree (events can no longer be fired).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-hide.html',
	params = {} },
	['Tree.items read/write propertyiterable'] = {
	desc = [[Provides access to the Tree items.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-items.html',
	params = {} },
	['Tree:onChange(item , action) event'] = {
	desc = [[This event is fired when an item have been removed or edited by the user after a call to TreeItem:edit().]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-onchange.html',
	params = {
		{
			name = [[If an item has been removed, this argument represent the item text as a string If an item have been edited, this argument represent the edited TreeItem. action]],
			desc = [[ A string that represent the change that occured : 
        - "removed" : TreeItem has been removed. 
        - "edited" : TreeItem has been edited. ]],
			color = 'lightpurple'}, nil} },
	['Tree:onContext(item) event'] = {
	desc = [[This event is fired when a right-click occured on the Tree.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-oncontext.html',
	params = {} },
	['Tree:onCreate() event'] = {
	desc = [[This event is fired when the Tree object has just been created (just after the Tree:constructor() call). This event is particularly interesting when you want to inherit from a widget object, or to initialize its properties.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-oncreate.html',
	params = {} },
	['Tree:onDoubleClick(TreeItem) event'] = {
	desc = [[Event fired when the user has double-clicked on an item in the Tree.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-ondoubleclick.html',
	params = {} },
	['Tree:onHide() event'] = {
	desc = [[This event is fired when the Tree is hidden (with a call to Tree:hide() or setting the Tree.visible property to false).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-onhide.html',
	params = {} },
	['Tree:onHover() event'] = {
	desc = [[This event is fired when the user moves the mouse pointer over the Tree.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-onhover.html',
	params = {
		{
			name = [[x]],
			desc = [[ The horizontal position of the mouse in the Tree area (zero means the left border of the widget). ]],
			color = 'lightyellow'}, 
		{
			name = [[y]],
			desc = [[ The vertical position of the mouse in the Tree area (zero means the top border of the widget). ]],
			color = 'lightyellow'}, nil} },
	['Tree:onLeave() event'] = {
	desc = [[This event is fired when the mouse cursor leaves the Tree.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-onleave.html',
	params = {} },
	['Tree:onSelect(TreeItem) event'] = {
	desc = [[Event fired when the user has choosen an item in the Tree.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-onselect.html',
	params = {} },
	['Tree:onShow() event'] = {
	desc = [[This event is fired when the Tree is shown (with a call to Tree:show() or setting the Tree.visible property to true).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-onshow.html',
	params = {} },
	['Tree.parent readonly property'] = {
	desc = [[Get the parent object that owns the Tree.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-parent.html',
	params = {} },
	['Tree:remove(text | item) method'] = {
	desc = [[Remove an item from the Tree. text | item A string, representing the item's text or the TreeItem to be removed.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-remove.html',
	params = {} },
	['Tree.selected readonly property'] = {
	desc = [[Get or set the current selected TreeItem or nil if no selection has been made.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-selected.html',
	params = {} },
	['Tree:show() method'] = {
	desc = [[Show and activate the Tree (events can now be fired).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-show.html',
	params = {} },
	['Tree:sort(subitems) method'] = {
	desc = [[Sort Tree root items by ascending order.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-sort.html',
	params = {
		{
			name = [[subitems]],
			desc = [[ An optional boolean value that specifies to sort all levels of child items below the root items, defaulting to false. ]],
			color = 'lightyellow'}, nil} },
	['Tree.style read/write property'] = {
	desc = [[Get or set the current style, a string value, that specifies the Tree appearance : 
        - "text" : Tree will display only text items (default). 
        - "icons" : Tree will display text items with their icons, using TreeItem:loadicon(). When setting the style property from "icons" to "text" all previously loaded icons are preserved.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-style.html',
	params = {} },
	['Tree.visible read/write property'] = {
	desc = [[Get or set the Tree visibility on screen, a true value means that the Tree is shown, a false value means that the Tree is hidden. The tree.visible property is also affected by the Tree:show() and Tree:hide() methods.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-visible.html',
	params = {} },
	['Tree.width read/write property'] = {
	desc = [[Get or set the Tree width. The width starts from 0 (left side of the Tree) and increases to the right direction.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-width.html',
	params = {} },
	['Tree.x read/write property'] = {
	desc = [[Get or set the Tree horizontal position. The horizontal position start from 0 (left side of the parent widget) and increase to the right (right side of the parent widget).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-x.html',
	params = {} },
	['Tree.y read/write property'] = {
	desc = [[Get or set the Tree vertical position. The vertical position start from 0 (top side of the parent widget) and increase to the bottom side of the parent widget.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Tree-y.html',
	params = {} },
	['ui.Tree'] = {
	desc = [[Tree is an Object representation of a single line text field.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\tree.html',
	params = {} },
	['TreeItem:add(itemtext) method'] = {
	desc = [[Add a subitem to the current TreeItem.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\TreeItem-add.html',
	params = {} },
	['TreeItem:clear() method'] = {
	desc = [[Removes all the TreeItem's subitems.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\TreeItem-clear.html',
	params = {} },
	['TreeItem.count readonly property'] = {
	desc = [[Get the item's count of subitems, or 0 if the item don't have any subitems.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\TreeItem-count.html',
	params = {} },
	['TreeItem:edit() method'] = {
	desc = [[Begins in-place editing of the specified TreeItem's caption. When the user has done editing it, the parent Tree receives an TreeItem:onChange() event.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\TreeItem-edit.html',
	params = {} },
	['TreeItem:expand(expanding) method'] = {
	desc = [[Expands or collapses the subitems associated with the TreeItem.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\TreeItem-expand.html',
	params = {} },
	['TreeItem:loadicon(path, index) method'] = {
	desc = [[Loads an item icon, displayed to the left of the item's text.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\TreeItem-loadicon.html',
	params = {
		{
			name = [[path]],
			desc = [[ Represent any of the following possible icon locations : 
        - A string which represents the path to an ".ico" icon file, or gets the icon associated with the provided file/directory. 
        - A Widget object, whose icon will be used by the item. 
        - A Directory or File object, representing an ".ico" file, or gets the icon associated with the provided file/directory. 
        - A Buffer object, whose binary content will represent the icon. ]],
			color = 'brightwhite'}, 
		{
			name = [[index]],
			desc = [[ The icon index, a optional number starting from 1, that represent the icon to select. ]],
			color = 'lightyellow'}, nil} },
	['TreeItem.parent readonly property'] = {
	desc = [[Get the Object that owns the item : either a TreeItem for subitem or a Tree for root items. TreeItems are ephemeral objects that are instantiated by their parent objects, with the "items" property. It is not recommended that you store them in a table or use them globally since they may refer to items that have been deleted.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\TreeItem-parent.html',
	params = {} },
	['TreeItem:remove() method'] = {
	desc = [[Removes the item from the Tree it belongs to.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\TreeItem-remove.html',
	params = {} },
	['TreeItem:show() method'] = {
	desc = [[Ensures that the TreeItem is visible, expanding the parent item or scrolling the Tree, if necessary.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\TreeItem-show.html',
	params = {} },
	['TreeItem:sort(recursive) method'] = {
	desc = [[Sorts all the subitems in ascending order.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\TreeItem-sort.html',
	params = {} },
	['TreeItem.subitems read/write properties'] = {
	desc = [[Get or set the item's subitems, a proxy table that contains a tree of TreeItem, indexed only by their TreeItem.text property.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\TreeItem-subitems.html',
	params = {} },
	['TreeItem.textread/write property'] = {
	desc = [[Get or set the item caption as a string.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\TreeItem-text.html',
	params = {} },
	['TreeItem'] = {
	desc = [[TreeItem is an Object representation of an existing item in a Tree. TreeItem Object has no constructor, it cannot be instantiated by programming. Only its owner can create it dynamically. TreeItems are ephemeral objects that are instantiated by their owner objects, with the "items" property. It is not recommended that you store them in a table or use them globally since they may refer to items that have been deleted.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\treeitem.html',
	params = {} },
	['ui.update()'] = {
	desc = [[Update the user interface, process Windows messages and call widgets events.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\update.html',
	params = {} },
	['ui.warn (msg , title)'] = {
	desc = [[Displays a warning message dialog box with an exclamation-point icon, and waits for the user to click the OK button.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\warn.html',
	params = {
		{
			name = [[msg]],
			desc = [[ A string that indicates the message to display to the user. ]],
			color = 'lightpurple'}, 
		{
			name = [[title]],
			desc = [[ An optional string that indicates the title of the message dialog box (defaulting to "Warning") ]],
			color = 'lightpurple'}, nil} },
	['Window:align(alignment) method'] = {
	desc = [[Change Window position and size to be aligned relative to the Windows Desktop.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-align.html',
	params = {
		{
			name = [[alignment]],
			desc = [[ A string that specifies the Window alignment : 
        - "all" : the Window will be aligned along all the Desktop borders. 
        - "bottom" : the Window will be aligned along the bottom border of the Desktop, preserving its height. 
        - "top" : the Window will be aligned along the the top border of the Desktop, preserving its height. 
        - "right" : the Window will be aligned along the right border of the Desktop, preserving its width. 
        - "left" : the Window will be aligned along the left border of the Desktop, preserving its width. ]],
			color = 'lightpurple'}, nil} },
	['Window:center() method'] = {
	desc = [[Centers the Window on the parent widget.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-center.html',
	params = {} },
	['Window.constructor(title, style, width , height) constructor'] = {
	desc = [[The Window constructor returns a Window value representing a user interface dialog. Please note that a created Window will not be shown until its method Window:show() has been called.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-constructor.html',
	params = {
		{
			name = [[title]],
			desc = [[ A string representing the Window's title. ]],
			color = 'lightpurple'}, 
		{
			name = [[style]],
			desc = [[ A string that describe the Window style : 
        - "dialog" : the default, a resizeable dialog. The user can maximize/minimize the window. 
        - "fixed" : a fixed size dialog. 
        - "float" : a tiny float window. ]],
			color = 'lightpurple'}, 
		{
			name = [[width]],
			desc = [[ An optional number that indicates the Window width, in pixels, defaulting to 640. ]],
			color = 'lightyellow'}, 
		{
			name = [[height]],
			desc = [[ An optional number that indicates the Window height, in pixels, defaulting to 480. The width and height arguments represent the size of the client area of window, excluding borders and title bar. ]],
			color = 'lightyellow'}, nil} },
	['Window.cursor read/write property'] = {
	desc = [[Get or set the image of the mouse cursor as it hovers over the window. Mouse cursors are represented by strings : 
        - "arrow" : the default Windows mouse cursor. 
        - "cross" : a crosshair cursor. 
        - "working" : the standard arrow with a small hourglass. 
        - "hand" : a hand cursor. 
        - "help" : the standard arrow with a small question mark. 
        - "ibeam" : the default I-Beam text cursor. 
        - "forbidden" : a slashed circle cursor. 
        - "cardinal" : a four-pointed arrow pointing up, down, right, and left. 
        - "horizontal" : a double-pointed arrow pointing right and left. 
        - "vertical" : a double-pointed arrow pointing up and down. 
        - "leftdiagonal" : a double-pointed arrow pointing topright down to the left. 
        - "rightdiagonal" : a double-pointed arrow pointing topleft down to the right. 
        - "up" : an arrow pointing up. 
        - "wait" : a hourglass cursor. 
        - "none" : no cursor is displayed. The window.cursor property affects only the window client area. The title bar and borders are not affected.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-cursor.html',
	params = {} },
	['Window.enabled read/write property'] = {
	desc = [[Get or set the window ability to respond to mouse, keyboard and any other events. When set to false, disable the Window (the user will not be able to interact with it), and window's events won't be fired anymore.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-enabled.html',
	params = {} },
	['Window:focus (control) method'] = {
	desc = [[Sets focus to a child control on the window.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-focus.html',
	params = {
		{
			name = [[menu]],
			desc = [[ A child control on the window to be focused ]],
			color = 'brightwhite'}, nil} },
	['Window.font read/write property'] = {
	desc = [[Get the window font, a string value that represent the font name. To set the Window.font property, you can assign a string that represent either a font name of an installed system font, or a font file (*.ttf, *.fon...). A File can also be provided, representing a font file. Note that only the font family is changed. The font style and font size are not affected (see the Window.fontstyle and the Window.fontsize properties). By changing the font of the window, you also change the font of any newly created child controls. When setting the window font, if the specified font is not installed on the system, the font is not changed.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-font.html',
	params = {} },
	['Window.fontsize read/write property'] = {
	desc = [[Get or set the window font size, a number that represent the font size in pixels. By changing the font size of the window, you also change the font size of any newly created child controls.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-fontsize.html',
	params = {} },
	['Window.fontstyle read/write property'] = {
	desc = [[Get or set the window font style, a table value that contains the following keys/values : 
        - "italic" : set to true if the font is in italic. 
        - "underline" : set to true if the font is underlined. 
        - "strike" : set to true if the font is striked. 
        - "thin" : set to true if the font is thin. 
        - "bold" : set to true if the font is bold. 
        - "heavy" : set to true if the font is heavy. By changing the font style of the window, you also change the font style of any newly created child controls.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-fontstyle.html',
	params = {} },
	['Window.height read/write property'] = {
	desc = [[Get or set the Window's height. The height starts from 0 (top side of the Window) and increase to the bottom direction. The height property represent the height of the client area of the window, excluding the title bar.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-height.html',
	params = {} },
	['Window:hide() method'] = {
	desc = [[Hide and deactivate the Window (events can no longer be fired).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-hide.html',
	params = {} },
	['Window:loadicon(path, index) method'] = {
	desc = [[Loads the window icon, displayed at the top left corner and on the taskbar.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-loadicon.html',
	params = {
		{
			name = [[path]],
			desc = [[ Represent any of the following possible icon locations : 
        - A string which represents the path to an ".ico" icon file, or gets the icon associated with the provided file/directory. 
        - A Widget object, whose icon will be used by the Window. 
        - A Directory or File object, representing an ".ico" file, or gets the icon associated with the provided file/directory. 
        - A Buffer object, whose binary content will represent the icon. ]],
			color = 'brightwhite'}, 
		{
			name = [[index]],
			desc = [[ The icon index, a optional number starting from 1, that represent the icon to select. ]],
			color = 'lightyellow'}, nil} },
	['Window:maximize() method'] = {
	desc = [[Maximize and activate the Window to the entire desktop. Fixed Windows cannot be resized, minimized or maximized by the user. However it can still be done programmatically]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-maximize.html',
	params = {} },
	['Window.menu read/write property'] = {
	desc = [[Get or set the main Menu of the Window. Assigning the nil value to this property will remove the main menu from the window]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-menu.html',
	params = {} },
	['Window:minimize() method'] = {
	desc = [[Minimize the Window to the taskbar(events can no longer be fired).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-minimize.html',
	params = {} },
	['Window:onClick (x, y) event'] = {
	desc = [[This event is fired when the user has clicked on the window.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-onclick.html',
	params = {
		{
			name = [[x]],
			desc = [[ The horizontal position of the mouse in the window's client area. ]],
			color = 'lightyellow'}, 
		{
			name = [[y]],
			desc = [[ The vertical position of the mouse in the window's client area. ]],
			color = 'lightyellow'}, nil} },
	['Window:onContext() event'] = {
	desc = [[This event is fired when the user has clicked on the window with the right mouse button.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-oncontext.html',
	params = {} },
	['Window:onCreate() event'] = {
	desc = [[This event is fired when the Window object has just been created (just after the Window:constructor() call). This event is particularly interesting when you want to inherit from a widget object, or to initialize its properties.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-oncreate.html',
	params = {} },
	['Window:onHide() event'] = {
	desc = [[This event is fired when the window is hidden by the user (when clicking on the window close button) or when calling Window:hide() or setting the Window.visible property to false.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-onhide.html',
	params = {} },
	['Window:onHover (x, y) event'] = {
	desc = [[This event is fired when the user moves the mouse pointer over the window.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-onhover.html',
	params = {
		{
			name = [[x]],
			desc = [[ The horizontal position of the mouse in the window's client area. ]],
			color = 'lightyellow'}, 
		{
			name = [[y]],
			desc = [[ The vertical position of the mouse in the window's client area. ]],
			color = 'lightyellow'}, nil} },
	['Window:onMove() event'] = {
	desc = [[This event is fired when the window position has been changed by the user or by setting the Window.x or Window.y properties.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-onmove.html',
	params = {} },
	['Window:onResize() event'] = {
	desc = [[This event is fired when the window has been resized by the user or by setting the Window.width or Window.height properties.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-onresize.html',
	params = {} },
	['Window:onShow() event'] = {
	desc = [[This event is fired when the window is shown (with a call to Window:show() or setting the Window.visible property to true).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-onshow.html',
	params = {} },
	['Window:popup(menu) method'] = {
	desc = [[Shows a popup menu at the current window mouse position.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-popup.html',
	params = {
		{
			name = [[menu]],
			desc = [[ A Menu object to be displayed. ]],
			color = 'brightwhite'}, nil} },
	['Window:shortcut (key, func, ctrl, shift, alt) method'] = {
	desc = [[Sets a new keyboard shortcut for the window.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-shortcut.html',
	params = {
		{
			name = [[key]],
			desc = [[ A one character string, which represents the key that will throw the shortcut's provided function. ]],
			color = 'lightpurple'}, 
		{
			name = [[func]],
			desc = [[ A function that will be executed each time the user press the key. ]],
			color = 'brightwhite'}, 
		{
			name = [[ctrl]],
			desc = [[ An optional boolean value, indicating if the CONTROL key must be pressed to fire the shortcut. ]],
			color = 'lightyellow'}, 
		{
			name = [[shift]],
			desc = [[ An optional boolean value, indicating if the SHIFT key must be pressed to fire the shortcut. ]],
			color = 'lightyellow'}, 
		{
			name = [[alt]],
			desc = [[ An optional boolean value, indicating if the ALT key must be pressed to fire the shortcut. ]],
			color = 'lightyellow'}, nil} },
	['Window:show() method'] = {
	desc = [[Show and activate the Window (events can now be fired).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-show.html',
	params = {} },
	['Window:showmodal(child) method'] = {
	desc = [[Show and activate a child Window. The parent widget is disabled and can not be used until the user closes the child Window.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-showmodal.html',
	params = {
		{
			name = [[child]],
			desc = [[ The Window abject that will be shown. ]],
			color = 'brightwhite'}, nil} },
	['Window:status (msg, ...) method'] = {
	desc = [[Sets the window status bar to display one or more messages.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-status.html',
	params = {
		{
			name = [[msg]],
			desc = [[ One or more strings, which represents messages to be displayed in the status bar. Each message is displayed in a part of the status bar. ]],
			color = 'lightpurple'}, nil} },
	['Window.title read/write property'] = {
	desc = [[Get or set the Window title as a string.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-title.html',
	params = {} },
	['Window.visible read/write property'] = {
	desc = [[Get or set the window visibility on screen, a true value means that the window is shown, a false value means that the window is hidden. The window.visible property is also affected by the Window:show() and Window:hide() methods.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-visible.html',
	params = {} },
	['Window.width read/write property'] = {
	desc = [[Get or set the Window's width. The width starts from 0 (left side of the Window) and increase to the right direction. The width property represent the width of the client area of the window, excluding the left and right borders]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-width.html',
	params = {} },
	['Window.x read/write property'] = {
	desc = [[Get or set the Window horizontal position. The horizontal position start from 0 (left side of the Desktop) and increase to the right (right side of the Desktop).]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-x.html',
	params = {} },
	['Window.y read/write property'] = {
	desc = [[Get or set the Window vertical position. The vertical position start from 0 (top side of the Desktop) and increase to the bottom side of the Desktop.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\Window-y.html',
	params = {} },
	['ui.Window'] = {
	desc = [[Window is an Object representation of a standard Windows dialog. A Window has a title bar, can contain other widgets and respond to events.]],
	addr = 'https:\\\\www.luart.org\\doc\\ui\\window.html',
	params = {} },
	['zip module'] = {
	desc = [[The zip module provides properties and functions to create, read, write and list a ZIP file. The ZIP file format is a common archive and compression standard defined by PKWARE Inc.]],
	addr = 'https:\\\\www.luart.org\\doc\\zip\\index.html',
	params = {} },
	['zip.isvalid(file)'] = {
	desc = [[The isvalid() function checks wether a file represent a valid ZIP archive.]],
	addr = 'https:\\\\www.luart.org\\doc\\zip\\isvalid.html',
	params = {
		{
			name = [[file]],
			desc = [[ A string or a File object, to be checked for ZIP validity. ]],
			color = 'brightwhite'}, nil} },
	['Zip:close() method'] = {
	desc = [[Closes the zip archive. Zip values that are garbage collected (for example when going out of scope) are automatically closed before destruction. In this case, it's not necessary to call the Zip:close() method.]],
	addr = 'https:\\\\www.luart.org\\doc\\zip\\zip-close.html',
	params = {} },
	['Zip(file, mode, level) constructor'] = {
	desc = [[The Zip constructor opens the specified ZIP archive using the specified mode. Returns a Zip value representing the ZIP archive.]],
	addr = 'https:\\\\www.luart.org\\doc\\zip\\zip-constructor.html',
	params = {
		{
			name = [[file]],
			desc = [[ A string or a File object, representing the ZIP archive to be opened. ]],
			color = 'brightwhite'}, 
		{
			name = [[mode]],
			desc = [[ An optional string that indicate the open mode : 
        - "read" : The zip archive is open for reading (the zip archive must exist). It's the default mode. 
        - "append" : The zip archive is open for appending (the zip archive must exist). 
        - "write" : The zip archive is open for writing (the zip archive is created and overwrites any previous existing archive). ]],
			color = 'lightpurple'}, nil} },
	['Zip.count readonly property'] = {
	desc = [[Get the number of entries in the zip archive.]],
	addr = 'https:\\\\www.luart.org\\doc\\zip\\zip-count.html',
	params = {} },
	['Zip.error readonly property'] = {
	desc = [[Get the last error message after a zip operation failed.]],
	addr = 'https:\\\\www.luart.org\\doc\\zip\\zip-error.html',
	params = {} },
	['Zip : extract(path, dir, use_entrypath) method'] = {
	desc = [[Extract on the disk an entry with the given path/name in the Zip archive. The Zip archive should have been previously opened in "read" mode.]],
	addr = 'https:\\\\www.luart.org\\doc\\zip\\zip-extract.html',
	params = {
		{
			name = [[path]],
			desc = [[ A string representing the path and/or the name of the entry in the Zip archive to be extracted. ]],
			color = 'lightpurple'}, 
		{
			name = [[dir]],
			desc = [[ An optional destination folder where the entry will be extracted on the disk, as a string or a Directory value.If omitted, the entry will be extracted in the current directory. ]],
			color = 'gray'}, 
		{
			name = [[use_entrypath]],
			desc = [[ An optional boolean value that indicate to extract the entry conserving its path or not. ]],
			color = 'lightyellow'}, nil} },
	['Zip : extractall(dir) method'] = {
	desc = [[Extract on the disk all the content of the Zip archive. The Zip archive should have been previously opened in "read" mode.]],
	addr = 'https:\\\\www.luart.org\\doc\\zip\\zip-extractall.html',
	params = {
		{
			name = [[dir]],
			desc = [[ The destination folder where the entire archive content will be extracted on the disk, as a string or a Directory value.If omitted, the archive will be extracted in the current folder. ]],
			color = 'gray'}, nil} },
	['Zip.file readonly property'] = {
	desc = [[Get the File object representing the current opened ZIP archive.]],
	addr = 'https:\\\\www.luart.org\\doc\\zip\\zip-file.html',
	params = {} },
	['Zip : isdirectory(path) method'] = {
	desc = [[Check if the entry with the given path/name in the Zip archive represents a directory. The Zip archive should have been previously opened in "read" mode.]],
	addr = 'https:\\\\www.luart.org\\doc\\zip\\zip-isdirectory.html',
	params = {
		{
			name = [[path]],
			desc = [[ A string representing the path and/or the name of the entry in the Zip archive. ]],
			color = 'lightpurple'}, nil} },
	['Zip.iszip64 readonly property'] = {
	desc = [[Checks if the zip archive uses the ZIP64 extensions (for zip archives larger than 4 GiB).]],
	addr = 'https:\\\\www.luart.org\\doc\\zip\\zip-iszip64.html',
	params = {} },
	['Zipiterable'] = {
	desc = [[Zip objects are iterable through the Luart helper function each, returning two values at each iteration : 
        - The next entry name in the zip archive as a string. 
        - A boolean value indicating if this entry represent a directory.]],
	addr = 'https:\\\\www.luart.org\\doc\\zip\\zip-iterator.html',
	params = {} },
	['#Ziplength operator'] = {
	desc = [[The length operator # applied on a Zip object returns the total number of entries that the zip archive contains. The Zip.count property returns the same value.]],
	addr = 'https:\\\\www.luart.org\\doc\\zip\\zip-length.html',
	params = {} },
	['Zip : read(path) method'] = {
	desc = [[Reads an entry in memory at the given path/name in the Zip archive. The Zip archive should have been previously opened in "read" mode.]],
	addr = 'https:\\\\www.luart.org\\doc\\zip\\zip-read.html',
	params = {
		{
			name = [[path]],
			desc = [[ A string representing the path and/or the name of the entry in the Zip archive. ]],
			color = 'lightpurple'}, nil} },
	['Zip:reopen(mode, level) method'] = {
	desc = [[Closes the zip archive and reopens it using the specified mode and level compression.]],
	addr = 'https:\\\\www.luart.org\\doc\\zip\\zip-reopen.html',
	params = {
		{
			name = [[mode]],
			desc = [[ An optional string that indicate the open mode : 
        - "read" : The zip archive is open for reading (the zip archive must exist). It's the default mode. 
        - "append" : The zip archive is open for appending (the zip archive must exist). 
        - "write" : The zip archive is open for writing (the zip archive is created and overwrites any previous existing archive). ]],
			color = 'lightpurple'}, 
		{
			name = [[level]],
			desc = [[ An optional number that indicate the compression level, from 0 (no compression) to 10 (maximum compression level), defaulting to 6. ]],
			color = 'lightyellow'}, nil} },
	['Zip.size readonly property'] = {
	desc = [[Get the size in bytes of the zip archive.]],
	addr = 'https:\\\\www.luart.org\\doc\\zip\\zip-size.html',
	params = {} },
	['Zip : write(entry, path) method'] = {
	desc = [[Writes a new entry in the Zip archive. The Zip archive should have been previously opened in "write" or "append" mode.]],
	addr = 'https:\\\\www.luart.org\\doc\\zip\\zip-write.html',
	params = {
		{
			name = [[entry]],
			desc = [[ The new entry to be written in the Zip archive. An entry can be : 
        - A string that represents a filename or directory whose contents will be archived. A Buffer whose binary content will be archived at the specified path. 
        - A File value whose content will be archived. 
        - A Directory value whose content will be archived ]],
			color = 'gray'}, 
		{
			name = [[path]],
			desc = [[ An optional string representing the path and/or the name of the new entry in the Zip archive. If omitted, the new entry will be archived at the root of the Zip archive, using the name provided by the entry parameter. ]],
			color = 'lightpurple'}, nil} },
	['Zip'] = {
	desc = [[The Zip Object provides Zip file capabilities. Because the "zip" module contains only the Zip Object, requiring the "zip" module returns directly the Zip object]],
	addr = 'https:\\\\www.luart.org\\doc\\zip\\zip.html',
	params = {} },
nil
}