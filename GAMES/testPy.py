
program = b"\x60\x00\xf0\x29\x61\x00\x62\x00\xd1\x25\x12\x08"

with open("test.ch8", "wb") as f:
	#print("Writing: " + bytes.fromhex(program))
	f.write(program)
